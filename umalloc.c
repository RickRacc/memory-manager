#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <assert.h>
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Rakesh Singh, rps2439" ANSI_RESET;

/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;

/*
 * block_metadata - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL);
    return block->block_metadata & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_metadata |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    //printf("Deallocate: %ld \n", block->block_metadata);
    block->block_metadata &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_metadata & ~(ALIGNMENT-1);

    
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
    
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_metadata = size | alloc;
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required. 
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    memory_block_t *cur = free_head;
    // goes through free list and returns the first block with size
    // greater than or equal to 'size', and NULL otherwise
    while (cur != NULL) {
        if (get_size(cur) >= size) {
            return cur;
        } else cur = get_next(cur);
    }
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    // create a new free block of 'size'
    memory_block_t *rand = (memory_block_t *)csbrk(size);
    put_block(rand, size - ALIGNMENT, false);
    return rand;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //make the 'left'/lower memory address side of the block the new free block    
                                                                  // B = H + s  + H + S
                                                                  // s = B - H - H - S
                                                                  // s = get_size - H - S
    unsigned long freeBlockSize = get_size(block) - size - ALIGNMENT; 
    block->block_metadata = freeBlockSize | false;
    
    //make the 'right'/higher memory address side of the block the new allocated block 
    char *allocatedAddress = (char *)(block + 1) + get_size(block);
    memory_block_t *allocatedBlock = (memory_block_t *)allocatedAddress;
    put_block(allocatedBlock, size, true);

    return allocatedBlock;
}


/*
 * coalesce - coalesces a free memory block with neighbors. - GIVEN
 */
memory_block_t *coalesce(memory_block_t *block) {
    //when the free list is empty
    if(free_head == NULL)  {
        free_head = block;
        return free_head;
    }
    void* nextAdressOfBlock = (char *)(block + 1) + get_size(block);
    //when 'block' has a lower memory address than the free_head 
    if (block < free_head) {
        //in case the blocks are right next to each other, merge them
        if (nextAdressOfBlock == free_head) {
            block->next = free_head->next;
            block->block_metadata += ALIGNMENT + get_size(free_head);
        } else block->next = free_head;
        free_head = block;
    } else {
        // getting ready to traverse through free list
        memory_block_t *prevBlock = free_head;
        memory_block_t *nextBlock = get_next(prevBlock);
        bool foundPrev = false;
        while (!foundPrev) {
            //this mean we have reached the end of the free list
            if (nextBlock == NULL) {
                break;
            }
            // in case 'block' is in between two free blocks
            if (prevBlock < block && block < nextBlock) {
                foundPrev = true;
                void* nextAddress = (char *)(prevBlock + 1) + get_size(prevBlock);
                //check if the lower memory address free block and 'block' are
                //right next to each other, if so merge them
                if (nextAddress == block) {
                    prevBlock->block_metadata += ALIGNMENT + get_size(block);
                    block = prevBlock;
                } else {
                    prevBlock->next = block;
                    block->next  = nextBlock;
                }
                // check if 'block' and the higher addressed free block are right next to each other
                // if so merge them
                if (nextAdressOfBlock == nextBlock) {
                    block->next = nextBlock->next;
                    block->block_metadata += ALIGNMENT + get_size(nextBlock);
                } 
            } else {
                prevBlock = nextBlock;
                nextBlock = get_next(nextBlock);
            }
        }
        // in case 'block' occurs after the free list
        if (!foundPrev) {
            void* nextAddress = (char *)(prevBlock + 1) + get_size(prevBlock);
            // if the block at the end of the free list, and 'block' are
            // right next to each other, merge them
            if (nextAddress == block) {
                prevBlock->block_metadata += ALIGNMENT + get_size(block);
                block = prevBlock;
            } else {
                prevBlock->next = block;
                block->next = NULL;
            } 
        }
    }
    return block;
}

/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    free_head = extend(PAGESIZE);
    if (free_head == NULL) {
        return -1;
    }
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    size = ALIGN(size);
    // get the first fit block for 'size'
    memory_block_t *bestBlock = find(size);
    // if that block does not exist, extend the free list accordingly
    if (bestBlock == NULL) {
        size_t sizeToAllocate = size + ALIGNMENT;
        if (sizeToAllocate > PAGESIZE) {
            bestBlock = extend(sizeToAllocate);
        } else bestBlock = extend(PAGESIZE);
        //if the free list is empty, set the free_head to the newly aquired
        //memory blocks in 'bestBlock'
        if (free_head == NULL) {
            free_head = bestBlock;
        }
        //otherwise add the newly aquired block to the end of the free list
        else {
            memory_block_t *tail = free_head;
            while (tail->next != NULL) {
                tail = tail->next;
            }
            tail->next = bestBlock;
        }
    }
    
    // split the 'bestBlock' into a free block and an allocated block
    // of 'size' if it's not a perfect fit, 
    if (get_size(bestBlock) != size) {
        return get_payload(split(bestBlock, size));
    } else {
        allocate(bestBlock);
        // in case the free list is empty, simply return bestBlock
        if(bestBlock == free_head) {
            free_head = NULL;
            return get_payload(bestBlock);
        }

        memory_block_t *prev = free_head;
        memory_block_t *next = get_next(prev);
        // if free list is not empty, remove bestBlock from the free list
        while(next != NULL) {
            if (next == bestBlock) {
                prev->next = bestBlock->next;
                break;
            }
            prev = next;
            next = get_next(next);
        }

        bestBlock->next = NULL;
        return get_payload(bestBlock);
    }
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    memory_block_t *toBeFreed = get_block(ptr);
    deallocate(toBeFreed);
    coalesce(toBeFreed);
    
}
