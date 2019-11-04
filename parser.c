#include "parser.h"
#include "interpreter.h"
#include "stdlib.h"
#include "stdio.h"





int parseHex(PARSERSTATE* state, int skip, uint64_t* ret){

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


int parseIdentifier(PARSERSTATE* state, char x, uint64_t* ret){

  char c = state->text[state->head];
  if(c == x){
    return parseHex(state, 1, ret);
  }
  *ret = 0;
  return 0;
}


int parseVar(PARSERSTATE* state, uint64_t* ret){
  return parseIdentifier(state, 'v', ret);
}

int parseTyp(PARSERSTATE* state, uint64_t* ret){
  return parseIdentifier(state, 't', ret);
}

int parseFnc(PARSERSTATE* state, uint64_t* ret){
  return parseIdentifier(state, 'f', ret);
}

int parseInt(PARSERSTATE* state, int64_t*  ret){
  uint64_t x;
  int r = parseIdentifier(state, 'i', &x);
  *ret = *(int64_t*)(&x);
  return r;
}

int parseUnt(PARSERSTATE* state, uint64_t* ret){
  return parseIdentifier(state, 'u', &ret);
}

int parseFlt(PARSERSTATE* state, double* ret){
  uint64_t x;
  int r = parseIdentifier(state, 'r', &x);
  *ret = *(double*)(&x);
  return r;
}

int parseOpc(PARSERSTATE* state, uint64_t* ret){
  return parseIdentifier(state, 'x', ret);
}


int parseString(PARSERSTATE* state, STRING* ret){

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

void skipWhitespace(PARSERSTATE* state){
  while(1){
    char c = state->text[state->head];
    if((c == ' ') || (c == '\t') || (c == '\v') || (c == '\n')){
      state->head++;
    }else{
      return;
    }
    if(state->head == state->size){
      return;
    }
  }
}


LISP* parseLisp(int skip, PARSERSTATE* state){
  LISP* ret = NULL;


  char* skipstr = malloc(sizeof(char) * (skip + 1));
  for(int i = 0; i < skip; i++) skipstr[i] = ' ';
  skipstr[skip] = '\0';

  skipWhitespace(state);
  if(state->text[state->head] != '('){
    return NULL;
  }else{
    ret = malloc(sizeof(LISP));
    state->head++;
    LISP* head = ret;
    int cont = 1;
    while(cont){
      skipWhitespace(state);
      //printf("%s%c\n", skipstr, state->text[state->head]);
      switch(state->text[state->head]){

        // LISP HEAD
        case '(':{
          printf("%s(LISP:\n", skipstr);
          head->here.PVAL = parseLisp(skip+2, state);
          head->type = LSPTYP;
          printf("%s)\n", skipstr);
        } break;

        // VAR
        case 'v':{
          if(!parseVar(state, &head->here.UVAL)){
            printf("%sError at %i, invalid variable.\n", skipstr, state->head);
            return ret;
          }
          printf("%sVar: v%i\n", skipstr, head->here.UVAL);
        } break;

        // TYP
        case 't':{
          if(!parseTyp(state, &head->here.UVAL)){
            printf("%sError at %i, invalid type.\n", skipstr, state->head);
            return ret;
          }
          printf("%sTyp: t%i\n", skipstr, head->here.UVAL);
        } break;

        // FNC
        case 'f':{
          if(!parseFnc(state, &head->here.UVAL)){
            printf("%sError at %i, invalid function.\n", skipstr, state->head);
            return ret;
          }
          printf("%sFnc: f%i\n", skipstr, head->here.UVAL);
        } break;

        // INT
        case 'i':{
          if(!parseInt(state, &head->here.IVAL)){
            printf("%sError at %i, invalid integer.\n", skipstr, state->head);
            return ret;
          }
          printf("%sInt: i%i\n", skipstr, head->here.IVAL);
        } break;

        // UNT
        case 'u':{
          if(!parseUnt(state, &head->here.UVAL)){
            printf("%sError at %i, invalid unsigned integer.\n", skipstr, state->head);
            return ret;
          }
          printf("%sUnt: u%i\n", skipstr, head->here.UVAL);
        } break;

        // FLT
        case 'r':{
          if(!parseFlt(state, &head->here.FVAL)){
            printf("%sError at %i, invalid float.\n", skipstr, state->head);
            return ret;
          }
          printf("%sFlt: r%i\n", skipstr, head->here.FVAL);
        } break;

        // OP
        case 'x':{
          uint64_t opc;
          if(!parseOpc(state, &opc)){
            printf("%sError at %i, invalid opcode.\n", skipstr, state->head);
            return ret;
          }
          head->here.OVAL = opc;
          printf("%sOp : x%i\n", skipstr, head->here.UVAL);
        } break;

        // Str
        case '\"':{
          if(!parseString(state, &head->here.SVAL)){
            printf("%sError at %i, invalid variable.\n", skipstr, state->head);
            return ret;
          }
          printf("%sStr: s%i\n", skipstr, head->here.SVAL.size);
        } break;

        case ')':{
          head->here.PVAL = NULL;
          head->next = NULL;
          cont = 0;
        } break;

        default: {
          printf("%sError at %i, unexpected character %c (%i).\n", skipstr, state->head, state->text[state->head], state->text[state->head]);
          return ret;
        }break;
      }
      state->head++;
      if(state->head >= state->size) cont = 0;
    }
  }

  return ret;
}



void parseLispAlt(PARSERSTATE* state){
/*
  char* skipstr = malloc(sizeof(char) * (skip + 1));
  for(int i = 0; i < skip; i++) skipstr[i] = ' ';
  skipstr[skip] = '\0';
*/
  skipWhitespace(state);

  while(state->head < state->size){
    skipWhitespace(state);
    char c = state->text[state->head];
    switch(c){
      case '(':{
        printf("(\n");
      } break;

      case 'v':{
        uint64_t v;
        if(!parseVar(state, &v))
          printf("Var error at %i.\n", state->head);
        else
          printf("v%lu\n", v);
      } break;

      case 't':{
        uint64_t v;
        if(!parseTyp(state, &v))
          printf("Typ error at %i.\n", state->head);
        else
          printf("t%lu\n", v);
      } break;

      case 'f':{
        uint64_t v;
        if(!parseFnc(state, &v))
          printf("Fnc error at %i.\n", state->head);
        else
          printf("f%lu\n", v);
      } break;

      case 'i':{
        uint64_t v;
        if(!parseInt(state, &v))
          printf("Int error at %i.\n", state->head);
        else
          printf("i%li\n", v);
      } break;

      case 'u':{
        uint64_t v;
        if(!parseUnt(state, &v))
          printf("Unt error at %i.\n", state->head);
        else
          printf("u%lu\n", v);
      } break;

      case 'r':{
        double v;
        if(!parseFlt(state, &v))
          printf("Flt error at %i.\n", state->head);
        else
          printf("r%f\n", v);
      } break;

      case 'x':{
        uint64_t v;
        if(!parseOpc(state, &v))
          printf("Opc error at %i.\n", state->head);
        else
          printf("x%u\n", v);
      } break;

      case '\"':{
        STRING v;
        if(!parseString(state, &v))
          printf("Str error at %i.\n", state->head);
        else
          printf("s%i\n", v.size);
      } break;

      case ')':{
        printf(")\n");
      } break;

      case '\0':{
        return;
      } break;

      default: printf("@%i, unexpected: %c (%i)\n", state->head, state->text[state->head], state->text[state->head]);
    }
    state->head++;
  }

}
