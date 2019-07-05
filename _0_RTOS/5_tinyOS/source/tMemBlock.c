#include "tinyos.h"

//存储块初始化
void tMemBlockInit (tMemBlock *memBlock, uint8_t *memStart, uint32_t blockSize, uint32_t blockCnt){
	uint8_t *memBlockStart = (uint8_t *)memStart;
	uint8_t *memBlockEnd = memBlockStart + blockSize * blockCnt;

	// 每个存储块需要来放置链接指针，所以空间至少要比tNode大
	if (blockSize < sizeof(tNode)){
		return;
	}

	tEventInit(&memBlock->event, tEventTypeMemBlock);

	memBlock->memStart = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxCount = blockCnt;

	tListInit(memBlock->blockList);
	while (memBlockStart < memBlockEnd){
		tNodeInit((tNode*)memBlockStart);
		tListAddLast(memBlock->blockList, (tNode*)memBlockStart);

		memBlockStart += blockSize;
	}
}

//获取存储块
uint32_t tMemBlockWait (tMemBlock *memBlock, uint8_t **mem, uint32_t waitTicks){
    uint32_t status = tTaskEnterCritical();

    // 首先检查是否有空闲的存储块
    if (tListCount(memBlock->blockList) > 0){
    	// 如果有的话，取出一个
        *mem = (uint8_t *)tListRemoveFirst(memBlock->blockList);
    	tTaskExitCritical(status);
    	return tErrorNoError;
    }  
    else{
         // 然后将任务插入事件队列中
        tEventWait(&memBlock->event, currentTask, (void *)0,  tEventTypeMemBlock, waitTicks);
        tTaskExitCritical(status);

        // 最后再执行一次事件调度，以便于切换到其它任务
        tTaskSched();

        // 当切换回来时，从tTask中取出获得的消息
        *mem = currentTask->eventMsg;

        // 取出等待结果
        return currentTask->waitEventResult;
    }
}

//无等待获取存储块
uint32_t tMemBlockNoWaitGet (tMemBlock *memBlock, void **mem){
    uint32_t status = tTaskEnterCritical();

    // 首先检查是否有空闲的存储块
    if (tListCount(memBlock->blockList) > 0){
    	// 如果有的话，取出一个
        *mem = (uint8_t *)tListRemoveFirst(memBlock->blockList);
    	tTaskExitCritical(status);
    	return tErrorNoError;
    }
    else{
     	// 否则，返回资源不可用
			tTaskExitCritical(status);
    	return tErrorResourceUnavailable;
    }
}

//释放存储块
void tMemBlockNotify (tMemBlock *memBlock, uint8_t *mem){
    uint32_t status = tTaskEnterCritical();

    // 检查是否有任务等待
    if (tEventWaitCount(&memBlock->event) > 0){
    	// 如果有的话，则直接唤醒位于队列首部（最先等待）的任务
        tTask *task = tEventWakeUp(&memBlock->event, NULL, tErrorNoError);

        // 如果这个任务的优先级更高，就执行调度，切换过去
        if (task->prio < currentTask->prio){
            tTaskSched();
    	}
    }
    else{
    	// 如果没有任务等待的话，将存储块插入到队列中
    	tListAddLast(memBlock->blockList, (tNode *)mem);
   	}

	tTaskExitCritical(status);
}

//存储块状态信息
void tMemBlockGetInfo (tMemBlock *memBlock, tMemBlockInfo *info){
    uint32_t status = tTaskEnterCritical();

    // 拷贝需要的信息
    info->count = tListCount(memBlock->blockList);
    info->maxCount = memBlock->maxCount;
    info->blockSize = memBlock->blockSize;
    info->taskCount = tEventWaitCount(&memBlock->event);

    tTaskExitCritical(status);
}

//存储块的删除（将事件块中的任务删除）
uint32_t tMemBlockDestroy (tMemBlock *memBlock){
    uint32_t status = tTaskEnterCritical();

    // 清空事件控制块中的任务
    uint32_t count = tEventRemoveAll(&memBlock->event, NULL, tErrorDel);

    tTaskExitCritical(status);

    // 清空过程中可能有任务就绪，执行一次调度
    if (count > 0)
    {
        tTaskSched();
    }
    return count;
}


