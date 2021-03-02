#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"






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
		case TKN_COMMS     : return " #{}";
		case TKN_COMMENT   : return " #: ";
		case TKN_ADD       : return " +  ";
		case TKN_SUB       : return " -  ";
		case TKN_MUL       : return " *  ";
		case TKN_DIV       : return " /  ";
		case TKN_MOD       : return " %  ";
		case TKN_EXP       : return " ^  ";
		case TKN_NOT       : return " !  ";
		case TKN_AND       : return " &  ";
		case TKN_OR        : return " |  ";
		case TKN_SHL       : return " << ";
		case TKN_SHR       : return " >> ";
		case TKN_GTE       : return " >= ";
		case TKN_LSE       : return " =< ";
		case TKN_LS		   : return " <  ";
		case TKN_GT		   : return " >  ";
		case TKN_WILD      : return " _  ";
		case TKN_WHERE	   : return " @  ";
		case TKN_NEQ       : return " != ";
		case TKN_EQL       : return " =  ";
		case TKN_INT       : {
			sprintf(buffer, "INT %lu", tk.data.u64);
			return buffer;
		}break;
		case TKN_FLT       : {
			sprintf(buffer, "FLT %f", tk.data.f64);
			return buffer;
		}break;
		case TKN_STR       : {
			sprintf(buffer, "STR %s", tk.data.str.text);
			return buffer;
		}break;
		case TKN_TAG       : {
			sprintf(buffer, "TAG %s", tk.data.str.text);
			return buffer;
		}break;
		case TKN_ID        : {
			sprintf(buffer, "ID<%i>  %s", tk.data.str.len, tk.data.str.text);
			return buffer;
		}break;
		case TKN_TYID      : {
			sprintf(buffer, "TI<%i>  %s", tk.data.str.len, tk.data.str.text);
			return buffer;
		}break;
		case TKN_MID       : {
			sprintf(buffer, "~I<%i>  %s", tk.data.str.len, tk.data.str.text);
			return buffer;
		}break;
		case TKN_BID       : {
			sprintf(buffer, "#I<%i>  %s", tk.data.str.len, tk.data.str.text);
			return buffer;
		}break;
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
	if(lr->head+1 >= lr->size) return 0;

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




