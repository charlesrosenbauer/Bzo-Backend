#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "common.h"



int eqStrToken(StrToken a, StrToken b){
	if(a.len != b.len) return 0;
	for(int i = 0; i < a.len; i++)
		if(a.text[i] != b.text[i]) return 0;
	return 1;
}



char* printToken(Token tk, char* buffer){
	switch(tk.type){
		case TKN_VOID      : return " XX ";
		case TKN_PERIOD    : return " .  ";
		case TKN_DEFINE    : return " :: ";
		case TKN_COLON     : return " :  ";
		case TKN_SEMICOLON : return " ;  ";
		case TKN_COMMA     : return " ,  ";
		case TKN_NEWLINE   : return " \\n ";
		case TKN_L_ARROW   : return " <- ";
		case TKN_R_ARROW   : return " -> ";
		case TKN_L_DARROW  : return " <= ";
		case TKN_R_DARROW  : return " => ";
		case TKN_BRK_OPN   : return " [  ";
		case TKN_BRK_END   : return " ]  ";
		case TKN_PAR_OPN   : return " (  ";
		case TKN_PAR_END   : return " )  ";
		case TKN_BRC_OPN   : return " {  ";
		case TKN_BRC_END   : return " }  ";
		case TKN_COMMS     : return " #{}";
		case TKN_COMMENT   : return " #: ";
		case TKN_ASSIGN	   : return " := ";
		case TKN_CONSTRAIN : return " |: ";
		case TKN_FNTY	   : return " |> ";
		case TKN_PIPE      : return " \\  ";
		case TKN_WAT       : return " ?  ";
		case TKN_NWAT      : return " !? ";
		case TKN_ADD       : return " +  ";
		case TKN_SUB       : return " -  ";
		case TKN_MUL       : return " *  ";
		case TKN_DIV       : return " /  ";
		case TKN_MOD       : return " %  ";
		case TKN_EXP       : return " ^  ";
		case TKN_NOT       : return " !  ";
		case TKN_AND       : return " &  ";
		case TKN_OR        : return " |  ";
		case TKN_XOR       : return " ^^ ";
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
		case TKN_TVAR      : {
			sprintf(buffer, "TV<%i>  %s", tk.data.str.len, tk.data.str.text);
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
		case TKN_S_ID        : {
			sprintf(buffer, "ID#%lu", tk.data.u64);
			return buffer;
		}break;
		case TKN_S_TYID      : {
			sprintf(buffer, "TI#%lu", tk.data.u64);
			return buffer;
		}break;
		case TKN_S_TVAR      : {
			sprintf(buffer, "TV#%lu", tk.data.u64);
			return buffer;
		}break;
		case TKN_S_MID       : {
			sprintf(buffer, "~I#%lu", tk.data.u64);
			return buffer;
		}break;
		case TKN_S_BID       : {
			sprintf(buffer, "#I#%lu", tk.data.u64);
			return buffer;
		}break;
		
		case TKN_LOCID       : {
			sprintf(buffer, "@I%lu-%d", tk.data.loc.path[0], tk.data.loc.len);
			return buffer;
		}break;
		
		case TKN_LOCTY       : {
			sprintf(buffer, "@T%lu-%d", tk.data.loc.path[0], tk.data.loc.len);
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
		lr->column = 1;
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
	while(((c >= '0') && (c <= '9')) || (c == '_')){
		if(c != '_'){
			x *= 10;
			x += c - '0';
			ret = 1;
		}
		lr->head++;
		lr->column++;
		c = lr->text[lr->head];
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
	while(((c >= '0') && (c <= '9')) || (c == '_')){
		if(c != '_'){
			x  *= 10;
			sc *= 10;
			x  += c - '0';
			ret = 1;
		}
		lr->head++;
		lr->column++;
		c = lr->text[lr->head];
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


int lexSpace(LangReader* lr, Token* tk){
	LangReader lrOld = *lr;
	
	char c = lr->text[lr->head];
	while((c <= ' ') && c){
		c = lexerEatChar(lr);
	}
	if(lr->head < (lr->size-1)){
		lr->head  --;
		lr->column--;
	}
	if(lrOld.line != lr->line){
		tk->pos      = (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column};
		tk->type     = TKN_NEWLINE;
		return 1;
	}
	
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
	
	int apostraphes = 0;
	while(((c >= '0') && (c <= '9')) || (c == '_') || (c == '\'') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) && (len+lr->head < lr->size)){
		len++;
		c = lr->text[lr->head+len];
		apostraphes += (c == '\'');
	}
	
	TkType type;
	len  = (apostraphes >  1)? 0        : len;
	type = (apostraphes == 1)? TKN_TVAR : TKN_ID;
	
	if(len){
		tk->data.str.text = (char*)malloc(sizeof(char) * (len+2));
		tk->data.str.len  = len;
		for(int i = 0; i < len; i++)
			tk->data.str.text[i] = lr->text[i+lr->head];
		tk->data.str.text[len] = 0;
	
		lr->head   += len;
		lr->column += len;
		tk->pos      = (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column};
		tk->type     = type;
		return 1;
	}
	
	*lr = lrOld;
	return 0;
}



int lexString(LangReader* lr, char wrap, Token* ret){
	LangReader lrOld = *lr;

	char c = lr->text[lr->head];
	if(c != wrap) return 0;

	//Figure out how long the string is.
	int len = 0;
	int ix = lr->head+1;
	int cont = 1;
	while(cont){
		if(lr->text[ix] == '\\'){
			ix++;
		}else if(lr->text[ix] == wrap){
			len--;
			cont = 0;
		}
		len++;
		ix++;
		cont = cont && (ix < lr->size);
	}
	if(ix == lr->size) return 0;

	ret->data.str.text = malloc(sizeof(char) * (len+1));
	ret->data.str.text[len] = '\0';
	int tix = 0;
	cont    = 1;
	lexerEatChar(lr);	// Eat first wrap character
	while(cont){
		c = lexerEatChar(lr);
		if(c == wrap){
			tix++;
			break;
		}else if(c == '\\'){
			c = lexerEatChar(lr);
			switch(c){
				case 'n' : ret->data.str.text[tix] = '\n'; break;
				case '\n': ret->data.str.text[tix] = '\n'; break;
				case 'v' : ret->data.str.text[tix] = '\v'; break;
				case 't' : ret->data.str.text[tix] = '\t'; break;
				case 'r' : ret->data.str.text[tix] = '\r'; break;
				case '\\': ret->data.str.text[tix] = '\\'; break;
				case '\'': ret->data.str.text[tix] = '\''; break;
				case '"' : ret->data.str.text[tix] =  '"'; break;
				case '0' : ret->data.str.text[tix] = '\0'; break;
				case 'a' : ret->data.str.text[tix] = '\a'; break;
				default  : {*lr = lrOld; return 0;}
			}
			c = ~wrap;
		}else{
			ret->data.str.text[tix] = c;
		}
		tix++;
		cont = (tix < len) && (c != wrap);
	}
	
	// This line might be necessary in some cases, but valgrind says there's an off-by-one bug here
	//ret->data.str.text[tix] = '\0';
	ret->pos.fileId    = lrOld.fileId;
	ret->pos.lineStart = lrOld.line;
	ret->pos.colStart  = lrOld.column;
	ret->pos.lineEnd   = lr->line;
	ret->pos.colEnd    = lr->column;

	lr->head = ix;
	ret->type = (wrap == '"')? TKN_STR : TKN_TAG;
	ret->data.str.len = len;
	return 1;
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
				while(((dx != '#') || (ex != '}')) && (ex != 0)){ dx = ex; ex = lexerEatChar(lr); }
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
			Token tk;
			if(lexSpace(lr, &tk)){
				ret.tks[ret.tkct] = tk;
				ret.tkct++;
			}
		}else if((skip < 6) && ((c == '"') || (c == '\''))){
			// string and tag
			Token tk;
			if(lexString(lr, c, &tk)){
				ret.tks[ret.tkct] = tk;
				ret.tkct++;
			}
		}else if (skip < 7){
			// Symbols
			LangReader lrOld = *lr;
			char  cx = lexerEatChar(lr);
			Token tk;
			switch(cx){
				case '[' : tk = (Token){TKN_BRK_OPN  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ']' : tk = (Token){TKN_BRK_END  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '(' : tk = (Token){TKN_PAR_OPN  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ')' : tk = (Token){TKN_PAR_END  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '{' : tk = (Token){TKN_BRC_OPN  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '}' : tk = (Token){TKN_BRC_END  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '.' : tk = (Token){TKN_PERIOD   , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ';' : tk = (Token){TKN_SEMICOLON, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ':' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == ':'){
						tk  = (Token){TKN_DEFINE , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else if(dx == '='){
						tk  = (Token){TKN_ASSIGN , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
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
				case '^' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '^'){
						tk  = (Token){TKN_XOR    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_EXP    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
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
						tk  = (Token){TKN_SHL     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else if(dx == '='){
						tk  = (Token){TKN_L_DARROW, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_LS      , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '!' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '='){
						tk  = (Token){TKN_NEQ    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else if(dx == '?'){
						tk  = (Token){TKN_NWAT   , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_NOT    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '=' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == '<'){
						tk  = (Token){TKN_LSE     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else if(dx == '>'){
						tk  = (Token){TKN_R_DARROW, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else{
						*lr = lr0;
						tk  = (Token){TKN_EQL     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '|' :{
					LangReader lr0 = *lr;
					char dx = lexerEatChar(lr);
					if(dx == ':'){
						tk  = (Token){TKN_CONSTRAIN, (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};	
					}else if(dx == '>'){
						tk  = (Token){TKN_FNTY     , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}else{
						*lr = lr0;
						tk  = (Token){TKN_OR       , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}};
					}
				}break;
				case '\\': tk = (Token){TKN_PIPE   , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '?' : tk = (Token){TKN_WAT    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case ',' : tk = (Token){TKN_COMMA  , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '+' : tk = (Token){TKN_ADD    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '*' : tk = (Token){TKN_MUL    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '/' : tk = (Token){TKN_DIV    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '%' : tk = (Token){TKN_MOD    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
				case '&' : tk = (Token){TKN_AND    , (Position){lr->fileId, lrOld.line, lr->line, lrOld.column, lr->column}}; break;
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


int builtinId(char* b){
	
	if(b[1] == 'B'){
		if(!strcmp(b, "#Bool"      )) return BID_BOOL;
	}else if(b[1] == 'F'){
		if(!strcmp(b, "#F16"       )) return BID_F16;
		if(!strcmp(b, "#F32"       )) return BID_F32;
		if(!strcmp(b, "#F64"       )) return BID_F64;
	}else if(b[1] == 'i'){
		if(!strcmp(b, "#import"    )) return BID_IMPORT;	
	}else if(b[1] == 'I'){
		if(!strcmp(b, "#I8"        )) return BID_I8;
		if(!strcmp(b, "#I16"       )) return BID_I16;
		if(!strcmp(b, "#I32"       )) return BID_I32;
		if(!strcmp(b, "#I64"       )) return BID_I64;
		if(!strcmp(b, "#I128"      )) return BID_I128;
		if(!strcmp(b, "#I256"      )) return BID_I256;
	}else if(b[1] == 'm'){
		if(!strcmp(b, "#module"    )) return BID_MODULE;
	}else if(b[1] == 'p'){
		if(!strcmp(b, "#popcount32")) return BID_POPCOUNT32;
		if(!strcmp(b, "#popcount64")) return BID_POPCOUNT64;
	}else if(b[1] == 'P'){
		if(!strcmp(b, "#P8"        )) return BID_P8;
		if(!strcmp(b, "#P16"       )) return BID_P16;
		if(!strcmp(b, "#P32"       )) return BID_P32;
		if(!strcmp(b, "#P64"       )) return BID_P64;
	}else if(b[1] == 'Q'){
		if(!strcmp(b, "#Q8"        )) return BID_Q8;
		if(!strcmp(b, "#Q16"       )) return BID_Q16;
		if(!strcmp(b, "#Q32"       )) return BID_Q32;
		if(!strcmp(b, "#Q64"       )) return BID_Q64;
	}else if(b[1] == 'U'){
		if(!strcmp(b, "#U8"        )) return BID_U8;
		if(!strcmp(b, "#U16"       )) return BID_U16;
		if(!strcmp(b, "#U32"       )) return BID_U32;
		if(!strcmp(b, "#U64"       )) return BID_U64;
		if(!strcmp(b, "#U128"      )) return BID_U128;
		if(!strcmp(b, "#U256"      )) return BID_U256;
	}
	
	return -1;
}




void symbolizeTokens(SymbolTable* tab, LexerState* ls){
	for(int i = 0; i < ls->tkct; i++){
		Token* tk = &ls->tks[i];
		if(tk->type == TKN_ID){
			int id = insertSymbolText(tab, tk->data.str.text, SF_ID  );
			tk->data.u64 = id;
			tk->type = TKN_S_ID;
		}else if(tk->type == TKN_MID){
			int id = insertSymbolText(tab, tk->data.str.text, SF_MID );
			tk->data.u64 = id;
			tk->type = TKN_S_MID;
		}else if(tk->type == TKN_TYID){
			int id = insertSymbolText(tab, tk->data.str.text, SF_TYID);
			tk->data.u64 = id;
			tk->type = TKN_S_TYID;
		}else if(tk->type == TKN_TVAR){
			int id = insertSymbolText(tab, tk->data.str.text, SF_TVAR);
			tk->data.u64 = id;
			tk->type = TKN_S_TVAR;
		}else if(tk->type == TKN_BID){
			int id = builtinId(tk->data.str.text);
			tk->data.u64 = id;
			tk->type = TKN_S_BID;
		}
	}
}



