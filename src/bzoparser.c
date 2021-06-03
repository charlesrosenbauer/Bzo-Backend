#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "program.h"




/*
	Type Parsing Rules
*/
/*
// Recursion Headers
int parseType(ASTLine*, ErrorList*, ASTType*);

// TyElem	= ^		TyElem
//			| []	TyElem
//			| [Int]	TyElem
//			| TyId
//			| Id
int parseTyElem(ASTLine* line, ErrorList* errs, ASTTypeElem* ret){
	// Parse TyElem
	ASTLine ln = copyNoComms(line);
	printf("parseElem     | ");
	printASTLine(ln);
	
	int skip = 0;
	if(0){
		fail:
		free(ln.lst);
		printf("parseElem     > fail\n");
		return 0;
	}
	
	if(0){
		pass:
		free(ln.lst);
		printf("parseElem     > pass\n");
		return skip;
	}
	
	if(ln.size < 1) goto fail;
	ret->arrs  = malloc(sizeof(int) * ln.size);
	ret->pos   = ln.lst[0].pos;
	for(int i  = 0; i < ln.size; i++){
		ASTList* sub = ln.lst[i].here;
		if      ((ln.lst[i].kind == AL_TKN) &&  (ln.lst[i].tk.type == TKN_EXP)){
			ret->arrs[i] = -1;
			skip         =  i+1;
		}else if((ln.lst[i].kind == AL_BRK) &&  (sub == NULL)){
			ret->arrs[i] =  0;
			skip         =  i+1;
		}else if((ln.lst[i].kind == AL_BRK) &&  (sub != NULL) && (sub->kind == AL_TKN) && (sub->tk.type == TKN_INT) && (sub->next == NULL)){
			ret->arrs[i] =  sub->tk.data.i64;
			skip         =  i+1;
		}else if((ln.lst[i].kind == AL_TKN) && ((ln.lst[i].tk.type == TKN_S_ID) || (ln.lst[i].tk.type == TKN_S_TYID))){
			ret->arct = i;
			ret->tyid = ln.lst[i].tk.data.i64;
			skip      = i+1;
			goto pass;
		}else{
			appendError(errs, (Error){ERR_P_BAD_TYPE, ln.lst[i].pos});
			goto fail;
		}
	}
	
	skip = ln.size;
	goto pass;
}

// FTPars	= [TyElem, ... ]
//			| ()
int parseFTPars(ASTLine* ln, ErrorList* errs, ASTFTPars* ret){
	if(ln->size < 1) return 0;
	if((ln->lst[0].kind == AL_PAR) && (ln->lst[0].here == NULL)){	// ()
		ret->pos  = ln->lst[0].pos;
		ret->pars = NULL;
		ret->prct = 0;
		return 1;
	}
	if((ln->lst[0].kind == AL_BRK) && (ln->lst[0].here != NULL)){
		// TODO: get sub, remove newlines, split on commas, parseTyElems
		return 1;
	}
	return 0;
}

// FnType	= FTPars -> FTPars
//			| FTPars -> TyElem
//			| FTPars => FTPars -> FTPars
//			| FTPars => FTPars -> TyElem
int parseFnType(ASTLine* ln, ErrorList* errs, ASTFuncType* ret){
	int  skip = 0;
	ASTLine l = copyLine(ln);
	printf("parseFnType   | ");
	printASTLine(l);
	
	if(0){
		fail:
		free(l.lst);
		printf("parseFnType   > fail\n");
		return 0;
	}
	
	if(0){
		pass:
		free(l.lst);
		printf("parseFnType   > pass\n");
		return skip;
	}

	if(ln->size >= 5){
		// Polymorphism
		ASTFTPars a, b, c;
		ASTLine line;
		viewAt(&l, &line, 0);
		int passA = parseFTPars(&line, errs, &a);
		if(!passA) goto fail;
		viewAt(&l, &line, 2);
		int passB = parseFTPars(&line, errs, &b);
		if(!passB) goto fail;
		viewAt(&l, &line, 4);
		int passC = parseFTPars(&line, errs, &c);
		if(!passC) goto fail;
		if((l.lst[1].kind != AL_TKN) || (l.lst[1].tk.type != TKN_R_DARROW) ||
		   (l.lst[3].kind != AL_TKN) || (l.lst[3].tk.type != TKN_R_ARROW)) goto fail;
		   
		// TODO: link a, b, and c to ret
		skip = 5;
		goto pass;
	}
	if(ln->size >= 3){
		// No polymorphism
		ASTFTPars a, b, c;
		ASTLine line;
		viewAt(&l, &line, 0);
		int passA = parseFTPars(&line, errs, &a);
		if(!passA) goto fail;
		viewAt(&l, &line, 2);
		int passB = parseFTPars(&line, errs, &b);
		if(!passB) goto fail;
		if((l.lst[1].kind != AL_TKN) || (l.lst[1].tk.type != TKN_R_ARROW)) goto fail;
		
		// TODO: link a and b to ret
		skip = 3;
		goto pass;
	}
	goto fail;
}


// StLn		= Id : Type
int parseStLn(ASTLine* ln, ErrorList* errs, int* id, ASTType* type){
	ASTLine l = *ln;
	printf("parseStLn     | ");
	printASTLine(l);
	int skip = 0;
	if(0){
		fail:
		printf("parseStLn     > fail\n");
		return 0;
	}
	if(0){
		pass:
		printf("parseStLn     > pass\n");
		return skip;
	}
	if(ln->size < 3) goto fail;
	TkType patn[] = {TKN_S_ID, TKN_COLON};
	if      (tokenMatch(ln, patn, 2)){
		ASTLine at;
		viewAt(ln, &at, 2);
		*id  =  ln->lst[0].tk.data.i64;
		skip = parseType(&at, errs, type);
		if(skip) goto pass;
		goto fail;
	}

	goto fail;
}


// Struct	= [ StLn ; ... ]
int parseStruct(ASTLine* ln, ErrorList* errs, ASTStruct* ret){
	ASTLine l = *ln;
	printf("parseStruct   | ");
	l = copyLine(&l);
	if(0){
		fastfail:
		free(l.lst);
		printf("parseStruct   > fail\n");
		return 0;
	}
	if(0){
		fail:
		free(ret->vals);
		free(ret->labels);
		free(l.lst);
		printf("parseStruct   > fail\n");
		return 0;
	}
	if(0){
		pass:
		free(l.lst);
		printf("parseStruct   > pass\n");
		return 1;
	}
	
	
	if(l.size < 1             ) goto fastfail;
	if(l.lst[0].kind != AL_BRK) goto fastfail;
	free(l.lst);
	l = toLine(ln->lst[0].here);
	cleanLines(&l);
	printASTLine( l);
	
	ret->vals     = malloc(sizeof(ASTType) * l.size / 2);
	ret->labels   = malloc(sizeof(int)     * l.size / 2);
	ret->valct    = 0;
	
	int cont      = 1;
	ASTLine     a = l, b;
	ASTType* vals = ret->vals;
	while(cont){
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		if(a.size > 0){
			int skip = parseStLn(&a, errs, &ret->labels[ret->valct], &vals[ret->valct]);
			if(!skip) goto fail;
			ret->valct++;
		}
		a = b;
	}
	
	goto pass;
}

// Union	= ( StLn ; ... )
int parseUnion(ASTLine* ln, ErrorList* errs, ASTUnion* ret){
	ASTLine l = *ln;
	printf("parseUnion    | ");
	l = copyLine(&l);
	if(0){
		fastfail:
		free(l.lst);
		printf("parseUnion    > fail\n");
		return 0;
	}
	if(0){
		fail:
		free(ret->vals);
		free(ret->labels);
		free(l.lst);
		printf("parseUnion    > fail\n");
		return 0;
	}
	if(0){
		pass:
		free(l.lst);
		printf("parseUnion    > pass\n");
		return 1;
	}
	
	
	if(l.size < 1             ) goto fastfail;
	if(l.lst[0].kind != AL_PAR) goto fastfail;
	free(l.lst);
	l = toLine(ln->lst[0].here);
	cleanLines(&l);
	printASTLine( l);
	
	ret->vals     = malloc(sizeof(ASTType) * l.size / 2);
	ret->labels   = malloc(sizeof(int)     * l.size / 2);
	ret->valct    = 0;
	
	int cont      = 1;
	ASTLine     a = l, b;
	ASTType* vals = ret->vals;
	while(cont){
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		if(a.size > 0){
			int skip = parseStLn(&a, errs, &ret->labels[ret->valct], &vals[ret->valct]);
			if(!skip) goto fail;
			ret->valct++;
		}
		a = b;
	}
	
	goto pass;
}


// EnLn		= Id =  Int
//			| Id = -Int
int parseEnLn(ASTLine* ln, ErrorList* errs, int* id, int64_t* tag){
	ASTLine l = *ln;
	printf("parseEnLn     | ");
	printASTLine(l);
	int skip = 0;
	if(0){
		fail:
		printf("parseEnLn     > fail\n");
		return 0;
	}
	if(0){
		pass:
		printf("parseEnLn     > pass\n");
		return skip;
	}
	if(ln->size < 3) goto fail;
	TkType patA[] = {TKN_S_ID, TKN_EQL, TKN_SUB, TKN_INT};
	TkType patB[] = {TKN_S_ID, TKN_EQL, TKN_INT};
	if      (tokenMatch(ln, patA, 4)){
		*id  =  ln->lst[0].tk.data.i64;
		*tag = -ln->lst[3].tk.data.i64;
		skip =  4;
		goto pass;
	}else if(tokenMatch(ln, patB, 3)){
		*id  =  ln->lst[0].tk.data.i64;
		*tag =  ln->lst[2].tk.data.i64;
		skip =  3;
		goto pass;
	}

	goto fail;
}


// Enum		= ( TId : EnLn ; ... )
int parseEnum(ASTLine* ln, ErrorList* errs, ASTEnum* ret){
	ASTLine l = *ln;
	printf("parseEnum     | ");
	l = copyLine(&l);
	if(0){
		fastfail:
		free(l.lst);
		printf("parseEnum     > fail\n");
		return 0;
	}
	if(0){
		fail:
		free(ret->vals);
		free(ret->tags);
		free(l.lst);
		printf("parseEnum     > fail\n");
		return 0;
	}
	if(0){
		pass:
		free(l.lst);
		printf("parseEnum     > pass\n");
		return 1;
	}
	
	
	if(l.size < 1             ) goto fastfail;
	if(l.lst[0].kind != AL_PAR) goto fastfail;
	free(l.lst);
	l = toLine(ln->lst[0].here);
	cleanLines(&l);
	if(l.size < 3) goto fastfail;
	TkType pat[] = {TKN_S_TYID, TKN_COLON};
	if(!tokenMatch(&l, pat, 2)) goto fastfail;
	printASTLine( l);
	
	ret->type = l.lst[0].tk.data.i64;
	
	ret->vals     = malloc(sizeof(int    ) * l.size / 2);
	ret->tags     = malloc(sizeof(int64_t) * l.size / 2);
	ret->valct    = 0;
	
	int cont      = 1;
	ASTLine     a, b;
	viewAt(&l, &a, 2);
	while(cont){
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		if(a.size > 0){
			int skip = parseEnLn(&a, errs, &ret->vals[ret->valct], &ret->tags[ret->valct]);
			if(!skip) goto fail;
			ret->valct++;
		}
		a = b;
	}
	
	goto pass;
}


// BId		= BId
int parseBuiltin (ASTLine* ln, ErrorList* errs, ASTBuiltin* ret){
	ASTLine l = *ln;
	printf("parseBuiltin  | ");
	printASTLine(l);
	
	if((ln->size > 0) && (ln->lst[0].kind == AL_TKN) && (ln->lst[0].tk.type == TKN_S_BID)){
		ret->bid = ln->lst[0].tk.data.i64;
		ret->pos = ln->lst[0].tk.pos;
		printf("parseBuiltin  > pass\n");
		return 1;
	}
	printf("parseBuiltin  > fail\n");
	return 0;
}

// UnLn		=  Int = TId : Type
//			| -Int = TId : Type
int parseUnLn(ASTLine* ln, ErrorList* errs, int64_t* tag, int* tid, ASTType* type){
	ASTLine l = *ln;
	printf("parseUnLn     | ");
	printASTLine(l);
	int skip = 0;
	if(0){
		fail:
		printf("parseUnLn     > fail\n");
		return 0;
	}
	if(0){
		pass:
		printf("parseUnLn     > pass\n");
		return skip;
	}
	if(ln->size < 5) goto fail;
	TkType patA[] = {         TKN_INT, TKN_EQL, TKN_S_TYID, TKN_COLON};
	TkType patB[] = {TKN_SUB, TKN_INT, TKN_EQL, TKN_S_TYID, TKN_COLON};
	if      (tokenMatch(ln, patA, 4)){
		*tag =  ln->lst[0].tk.data.i64;
		*tid =  ln->lst[2].tk.data.i64;
		ASTLine l; viewAt(ln, &l, 4);
		skip = parseType(&l, errs, type);
		if(skip) goto pass;
		goto fail;
	}else if(tokenMatch(ln, patB, 5)){
		*tag = -ln->lst[1].tk.data.i64;
		*tid =  ln->lst[3].tk.data.i64;
		ASTLine l; viewAt(ln, &l, 5);
		skip = parseType(&l, errs, type);
		if(skip) goto pass;
		goto fail;
	}
	

	goto fail;
}


// TagUnion = (Id TyId : UnLn ; ... )
int parseTagUnion(ASTLine* ln, ErrorList* errs, ASTTagUnion* ret){
	ASTLine l = *ln;
	printf("parseTagUnion | ");
	l = copyLine(&l);
	if(0){
		fastfail:
		free(l.lst);
		printf("parseTagUnion > fail\n");
		return 0;
	}
	if(0){
		fail:
		free(ret->vals);
		free(ret->tags);
		free(ret->labels);
		free(l.lst);
		printf("parseTagUnion > fail\n");
		return 0;
	}
	if(0){
		pass:
		free(l.lst);
		printf("parseTagUnion > pass\n");
		return 1;
	}
	
	
	if(l.size < 1             ) goto fastfail;
	if(l.lst[0].kind != AL_PAR) goto fastfail;
	free(l.lst);
	l = toLine(ln->lst[0].here);
	cleanLines(&l);
	if(l.size < 4) goto fastfail;
	TkType pat[] = {TKN_S_ID, TKN_S_TYID, TKN_COLON};
	if(!tokenMatch(&l, pat, 3)) goto fastfail;
	printASTLine( l);
	
	ret->tagname = l.lst[0].tk.data.i64;
	ret->tagtype = l.lst[1].tk.data.i64;
	
	ret->labels   = malloc(sizeof(int    ) * l.size / 2);
	ret->tags     = malloc(sizeof(int64_t) * l.size / 2);
	ret->vals     = malloc(sizeof(ASTType) * l.size / 2);
	ret->valct    = 0;
	
	ASTType* vals = ret->vals;
	
	int cont      = 1;
	ASTLine     a, b;
	viewAt(&l, &a, 3);
	while(cont){
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		if(a.size > 0){
			int skip = parseUnLn(&a, errs, &ret->tags[ret->valct], &ret->labels[ret->valct], &vals[ret->valct]);
			if(!skip) goto fail;
			ret->valct++;
		}
		a = b;
	}
	
	goto pass;
}


// TySet	= TId  = TId | ...


// Type		= TyElem
//			| Struct
//			| Union
//			| TagUnion
//			| FnType
//			| Enum
//			| BId
int parseType(ASTLine* ln, ErrorList* errs, ASTType* ret){
	printf("parseType     | ");
	ASTLine line = *ln;
	printASTLine(line);
	
	int skip = 0;
	if(0){
		pass:
		printf("parseType     > pass\n");
		return skip;
	}
	
	ASTFuncType    fnty;
	skip = parseFnType  (ln, errs, &fnty);  // Parse Function Type
	if(skip){ ret->kind = TT_FUNC; ret->type.func = fnty; goto pass; }
	
	ASTTypeElem    elem;
	skip = parseTyElem  (ln, errs, &elem);	// Parse TyElem
	if(skip){ ret->kind = TT_ELEM; ret->type.elem = elem; goto pass; }

	ASTStruct      strc;
	skip = parseStruct  (ln, errs, &strc);	// Parse Struct
	if(skip){ ret->kind = TT_STRC; ret->type.strc = strc; goto pass; }
	
	ASTUnion       unon;
	skip = parseUnion   (ln, errs, &unon);	// Parse Union
	if(skip){ ret->kind = TT_UNON; ret->type.unon = unon; goto pass; }
	
	ASTTagUnion    tgun;
	skip = parseTagUnion(ln, errs, &tgun);  // Parse Tagged Union
	if(skip){ ret->kind = TT_TGUN; ret->type.tgun = tgun; goto pass; }
	
	ASTEnum        enmt;
	skip = parseEnum    (ln, errs, &enmt);  // Parse Enum
	if(skip){ ret->kind = TT_ENUM; ret->type.enmt = enmt; goto pass; }
	
	ASTBuiltin     bity;
	skip = parseBuiltin (ln, errs, &bity);  // Parse Builtin
	if(skip){ ret->kind = TT_BITY; ret->type.bity = bity; goto pass; }
	
	// If all else fails, fail completely
	printf("parseType     > fail\n");
	return 0;
}


// Pars		= [ Id : TyElem , ... ]


// TPars	= [ Id : TyElem ,	...	]
int parseTPars(ASTLine* ln, ErrorList* errs, ASTTPars* ret){
	if(ln->lst[0].kind == AL_BRK){
		// Split on comma, parse Id : TyElem lines
		return 1;
	}
	return 0;
}


// TyDef	= TId :: Type
//			| TId :: TPars => Type
int parseTyDef(ASTLine* ln, ErrorList* errs, ASTTyDef* ret){
	printf("parseTyDef    | ");
	ASTLine line = *ln;
	printASTLine(line);
	
	int skip = 0;
	if(0){
		pass:
		printf("parseTyDef    > pass\n\n");
		return skip;
	}

	TkType pattern[] = {TKN_S_TYID, TKN_DEFINE};
	if((ln->size >= 3) && tokenMatch(ln, pattern, 2)){
		if((ln->size >= 5) && (ln->lst[3].kind == AL_TKN) && (ln->lst[3].tk.type == TKN_R_DARROW)){
			// Parse TPars and Type
		}
		ASTLine line;
		viewAt(ln, &line, 2);
		// Parse Type
		skip = parseType(&line, errs, &ret->type);
		if(skip) goto pass;
	}
	printf("parseTyDef    > fail\n\n");
	return 0;
}*/


