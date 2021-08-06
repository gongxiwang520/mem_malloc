#include "mem_malloc.h"

static unsigned int sum = 0;
static char mem[MEM_SIZE];

#define DEBUG_EN          0
#define MEM_START         &mem[0]
#define MEM_END           &mem[MEM_SIZE]
#define BLK_SIZE          sizeof(mem_block)

void print_mem_info(void){
	printf("------------mem_info--------------\n");
	printf("sizeof(mem_block)=%d\n", BLK_SIZE);
	printf("MEM_START = %d(0x%x)\n", (int)MEM_START, (int)MEM_START);
	printf("MEM_END   = %d(0x%x)\n", (int)MEM_END, (int)MEM_END);
	printf("MEM_SIZE  = %d(0x%x)\n", (int)MEM_SIZE, (int)MEM_SIZE);
	printf("----------------------------------\n");	
}

void print_hex(char *data, int len){
	for(int i=0; i<len; i++){
		printf("%02x ", (unsigned char)data[i]);
		if((i+1)%12 == 0)   printf("\n");
	}
	printf("\n");
}

void print_mem_hex(int size){
	print_hex(mem, size);
}

/**
 * @brief   从缓存区中申请内存
 * @param   msize：申请的内存块大小
 * @retval  申请的次数，0 -> 申请失败
*/
int mem_malloc(unsigned int msize)
{
  unsigned int all_size = msize + sizeof(mem_block);    // 所需总空间
  mem_block tmp_blk;

  if (msize == 0)
    return 0;

  if (sum)
  {
    mem_block *ptr_blk = (mem_block*)(MEM_START + BLK_SIZE * (sum - 1));    // 上次分配的控制块
    int free_blk = (char *)ptr_blk->mem_ptr - (MEM_START + BLK_SIZE * sum); // 利用上次分配的控制块计算剩余空间

    if (all_size <= free_blk)
    {
      tmp_blk.mem_ptr = ptr_blk->mem_ptr - msize;
      tmp_blk.mem_size = msize;
      tmp_blk.mem_index = ptr_blk->mem_index + 1;
      // 从上次分配的地址往后分配
      memcpy(MEM_START + BLK_SIZE * sum, &tmp_blk, BLK_SIZE);   
      sum += 1;

    #if DEBUG_EN
      printf("mem_ptr = 0x%x\n", (int)tmp_blk.mem_ptr);
      printf("mem_size = 0x%x\n", tmp_blk.mem_size);
      printf("mem_index = 0x%x\n", tmp_blk.mem_index);
    #endif 

      return tmp_blk.mem_index;      
    }
  } 
  else
  {
    if (all_size <= MEM_SIZE)
    {// 第1次分配
      tmp_blk.mem_ptr = MEM_END - msize;    // 控制块放缓存区前面，数据内容放缓存区后面
      tmp_blk.mem_size = msize;
      tmp_blk.mem_index = 1;
      // 从缓存区起始开始分配,
      memcpy(MEM_START, &tmp_blk, BLK_SIZE);  
      sum = 1;

    #if DEBUG_EN
      printf("mem_ptr = 0x%x\n", (int)tmp_blk.mem_ptr);
      printf("mem_size = 0x%x\n", tmp_blk.mem_size);
      printf("mem_index = 0x%x\n", tmp_blk.mem_index);
    #endif  

      return 1;
    }
  }

  return 0; 
}

