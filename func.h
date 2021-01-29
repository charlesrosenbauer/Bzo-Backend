#ifndef __FUNC_HEADER__
#define __FUNC_HEADER__

#include "type.h"
#include "program.h"

#include "stdint.h"




FuncTable makeFuncTable(int, int);

FuncDef   makeFuncDef  (Type, Type, int);
CodeBlock makeCodeBlock(int, int, int);

ExprUnion makeExpr(int);
ExprUnion makeCmpd(int);
ExprUnion makePoly(int);

void      appendToBlock(CodeBlock*, ProgramCode);
int       makeVar      (FuncDef*, Type);

void      setIx   (ExprUnion*, ExprUnion, ExprKind, int);

void      printExpr     (ExprUnion, ExprKind);
void      printProgCode (ProgramCode);
void      printCodeBlock(CodeBlock);
void      printFunc     (FuncDef);

void      buildFunc(FuncDef*, FuncTable*);




#endif
