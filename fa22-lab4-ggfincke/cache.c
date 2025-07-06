#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "cache.h"
#include "jbod.h"

static cache_entry_t *cache = NULL;
static int cache_size = 0;
static int num_queries = 0;
static int num_hits = 0;
//int created keeping track of if cache is created
int created = 0;

//create cache
int cache_create(int num_entries)
{
  //if already created or out of bounds return -1
  if (created == 1 || num_entries < 2 || num_entries > 4096)
    {
      return -1;
    }
  else
    {
      //void calloc to create cache for cache_entry_t
      created = 1;
      cache = (void*)calloc(num_entries, sizeof(cache_entry_t));
      cache_size = num_entries;
    }
  return 1;
}

//destroy cache
int cache_destroy(void)
{
  //if not created return -1
  if (created == 0)
    {
      return -1;
    }
  //else free cache and set to NULL
  else
    {
      created = 0;
      free(cache);
      cache = NULL;
      cache_size = 0;
    }
  return 1;
}

//cache lookup function; returns 1 on success and -1 on failure
//buffer copies the information from the lookup
int cache_lookup(int disk_num, int block_num, uint8_t *buf)
{
  //cases where will not be able to work; cache[0].num_accesses = 0 means that cache is empty
  if (created == 0 || buf == NULL || cache[0].num_accesses == 0)
    {
      return -1;
    }
  //num_queries is incremented on each lookup call
  num_queries++;
  //for loop goes through cache, searches for equal disk and block num
  for (int i = 0; i < cache_size; i++)
    {
      if ((cache[i]).disk_num == disk_num)
	{
	  //if found, increments accesses/hits, and copies block contents into temp buf 
	  if ((cache[i]).block_num == block_num)
	    {
	      memcpy(&buf[0],(cache[i]).block, 256);
	      cache[i].num_accesses++;
	      num_hits++;
	      return 1;
	    }
	}
    }
  //if not found returns -1
  return -1;
}

//cache_update function that updates cache based on given buffer
void cache_update(int disk_num, int block_num, const uint8_t *buf)
{
  //for loop goes through cache searching for disk/block ID
  for (int i = 0; i < cache_size; i++)
    {
      if ((cache[i]).disk_num == disk_num)
	{
	  //if found, updates block in cache and increments accesses
	  if ((cache[i]).block_num == block_num)
	    {
	      memcpy((cache[i]).block, buf, 256);
	      (cache[i]).num_accesses++;
	    }
	}
    }   
}

//cache_insert inserts a block into the cache 
int cache_insert(int disk_num, int block_num, const uint8_t *buf)
{
  //various cases
  if (buf == NULL || sizeof(buf) > 256 || disk_num > 15 || disk_num < 0 || block_num > 255 || block_num < 0 || created == 0)
    {
      return -1;
    }

  //lowestindex finds index of LFU
  int lowestIndex = 0; 
  for (int i = 0; i < cache_size; i++)
    {
      //if not accessed (=0 because of calloc)
      if ((cache[i]).num_accesses == 0)
	{
	  //creating cache entry
	  (cache[i]).valid = 1;
	  (cache[i]).disk_num = disk_num;
	  (cache[i]).block_num = block_num;
	  memcpy((cache[i]).block, buf, 256);
	  (cache[i]).num_accesses = 1;
	  return 1;
	}

      //setting lowest index
      if (cache[lowestIndex].num_accesses > cache[i].num_accesses) //if lowest access is less than the current lowest access in for loop, replaces  
	{
	  lowestIndex = i;
	}

      //if already in cache fail
      if ((cache[i]).disk_num == disk_num) // 
	{
	  if ((cache[i]).block_num == block_num)
	    {
	      return -1;
	    }
	}
    }
  
  //if getting through the whole loop, uses lowestIndex to evict LFU
  cache[lowestIndex].disk_num = disk_num;
  cache[lowestIndex].block_num = block_num;
  memcpy(cache[lowestIndex].block, buf, 256);
  cache[lowestIndex].num_accesses = 1;
  return 1;
}


//untouched
bool cache_enabled(void)
{
	return cache != NULL && cache_size > 0;
}

void cache_print_hit_rate(void)
{
	fprintf(stderr, "num_hits: %d, num_queries: %d\n", num_hits, num_queries);
	fprintf(stderr, "Hit rate: %5.1f%%\n", 100 * (float) num_hits / num_queries);
}
