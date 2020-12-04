#include "parser.h"
#include "stdlib.h"
#include "stdio.h"
#include "util.h"





int parseHex(ParserState* state, int skip, uint64_t* ret){

	int index = state->head + skip;
	int revert= index;
	uint64_t val = 0;
	while((index < state->size)){
		char c = state->text[index];
		if((c >= '0') && (c <= '9')){
			val *= 16;
			val |= (c - '0');
		}else if((c >= 'A') && (c <= 'F')){
			val *= 16;
			val |= (c - 'A') + 10;
		}else if((c >= 'a') && (c <= 'f')){
			val *= 16;
			val |= (c - 'a') + 10;
		}else{
			*ret = val;
			state->head = index-1;
			return 1;
		}
		index++;
	}
	state->head = index-1;
	*ret = val;
	return 1;
}


int parseIdentifier(ParserState* state, char x, uint64_t* ret){

	char c = state->text[state->head];
	if(c == x)	return parseHex(state, 1, ret);
	*ret = 0;
	return 0;
}


int parseVar(ParserState* state, uint64_t* ret){
	return parseIdentifier(state, 'v', ret);
}

int parseTyp(ParserState* state, uint64_t* ret){
	return parseIdentifier(state, 't', ret);
}

int parseFnc(ParserState* state, uint64_t* ret){
	return parseIdentifier(state, 'f', ret);
}

int parseInt(ParserState* state, int64_t*  ret){
	uint64_t x;
	int r = parseIdentifier(state, 'i', &x);
	*ret = *(int64_t*)(&x);
	return r;
}

int parseUnt(ParserState* state, uint64_t* ret){
	return parseIdentifier(state, 'u', ret);
}

int parseFlt(ParserState* state, double* ret){
	uint64_t x;
	int r = parseIdentifier(state, 'r', &x);
	*ret = *(double*)(&x);
	return r;
}

int parseOpc(ParserState* state, uint64_t* ret){
	return parseIdentifier(state, 'x', ret);
}

int parseBitset(ParserState* state, uint64_t* ret){
	char c = state->text[state->head];
	*ret = 0;
	if(c != 'b') return 0;
	int ix = state->head+1;
	while((state->text[ix] == '#') || (state->text[ix] == '_')){
		*ret += *ret;
		if(state->text[ix] == '#') *ret |= 1;
		ix++;
	}
	state->head = ix-1;
	return 1;
}

int parseComment(ParserState* state){
	char c = state->text[state->head];
	if(c != ';'){
		return 0;
	}
	int ix = state->head+1;
	while((state->text[ix] != '\n') && (ix < state->size)) ix++;
	state->head = ix;
	return 1;
}


int parseString(ParserState* state, String* ret){

	char c = state->text[state->head];
	if(c != '\"'){
		ret->size = 0;
		ret->text = NULL;
		return 0;
	}

	//Figure out how long the string is.
	int len = 0;
	int ix = state->head+1;
	int cont = 1;
	while(cont){
		if(state->text[ix] == '\\'){
			ix++;
		}else if(state->text[ix] == '\"'){
			len--;
			cont = 0;
		}
		len++;
		ix++;
		cont = cont && (ix < state->size);
	}

	ret->text = malloc(sizeof(char) * len);
	ix      = state->head+1;
	int tix = 0;
	cont = 1;
	while(cont){
		c = state->text[ix];
		if(c == '\\'){
			switch(state->text[ix+1]){
						case 'n' : ret->text[tix] = '\n'; break;
				case 'v' : ret->text[tix] = '\v'; break;
				case 't' : ret->text[tix] = '\t'; break;
				case 'r' : ret->text[tix] = '\r'; break;
				case '\\': ret->text[tix] = '\\'; break;
				case '\'': ret->text[tix] = '\''; break;
				case '\"': ret->text[tix] = '\"'; break;
				case '0' : ret->text[tix] = '\0'; break;
				case 'a' : ret->text[tix] = '\a'; break;
				default  : return 0;
			}
			ix++;
		}
		ret->text[tix] = state->text[ix];
		ix ++;
		tix++;
		cont = (tix < len);
		cont = cont && (state->text[ix+1] != '\"');
	}

	state->head = ix+1;
	ret->size = len;



	return 1;
}

