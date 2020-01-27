#include "interpreter.h"
#include "parser.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"




VALOBJ extractVal(VALOBJ x, LISPENV env, int stackframe){
  if(x.typ == LSPTYP){
    return eval(x.val.PVAL, env, stackframe);
  }
  return (x.typ == VARTYP)? env.stack[stackframe + x.val.UVAL] : x;
}


void unflatten(LISP* to, VALOBJ* from, int size){
  for(int i = 0; i < size; i++){
    to[i].next = &to[i+1];
    to[i].here = from[i];
    to[i].refc = 1;
  }
  to[size-1].next = NULL;
}


VALOBJ makeInt(int x){
  VALOBJ ret;
  ret.typ      = INTTYP;
  ret.val.IVAL = x;
  return ret;
}


VALOBJ makeFlt(float x){
  VALOBJ ret;
  ret.typ      = FLTTYP;
  ret.val.FVAL = x;
  return ret;
}


int flatten(LISP* l, VALOBJ* buffer, int size){

  LISP* head = l;
  for(int i = 0; i < size; i++){
    if(l == NULL){
      return i-1;
    }
    buffer[i] = l->here;
    l = l->next;
  }

  return size;
}


void extractUnop(LISP* function, LISPENV env, int stackframe, VALOBJ* a){
  if(function->next == NULL){
    printf("Expected 1 parameter, found none.\n");
    exit(3);
  }

  LISP* lispA = function->next;
  *a = extractVal(lispA->here, env, stackframe);
}


void extractBinop(LISP* function, LISPENV env, int stackframe, VALOBJ* a, VALOBJ* b){
  if(function->next == NULL){
    printf("Expected 2 parameters, found none.\n");
    exit(3);
  }

  LISP* lispA = function->next;
  *a = extractVal(lispA->here, env, stackframe);
  if(lispA->next == NULL){
    printf("Expected 2 parameters, found one.\n");
    exit(4);
  }
  LISP* lispB = lispA->next;
  *b = extractVal(lispB->here, env, stackframe);
}


VALOBJ call(int function, LISP* input, LISPENV env, int stackframe){
  PROGRAM*  p = env.prog;
  if(p == NULL){
    printf("No program.\n");
    exit(5);
  }
  if(function > p->fnct){
    printf("Invalid function.\n");
    exit(6);
  }
  FUNCTION* f = &(p->funcs[function]);

  // Extract function
  int parct = f->prct;
  if(parct > 256){
    printf("Too many paramters.\n");
    exit(7);
  }

  int ct = flatten(input, &env.stack[env.stacktop], parct);
  if(parct != ct){
    printf("Expected %i parameters for function %i, found %i.\n", parct, function, ct);
    exit(8);
  }
  env.stacktop += parct;

  VALOBJ ret;

  // Evaluate function
  ret = eval(f->code, env, stackframe);

  // Cleanup
  env.stacktop -= parct;
  return ret;
}