/*
	Function Parsing Rules
*/
// FnDef 	= Id :: Pars -> FTPars Block
//			| Id :: Pars -> TyElem Block
//			| Id :: Pars => Pars -> FTPars Block
//			| Id :: Pars => Pars -> TyElem Block
/*
int parseFnDef(ASTLine* ln, ErrorList* errs, ASTFnDef* ret){
	printf("parseFnDef    | ");
	ASTLine line = *ln;
	printASTLine(line);
	
	int skip = 0;
	if(0){
		pass:
		printf("parseFnDef    > pass\n\n");
		return skip;
	}

	TkType pattern[] = {TKN_S_ID, TKN_DEFINE};
	if((ln->size >= 3) && tokenMatch(ln, pattern, 2)){
		if((ln->size >= 5) && (ln->lst[3].kind == AL_TKN) && (ln->lst[3].tk.type == TKN_R_DARROW)){
			// Parse TPars and Type
		}
		ASTLine line;
		viewAt(ln, &line, 2);
		// Parse Block
		skip = 1;
		if(skip) goto pass;
	}
	printf("parseFnDef    > fail\n\n");
	return 0;
}

// Block	= { Stmt ; ... Expr }
//			| { Expr }

// Stmt		= XPrs := Expr

// XPrs		= Id, ...

// Lens		= Id . Id
//			| Id . Lens

// Loc		= Id @ Id
//			| Id @ Loc

// Expr		= Id
//			| MId
//			| Loc
//			| Lens
//			| Int
//			| Flt
//			| Str
//			| Tag
//			| FnCall
//			| TyCall
//			| Match
//			| MatchVal
//			| If
//			| IfVal
//			| Expr Binop Expr
//			| Id[Expr]
//			| MId[Expr]
//			| Unop Expr
//			| Expr Id
//			| Expr MId
//			| Expr Loc
//			| ( Expr )
//			| Lmda
//			| Wild
//			| Expr, Expr
int parseExpr(ASTLine* l, ErrorList* errs, ASTExpr* ret){
	return 0;
}

// Lmda		= [Id, ... ]   Block
//			| [Id, ... ] ! Block

// Binop	= + - * / % ^ < > >= =< & | = !=
int parseBinop(ASTLine* l, ErrorList* errs, ASTExpr* ret){
	return 0;
}

// Unop		= - ^ ! <-

// FnCall	= [ Id  : Expr, ... ]
//			= [ MId : Expr, ... ]
//			| [ Loc : Expr, ... ]

// TyCall	= [ TId : Expr, ... ]

// If		= [ ? : Expr ? Block ; ... ]

// IfVal	= [ ? : Expr ? := Expr ; ... ]

// Match	= [Expr ? : Expr ? Block ; ... ]

// MatchVal = [Expr ? : Expr ? := Expr ; ... ]


*/