void skipWhitespace(ParserState* state){
	while(1){
		char c = state->text[state->head];
		if((c == ' ') || (c == '\t') || (c == '\v') || (c == '\n')){
			state->head++;
		}else if(c == ';'){
			parseComment(state);
		}else{
			return;
		}
		if(state->head == state->size){
			return;
		}
	}
}



Lisp* parseLispAlt(ParserState* state){

	skipWhitespace(state);

	if(state->head == state->size) return NULL;

	if(state->text[state->head] != '('){
		printf("Error: Expected lisp head, found %c (%i) instead.\n", state->text[state->head], state->text[state->head]);
		exit(1);
	}
	state->head++;

	skipWhitespace(state);

	int nodes = 0;
	Val vals[128];
	int typs[128];

	int cont = 1;
	while(cont && (state->head < state->size)){
		if(nodes == 128){
			printf("Error: Excessively long list.");
			exit(2);
		}

		skipWhitespace(state);
		char c = state->text[state->head];
		switch(c){
			case '(':{
				//printf("(\n");
				vals[nodes].PVAL = parseLispAlt(state);
				typs[nodes]      = LSPTYP;
				nodes++;
			} break;

			case 'v':{
				uint64_t v;
				if(!parseVar(state, &v)){
					printf("Var error at %i.\n", state->head);
				}else{
					//printf("v%lu\n", v);
					vals[nodes].UVAL = v;
					typs[nodes]      = VARTYP;
					nodes++;
				}
			} break;

			case 't':{
				uint64_t v;
				if(!parseTyp(state, &v)){
					printf("Typ error at %i.\n", state->head);
				}else{
					//printf("t%lu\n", v);
					vals[nodes].UVAL = v;
					typs[nodes]      = TYPTYP;
					nodes++;
				}
			} break;

			case 'f':{
				uint64_t v;
				if(!parseFnc(state, &v)){
					printf("Fnc error at %i.\n", state->head);
				}else{
					//printf("f%lu\n", v);
					vals[nodes].UVAL = v;
					typs[nodes]      = FNCTYP;
					nodes++;
				}
			} break;

			case 'i':{
				int64_t v;
				if(!parseInt(state, &v)){
					printf("Int error at %i.\n", state->head);
				}else{
					//printf("i%li\n", v);
					vals[nodes].IVAL = v;
					typs[nodes]      = INTTYP;
					nodes++;
				}
			} break;

			case 'u':{
				uint64_t v;
				if(!parseUnt(state, &v)){
					printf("Unt error at %i.\n", state->head);
				}else{
					//printf("u%lu\n", v);
					vals[nodes].UVAL = v;
					typs[nodes]      = VARTYP;
					nodes++;
				}
			} break;

			case 'r':{
				double v;
				if(!parseFlt(state, &v)){
					printf("Flt error at %i.\n", state->head);
				}else{
					//printf("r%f\n", v);
					vals[nodes].FVAL = v;
					typs[nodes]      = FLTTYP;
					nodes++;
				}
			} break;

			case 'x':{
				uint64_t v;
				if(!parseOpc(state, &v)){
					printf("Opc error at %i.\n", state->head);
				}else{
					//printf("x%lu\n", v);
					vals[nodes].UVAL = v;
					typs[nodes]      = OPRTYP;
					nodes++;
				}
			} break;

			case 'b':{
				uint64_t b;
				if(!parseBitset(state, &b)){
					printf("Bitset error at %i.\n", state->head);
				}else{
					vals[nodes].BVAL = b;
					typs[nodes]      = BSTTYP;
					nodes++;
				}
			} break;

			case ';':{
				if(!parseComment(state)){
					printf("Comment error at %i.\n", state->head);
				}
			} break;

			case '_':{
				//printf("_\n");
				typs[nodes]      = HOLTYP;
				nodes++;
			} break;

			case '-':{
				//printf("_\n");
				typs[nodes]      = PTRTYP;
				vals[nodes].PVAL = NULL;
				nodes++;
			} break;

			case '\"':{
				String v;
				if(!parseString(state, &v)){
					printf("Str error at %i.\n", state->head);
				}else{
					//printf("s%i\n", v.size);
					vals[nodes].SVAL = v;
					typs[nodes]      = STRTYP;
					nodes++;
				}
			} break;

			case ')':{
				//printf(")\n");
				state->head--;
				cont = 0;
			} break;

			case '\0':{
				cont = 0;
			} break;

			default: printf("@%i, unexpected: %c (%i)\n", state->head, state->text[state->head], state->text[state->head]);
		}
		state->head++;
	}

	if(nodes == 0){
		return NULL;
	}

	Lisp  head;
	Lisp* lasthead =  NULL;
	Lisp* tapehead = &head;
	for(int i = 0; i < nodes; i++){
		Valobj val;
		val.val = vals[i];
		val.typ = typs[i];
		lasthead = tapehead;
		tapehead = malloc(sizeof(Lisp));
		lasthead->next = tapehead;
		tapehead->refc = 1;
		tapehead->here = val;
		tapehead->next = NULL;
	}
	state->head++;

	return head.next;
}



