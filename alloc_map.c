#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "alloc_map.h"

#define Byte_index(siz,idx) (siz / idx)
#define Bit_index(siz,idx) (siz % idx)
#define On(a) 1UL << (a)
#define Off(a) ~(1UL << (a))


struct alloc_map 
{
  void * start_addr;
  size_t word_size;
  size_t map_size;
  uint8_t bits[];
};

static void *
alloc_map_create_get_start_addr(size_t size)
{
  void *p = malloc(size);
  return p;
}

alloc_map_t *
alloc_map_create(void *start_addr, size_t word_size, size_t block_size)
{
  alloc_map_t *alloc_map = alloc_map_create_get_start_addr((block_size/word_size)+2*sizeof(size_t)+sizeof(void*));
  alloc_map->start_addr = start_addr;
  alloc_map->word_size = word_size;
  alloc_map->map_size = (block_size/word_size);
  for(size_t i = 0; i <= (alloc_map->map_size/alloc_map->word_size); ++i){
    alloc_map->bits[i] = 0;
  }
  return alloc_map;
}

static size_t 
alloc_map_check_offset(alloc_map_t *alloc_map, void *ptr){
  size_t mem_offset = (size_t)ptr - (size_t)alloc_map->start_addr;
  if(((mem_offset % (alloc_map->word_size)) == 0) && 
     (alloc_map->map_size > (mem_offset/alloc_map->word_size)) &&
     ((size_t)ptr >= (size_t)alloc_map->start_addr))
    {
      return mem_offset;
    }
  else 
    {
      return -1;
    }
}
 
bool 
alloc_map_ptr_used(alloc_map_t *alloc_map, void *ptr)
{
  size_t mem_offset = alloc_map_check_offset(alloc_map, ptr);
  if(mem_offset == (size_t)-1){
    assert(false && "Memory address out of scope");
    return false;
  }
  return alloc_map->bits[mem_offset / alloc_map->word_size] & On(mem_offset % alloc_map->word_size);
}


bool
alloc_map_set(alloc_map_t *alloc_map, void *ptr, bool state)
{
  size_t mem_offset = alloc_map_check_offset(alloc_map, ptr);
  if(mem_offset == (size_t)-1){
    assert(false && "Memory address out of scope");
    return false;
  }
  if(state)
    {
      alloc_map->bits[mem_offset / alloc_map->word_size] |= On(mem_offset % alloc_map->word_size);
    }
  else
    {
      alloc_map->bits[mem_offset / alloc_map->word_size] &= Off(mem_offset % alloc_map->word_size);
    }
  return true;
}




/*  
bitset_t *bs_new(size_t siz)
{
  bitset_t *b = calloc(1, siz + sizeof(size_t));
  return b;
}
void bs_free(bitset_t *b)
{
  free(b);
}

bool bs_contains(bitset_t *b, size_t v)
{
  Index_check(v, b->size / 8);
  return b->bits[v / 8] & On(v % 8);
}
void bs_set(bitset_t *b, size_t v)
{
  Index_check(v, b->size / 8);
  b->bits[v / 8] |= On(v % 8);
}
void bs_unset(bitset_t *b, size_t v)
{
  Index_check(v, b->size / 8);
  b->bits[v / 8] &= Off(v % 8);
}
*/
