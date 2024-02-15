#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "cache.h"
#include "jbod.h"


static cache_entry_t *cache = NULL;
static int cache_size = 0;
static int clock = 0;
static int num_queries = 0;
static int num_hits = 0;


int cache_create(int num_entries) {
  if(cache != NULL && cache_size > 0)
    return -1;



  if (num_entries < 2 || num_entries > 4096){
    return -1;
  }
  clock = 0;
  num_entries = num_entries;
  num_hits =0;

  cache = (cache_entry_t *) calloc(num_entries, sizeof(cache_entry_t));

    cache_size = num_entries;
    for(int i = 0; i<cache_size; i++){
      cache[i].valid=false;
    }
      return 1;


} 


int cache_destroy(void) {
  if(cache != NULL){
    cache=NULL;
    cache_size = 0;
  
  return 1;

  }
  free(cache);
  cache=NULL;

  return -1;
}


int cache_lookup(int disk_num, int block_num, uint8_t *buf) {
  //printf("lookup \n" );
  if(cache == NULL){
    printf("check 4\n");
    return -1;
  }
  if(buf == NULL){
    //printf("check 5\n");
    return -1;
  }
  if(disk_num >= JBOD_NUM_DISKS || block_num >= JBOD_NUM_BLOCKS_PER_DISK || disk_num < 0 || block_num < 0)
    return -1;
  num_queries ++;
  for(int i = 0; i<cache_size; i++){
    //printf("check 3\n");
    if(cache[i].valid && cache[i].disk_num == disk_num && cache[i].block_num == block_num){
      cache[i].clock_accesses = ++clock;
      memcpy(buf, cache[i].block, JBOD_BLOCK_SIZE);
      num_hits++;
      return 1;
         
         }
}

return -1;
}



void cache_update(int disk_num, int block_num, const uint8_t *buf) {
  if(cache == NULL)return;
  if(buf == NULL)return;
  if(disk_num >= JBOD_NUM_DISKS || block_num >= JBOD_NUM_BLOCKS_PER_DISK || disk_num < 0 || block_num < 0)
  return;

  for(int i = 0; i<cache_size; i++){
    if(cache[i].valid && cache[i].disk_num == disk_num && cache[i].block_num == block_num){
      cache[i].clock_accesses = ++clock;
      memcpy(cache[i].block,buf,JBOD_BLOCK_SIZE);
      return;
    }
      
  
    }
  }

 


int cache_insert(int disk_num, int block_num, const uint8_t *buf) {
  //printf("entering here \n\n");
   if(cache == NULL)
    return -1;
  if(buf == NULL)
    return -1;
  if(disk_num < 0 || disk_num >= 16 ||block_num<0||block_num >=256){
  return -1;
  }
  
  for(int i = 0; i<cache_size; i++){
    if(cache[i].block_num == block_num && cache[i].disk_num == disk_num && cache[i].valid == true) {
      //printf("blah \n");
      return -1;
    }
  }
  for(int i = 0; i<cache_size; i++){
    if(cache[i].valid == false){
      cache[i].block_num = block_num;
      cache[i].disk_num = disk_num;
      cache[i].valid = true;
      cache[i].clock_accesses = ++clock;
      memcpy(cache[i].block,buf,JBOD_BLOCK_SIZE);
      //printf("second /n");
      return 1;
  
    }
  
  }
    int mru_index = 0;
    int highest_clock = 0;
    for(int i = 0; i<cache_size; i++){
      if(highest_clock<cache[i].clock_accesses){
        highest_clock = cache[i].clock_accesses;
        mru_index = i;
      }
    }
    //printf("third /n");
      cache[mru_index].block_num = block_num;
      cache[mru_index].disk_num = disk_num;
      cache[mru_index].valid = true;
      cache[mru_index].clock_accesses = ++clock;
      memcpy(cache[mru_index].block,buf,JBOD_BLOCK_SIZE);
      return 1;
     
    }
  

bool cache_enabled(void) {
  return cache != NULL && cache_size > 0;
}

void cache_print_hit_rate(void) {
	fprintf(stderr, "num_hits: %d, num_queries: %d\n", num_hits, num_queries);
  fprintf(stderr, "Hit rate: %5.1f%%\n", 100 * (float) num_hits / num_queries);
}

int cache_resize(int new_num_entries) {
  if(new_num_entries < cache_size || new_num_entries > 4096 || cache == NULL){
    return -1;
  }
  cache = (cache_entry_t *)realloc(cache, new_num_entries * sizeof(cache_entry_t));
  for (int i = cache_size; i< new_num_entries; i++){
    cache[i].valid = true;
    cache[i].clock_accesses = 0;
  }
  cache_size = new_num_entries;
  return cache_size;
}
