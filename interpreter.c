#include "interpreter.h"
#include "parser.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"


/*
LISP* eval(LISP* input){
  if(input == NULL) return NULL;

  if(input->type == OPRTYP){
    uint64_t op = input->here.UVAL;
    if(op <= 0x00FF){
      if      ((1l << op) & UNOP){
        // Get parameter
        if(input->next == NULL){
          printf("On opcode %lu, expected parameter. Found none.", op);
          exit(1);
        }
        LISP* here = eval((LISP*)(input->next));
        VAL x = here->here;
        VAL q;
        LISP* ret = malloc(sizeof(LISP));
        ret->refc = 0;
        ret->type = here->type;
        ret->next = NULL;

        switch(op){
          case NOT  : q.UVAL = ~x.UVAL;                     break;
          case NZ   : q.UVAL =  x.UVAL != 0;                break;
          case PCT  : q.UVAL = __builtin_popcountl(x.UVAL); break;
          case CTZ  : q.UVAL = __builtin_ctzl     (x.UVAL); break;
          case CLZ  : q.UVAL = __builtin_clzl     (x.UVAL); break;
          case INC  : q.UVAL = x.UVAL + 1;                  break;
          case DEC  : q.UVAL = x.UVAL - 1;                  break;

          case SXOR:{
            uint64_t n = x.UVAL;
            n = (n ^ (n >>  1));
            n = (n ^ (n >>  2));
            n = (n ^ (n >>  4));
            n = (n ^ (n >>  8));
            n = (n ^ (n >> 16));
            n = (n ^ (n >> 32));
            q.UVAL = n;
          }break;

          case SXNOR:{
            uint64_t n = x.UVAL;
            n = ~(n ^ (n >>  1));
            n = ~(n ^ (n >>  2));
            n = ~(n ^ (n >>  4));
            n = ~(n ^ (n >>  8));
            n = ~(n ^ (n >> 16));
            n = ~(n ^ (n >> 32));
            q.UVAL = n;
          }break;

          case SAND:{
            uint64_t n = x.UVAL;
            n = (n & (n >>  1));
            n = (n & (n >>  2));
            n = (n & (n >>  4));
            n = (n & (n >>  8));
            n = (n & (n >> 16));
            n = (n & (n >> 32));
            q.UVAL = n;
          }break;

          case SNAND:{
            uint64_t n = x.UVAL;
            n = ~(n & (n >>  1));
            n = ~(n & (n >>  2));
            n = ~(n & (n >>  4));
            n = ~(n & (n >>  8));
            n = ~(n & (n >> 16));
            n = ~(n & (n >> 32));
            q.UVAL = n;
          }break;

          case SOR:{
            uint64_t n = x.UVAL;
            n = (n | (n >>  1));
            n = (n | (n >>  2));
            n = (n | (n >>  4));
            n = (n | (n >>  8));
            n = (n | (n >> 16));
            n = (n | (n >> 32));
            q.UVAL = n;
          }break;

          case SNOR:{
            uint64_t n = x.UVAL;
            n = ~(n | (n >>  1));
            n = ~(n | (n >>  2));
            n = ~(n | (n >>  4));
            n = ~(n | (n >>  8));
            n = ~(n | (n >> 16));
            n = ~(n | (n >> 32));
            q.UVAL = n;
          }break;

          default:{
            printf("Invalid opcode %lu.", op);
            exit(2);
          }break;
        }
        ret->here = q;
        return ret;

      }else if((1l << op) & BINOP){

        // Get parameters
        if(input->type == LSPTYP){
          input = eval(input);
        }
        if(input->next == NULL){
          printf("On opcode %lu, expected 2 parameters. Found none.", op);
          exit(3);
        }
        LISP* la = eval((LISP*)(input->next));
        if(input->next == NULL){
          printf("On opcode %lu, expected 2 parameters. Found one.", op);
          exit(4);
        }
        LISP* lb = eval((LISP*)(la->next));


        VAL a = la->here;
        VAL b = lb->here;
        VAL q;
        LISP* ret = malloc(sizeof(LISP));
        ret->refc = 0;
        ret->type = la->type;
        ret->next = NULL;


        // TODO: Add binop switch
        switch(op){
          case ADDI  : q.IVAL = a.IVAL + b.IVAL;  break;
          case SUBI  : q.IVAL = a.IVAL - b.IVAL;  break;
          case MULI  : q.IVAL = a.IVAL * b.IVAL;  break;
          case MULU  : q.UVAL = a.UVAL * b.UVAL;  break;
          case DIVU  : {
            if(b.UVAL == 0){
              printf("Divide by zero.");
              exit(5);
            }
            q.UVAL = a.UVAL / b.UVAL;
          } break;
          case DIVI  : {
            if(b.IVAL == 0){
              printf("Divide by zero.");
              exit(5);
            }
            q.IVAL = a.IVAL / b.IVAL;
          } break;
          case MODU  : {
            if(b.UVAL == 0){
              printf("Divide by zero.");
              exit(5);
            }
            q.UVAL = a.UVAL % b.UVAL;
          } break;
          case MODI  : {
            if(b.IVAL == 0){
              printf("Divide by zero.");
              exit(5);
            }
            q.IVAL = a.IVAL % b.IVAL;
          } break;
          case AND   : q.UVAL = a.UVAL &  b.UVAL;  break;
          case XOR   : q.UVAL = a.UVAL ^  b.UVAL;  break;
          case OR    : q.UVAL = a.UVAL |  b.UVAL;  break;
          case SHL   : q.UVAL = a.UVAL << b.UVAL;  break;
          case SHR   : q.UVAL = a.UVAL >> b.UVAL;  break;
          case RTL   : q.UVAL = (a.UVAL << b.UVAL) | (a.UVAL >> (64 - b.UVAL)); break;
          case RTR   : q.UVAL = (a.UVAL >> b.UVAL) | (a.UVAL << (64 - b.UVAL)); break;
          case MAXI  : q.IVAL = (a.IVAL > b.IVAL)? a.IVAL : b.IVAL;  break;
          case MINI  : q.IVAL = (a.IVAL > b.IVAL)? b.IVAL : a.IVAL;  break;
          case MAXU  : q.UVAL = (a.UVAL > b.UVAL)? a.UVAL : b.UVAL;  break;
          case MINU  : q.UVAL = (a.UVAL > b.UVAL)? b.UVAL : a.UVAL;  break;
          case ILS   : q.IVAL = a.IVAL <  b.IVAL;  break;
          case ILSE  : q.IVAL = a.IVAL <= b.IVAL;  break;
          case IGT   : q.IVAL = a.IVAL >  b.IVAL;  break;
          case IGTE  : q.IVAL = a.IVAL >= b.IVAL;  break;
          case ULS   : q.UVAL = a.UVAL <  b.UVAL;  break;
          case ULSE  : q.UVAL = a.UVAL <= b.UVAL;  break;
          case UGT   : q.UVAL = a.UVAL >  b.UVAL;  break;
          case UGTE  : q.UVAL = a.UVAL >= b.UVAL;  break;
          case EQ    : q.UVAL = a.UVAL == b.UVAL;  break;
          case NEQ   : q.UVAL = a.UVAL != b.UVAL;  break;
          case MTCH  : q.UVAL = __builtin_popcountl(a.UVAL & b.UVAL);  break;
          case SBST  : q.UVAL = a.UVAL == (a.UVAL & b.UVAL);  break;
          case SPST  : q.UVAL = b.UVAL == (a.UVAL & b.UVAL);  break;
          case DSJT  : q.UVAL = (a.UVAL | b.UVAL) == (a.UVAL ^ b.UVAL); break;
          case NCMP  : q.UVAL = (a.UVAL != (a.UVAL & b.UVAL)) && (b.UVAL != (a.UVAL & b.UVAL)); break;
        }
        ret->here = q;
        return ret;
      }
    }
  }else{
    return input;
  }

  return NULL;
}*/


