#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"



uint64_t symbolHash(char* sym){
	uint64_t ret = 718397189153151;
	int ix = 0;
	while(sym[ix] != 0){
		ret    ^=  sym[ix];
		int rot =  ret % 64;
		ret     = (ret << rot) | (ret >> (64 - rot));
		ix++;
	}
	ret += ix;
	if(ret == 0) ret += ix;		// this should almost never fire of course
	return ret;
}




SymbolTable makeSymbolTable(int size){
	SymbolTable ret;
	ret.syms = malloc(sizeof(Symbol) * size);
	ret.size = size;
	ret.fill = 0;
	for(int i = 0; i < size; i++){
		ret.syms[i].text = NULL;
		ret.syms[i].hash = 0;
	}
	return ret;
}


void insertSymbol(SymbolTable*, Symbol);

void growSymbolTable(SymbolTable* tab){
	Symbol* tmp = tab->syms;
	int oldsize = tab->size;
	int oldfill = tab->fill;
	*tab = makeSymbolTable(oldsize * 2);
	tab->fill   = oldfill;
	for(int i = 0; i < oldsize; i++) if(tmp[i].text != NULL) insertSymbol(tab, tmp[i]);
	free(tab);
}

void insertSymbol(SymbolTable* tab, Symbol s){
	if((tab->fill + 1) >= tab->size) growSymbolTable(tab);
	int offset = s.hash % tab->size;
	for(int i = 0; i < tab->size; i++){
		int ix = i + offset;
		ix = (ix >= tab->size)? ix - tab->size : ix;
		if(tab->syms[i].hash == 0){
			tab->syms[i] = s;
			tab->fill++;
			return;
		}
	}
}

void insertSymbolText(SymbolTable* tab, char* text){
	Symbol s;
	s.text = text;
	s.hash = symbolHash(text);
	insertSymbol(tab, s);
}

Symbol searchSymbol(SymbolTable* tab, Symbol s){
	int offset = s.hash % tab->size;
	Symbol ret;
	ret.hash = 0;
	ret.text = NULL;
	for(int i = 0; i < tab->size; i++){
		int ix = i + offset;
		ix = (ix >= tab->size)? ix - tab->size : ix;
		if(tab->syms[i].hash == s.hash){
			if(!strcmp(tab->syms[i].text, s.text)) return tab->syms[i];
		}else if(tab->syms[i].hash == 0){
			return ret;
		}
	}
	return ret;
}


void printSymbolTable(SymbolTable tab){
	for(int i = 0; i < tab.size; i++)
		if(tab.syms[i].hash != 0) printf("%i : #%lu => %s\n", i, tab.syms[i].hash, tab.syms[i].text);
}


char* printToken(Token tk, char* buffer){
	switch(tk.type){
		case TKN_PERIOD    : return " .  ";
		case TKN_DEFINE    : return " :: ";
		case TKN_COLON     : return " :  ";
		case TKN_SEMICOLON : return " ;  ";
		case TKN_COMMA     : return " ,  ";
		case TKN_NEWLINE   : return " \\n ";
		case TKN_L_ARROW   : return " <- ";
		case TKN_R_ARROW   : return " -> ";
		case TKN_BRK_OPN   : return " [  ";
		case TKN_BRK_END   : return " ]  ";
		case TKN_PAR_OPN   : return " (  ";
		case TKN_PAR_END   : return " )  ";
		case TKN_BRC_OPN   : return " {  ";
		case TKN_BRC_END   : return " }  ";
		case TKN_ADD       : return " +  ";
		case TKN_SUB       : return " -  ";
		case TKN_MUL       : return " *  ";
		case TKN_DIV       : return " /  ";
		case TKN_MOD       : return " %  ";
		case TKN_EQL       : return " =  ";
		case TKN_INT       : {
			sprintf(buffer, "INT %lu", tk.data.u64);
			return buffer;
		}break;
		case TKN_FLT       : {
			sprintf(buffer, "FLT %f", tk.data.f64);
			return buffer;
		}break;
		case TKN_STR       : return tk.data.str.text;
		case TKN_TAG       : return tk.data.str.text;
	}
	
	return "<?>";
}


void printLexerState(LexerState ls){
	for(int i = 0; i < ls.tkct; i++){
		char buffer[1024];
		Position p = ls.tks[i].pos;
		printf("%i : %s @ (%i:%i - %i:%i | %i)\n", i, printToken(ls.tks[i], buffer), p.lineStart, p.colStart, p.lineEnd, p.colEnd, p.fileId);
	}
}



char lexerEatChar(LangReader* lr){
	char ret = lr->text[lr->head];
	lr->head++;
	if(ret == '\n'){
		lr->line++;
		lr->column = 0;
	}else if(ret == '\v'){
		lr->line++;
	}else if(ret == '\t'){
		lr->column += 4;
	}else{
		lr->column++;
	}
	return ret;
}