int lexId(LangReader* lr, char prefix, Token* tk){
	LangReader lrOld = *lr;
	
	char  c = lr->text[lr->head];
	int len = 0;
	if((prefix != 0) && (c == prefix)){
		len++;
		c = lr->text[lr->head+1];
	}else if(prefix != 0){
		return 0;
	}
	
	while(((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) && (len+lr->head < lr->size)){
		len++;
		c = lr->text[lr->head+len];
	}
	
	if(len){
		tk->data.str.text = (char*)malloc(sizeof(char) * (len+2));
		tk->data.str.len  = len;
		for(int i = 0; i < len; i++)
			tk->data.str.text[i] = lr->text[i+lr->head];
		tk->data.str.text[len] = 0;
	
		lr->head   += len;
		lr->column += len;
		tk->pos      = (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column};
		tk->type     = TKN_ID;
		return 1;
	}
	
	*lr = lrOld;
	return 0;
}






LexerState lexer(LangReader* lr){
	
	LexerState ret;
	ret.tks   = malloc(sizeof(Token) * lr->size);
	ret.tkct  = 0;
	ret.tkcap = lr->size;
	
	int cont = 1;
	while(cont){
		int  skip        =  0;
		char c           =  lr->text[lr->head];
		LangReader lrOld = *lr;
		
		lexerFork:
		
		if      ((skip < 1) &&  (c >= '0') && (c <= '9')){
			// Numbers
			Token tk;
			int pass = lexInt(lr, &tk);
			if(!pass){ *lr = lrOld; skip = 1; goto lexerFork; }
			
			LangReader lr0 = *lr;
			char cx = lexerEatChar(lr);
			if(cx == '.'){
				// Might be a float
				Token fk;
				pass = lexFltFrac(lr, &fk);
				if(!pass){ *lr = lrOld; skip = 1; goto lexerFork; }
				
				fk.data.f64 += (double)tk.data.u64;
				ret.tks[ret.tkct] = fk;
				ret.tkct++;
			}else if((cx >= 'a') && (cx <= 'z')){
				// Might have a suffix
				*lr = lr0;
			}else{
				// Just an int
				*lr = lr0;
				ret.tks[ret.tkct] = tk;
				ret.tkct++;
			}
			
		}else if((skip < 2) &&  (c >= 'a') && (c <= 'z')){
			// Common identifiers
			Token tk;
			int pass = lexId(lr, 0, &tk);
			if(!pass){ *lr = lrOld; skip = 2; goto lexerFork; }
			ret.tks[ret.tkct] = tk;
			ret.tkct++;
		}else if((skip < 3) &&  (c >= 'A') && (c <= 'Z')){
			// Type identifiers
			Token tk;
			int pass = lexId(lr, 0, &tk);
			if(!pass){ *lr = lrOld; skip = 2; goto lexerFork; }
			
			tk.type = TKN_TYID;
			ret.tks[ret.tkct] = tk;
			ret.tkct++;
		}else if((skip < 4) && ((c == '~') || (c == '#'))){
			// Special identifiers and comments
			LangReader lr0 = *lr;
			Token tk;
			char cx = lexerEatChar(lr);
			cx = lexerEatChar(lr);
			if      ((c == '#') && (cx == '{')){
				char dx = lexerEatChar(lr);
				dx      = lexerEatChar(lr);
				char ex = lexerEatChar(lr);
				while(((dx != '#') || (ex != '}')) && (ex != 0)){ printf(">%c%c\n", dx, ex); dx = ex; ex = lexerEatChar(lr); }
				tk = (Token){TKN_COMMS, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
			}else if((c == '#') && (cx == ':')){
				char dx = lexerEatChar(lr);
				dx = lexerEatChar(lr);
				while((dx != '\n') && (dx != 0)) dx = lexerEatChar(lr);
				tk = (Token){TKN_COMMENT, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
			}else{
				*lr = lr0;
				int pass = lexId(lr, c, &tk);
				if(!pass){ *lr = lrOld; skip = 2; goto lexerFork; }
			
				tk.type = (c == '~')? TKN_MID : TKN_BID;
			}
			ret.tks[ret.tkct] = tk;
			ret.tkct++;
		}else if((skip < 5) && ((c == ' ') || (c == '\n') || (c == '\v') || (c == '\t'))){
			// Whitespace
			LangReader lrOld = *lr;
			char cx = lexerEatChar(lr);
			if(cx == '\n'){
				ret.tks[ret.tkct] = (Token){TKN_NEWLINE, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
				ret.tkct++;
			}
		}else if((skip < 6) && ((c == '"') || (c == '\''))){
			// string and tag
			char cx = lexerEatChar(lr);
			cx = lexerEatChar(lr);
			while((cx != c) && (cx != 0)) cx = lexerEatChar(lr);
		}else if (skip < 7){
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
				case '>' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '>'){
						tk  = (Token){TKN_SHR    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else if(dx == '='){
						tk  = (Token){TKN_GTE    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_GT     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '<' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '<'){
						tk  = (Token){TKN_SHL    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_LS     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '!' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '='){
						tk  = (Token){TKN_NEQ    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_NOT    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '=' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '<'){
						tk  = (Token){TKN_LSE    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_EQL    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case ',' : tk = (Token){TKN_COMMA  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '+' : tk = (Token){TKN_ADD    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '*' : tk = (Token){TKN_MUL    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '/' : tk = (Token){TKN_DIV    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '%' : tk = (Token){TKN_MOD    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '^' : tk = (Token){TKN_EXP    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '&' : tk = (Token){TKN_AND    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '|' : tk = (Token){TKN_OR     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '@' : tk = (Token){TKN_WHERE  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '_' : tk = (Token){TKN_WILD   , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
			}
			ret.tks[ret.tkct] = tk;
			ret.tkct++;
		}else{
			lr->head++;
			printf("!!\n");
		}
		
		cont = lr->head+1 < lr->size;
	}
	
	return ret;
}

















