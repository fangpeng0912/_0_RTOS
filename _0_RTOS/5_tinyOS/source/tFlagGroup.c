#include "tinyOS.h"

//事件标志组初始化
void tFlagGroupInit (tFlagGroup *flagGroup, uint32_t flags){
	tEventInit(&flagGroup->event, tEventTypeFlagGroup);
	flagGroup->flags = flags;
}

//事件标志组的等待
uint32_t tFlagGroupWait (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requestFlag,
						uint32_t *resultFlag, uint32_t waitTicks){
	uint32_t result;
    uint32_t flags = requestFlag;

    uint32_t status = tTaskEnterCritical();
    result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
    if (result != tErrorNoError){
	    // 如果事件标志不满足条件，则插入到等待队列中
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		tEventWait(&flagGroup->event, currentTask, NULL,  tEventTypeFlagGroup, waitTicks);

        tTaskExitCritical(status);

		// 再执行一次事件调度，以便于切换到其它任务
		tTaskSched();

        *resultFlag = currentTask->eventFlags;
        result = currentTask->waitEventResult;
	}
    else{
        *resultFlag = flags;
        tTaskExitCritical(status);
    }

	return result;
}

//无等待获取
uint32_t tFlagGroupNoWaitGet (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requstFlag, uint32_t *requestFlag){
	uint32_t flags = requestFlag;

    uint32_t status = tTaskEnterCritical();
    uint32_t result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	tTaskExitCritical(status);

	*resultFlag = flags;
	return status;
}

//事件标志组的通知
void tFlagGroupNotify (tFlagGroup *flagGroup, uint8_t isSet, uint32_t flags){
	tList *waitList;
    tNode * node;
    tNode * nextNode;
    uint8_t sched = 0;

    uint32_t status = tTaskEnterCritical();

    if (isSet) {
        flagGroup->flags |= flags;     // 置1事件
    } else {
        flagGroup->flags &= ~flags;    // 清0事件
    }

    // 遍历所有的等待任务, 获取满足条件的任务，加入到待移除列表中
    waitList = &flagGroup->event.waitList;
    for (node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode) {
        uint32_t result;
        tTask *task = tNodeParent(node, tTask, linkNode);
        uint32_t flags = task->eventFlags;
        nextNode = node->nextNode;

        // 检查标志
        result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
        if (result == tErrorNoError) {
            // 唤醒任务
            task->eventFlags = flags;
            tEventWakeUpTask(&flagGroup->event, task, (void *)0, tErrorNoError);
            sched = 1;
        }
    }

    // 如果有任务就绪，则执行一次调度
    if (sched)
    {
        tTaskSched();
    }

    tTaskExitCritical(status);

}

//事件标志组的检查与消耗
static uint32_t tFlagGroupCheckAndConsume (tFlagGroup *flagGroup, uint32_t type, uint32_t *flags){
	uint32_t srcFlags = *flags;
	uint32_t isSet = type & TFLAGGROUP_SET;
	uint32_t isAll = type & TFLAGGROUP_ALL;
	uint32_t isConsume = type & TFLAGGROUP_CONSUME;

	// 有哪些类型的标志位出现
	// flagGroup->flags & flags：计算出哪些位为1
	// ~flagGroup->flags & flags:计算出哪位为0
	uint32_t calcFlag = isSet ? (flagGroup->flags & srcFlags) : (~flagGroup->flags & srcFlags);

	// 所有标志位出现, 或者做任意标志位出现，满足条件
	if (((isAll != 0) && (calcFlag == srcFlags)) || ((isAll == 0) && (calcFlag != 0))){
		// 是否消耗掉标志位
		if (isConsume){
			if (isSet){
				// 清除为1的标志位，变成0
				flagGroup->flags &= ~srcFlags;
			}
			else {
				// 清除为0的标志位，变成1
				flagGroup->flags |= srcFlags;
			}
		}
		*flags = calcFlag;
		return tErrorNoError;
	}

	*flags = calcFlag;
	return tErrorResourceUnavailable;
}

void tFlagGroupGetInfo (tFlagGroup * flagGroup, tFlagGroupInfo * info){
	
}


uint32_t tFlagGroupDestroy (tFlagGroup * flagGroup);