void printVal(Valobj val){
	switch(val.typ){
		case FNCTYP : printf("f%lu ", val.val.UVAL     ); break;
		case INTTYP : printf("i%li ", val.val.IVAL     ); break;
		case UNTTYP : printf("u%lu ", val.val.UVAL     ); break;
		case FLTTYP : printf("r%f ",  val.val.FVAL     ); break;
		case STRTYP : printf("s%i ",  val.val.SVAL.size); break;
		case LSPTYP : printLisp(val.val.PVAL           ); break;
		case VARTYP : printf("v%lu ", val.val.UVAL     ); break;
		case OPRTYP : printf("x%lu ", val.val.UVAL     ); break;
		case TYPTYP : printf("t%lu ", val.val.UVAL     ); break;
		case ARRTYP : {
			Arr array = *(Arr*)val.val.PVAL;
			printf("[ARR<%i, %i>\n", array.type, array.size);
			Valobj* data = array.data;
			for(int i = 0; i < array.size; i++){
				printf("  ");
				printVal(data[i]);
				printf("\n");
			}
			printf("]\n");
		}break;
		case BSTTYP : {
			printf("b");
			for(int i = (64 - __builtin_clz(val.val.BVAL))-1; i >= 0; i--)
				printf((val.val.BVAL & (1l << i))? "#" : "_");
			printf(" ");
		}break;
		default:      printf("%i "  , val.typ          ); break;
	}
}



int parseFunction(Lisp* l, FnDef* fns, int fnlimit){
	if(lispSize(l) != 9) return -1;

	Valobj fnid  = lispIx(l, 1);
	if(fnid .typ != FNCTYP) return 1;
	 
	Valobj tprct = lispIx(l, 2);
	if(tprct.typ != INTTYP) return 2;
	
	Valobj parct = lispIx(l, 3);
	if(parct.typ != INTTYP) return 3;
	
	Valobj retct = lispIx(l, 4);
	if(retct.typ != INTTYP) return 4;
	
	Valobj tprs  = lispIx(l, 5);
	if(tprs .typ != LSPTYP) return 5;
	
	Valobj pars  = lispIx(l, 6);
	if(pars .typ != LSPTYP) return 6;
	
	Valobj rets  = lispIx(l, 7);
	if(rets .typ != LSPTYP) return 7;
	
	Valobj expr  = lispIx(l, 8);
	if(expr .typ != LSPTYP) return 8;
	
	if(fnid.val.UVAL > fnlimit) return -2;
	FnDef* ret = &fns[fnid.val.UVAL];
	
	ret->tprsSource = tprs.val.PVAL;
	ret->parsSource = pars.val.PVAL;
	ret->retsSource = rets.val.PVAL;
	ret->codeSource = expr.val.PVAL;
	ret->fnid       = fnid.val.UVAL;
	
	ret->tpct = tprct.val.UVAL;
	ret->prct = parct.val.UVAL;
	ret->rtct = retct.val.UVAL;
	
	if(ret->tpct != lispSize(ret->tprsSource)) return -3;
	if(ret->prct != lispSize(ret->parsSource)) return -4;
	if(ret->rtct != lispSize(ret->retsSource)) return -5;
	
	return 0;
}


