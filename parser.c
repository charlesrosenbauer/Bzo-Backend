#include "parser.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "util.h"

#include "program.h"
#include "func.h"
#include "type.h"



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
		cont = cont && (state->text[ix] != '\"');
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




/******************************

	Program Parsing

******************************/

int parseTypeUnion(TypeUnion* tu, TypeKind* k, Lisp* l){
	if(l->here.typ == LSPTYP){
		return parseTypeUnion(tu, k, l->here.val.PVAL);
	}else if(l->here.typ == OPRTYP){
		switch(l->here.val.OVAL){
			case LO_CMPD : {
				int sz   = lispSize(l);
				tu->strc = makeStruct(sz-1);
				TypeUnion* pars = tu->strc.pars;
				for(int i = 1; i < sz; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseTypeUnion(&pars[i-1], &tu->strc.kinds[i-1], &lx)) return 0;
				}
				tu->strc.parct = sz-1;
				*k = TK_STRUCT;
			}break;
			
			case LO_POLY : {
				int sz   = lispSize(l);
				tu->unon = makeUnion(sz-1);
				TypeUnion* pars = tu->unon.pars;
				for(int i = 1; i < sz; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseTypeUnion(&pars[i-1], &tu->unon.kinds[i-1], &lx)) return 0;
				}
				tu->unon.parct = sz-1;
				*k = TK_UNION;
			}break;
			
			case LO_ARRAY : {
				Lisp lx;
				lx.here        = lispIx(l, 2);
				tu->arry       = makeArray();
				*k             = TK_ARRAY;
				tu->arry.count = lispIx(l, 1).val.UVAL;
				return parseTypeUnion(tu->arry.val, &tu->arry.kind, &lx);
			}break;
			
			case LO_FNTY : {
				TypeUnion* ts = malloc(sizeof(TypeUnion) * 2);
				Lisp li, lo;
				li.here  = lispIx(l, 1);
				lo.here  = lispIx(l, 2);
				*k       = TK_FUNCTION;
				
				int iret = parseTypeUnion(&ts[0], &tu->func.ikind, &li);
				int oret = parseTypeUnion(&ts[1], &tu->func.okind, &lo);
				tu->func.ios = ts;
				
				return iret & oret;
			}break;
			
			case LO_I8  : {tu->prim = P_I8;  *k = TK_PRIMITIVE; }break;
			case LO_I16 : {tu->prim = P_I16; *k = TK_PRIMITIVE; }break;
			case LO_I32 : {tu->prim = P_I32; *k = TK_PRIMITIVE; }break;
			case LO_I64 : {tu->prim = P_I64; *k = TK_PRIMITIVE; }break;
			case LO_U8  : {tu->prim = P_U8;  *k = TK_PRIMITIVE; }break;
			case LO_U16 : {tu->prim = P_U16; *k = TK_PRIMITIVE; }break;
			case LO_U32 : {tu->prim = P_U32; *k = TK_PRIMITIVE; }break;
			case LO_U64 : {tu->prim = P_U64; *k = TK_PRIMITIVE; }break;
			case LO_F16 : {tu->prim = P_F16; *k = TK_PRIMITIVE; }break;
			case LO_F32 : {tu->prim = P_F32; *k = TK_PRIMITIVE; }break;
			case LO_F64 : {tu->prim = P_F64; *k = TK_PRIMITIVE; }break;
			case LO_Ptr : {tu->prim = P_Ptr; *k = TK_PRIMITIVE; }break;
			
			
			default: return 0;
		}
		return 1;
	}else if(l->here.typ == TYPTYP){
		tu->name.tyid = l->here.val.UVAL;
		*k            = TK_NAMED;
		return 1;
	}
	
	return 0;
}