int lexInt(LangReader* lr, Token* tk){
	LangReader lrOld = *lr;
	
	uint64_t x = 0;
	char     c = lr->text[lr->head];
	int    ret = 0;
	while((c >= '0') && (c <= '9')){
		x *= 10;
		x += c - '0';
		lr->head++;
		lr->column++;
		c = lr->text[lr->head];
		ret = 1;
	}
	
	if(ret){
		tk->pos      = (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column};
		tk->data.u64 = x;
		tk->type     = TKN_INT;
		return 1;
	}
	
	*lr = lrOld;
	return 0;
}


int lexFltFrac(LangReader* lr, Token* tk){
	LangReader lrOld = *lr;
	
	int64_t  x = 0;
	int64_t sc = 1;
	char     c = lr->text[lr->head];
	int    ret = 0;
	while((c >= '0') && (c <= '9')){
		x  *= 10;
		sc *= 10;
		x  += c - '0';
		lr->head++;
		lr->column++;
		c = lr->text[lr->head];
		ret = 1;
	}
	
	if(ret){
		tk->pos      = (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column};
		tk->data.f64 = ((double)x) / ((double)sc);
		tk->type     = TKN_FLT;
		return 1;
	}
	
	*lr = lrOld;
	return 0;
}






LexerState lexer(LangReader* lr){
	
	LexerState ret;
	ret.tks   = malloc(sizeof(Token) * lr->size / 4);
	ret.tkct  = 0;
	ret.tkcap = lr->size / 4;
	
	int cont = 1;
	while(cont){
		int  skip        =  0;
		char c           =  lr->text[lr->head];
		LangReader lrOld = *lr;
		
		lexerFork:
		
		if      ((skip < 1) &&  (c >= '0') && (c <= '9')){
			// Numbers
			int pass = 1;
			Token tk;
			pass = lexInt(lr, &tk);
			if(!pass){ *lr = lrOld; skip = 1; goto lexerFork; }
			
			char cx = lexerEatChar(lr);
			if(cx == '.'){
				// Might be a float
				Token fk;
				lexerEatChar(lr);
				pass = lexFltFrac(lr, &fk);
				if(!pass){ *lr = lrOld; skip = 1; goto lexerFork; }
				
				fk.data.f64 += (double)tk.data.u64;
				ret.tks[ret.tkct] = fk;
				ret.tkct++;
			}else if((cx >= 'a') && (cx <= 'z')){
				// Might have a suffix
				
			}else{
				// Just an int
				ret.tks[ret.tkct] = tk;
				ret.tkct++;
			}
			
		}else if((skip < 2) &&  (c >= 'a') && (c <= 'z')){
			// Common identifiers
			lr->head++;
		}else if((skip < 3) &&  (c >= 'A') && (c <= 'Z')){
			// Type identifiers
			lr->head++;
		}else if((skip < 4) && ((c == '~') || (c == '#'))){
			// Special identifiers
			lr->head++;
		}else if((skip < 5) && ((c == ' ') || (c == '\n') || (c == '\v') || (c == '\t'))){
			// Whitespace
			LangReader lrOld = *lr;
			char cx = lexerEatChar(lr);
			if(cx == '\n'){
				ret.tks[ret.tkct] = (Token){TKN_NEWLINE, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
				ret.tkct++;
			}
		}else if (skip < 6){
			// Symbols
			LangReader lrOld = *lr;
			char  cx = lexerEatChar(lr);
			Token tk;
			switch(cx){
				case '[' : tk = (Token){TKN_BRK_OPN, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ']' : tk = (Token){TKN_BRK_END, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '(' : tk = (Token){TKN_PAR_OPN, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ')' : tk = (Token){TKN_PAR_END, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '{' : tk = (Token){TKN_BRC_OPN, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '}' : tk = (Token){TKN_BRC_END, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '.' : tk = (Token){TKN_PERIOD , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ':' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == ':'){
						tk  = (Token){TKN_DEFINE , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_COLON  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '-' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '>'){
						tk  = (Token){TKN_R_ARROW, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_SUB    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '=' : tk = (Token){TKN_EQL    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ',' : tk = (Token){TKN_COMMA  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '+' : tk = (Token){TKN_ADD    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '*' : tk = (Token){TKN_MUL    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '/' : tk = (Token){TKN_DIV    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '%' : tk = (Token){TKN_MOD    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
			}
			ret.tks[ret.tkct] = tk;
			ret.tkct++;
		}else{
			lr->head++;
			printf("!!\n");
		}
		
		cont = lr->head < lr->size;
	}
	
	return ret;
}

















