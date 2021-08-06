#ifndef _MEM_MALLOC_H
#define _MEM_MALLOC_H

#ifdef __cplusplus
extern "C"  {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#pragma pack(1)
typedef struct mem_block
{
  void *mem_ptr;            // 内存单元地址
  unsigned int mem_size;    // 分配空间
  unsigned int mem_index;   // 索引
} mem_block;
#pragma pack()


#define MEM_SIZE            128

void print_mem_info(void);
void print_hex(char *data, int len);
void print_mem_hex(int size);
int mem_malloc(unsigned int msize);
int mem_realloc(int id, unsigned int msize);
void *mem_buffer(int id);
int mem_free(int id);

#ifdef __cplusplus
}
#endif

#endif
