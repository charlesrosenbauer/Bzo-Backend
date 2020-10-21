#include "hashtable.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "util.h"








HashTable makeHashTable(int capacity, size_t size){
  HashTable ret;
  ret.entrycount = capacity;
  ret.stride     = size;
  ret.entries    = malloc(sizeof(HashEntry) * capacity);
  ret.stack      = malloc(size              * capacity);
  ret.stacktop   = 0;
  for(int i = 0; i < capacity; i++) ret.entries[i].val = NULL;
  return ret;
}




uint64_t hash(uint64_t x){
  int rot = x >> 56;
  x  = 317850371801 + (13571801537 * x);
  x  = (x >> rot) | (x << (64 - rot));
  x  = 146171441379 + (92783469991 * x);
  x ^= (__builtin_popcountl(x) & 1)? 3137108781 : 1083701035;
  return x;
}



int insertHashTable(HashTable* tab, uint64_t key, void* value){
  if(tab->stacktop >= tab->entrycount){
    return 1;
  }
  memcpy(&tab->stack[tab->stacktop * tab->stride], value, tab->stride);

  // Insert at location
  uint64_t ix = hash(key) % tab->entrycount;
  while(tab->entries[ix].val != NULL){
    ix++;
    ix = (ix >= tab->entrycount)? 0 : ix;
  }
  tab->entries[ix].val = &tab->stack[tab->stacktop * tab->stride];
  tab->entries[ix].key =  key;

  tab->stacktop++;
  return 0;
}



void* lookupHashTable(HashTable* tab, uint64_t key){
  uint64_t ix = hash(key) % tab->entrycount;
  while(tab->entries[ix].val != NULL){
    if(tab->entries[ix].key == key){
      return tab->entries[ix].val;
    }
    ix++;
    ix = (ix >= tab->entrycount)? 0 : ix;
  }

  return NULL;
}




uint64_t strToHash(String s){
  uint64_t accumulator = 0x7f3191aef93f490f;
  for(int i = 0; i < s.size; i+=8){
    if((s.size - i) < 8){
      for(int j = i; j < s.size; j++){
        accumulator ^= (s.text[j] << (8 * (j - i)));
      }
    }else{
      uint64_t txt = *(uint64_t*)(&s.text[i]);
      accumulator ^= txt;
    }
    int rot = accumulator >> 58;
    accumulator  = (accumulator << rot) | (accumulator >> (64 - rot));
    accumulator += 5319803817053985;
  }
  return accumulator;
}