int parseType(Lisp* l, TyDef* tys, int tylimit){
	if(lispSize(l) != 5) return -1;
	
	Valobj tyid  = lispIx(l, 1);
	if(tyid .typ != TYPTYP) return 1;
	
	Valobj tprct = lispIx(l, 2);
	if(tprct.typ != INTTYP) return 2;
	
	Valobj tpars = lispIx(l, 3);
	if(tpars.typ != LSPTYP) return 3;
	
	Valobj tdef  = lispIx(l, 4);
	if(tdef .typ != LSPTYP) return 4;
	
	if(tyid.val.UVAL > tylimit) return -2;
	TyDef* ret = &tys[tyid.val.UVAL];
	
	ret->parSource = tpars.val.PVAL;
	ret->defSource = tdef .val.PVAL;
	ret->parct     = tprct.val.UVAL;
	ret->tyid      = tyid .val.UVAL;
	
	if(ret->parct != lispSize(ret->parSource)) return -3;
	
	ret->size      = -1;
	ret->align     = -1;
	
	return 0;
}


int parseClass(Lisp* l, TCDef* tcs, int tclimit){
	// We're not going to handle type classes yet. This is a placeholder function.
	return 0;
}


int parseImpl(Lisp* l, TCDef* tcs, TyDef* tys, int tclimit, int tylimit){
	// We're not going to handle type impls yet. This is a placeholder function.
	return 0;
}



Program* parseProgram(ParserState* state, int fnlimit, int tylimit, int tclimit, int imlimit){
	Program* ret = malloc(sizeof(Program   ));
	ret->funcs   = malloc(sizeof(FnDef) * fnlimit);
	ret->fnct    = 0;
	ret->types   = malloc(sizeof(TyDef) * tylimit);
	ret->tyct    = 0;
	ret->classes = malloc(sizeof(TCDef) * tclimit);
	ret->tcct    = 0;
	ret->impls   = malloc(sizeof(ImDef) * imlimit);
	ret->imct    = 0;

	while((state->head != state->size) && (state->text[state->head] != '\0')){
		Lisp* l = parseLispAlt(state);

		if(l != NULL){
			int def = lispIx(l, 0).val.UVAL;
			if      (def == DEFN){
				// make function
				int fnErr = parseFunction(l, &ret->funcs[ret->fnct], fnlimit);
				if(fnErr){ printf("Unable to parse function. Error: %i\n", fnErr); return NULL; }
				ret->fnct++;
			}else if(def == DEFTY){
				// make type
				int tyErr = parseType(l, &ret->types[ret->tyct], tylimit);
				if(tyErr){ printf("Unable to parse type. Error: %i\n", tyErr); return NULL; }
				ret->tyct++;
			}else if(def == DEFTC){
				// make typeclass
				ret->tcct++;
			}else{
				printf("Unrecognized definition with symbol: %i\n", def);
			}
		}
	}
	return ret;
}



void printFunction(FnDef* fn){
	printf("FUNC %i <%i> (%i) -> (%i)\n", fn->fnid, fn->tpct, fn->prct, fn->rtct);
}


void printType(TyDef* ty){
	printf("TYPE %i <%i> [%i bytes, %i align]\n", ty->tyid, ty->parct, ty->size, ty->align);
}


void printTyClass(TCDef* tc){
	printf("CLSS %i\n", tc->tcid);
}


void printImpl(ImDef* im){
	printf("IMPL %i\n", im->imid);
}
