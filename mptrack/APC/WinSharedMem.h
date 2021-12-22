#ifndef _WIN_SHARED_MEM_H_
#define _WIN_SHARED_MEM_H_

/**
 *  \brief Opens or creates a shared memory block.
 * 
 *  \param name - Identifier, global for the user's computer.
 *  \param size - Size of the shared memory buffer in bytes.
 *  \param existsFlag (optional) - Pointer to a char flag to indicate whether
 *                                 the memory was already allocated previously.
 * 
 *  \returns Address to the shared memory buffer, 0 on failure.
**/
void *SharedMem_Open(const char *name, unsigned long long size, char *existsFlag = 0);

#endif