VALOBJ eval(LISP* function, LISPENV env, int stackframe){
  VALOBJ ret;
  ret.typ      = 0;
  ret.val.IVAL = 0;

  if(function == NULL){
    printf("Unexpected nil\n.");
    exit(1);
  }
  if(function->here.typ != OPRTYP){
    printf("Expected opcode\n.");
    exit(2);
  }

  // Handle opcode
  int64_t op = function->here.val.OVAL;
  //printf("OP:%li\n", op);
  if((1l << op) & BINOP){
    VALOBJ a, b;
    extractBinop(function, env, stackframe, &a, &b);

    switch(op){
      case ADDI   : ret.val.IVAL =  a.val.IVAL +  b.val.IVAL; ret.typ = INTTYP;   break;
      case SUBI   : ret.val.IVAL =  a.val.IVAL -  b.val.IVAL; ret.typ = INTTYP;   break;
      case MULI   : ret.val.IVAL =  a.val.IVAL *  b.val.IVAL; ret.typ = INTTYP;   break;
      case MULU   : ret.val.UVAL =  a.val.UVAL *  b.val.UVAL; ret.typ = UNTTYP;   break;
      case DIVI   : ret.val.IVAL =  a.val.IVAL /  b.val.IVAL; ret.typ = INTTYP;   break;
      case DIVU   : ret.val.UVAL =  a.val.UVAL /  b.val.UVAL; ret.typ = UNTTYP;   break;
      case MODI   : ret.val.IVAL =  a.val.IVAL %  b.val.IVAL; ret.typ = INTTYP;   break;
      case MODU   : ret.val.UVAL =  a.val.UVAL %  b.val.UVAL; ret.typ = UNTTYP;   break;
      case AND    : ret.val.UVAL =  a.val.UVAL &  b.val.UVAL; ret.typ = UNTTYP;   break;
      case OR     : ret.val.UVAL =  a.val.UVAL |  b.val.UVAL; ret.typ = UNTTYP;   break;
      case XOR    : ret.val.UVAL =  a.val.UVAL ^  b.val.UVAL; ret.typ = UNTTYP;   break;
      case SHL    : ret.val.UVAL =  a.val.UVAL << b.val.UVAL; ret.typ = UNTTYP;   break;
      case SHR    : ret.val.UVAL =  a.val.UVAL >> b.val.UVAL; ret.typ = UNTTYP;   break;
      case RTL    : ret.val.UVAL = (a.val.UVAL << b.val.UVAL) | (a.val.UVAL >> (64-b.val.UVAL)); ret.typ = UNTTYP;   break;
      case RTR    : ret.val.UVAL = (a.val.UVAL >> b.val.UVAL) | (a.val.UVAL << (64-b.val.UVAL)); ret.typ = UNTTYP;   break;
      case MAXI   : ret.val.IVAL = (a.val.IVAL >  b.val.IVAL)? a.val.IVAL : b.val.IVAL; ret.typ = INTTYP; break;
      case MINI   : ret.val.IVAL = (a.val.IVAL >  b.val.IVAL)? b.val.IVAL : a.val.IVAL; ret.typ = INTTYP; break;
      case MAXU   : ret.val.UVAL = (a.val.UVAL >  b.val.UVAL)? a.val.UVAL : b.val.UVAL; ret.typ = UNTTYP; break;
      case MINU   : ret.val.UVAL = (a.val.UVAL >  b.val.UVAL)? b.val.UVAL : a.val.UVAL; ret.typ = UNTTYP; break;
      case ILS    : ret.val.IVAL =  a.val.IVAL <  b.val.IVAL; ret.typ = INTTYP; break;
      case ILSE   : ret.val.IVAL =  a.val.IVAL <= b.val.IVAL; ret.typ = INTTYP; break;
      case IGT    : ret.val.IVAL =  a.val.IVAL >  b.val.IVAL; ret.typ = INTTYP; break;
      case IGTE   : ret.val.IVAL =  a.val.IVAL >= b.val.IVAL; ret.typ = INTTYP; break;
      case ULS    : ret.val.UVAL =  a.val.UVAL <  b.val.UVAL; ret.typ = UNTTYP; break;
      case ULSE   : ret.val.UVAL =  a.val.UVAL <= b.val.UVAL; ret.typ = UNTTYP; break;
      case UGT    : ret.val.UVAL =  a.val.UVAL >  b.val.UVAL; ret.typ = UNTTYP; break;
      case UGTE   : ret.val.UVAL =  a.val.UVAL >= b.val.UVAL; ret.typ = UNTTYP; break;
      case EQ     : ret.val.UVAL =  a.val.UVAL == b.val.UVAL; ret.typ = UNTTYP; break;
      case NEQ    : ret.val.UVAL =  a.val.UVAL != b.val.UVAL; ret.typ = UNTTYP; break;
      case MTCH   : ret.val.UVAL =  __builtin_popcountl(a.val.UVAL & b.val.UVAL); ret.typ = UNTTYP; break;
      case SBST   : ret.val.UVAL =  a.val.UVAL == (a.val.UVAL & b.val.UVAL); ret.typ = UNTTYP; break;
      case SPST   : ret.val.UVAL =  b.val.UVAL == (a.val.UVAL & b.val.UVAL); ret.typ = UNTTYP; break;
      case DSJT   : ret.val.UVAL = (a.val.UVAL |  b.val.UVAL) == (a.val.UVAL ^ b.val.UVAL); ret.typ = UNTTYP; break;
      case NCMP   : ret.val.UVAL = (a.val.UVAL != (a.val.UVAL & b.val.UVAL)) && (b.val.UVAL != (a.val.UVAL & b.val.UVAL)); ret.typ = UNTTYP; break;
      default     : printf("Invalid opcode: %li %li\n", op, (1l << op) & BINOP); exit(4); break;
    }
  }else if((1l << op) & UNOP){
    VALOBJ a;
    extractUnop(function, env, stackframe, &a);

    switch(op){
      case NOT    : ret.val.UVAL = ~a.val.UVAL;                     ret.typ = UNTTYP; break;
      case NZ     : ret.val.UVAL =  a.val.UVAL != 0;                ret.typ = UNTTYP; break;
      case PCT    : ret.val.UVAL = __builtin_popcountl(a.val.UVAL); ret.typ = UNTTYP; break;
      case CTZ    : ret.val.UVAL = __builtin_ctzl     (a.val.UVAL); ret.typ = UNTTYP; break;
      case CLZ    : ret.val.UVAL = __builtin_clzl     (a.val.UVAL); ret.typ = UNTTYP; break;
      case INC    : ret.val.UVAL =  a.val.UVAL - 1;                 ret.typ = UNTTYP; break;
      case DEC    : ret.val.UVAL =  a.val.UVAL + 1;                 ret.typ = UNTTYP; break;
      case SXOR:{
        uint64_t n = a.val.UVAL;
        n = (n ^ (n >>  1));
        n = (n ^ (n >>  2));
        n = (n ^ (n >>  4));
        n = (n ^ (n >>  8));
        n = (n ^ (n >> 16));
        n = (n ^ (n >> 32));
        ret.val.UVAL = n;
      }break;

      case SXNOR:{
        uint64_t n = a.val.UVAL;
        n = ~(n ^ (n >>  1));
        n = ~(n ^ (n >>  2));
        n = ~(n ^ (n >>  4));
        n = ~(n ^ (n >>  8));
        n = ~(n ^ (n >> 16));
        n = ~(n ^ (n >> 32));
        ret.val.UVAL = n;
      }break;

      case SAND:{
        uint64_t n = a.val.UVAL;
        n = (n & (n >>  1));
        n = (n & (n >>  2));
        n = (n & (n >>  4));
        n = (n & (n >>  8));
        n = (n & (n >> 16));
        n = (n & (n >> 32));
        ret.val.UVAL = n;
      }break;

      case SNAND:{
        uint64_t n = a.val.UVAL;
        n = ~(n & (n >>  1));
        n = ~(n & (n >>  2));
        n = ~(n & (n >>  4));
        n = ~(n & (n >>  8));
        n = ~(n & (n >> 16));
        n = ~(n & (n >> 32));
        ret.val.UVAL = n;
      }break;

      case SOR:{
        uint64_t n = a.val.UVAL;
        n = (n | (n >>  1));
        n = (n | (n >>  2));
        n = (n | (n >>  4));
        n = (n | (n >>  8));
        n = (n | (n >> 16));
        n = (n | (n >> 32));
        ret.val.UVAL = n;
      }break;

      case SNOR:{
        uint64_t n = a.val.UVAL;
        n = ~(n | (n >>  1));
        n = ~(n | (n >>  2));
        n = ~(n | (n >>  4));
        n = ~(n | (n >>  8));
        n = ~(n | (n >> 16));
        n = ~(n | (n >> 32));
        ret.val.UVAL = n;
      }break;
      default     : printf("Invalid opcode: %li %li\n", op, (1l << op) & UNOP); exit(4); break;
    }
  }else if((1l << (op-256)) & FBINOP){
    VALOBJ a, b;
    extractBinop(function, env, stackframe, &a, &b);

    switch(op){
      case ADDF: ret.val.FVAL =  a.val.FVAL +  b.val.FVAL; ret.typ = FLTTYP;   break;
      case SUBF: ret.val.FVAL =  a.val.FVAL +  b.val.FVAL; ret.typ = FLTTYP;   break;
      case MULF: ret.val.FVAL =  a.val.FVAL +  b.val.FVAL; ret.typ = FLTTYP;   break;
      case DIVF: ret.val.FVAL =  a.val.FVAL +  b.val.FVAL; ret.typ = FLTTYP;   break;
      case MODF: ret.val.FVAL =  a.val.FVAL +  b.val.FVAL; ret.typ = FLTTYP;   break;
      default     : printf("Invalid opcode: %li\n", op); exit(4); break;
    }
  }else if((1l << (op-256)) & FUNOP){
    VALOBJ a;
    if(function->next == NULL){
      printf("Expected 1 parameter, found none.\n");
      exit(3);
    }

    LISP* lispA = function->next;
    a = extractVal(lispA->here, env, stackframe);

    switch(op){
      case ABSF: ret.val.FVAL = fabs(a.val.FVAL); ret.typ = FLTTYP;   break;
      case SIN : ret.val.FVAL =  sin(a.val.FVAL); ret.typ = FLTTYP;   break;
      case COS : ret.val.FVAL =  cos(a.val.FVAL); ret.typ = FLTTYP;   break;
      case TAN : ret.val.FVAL =  tan(a.val.FVAL); ret.typ = FLTTYP;   break;
      case ASIN: ret.val.FVAL = asin(a.val.FVAL); ret.typ = FLTTYP;   break;
      case ACOS: ret.val.FVAL = acos(a.val.FVAL); ret.typ = FLTTYP;   break;
      case ATAN: ret.val.FVAL = atan(a.val.FVAL); ret.typ = FLTTYP;   break;

      default     : printf("Invalid opcode: %li\n", op); exit(4); break;
    }
  }else{
    switch(op){
      case MAP:{
        VALOBJ a, b;
        extractBinop(function, env, stackframe, &a, &b);

        ARR* array = b.val.PVAL;
        if(array != NULL){
          int    len = array->size;
          LISP** arr = array->data;
          for(int i = 0; i < len; i++){
            call(a.val.IVAL, arr[i], env, stackframe);
          }
        }


      }break;

      case FOLD:{

      }break;

      case SCAN:{

      }break;

      case FILTER:{

      }break;

      case ANY:{

      }break;

      default:{
        printf("Invalid opcode: %li (%li)\n", op, (1l << op) & BINOP);
        exit(4);
      }
    }
  }
  return ret;
}
