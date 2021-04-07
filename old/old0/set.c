#include "struct.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"






/*
	This is an implementation of a simple set data structure.
	
	For now, only insert, inclusion search, intersection, and union will be supported.
	
	It's also aimed at high performance on small sets. I don't think it will be necessary
	to generalize it to large numbers of elements. A slightly different implementation may
	be used in that case. Small sets are going to be the common case for most compiler
	operations.
*/

int checkBloom(Set32* s, int32_t n){
	uint64_t x = n;
	return ((s->bloom[0] & (1l <<         (x % 64) )) != 0) &
		   ((s->bloom[1] & (1l << ((x/64)+(x % 64)))) != 0);
}



void insertBloom(Set32* s, int32_t n){
	uint64_t x = n;
	s->bloom[0] |= (1l <<         (x % 64) );
	s->bloom[1] |= (1l << ((x/64)+(x % 64)));
}



Set32 initSet32  (int cap){
	Set32 ret;
	ret.vals     = malloc(sizeof(int32_t) * cap);
	ret.size     = 0;
	ret.cap      = cap;
	ret.bloom[0] = 0;
	ret.bloom[1] = 0;
	return ret;
}



int   expandSet32(Set32* s, int cap){
	if(cap > s->cap){
		int32_t* tmp = s->vals;
		s->vals      = malloc(sizeof(int32_t) * cap);
		for(int i = 0; i < s->size; i++) tmp[i] = s->vals[i];
		free(tmp);
		return 1;
	}
	return 0;
}



int   insertSet32(Set32* s, int32_t x){
	if(checkBloom(s, x)){
		// x might be in s, so we need to scan first.
		for(int i = 0; i < s->size; i++) if(s->vals[i] == x) return 0;
	}else{
		// x is not in s, just insert at the end
		insertBloom(s, x);
	}
	if((s->size+1) >= s->cap) expandSet32(s, s->cap * 2);
	s->vals[s->size] = x;
	s->size++;
	return 1;
}



int   checkSet32 (Set32* s, int32_t x){
	if(checkBloom(s, x))
		for(int i = 0; i < s->size; i++) if(s->vals[i] == x) return 1;
	return 0;
}



void  copySet32(Set32* dst, Set32* src){
	if(dst->cap < src->size){
		free(dst->vals);
		dst->vals = malloc(sizeof(int32_t) * src->cap);
		dst->cap  = src->cap;
	}
	for(int i = 0; i < src->size; i++) dst->vals[i] = src->vals[i];
	dst->size = src->size;
	dst->bloom[0] = src->bloom[0];
	dst->bloom[1] = src->bloom[1];
}



Set32 intersect32(Set32 a, Set32 b){
	Set32 ret = initSet32(a.cap);
	if(a.size > b.size){
		// Sets aren't sorted here to minimize the cost of insertions.
		// Bloom filters reduce cost of checks (assuming the size of the set is small).
		// This is kind of a nitpick, but I'm pretty sure things will go faster if we
		// check against the smaller array. Fewer false positives in the bloom filter.
		Set32 tmp = a;
		b = a;
		a = tmp;
		if(a.size <= 0) return ret;	// Empty set, who cares?
	}
	
	for(int i = 0; i < b.size; i++)
		if(checkSet32(&a, b.vals[i]))
			insertSet32(&ret, b.vals[i]);
	
	return ret;
}



Set32 union32    (Set32 a, Set32 b){
	Set32 ret = initSet32((a.size + b.size) * 2);
	if(a.size > b.size){
		Set32 tmp = a;
		b = a;
		a = tmp;
	}
	
	copySet32(&ret, &b);
	if(a.size <= 0) return ret;
	
	for(int i = 0; i < a.size; i++)
		if(!checkSet32(&b, a.vals[i])) insertSet32(&ret, a.vals[i]);
	
	return ret;
}


void printSet32(Set32 s){
	float ap  = __builtin_popcountl(s.bloom[0]);
	float bp  = __builtin_popcountl(s.bloom[1]);
	printf("%f %f\n", ap, bp);
	float acc = ap * bp / 40.96;
	printf("SET 32 : %i / %i : %f%%\n", s.size, s.cap, acc);
	for(int i = 0; i < s.size; i++) printf("  %i\n", s.vals[i]);
}
