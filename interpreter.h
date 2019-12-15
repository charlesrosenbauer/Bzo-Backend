#ifndef __INTERPRETER_HEADER__
#define __INTERPRETER_HEADER__

#include "stdint.h"
#include "util.h"



static const uint64_t BINOP =
											 (1l << ADDI) | (1l << SUBI) | (1l << MULI) | (1l << MULU) |
											 (1l << DIVI) | (1l << DIVU) | (1l << MODI) | (1l << MODU) |
							 				 (1l << AND ) | (1l << OR  ) | (1l << XOR ) | (1l << SHL ) |
											 (1l << SHR ) | (1l << RTL ) | (1l << RTR ) | (1l << MAXI) |
											 (1l << MINI) | (1l << MAXU) | (1l << MINU) | (1l << ILS ) |
											 (1l << ILSE) | (1l << IGT ) | (1l << IGTE) | (1l << EQ  ) |
											 (1l << NEQ ) | (1l << ULS ) | (1l << ULSE) | (1l << UGT ) |
											 (1l << UGTE) | (1l << MTCH) | (1l << SBST) | (1l << SPST) |
											 (1l << DSJT) | (1l << NCMP);

static const uint64_t UNOP  =
											 (1l << NOT ) | (1l << NZ  ) | (1l << PCT ) | (1l << CTZ ) |
											 (1l << CLZ ) | (1l << SXOR) | (1l << SAND) | (1l <<SXNOR) |
											 (1l <<SNAND) | (1l << SNOR) | (1l << SOR ) | (1l << INC ) |
											 (1l << DEC);





VALOBJ eval(LISP*, LISPENV, int);
VALOBJ call(int, LISP*, LISPENV, int);


void unflatten(LISP*, VALOBJ*, int);
VALOBJ makeInt(int);
VALOBJ makeFlt(float);



#endif
