/* useful tools.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#ifndef TOOLS_H
#define TOOLS_H

#include "system.h"

void fatalError(const char *message);
//a fatal error happened. Handle it and exit the compiler.

/* functions below is used for storage management.
* It works as a memory pool.
* Do not use the C library malloc()/free(), it need too much system call.
*/
#define MM_BASE_BUFF    (1 << 20) /* initlize memory buffer base size:1MB   */
#define MM_INC_BUFF     (1 << 19) /* memory buffer increament size   :512KB */
#define MM_HEADER_COUNT (1 << 10)

struct block_header {
    unsigned char *place;
    unsigned char *limit;
    struct block_header *next;
    size_t thisSize;
};

struct block_list {
    struct block_header *firstBlock; /*header block pointer*/
    struct block_header *lastBlock;
    unsigned char *buffer;
    size_t size;
    size_t alreadyInUsingSize;
    int allocatedBlockNum;
};

union align {
    long l;
    char *cp;
    double d;
    int (*fp)(void);
};

extern struct block_list *memoryPool;
extern struct block_header *header;

void initMemoryManagement(void);
//initlize the memory management system.

void *allocateMemory(size_t size);
//allocate memory. It works as malloc()

void deallocateMemory(void *toFree);
//de-allocate memory. It works as free()


namespace utils
{

    unsigned long strHash(const char *);
}

#endif