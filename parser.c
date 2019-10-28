#include "parser.h"
#include "interpreter.h"
#include "stdlib.h"




typedef struct{
  char*  text;
  int    head;
  int    size;

  LISP** defs;
}PARSERSTATE;


int parseHex(PARSERSTATE* state, int skip, uint64_t* ret){

  int index = state->head + skip;
  int revert= index;
  uint64_t val = 0;
  while((index < state->size)){
    char c = state->text[index];
    if((c >= '0') && (c <= '9')){
      val <<= 4;
      val |= (c - '0');
    }else if((c >= 'A') && (c <= 'F')){
      val <<= 4;
      val |= (c - 'A') + 10;
    }else if((c >= 'a') && (c <= 'f')){
      val <<= 4;
      val |= (c - 'a') + 10;
    }else if(c == ' '){
      *ret = val;
      state->head = index;
      return 1;
    }else{
      *ret = 0;
      return 0;
    }
    index++;
  }
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
  return parseIdentifier(state, 'u', ret);
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
      len = ix;
      cont = 0;
    }
    ix++;
    cont = (ix < state->size);
  }

  ret->text = malloc(sizeof(char) * len);
  ix      = state->head+1;
  int tix = state->head+1;
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
  }

  return 1;
}
