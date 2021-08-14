#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"



typedef struct{
	int fileId, lineStart, lineEnd, colStart, colEnd;
}Position;

Position fusePosition(Position, Position);



typedef struct{
	void*	array;
	size_t	stride;
	int		size, cap;
}List;



int gcd(int, int);
int lcm(int, int);

int    loadFile      (char*, uint8_t**, int*);
void   printHexBuffer(uint8_t*, int);
void   leftpad       (int);






#endif
