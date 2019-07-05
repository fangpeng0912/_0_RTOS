#ifndef _TMEMBLOCK_H_
#define _TMEMBLOCK_H_

#include "tEvent.h"

typedef struct _tMemBlock{
	tEvent event;
	void *memStart;
	uint32_t blockSize;
	uint32_t maxCount;
	tList blockList; 
}tMemBlock;

typedef struct _tMemBlockInfo
{
	// 当前存储块的计数
    uint32_t count;

    // 允许的最大计数
    uint32_t maxCount;

    // 每个存储块的大小
    uint32_t blockSize;

    // 当前等待的任务计数
    uint32_t taskCount;
}tMemBlockInfo;

void tMemBlockInit (tMemBlock *memBlock, uint8_t *memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t tMemBlockWait (tMemBlock *memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet (tMemBlock *memBlock, void **mem);
void tMemBlockNotify (tMemBlock *memBlock, uint8_t *mem);
void tMemBlockGetInfo (tMemBlock *memBlock, tMemBlockInfo *info);
uint32_t tMemBlockDestroy (tMemBlock *memBlock);


#endif

