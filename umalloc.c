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
    //* STUDENT TODO
    memory_block_t *cur = free_head;
    bool found = false;
    while (!found) {
        if (get_size(cur) >= size) {
            found = true;
            return cur;
        }
        else {
            cur = get_next(cur);
        }
    }
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    //* STUDENT TODO

    // ??? THIS HOW TO DO IT???
    return csbrk(size);
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //* STUDENT TODO
    unsigned long blockSize = get_size;
    unsigned long unallocatedSize = blockSize - size - 16;
    
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    //* STUDENT TODO


    // DID THIS AS FIRST STEP
    block = firstFreeBlock(block);
    

    uint64_t newSize = get_size(block);
    memory_block_t *next = get_next(block);
    while (!is_allocated(next)) {
        newSize += (16 + get_size(next));
        next = get_next(next);
    }
    block->block_metadata &= ~(0xFFFFFFFFFFFFFFFULL);
    block->block_metadata |= (newSize & 0xFFFFFFFFFFFFFFFULL);
    return block;

}


// WROTE THIS TO GET FIRST FREE BLOCK THAT IS CONTIGUOSLY CONNECTED
// THROUGH OTHER FREE BLOCKS TO 'block'
memory_block_t *firstFreeBlock(memory_block_t *block) {
    memory_block_t *cur = free_head;
    memory_block_t *lastFreeblock = get_next(cur);
    bool beforeBlock = false;


    while (!beforeBlock) {
        if (cur == block) {
            return block;
        }
        else if (!is_allocated(cur)) {
            bool contiguous = true;
            while (contiguous) {
                if (lastFreeblock == block) {
                    return cur;
                }

                if (!is_allocated(lastFreeblock)) {
                    lastFreeblock = get_next(lastFreeblock);
                } else {
                    cur = get_next(lastFreeblock);
                    lastFreeblock = get_next(cur);
                    contiguous = false;
                }
            }
        }
        else {
            cur = get_next(cur);
            lastFreeblock = get_next(cur);
        }
    }
    
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    //* STUDENT TODO
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    //* STUDENT TODO
    return NULL;
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    //* STUDENT TODO
}
