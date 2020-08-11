#include "structs.h"
#include "stdlib.h"


DynArr newDynArr   (int capacity){
	DynArr ret;
	ret.data     = malloc(sizeof(int64_t) * capacity);
	ret.size     = 0;
	ret.capacity = capacity;
	return ret;
}


void growDynArr  (DynArr* arr, int mincap){
	int size = (arr->capacity * 2);
	size = (size > mincap)? size : mincap;

	int64_t* tmp = arr->data;
	arr->data    = malloc(sizeof(int64_t) * size);
	for(int i = 0; i < arr->size; i++) arr->data[i] = tmp[i];
	arr->data    = tmp;

	arr->capacity = mincap;
}


int appendDynArr(DynArr* arr, int64_t val){
	if(arr->size+1 >= arr->capacity) growDynArr(arr, 0);
	arr->data[arr->size] = val;
	arr->size++;
	return arr->size-1;
}


void resizeDynArr(DynArr* arr, int size){
	if(size >= arr->capacity) growDynArr(arr, size*2);

	for(int i = arr->size-1; i < size; i++) arr->data[i] = 0;

	arr->size = size;
}


void insertDynArr(DynArr* arr, int64_t val, int ix){
	if(ix >= arr->size) resizeDynArr(arr, ix+1);
	arr->data[ix] = val;
}


void cleanDynArr(DynArr* arr){
	free(arr->data);
}