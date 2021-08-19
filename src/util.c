#include "util.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"



void leftpad(int pad){
	for(int i = 0; i < pad; i++) printf("  ");
}


Position fusePosition(Position a, Position b){
	Position ret;
	ret.fileId    = a.fileId;
	ret.lineStart = a.lineStart;
	ret.colStart  = a.colStart;
	ret.lineEnd   = b.lineEnd;
	ret.colEnd    = b.colEnd;
	return ret;
}



int loadFile(char* fname, uint8_t** buffer, int* fsize){
	FILE*  pFile = fopen (fname,"r");
	size_t result;
	if (pFile == NULL){ printf("Cannot locate file %s.", fname); return 0; }
	fseek (pFile , 0 , SEEK_END);
	*fsize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	*buffer = malloc((sizeof(uint8_t)*(*fsize)) + 4);
	if (buffer == NULL) { printf("Memory error",stderr); exit(2); }

	// copy the file into the buffer:
	result = fread (*buffer,1,(*fsize),pFile);
	if (result != (*fsize)) { printf("Reading error",stderr); exit(3); }
	
	// This is a hack to deal with an off-by-one error that causes an infinite loop if we fix it the obvious way.
	for(int i = 0; i < 4; i++)
		(*buffer)[*fsize+i] = (uint8_t)'\n';
	*fsize += 4;
	fclose(pFile);
	return 1;
}


List makeList(int size, size_t stride){
	List ret  = (List){NULL, stride, 0, size};
	ret.array = malloc(stride * size);
	return ret;
}


void growList(List* l, int size){
	if(l->cap < size){
		uint8_t* tmp = l->array;
		l->array     = malloc(l->stride * size);
		memcpy(l->array, tmp, l->stride * l->size);
		free(tmp);
		l->cap       = size;
	}
}


int appendList(List* l, void* x){
	if(l->size+1  >= l->cap) growList(l, l->size * 2);
	uint8_t* array = l->array;
	uint8_t* elem  = x;
	memcpy(&array[l->stride * l->size], elem, l->stride);
	l->size++;
	return l->size-1;
}


void* getListBound(List* l, int i){
	if((i > 0) && (i < l->size))
		return (void*)&(((uint8_t*)l->array)[l->stride * i]);
	return NULL;
}


void* getList(List* l, int i){
	return (void*)&(((uint8_t*)l->array)[l->stride * i]);
}

void  freeList(List* l){
	free(l->array);
}




void printHexBuffer(uint8_t* buffer, int size){
	for(int i = 0; i < size; i++){
		uint8_t x = buffer[i];
		char a = (x % 16), b = (x / 16);
		a += (a > 9)? ('A'-10) : '0';
		b += (b > 9)? ('A'-10) : '0';
		printf("%c%c ", b, a);
	}
	printf("\n");
}