int parseExprUnion(ExprUnion* xp, ExprKind* k, Lisp* l){
	if(l->here.typ == LSPTYP){
		return parseExprUnion(xp, k, l->here.val.PVAL);
	}else if(l->here.typ == OPRTYP){
		switch(l->here.val.UVAL){
			case LO_LET : {
				int sz          = lispSize(l);
				if (sz < 3)       return 0;
				*xp             = makeLetx(sz-3);
				xp->letx.expct  = sz-3;
				ExprUnion* exps = xp->letx.exps;
				*k = XK_LMDA;
				for(int i = 2; i < sz-1; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseExprUnion(&exps[i-2], &xp->letx.kinds[i-2], &lx)) return 0;
				}
				Lisp lp; lp.here = lispIx(l, 1);
				if(!parseExprUnion((ExprUnion*)xp->letx.patn, &xp->letx.patk, &lp)) return 0;
				Lisp lr; lr.here = lispIx(l, sz-1);
				if(!parseExprUnion((ExprUnion*)xp->letx.retn, &xp->letx.retk, &lr)) return 0;
				return 1;
			}break;
			
			case LO_EXPR : {
				int sz          = lispSize(l);
				*xp             = makeExpr(sz-1);
				ExprUnion* pars = xp->expr.pars;
				for(int i = 1; i < sz; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseExprUnion(&pars[i-1], &xp->expr.kinds[i-1], &lx)) return 0;
				}
				*k = XK_EXPR;
				return 1;
			}break;
			
			case LO_CMPD : {
				int sz          = lispSize(l);
				*xp             = makeCmpd(sz-1);
				ExprUnion* pars = xp->cmpd.pars;
				for(int i = 1; i < sz; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseExprUnion(&pars[i-1], &xp->cmpd.kinds[i-1], &lx)) return 0;
				}
				*k = XK_CMPD;
				return 1;
			}break;
			
			case LO_POLY : {
				int sz          = lispSize(l);
				*xp             = makePoly(sz-1);
				ExprUnion* pars = xp->poly.pars;
				for(int i = 1; i < sz; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseExprUnion(&pars[i-1], &xp->poly.kinds[i-1], &lx)) return 0;
				}
				*k = XK_POLY;
				return 1;
			}break;
			
			case LO_LSP : {
				int sz          = lispSize(l);
				if (sz < 3)       return 0;
				*xp             = makePrfx(sz-2);
				Lisp lxp;
				lxp.here = lispIx(l, 1);
				ExprUnion* expr = xp->prfx.expr;
				if(!parseExprUnion(expr, &xp->prfx.xkind, &lxp)) return 0;
				
				ExprUnion* pars = xp->prfx.pars;
				for(int i = 2; i < sz; i++){
					Lisp lx;
					lx.here = lispIx(l, i);
					if(!parseExprUnion(&pars[i-2], &xp->prfx.kinds[i-2], &lx)) return 0;
				}
				*k = XK_PRFX;
				return 1;
			}break;
			
			default:{
			
				if((l->here.val.UVAL >= LO_NOP) && (l->here.val.UVAL < LO_MAX_OPC)){
					xp->prim.opc = l->here.val.UVAL;
					*k           = XK_PRIMOPC;
					return 1;
				}
				printf("Unrecognized IR Object = <%li>\n", l->here.val.IVAL);
				*k = XK_VOID;
				return 0;
			}break;
		}
	}else if(l->here.typ == VARTYP){
		xp->prim.i64 = l->here.val.IVAL;
		*k = XK_PRIMVAR;
		return 1;
	}else if(l->here.typ == FNCTYP){
		xp->prim.i64 = l->here.val.IVAL;
		*k = XK_PRIMFUN;
		return 1;
	}else if(l->here.typ == INTTYP){
		xp->prim.i64 = l->here.val.IVAL;
		*k = XK_PRIMINT;
		return 1;
	}else if(l->here.typ == UNTTYP){
		xp->prim.u64 = l->here.val.UVAL;
		*k = XK_PRIMUNT;
		return 1;
	}else if(l->here.typ == FLTTYP){
		xp->prim.i64 = l->here.val.IVAL;
		*k = XK_PRIMFLT;
		return 1;
	}else if(l->here.typ == STRTYP){
		xp->prim.str = malloc(l->here.val.SVAL.size+1);
		memcpy(xp->prim.str, l->here.val.SVAL.text, l->here.val.SVAL.size);
		xp->prim.str[l->here.val.SVAL.size] = '\0';
		*k = XK_PRIMSTR;
		return 1;
	}else if(l->here.typ == HOLTYP){
		*k = XK_PRIMWLD;
		return 1;
	}else{
		printf("NOT SURE ABOUT %i TOKEN IN IR\n", (int)l->here.typ);
	}
	return 0;
}



int parseFunc(FuncDef* fn, Lisp* l){
	int fnid = lispIx(l, 1).val.IVAL;
	
	Type ti, to;
	ti       = (Type){.size=0, .align=0, .isAlias=0};
	to       = (Type){.size=0, .align=0, .isAlias=0};
	Lisp li, lo, xp;
	li.here  = lispIx(l, 2);
	lo.here  = lispIx(l, 3);
	xp.here  = lispIx(l, 4);
	if(!parseTypeUnion(&ti.type, &ti.kind, &li)) return 0;
	if(!parseTypeUnion(&to.type, &to.kind, &lo)) return 0;
	
	*fn = makeFuncDef(ti, to, 2);
	
	return parseExprUnion(&fn->defn, &fn->defkind, &xp);
}


int parseType(Type*    ty, Lisp* l){
	Lisp lx;
	lx.here = lispIx(l, 2);
	int ret   = parseTypeUnion(&ty->type, &ty->kind, &lx);
	ty->size  = 0;
	ty->align = 0;
	//printType(*ty);
	return ret;
}



Program  parseProgram(uint8_t* file, int fsize){
	Program ret;
	int defestimate = fsize / 32;
	
	ret.types = makeTypeTable(0, defestimate);
	ret.funcs = makeFuncTable(0, defestimate);
	
	ParserState ps = (ParserState){(char*)file, 0, fsize, NULL};
	
	while(ps.head < fsize){
		skipWhitespace(&ps);
		if(ps.text[ps.head] == ';'){
			parseComment(&ps);
		}else if(ps.text[ps.head] == '('){
			Lisp* l = parseLispAlt(&ps);
			if(l->here.val.OVAL == LO_DEFN){
				int fnid = lispIx(l, 1).val.IVAL;
				printf("FN%i Pars: %i\n", fnid, lispSize(l));
				resizeFnTable(&ret.funcs, fnid+1);
				
				parseFunc(&ret.funcs.funcs[fnid], l);
			}else if(l->here.val.OVAL == LO_DEFTY){
				int tyid = lispIx(l, 1).val.IVAL;
				printf("TY%i Pars: %i\n", tyid, lispSize(l));
				resizeTyTable(&ret.types, tyid+1);
				
				parseType(&ret.types.types[tyid], l);
			}else{
				printf("%i\n", l->here.val.OVAL);
			}
		}else if(ps.text[ps.head] == 0){
			ps.head = fsize;
		}else{
			printf("Unexpected character %c (%i). Skipping...\n", ps.text[ps.head], ps.text[ps.head]);
			ps.head++;
		}
	}
	return ret;
}

