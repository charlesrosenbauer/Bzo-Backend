#ifndef __FUNC_HEADER__
#define __FUNC_HEADER__

#include "type.h"
#include "program.h"

#include "stdint.h"




FuncTable makeFuncTable(int, int);
void      resizeFnTable(FuncTable*, int);

FuncDef   makeFuncDef  (Type, Type, int);
CodeBlock makeCodeBlock(int);

ExprUnion makeExpr(int);
ExprUnion makeCmpd(int);
ExprUnion makePoly(int);
ExprUnion makePrfx(int);
ExprUnion makeLetx(int);

void      appendToBlock  (CodeBlock*, ProgramCode);
int       makeVar        (FuncDef*, Type);
int       appendCodeBlock(FuncDef*, CodeBlock);

void      setIx          (ExprUnion*, ExprUnion, ExprKind, int);

void      printExpr      (ExprUnion, ExprKind);
void      printProgCode  (ProgramCode);
void      printCodeBlock (CodeBlock);
void      printFunc      (FuncDef);

void      buildFunc      (FuncDef*, Program*);
int       buildFuncTable (Program*);



#endif