/**
 * @brief 重新分配某个内存块   
 * @param id：重新分配内存块的id
 * @param msize：重新分配的大小
*/
int mem_realloc(int id, unsigned int msize)
{
  for (int i = 0; i < sum; i++)
  {// i为重分配内存块对应的id - 1
    mem_block *ptr_blk = (mem_block*)(MEM_START + BLK_SIZE * i);

    if (id == ptr_blk->mem_index)
    {
      int free_blk = (char *)ptr_blk->mem_ptr - (MEM_START + BLK_SIZE * sum);
      int old_size = ptr_blk->mem_size; // 上次分配的内存块大小
      int offset = msize - old_size;    // 本次和上次分配的差值
      int move_size = 0;
      int n = sum - i;  // 重分配位置到分配最后的位置有几个内存块
      mem_block *ptr_tmp;

      if (offset == 0)
        return 0;
      else if (offset < 0)
      {
        offset = old_size - msize;
        for (int j = 1; j < n; j++)
        {// move_size：重新分配位置 到 最后的内存块总空间
          ptr_tmp = (mem_block*)(MEM_START + BLK_SIZE * (i + j));
          move_size += ptr_tmp->mem_size;
        }
        
        if (n == 1)
        {// 重分配的内存块为已申请的最后一个内存块
          ptr_tmp = (mem_block*)(MEM_START + BLK_SIZE * i);
        }

        move_size += msize;
        char *dst_addr = ptr_tmp->mem_ptr + move_size + offset - 1;
        char *src_addr = ptr_tmp->mem_ptr + move_size - 1;
      
        for (int j = move_size; j > 0; j--)
        {
          *dst_addr-- = *src_addr--;
        }

        memset(src_addr, 0, offset + 1);
        for (int j = 0; j < n; j++)
        {
          ptr_tmp = (mem_block*)(MEM_START + BLK_SIZE * (i + j));
          ptr_tmp->mem_ptr += offset;
          if (j == 0)
          {
            ptr_tmp->mem_size = msize;
          }
        }

        return 1;
      }
      else  // offset >= 0
      {
        if (offset <= free_blk)
        {
          for (int j = 1; j < n; j++)
          {
            ptr_tmp = (mem_block*)(MEM_START + BLK_SIZE * (i + j));
            move_size += ptr_tmp->mem_size;
          }

          if (n == 1)
          {
            ptr_tmp = (mem_block*)(MEM_START + BLK_SIZE * i);
          }

          move_size += old_size;
          char *dst_addr = ptr_tmp->mem_ptr - offset; // 增大新分配空间（高地址向低地址生长）
          char *src_addr = ptr_tmp->mem_ptr;

          for (int j = 0; j < move_size; j++)
          {
            *dst_addr++ = *src_addr++;
          }

          for (int j = 0; j < n; j++)
          {
            ptr_tmp = (mem_block*)(MEM_START + BLK_SIZE * (i + j));
            ptr_tmp->mem_ptr -= offset;
            if (j == 0)
            {
              ptr_tmp->mem_size = msize;
            }
          }

          return 1;
        }    
      }
    }
  }

  return 0;
}


/**
 * @brief 查看某个内存块的内存单元地址
 * @param id：内存块id
*/
void *mem_buffer(int id)
{
  for (int i = 0; i < sum; i++)
  {
    mem_block* ptr_blk = (mem_block*)(MEM_START + BLK_SIZE * i);
    if (id == ptr_blk->mem_index)
    {
      return ptr_blk->mem_ptr;
    }
  }

  return NULL;
}

/**
 * @brief 释放某个内存块
 * @param id：内存块id
*/
int mem_free(int id)
{
  for (int i = 0; i < sum; i++)
  {
    mem_block *ptr_blk = (mem_block*)(MEM_START + BLK_SIZE * i);
    if (id == ptr_blk->mem_index)
    {
      mem_block *ptr_old;

      if (i != (sum - 1))
      {
        int offset = ptr_blk->mem_size;
        int move_size = 0;
        int n = sum - i;
        mem_block *ptr_tmp;

				for(int j = 1; j < n; j++)
        {
					ptr_tmp = (mem_block *)(MEM_START + BLK_SIZE*(i+j));
					move_size += ptr_tmp->mem_size;
				}       

				char *dst_addr = ptr_tmp->mem_ptr + move_size + offset - 1;
				char *src_addr = ptr_tmp->mem_ptr + move_size - 1;

        for (int j = move_size; j > 0; j--)
        {
          *dst_addr-- = *src_addr--;
        }     

        memset(src_addr, 0, offset+1);
				for(int j=0; j<(n-1); j++)
        {
					ptr_tmp = (mem_block *)(MEM_START + BLK_SIZE*(i+j));
					ptr_old = (mem_block *)(MEM_START + BLK_SIZE*(i+j+1));
					memcpy(ptr_tmp, ptr_old, BLK_SIZE);
					ptr_tmp->mem_ptr += offset;
				}                    
      }
      else
      {
				ptr_old = (mem_block *)(MEM_START + BLK_SIZE*i);
				memset(ptr_old->mem_ptr, 0, ptr_old->mem_size);
      }

			memset(ptr_old, 0, BLK_SIZE);
			sum = sum - 1;
			return 1;      
    }
  }

  return 0;
}


