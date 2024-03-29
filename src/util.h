#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"
#include "string.h"



typedef struct{
	int fileId, lineStart, lineEnd, colStart, colEnd;
}Position;

Position fusePosition(Position, Position);
int      eqPos       (Position, Position);


typedef struct{
	void*	array;
	size_t	stride;
	int		size, cap;
}List;



List  makeList    (int,   size_t);
void  growList    (List*, int);
int   appendList  (List*, void*);
void* getListBound(List*, int);
void* getList     (List*, int);
void  freeList    (List*);



int gcd(int, int);
int lcm(int, int);

int    loadFile      (char*, uint8_t**, int*);
void   printHexBuffer(uint8_t*, int);
void   leftpad       (int);






#endif