VALOBJ extractVal(VALOBJ x, LISPENV env){
  if(x.typ == LSPTYP){
    return eval(x.val.PVAL, env);
  }
  return (x.typ == VARTYP)? env.vars[x.val.UVAL] : x;
}


VALOBJ eval(LISP* function, LISPENV env){
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
  if((1l << op) & BINOP){
    VALOBJ a, b;
    if(function->next == NULL){
      printf("Expected 2 parameters, found none.\n");
      exit(3);
    }

    LISP* lispA = function->next;
    a = extractVal(lispA->here, env);
    if(lispA->next == NULL){
      printf("Expected 2 parameters, found one.\n");
      exit(4);
    }
    LISP* lispB = lispA->next;
    b = extractVal(lispB->here, env);


    switch(op){
      case ADDI   : ret.val.IVAL = a.val.IVAL + b.val.IVAL; ret.typ = INTTYP;   break;
      case SUBI   : ret.val.IVAL = a.val.IVAL - b.val.IVAL; ret.typ = INTTYP;   break;
      case MULI   : ret.val.IVAL = a.val.IVAL * b.val.IVAL; ret.typ = INTTYP;   break;
      case MULU   : ret.val.UVAL = a.val.UVAL * b.val.UVAL; ret.typ = UNTTYP;   break;
      case DIVI   : ret.val.IVAL = a.val.IVAL / b.val.IVAL; ret.typ = INTTYP;   break;
      case DIVU   : ret.val.UVAL = a.val.UVAL / b.val.UVAL; ret.typ = UNTTYP;   break;
      case MODI   : ret.val.IVAL = a.val.IVAL % b.val.IVAL; ret.typ = INTTYP;   break;
      case MODU   : ret.val.UVAL = a.val.UVAL % b.val.UVAL; ret.typ = UNTTYP;   break;
    }
  }else{
    printf("Invalid opcode: %li\n", op);
    exit(4);
  }
  return ret;
}
