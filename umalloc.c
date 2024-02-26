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
    //printf("put_block size:%ld\n", size);
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
    //printf("in getBlock\n");

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

    //printf("in find\n");
    //* STUDENT TODO
    memory_block_t *cur = free_head;
    //memory_block_t *bestBlock = cur;
    //bool found = false;
    while (cur != NULL) {
        //printf("cur:%p\n", cur);
        if (get_size(cur) >= size) {
            //printf("exiting while");
            return cur;
        }
        else {
            cur = get_next(cur);
        }
        // else if (get_size(cur) > size) {
        //     found = true;
        //     if ((size - get_size(cur)) <= (size - get_size(bestBlock))) {
        //         bestBlock = cur;
        //     }
        // }
        // else {
        //     cur = get_next(cur);
        // }
    }
    // if (found) {
    //     return bestBlock;
    // }
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    //printf("in extend\n");

    //* STUDENT TODO
    //4096
    // SHOULD I DO +16 HERE>>>MAKEW SURE YOU CAN!!!!!!!!!!!!!!!!!!!!!!!!!!!
    memory_block_t *rand = (memory_block_t *)csbrk(size);
    put_block(rand, size, false);
    return rand;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //printf("in split\n");

    //* STUDENT TODO
    //printf("split size 1: %ld\n", size);
    // unsigned long mod = size % ALIGNMENT;
    // size += mod;
    size = ALIGN(size);
    //printf("split size 2: %ld\n", size);
    

    //keeping left portion of free block free    // B = H + s  + H + S
                                                // s = B - H - H - S
                                                // s = get_size - H - S
    unsigned long freeBlockSize = get_size(block) - size - ALIGNMENT; 
    block->block_metadata = freeBlockSize | false;

    char *allocatedAddress = (char *)(block + 1) + get_size(block);
    memory_block_t *allocatedBlock = (memory_block_t *)allocatedAddress;
    put_block(allocatedBlock, size, true);
    return allocatedBlock;
}

// goes through entire free list - NOT GIVEN
void customCoalesce() {
    memory_block_t *cur = free_head;
    memory_block_t *next = get_next(free_head);

    while (next != NULL) {
        memory_block_t  *nextAddress = (memory_block_t *)((char *)(cur + 1) + get_size(cur));
        if (nextAddress == next) {
            cur->block_metadata = (get_size(cur) + ALIGNMENT + get_size(next)) | false;
            cur->next = get_next(next);
        } else {
            cur = next;
            next = get_next(next);
        }
    }
    
}
/*
 * coalesce - coalesces a free memory block with neighbors. - GIVEN
 */
memory_block_t *coalesce(memory_block_t *block) {
    //* STUDENT TODO
    memory_block_t *cur = free_head;
    memory_block_t *next = get_next(free_head);
    memory_block_t *firstBlock = block;
    size_t newSize = 0;

    //neighbors before 'block'
    while (cur < block) {
        memory_block_t  *nextAddress = (memory_block_t *)((char *)(cur + 1) + get_size(cur));
        if (nextAddress == next) {
               if (firstBlock == block) {
                    firstBlock = cur;
                    newSize += get_size(cur);
                } else {
                    newSize += get_size(cur) + ALIGNMENT;
                }
        } else {
            newSize = 0;
            firstBlock = block;
        }
        cur = next;
        next = get_next(cur);
    }

    // count 'block'
    if (firstBlock != block) {
        newSize += get_size(block) + ALIGNMENT;
    } else {
        newSize += get_size(block);
    }
    
    // neighbors after 'block'
    while (next != NULL) {
        memory_block_t  *nextAddress = (memory_block_t *)((char *)(cur + 1) + get_size(cur));
        if (nextAddress == next) {
            newSize += get_size(next) + ALIGNMENT;
            cur = next;
            next = get_next(next);
        } else {
            break;
        }
    }
    
    firstBlock->next = next;
    firstBlock->block_metadata = newSize | false;
    return firstBlock;

}


// WROTE THIS TO GET FIRST FREE BLOCK THAT IS CONTIGUOSLY CONNECTED
// THROUGH OTHER FREE BLOCKS TO 'block'
// memory_block_t *firstFreeBlock(memory_block_t *block) {
//     memory_block_t *cur = free_head;
//     memory_block_t *lastFreeblock = get_next(cur);
//     bool beforeBlock = false;


//     while (!beforeBlock) {
//         if (cur == block) {
//             return block;
//         }
//         else if (!is_allocated(cur)) {
//             bool contiguous = true;
//             while (contiguous) {
//                 if (lastFreeblock == block) {
//                     return cur;
//                 }

//                 if (!is_allocated(lastFreeblock)) {
//                     lastFreeblock = get_next(lastFreeblock);
//                 } else {
//                     cur = get_next(lastFreeblock);
//                     lastFreeblock = get_next(cur);
//                     contiguous = false;
//                 }
//             }
//         }
//         else {
//             cur = get_next(cur);
//             lastFreeblock = get_next(cur);
//         }
//     }
    
//}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    //printf("in uinit\n");
    //* STUDENT TODO
    free_head = extend(PAGESIZE);
    if (free_head == NULL) {
        return -1;
    }
    return 0;
}

memory_block_t *getPrevBlock(memory_block_t *block) {
    if (block == free_head) {
        return NULL;
    }
    memory_block_t *prev = free_head;
    memory_block_t *cur = get_next(free_head); // free_head->next
    bool found = false;
    while (!found) {
        if (cur == block) {
            found = true;
            return prev;
        }
        else {
            prev = cur;
            cur = get_next(cur);
        }
    }
    return NULL;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    //* STUDENT TODO
    //printf("in umalloc\n");

    memory_block_t *bestBlock = find(size);
    if (bestBlock == NULL) {
        bestBlock = extend(PAGESIZE);
        memory_block_t *tail = free_head;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = bestBlock;
    
    }
    
    if (get_size(bestBlock) != size) {
        return split(bestBlock, size);
    }
    else {
        allocate(bestBlock);
        bestBlock->next = NULL;
        return bestBlock;
    }
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    memory_block_t *toBeFreed = get_block(ptr);
    if (is_allocated(toBeFreed)) {
        deallocate(toBeFreed);
    }
    else {
        return;
    }
    

    if (toBeFreed < free_head) {
        toBeFreed->next = free_head;
        free_head = toBeFreed;
    }
    else {
        memory_block_t *cur = free_head;
        memory_block_t *next = get_next(cur);

        bool foundPrev = false;
        while (!foundPrev) {
            if (next == NULL) {
                break;
            }
            if (cur < toBeFreed && toBeFreed < next) {
                foundPrev = true;
                toBeFreed->next = next;
                cur->next = toBeFreed;
            } 
            else {
                cur = next;
                next = get_next(next);
            }
        }
        if (!foundPrev) {
            cur->next = toBeFreed;
            toBeFreed->next = NULL;
        }
        
    }
    
}