/*
void printASTType(ASTType ty, int pad){
	leftpad(pad);
	if(ty.kind == TT_ELEM){
		for(int i = 0; i < ty.type.elem.arct; i++){
			if(ty.type.elem.arrs[i] < 0){
				printf("^");
			}else if(ty.type.elem.arrs[i] == 0){
				printf("[]");
			}else{
				printf("[%i]", ty.type.elem.arrs[i]);
			}
		}
		printf("T%i", ty.type.elem.tyid);
	}else if(ty.kind == TT_STRC){
		printf("[%i:\n", ty.type.strc.valct);
		ASTType* ts = ty.type.strc.vals;
		for(int i = 0; i < ty.type.strc.valct; i++){ printASTType(ts[i], pad+1); printf(" -> %i\n", ty.type.unon.labels[i]); }
		leftpad(pad);
		printf("]");
	}else if(ty.kind == TT_UNON){
		printf("(%i:\n", ty.type.strc.valct);
		ASTType* ts = ty.type.unon.vals;
		for(int i = 0; i < ty.type.unon.valct; i++){ printASTType(ts[i], pad+1); printf(" -> %i\n", ty.type.unon.labels[i]); }
		leftpad(pad);
		printf(")");
	}else{
		printf("BID%i", ty.type.bity.bid);
	}
}



void printASTProgram(ASTProgram prog){
	printf("Functions:[%i]\n", prog.fnct);
	for(int i = 0; i < prog.fnct; i++){
		Position p = prog.fns[i].pos;
		printf("  FN%i %i@(%i:%i - %i:%i)\n", i, p.fileId, p.lineStart, p.colStart, p.lineStart, p.lineEnd);
	}
	
	printf("Types:[%i]\n", prog.tyct);
	for(int i = 0; i < prog.tyct; i++){
		Position p = prog.tys[i].pos;
		printf("  TY%i %i@(%i:%i - %i:%i)\n", i, p.fileId, p.lineStart, p.colStart, p.lineStart, p.lineEnd);
		printASTType(prog.tys[i].type, 2);
		printf("\n");
	}
}*/
