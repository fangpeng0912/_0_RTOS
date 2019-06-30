#include "tSem.h"

void tSemInit(tSem *sem, uint32_t startCount, uint32_t maxCount){
	tEventInit(&sem->event, tEventTypeSem);

	sem->maxCount = maxCount;

	if(maxCount == 0){
		sem->count = startCount;
	}
	else{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}

//信号量的等待操作，若计数器为零，任务需进入等待队列
uint32_t tSemWait(tSem *sem, uint32_t waitTicks){
	uint32_t status = tTaskEnterCritical();

	if(sem->count > 0){
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else{
		tEventWait(&sem->event, currentTask, NULL, tEventTypeSem, waitTicks);
		tTaskExitCritical(status);

		tTaskSched();

		return currentTask->waitEventResult;
	}
}

//信号量的无等待获取，即即使计数器为零，任务也无需进入事件控制器
uint32_t tSemNoWaitGet(tSem *sem){
	uint32_t status = tTaskEnterCritical();

	if(sem->count > 0){
		--sem->count;
		tTaskExitCritical(status);
	    return tErrorNoError;
	}
	else{
		tTaskExitCritical(status);
		return tErrorResourceUnavailable;
	}
		
}

void tSemNotify(tSem *sem){
	uint32_t status = tTaskEnterCritical();

	if(tEventWaitCount(&sem->event) > 0){
		tTask *task = tEventWakeUp(&sem->event, NULL, tErrorNoError);
		if(task->prio < currentTask->prio){
			tTaskSched();
		}
	}
	else{
		++sem->count;
		if((sem->maxCount != 0) && (sem->count > sem->maxCount)){
			sem->count = sem->maxCount;
		}
	}

	tTaskExitCritical(status);
}

//获取计数信号量信息
void tSemGetInfo(tSem *sem, tSeminfo *info){
	uint32_t status = tTaskEnterCritical(); 

	info->count = sem->count;
	info->maxCount = sem->maxCount;
	info->taskCount = tEventWaitCount(&sem->event);

	tTaskExitCritical(status);
}

//计数信号量的删除
uint32_t tSemDestroy(tSem *sem){
	uint32_t status = tTaskEnterCritical(); 

	uint32_t count = tEventRemoveAll(&sem->event, NULL, tErrorDel);
	sem->count = 0;

	tTaskExitCritical(status);

	//计数信号量中有任务
	if(count > 0){
		tTaskSched();
	}

	return count;
}

