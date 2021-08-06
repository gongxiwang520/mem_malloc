#include "mem_malloc.h"

/*
  文章参考链接：https://blog.csdn.net/ybhuangfugui/article/details/118714352?spm=1001.2014.3001.5501
*/

char mem_id[10]={0};  // 10块内存块
 
void test_malloc(int i, int size)
{
 printf("------test_malloc-------\n");
 mem_id[i] = mem_malloc(size);
 if(mem_id[i] == 0)
 {
  printf("malloc --- fail\n");
  printf("size=%d\n", size);
 }
 else
 {
  char *p = mem_buffer(mem_id[i]);
        memset(p, i, size);
        printf("p = 0x%x, i=%d, id=%d, size=%d\n", (int)p, i, mem_id[i], size);
 }
 print_mem_hex(MEM_SIZE);
}
 
void test_buffer(int i, int size)
{
 printf("------test_buffer-------\n");
 printf("i=%d, id = %d, size=%d\n", i, mem_id[i], size);
 char *p = mem_buffer(mem_id[i]);
    if(p != NULL)
 {
  memset(p, 0xf0+i, size);
        print_mem_hex(MEM_SIZE);
 }
 else
 {
  printf("test_buffer---fail\n");
 }
}
 
void test_realloc(int i, int size)
{
 printf("------test_realloc-------\n");
    printf("i=%d, id = %d, size=%d\n", i, mem_id[i], size);
 int ret = mem_realloc(mem_id[i], size);
 if(ret)
 {
  char *p = mem_buffer(mem_id[i]);
  memset(p, 0xa0+i, size);
        print_mem_hex(MEM_SIZE);
 }
 else
 {
  printf("test_realloc---fail\n");
 }
}
 
void test_free(int i)
{
 printf("------test_free-------\n");
 printf("i=%d, id = %d\n", i, mem_id[i]);
 if(mem_free(mem_id[i]))
  print_mem_hex( MEM_SIZE);
}
 
void main(void)
{
 print_mem_info();   // 打印内存信息
 test_malloc(1, 10); // 给申请一块10个字节的内存，标记内存块id为1
 test_malloc(2, 8); // 给申请一块8个字节的内存，标记内存块id为2
 test_malloc(3, 20); // 给申请一块20个字节的内存，标记内存块id为2
 
 test_free(2);  // 释放id为2的内存块的内存
 
 test_malloc(4, 70); // 申请一块70个字节的内存
 
 test_free(1);       // 释放id为1的内存块内存
 
 test_buffer(3, 20); // 获取id为3的内存块地址，并往这个内存块重新写入0xf0+i的数据
 
 test_realloc(3, 10); // 重新分配内存，并往这个内存块重新写入0xa0+i的数据
 
 for(int i=0; i<10; i++)  // 释放所有内存块内存，已释放的不再重新释放
  test_free(i);
} 