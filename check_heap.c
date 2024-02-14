
#include "umalloc.h"

//Place any variables needed here from umalloc.c as an extern.
extern memory_block_t *free_head;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 *
 * STUDENT TODO:
 * Required to be completed for checkpoint 1:
 *
 *      - Ensure that the free block list is in the order you expect it to be in.
 *        As an example, if you maintain the free list in memory-address order,
 *        lowest address first, ensure that memory addresses strictly ascend as you
 *        traverse the free list.
 *
 *      - Check if any free memory_blocks overlap with each other. 
 *
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // Example heap check:
    // Check that all blocks in the free list are marked free.
    // If a block is marked allocated, return -1.
    /*
        memory_block_t *cur = free_head;
        while (cur) {
            if (is_allocated(cur)) {
                return -1;
            }
        }
    */

   memory_block_t *cur = free_head;
   memory_block_t *next = get_next(cur);

   //alignment
   if ((get_size(free_head) % 16) != 0) {
    return -1;
   }

   //expected order
   while (next != NULL) {
    if (cur >= next) {
        return -1;
    }
    
    //check overlap
    char *endCur = (char *)cur + get_size(cur);
    char *startNext = (char *)next;
    if (endCur >  startNext) {
        return -1;
    }



    



   }
    return 0;
}