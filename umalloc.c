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
    
    size = ALIGN(size);
    int i = get_size(block) + ALIGNMENT;

    //keeping left portion of free block free    // B = H + s  + H + S
                                                // s = B - H - H - S
                                                // s = get_size - H - S
    unsigned long freeBlockSize = get_size(block) - size - ALIGNMENT; 
    block->block_metadata = freeBlockSize | false;
    printf("Block size: %ld, freeBlockSize: %ld, fullBlockSize: %d  metadata: %ld \n",
     size, freeBlockSize, i, block->block_metadata);

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


 // have next 
 // have cur,
 // have starting block
 // have size = 0;
 // if next = 'block being freed',
 // check if current is unallocated
 //     if it is make it the 'starting block', and size = sizeOf(cur) + header and size of 'block being freed'
 // if cur is allocated, then make the 'starting block' = 'block being freed', and size = size of 'block being freed'

 // now check if the address of the next free block is right after 'block being freed'
 // if it is then size += header and size of next free block

 // now at the end make size of starting block = size
 // and make next of starting block, the next of the last freed block
 */
memory_block_t *coalesce(memory_block_t *block) {
    //* STUDENT TODO
    // memory_block_t *cur = free_head;
    // memory_block_t *next = get_next(free_head);
    // memory_block_t *firstBlock = block;
    // size_t newSize = 0;

    // //neighbors before 'block'
    // while (cur < block) {
    //     if (next == block) {
    //         memory_block_t  *nextAddress = (memory_block_t *)((char *)(cur + 1) + get_size(cur));
    //         if (nextAddress == block) {
    //             firstBlock = cur;
    //             newSize = get_size(cur);
    //         }
    //         break;
    //     }
    //     cur = next;
    //     next = get_next(next);
    // }

    // //count 'block'
    // if (firstBlock != block) {
    //     newSize += get_size(block) + ALIGNMENT;
    // } else {
    //     newSize += get_size(block);
    // }

    // next = get_next(block);
    // memory_block_t *nextAddress = (memory_block_t *)((char *)(block + 1) + get_size(block));
    // if (next == nextAddress) {
    //     newSize += ALIGNMENT + get_size(next);
    //     firstBlock->next = get_next(next);
    // } else {
    //     firstBlock->next = next;
    // }

    // firstBlock->block_metadata = newSize | false;
    // return firstBlock;
    
    //size_t newSize = 0;
    memory_block_t* nextAdressOfBlock = (memory_block_t *)((char *)(block + 1) + get_size(block));
    if (block < free_head) {
        if (nextAdressOfBlock == free_head) {
            block->next = free_head->next;
            block->block_metadata += ALIGNMENT + get_size(free_head);
        } else {
            block->next = free_head;
        }
        free_head = block;
    }
    else {
        memory_block_t *prevBlock = free_head;
        memory_block_t *nextBlock = get_next(prevBlock);

        bool foundPrev = false;
        while (!foundPrev) {
            if (nextBlock == NULL) {
                break;
            }
            if (prevBlock < block && block < nextBlock) {
                foundPrev = true;
                memory_block_t *nextAddress = (memory_block_t *)((char *)(prevBlock + 1) + get_size(prevBlock));
                if (nextAddress == block) {
                    prevBlock->block_metadata += ALIGNMENT + get_size(block);
                    block = prevBlock;
                } else {
                    prevBlock->next = block;
                    block->next  = nextBlock;
                }

                if (nextAdressOfBlock == nextBlock) {
                    block->next = nextBlock->next;
                    block->block_metadata += ALIGNMENT + get_size(block);
                } 
            } 
            else {
                prevBlock = nextBlock;
                nextBlock = get_next(nextBlock);
            }
        }
        if (!foundPrev) {
            memory_block_t *nextAddress = (memory_block_t *)((char *)(prevBlock + 1) + get_size(prevBlock));
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
    size = ALIGN(size);
    // if (size > PAGESIZE) {
    //     extend(size);
    // }
    // else {}
    memory_block_t *bestBlock = find(size);
    if (bestBlock == NULL) {
        if (size > PAGESIZE) {
            bestBlock = extend(size);
        } else {
            bestBlock = extend(PAGESIZE);
        }
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
        printf("found exact match\n");
        memory_block_t *prev = free_head;
        memory_block_t *next = get_next(prev);
        while(next != NULL) {
            if (next == bestBlock) {
                prev->next = bestBlock->next;
                break;
            }
            prev = next;
            next = get_next(next);
        }

        //bestblock.prev = bestblock.next
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
    deallocate(toBeFreed);
    
    // if (toBeFreed < free_head) {
    //     toBeFreed->next = free_head;
    //     free_head = toBeFreed;
    // }
    // else {
    //     memory_block_t *cur = free_head;
    //     memory_block_t *next = get_next(cur);

    //     bool foundPrev = false;
    //     while (!foundPrev) {
    //         if (next == NULL) {
    //             break;
    //         }
    //         if (cur < toBeFreed && toBeFreed < next) {
    //             foundPrev = true;
    //             toBeFreed->next = next;
    //             cur->next = toBeFreed;
    //         } 
    //         else {
    //             cur = next;
    //             next = get_next(next);
    //         }
    //     }
    //     if (!foundPrev) {
    //         cur->next = toBeFreed;
    //         toBeFreed->next = NULL;
    //     }
        
    //}

    coalesce(toBeFreed);
    
}
