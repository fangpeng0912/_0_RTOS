#include "tinyos.h"

extern tBitmap taskPrioBitmap;

//任务初始化(任务指针，任务函数指针，任务函数参数指针(放入R0寄存器)，堆栈缓冲区尾指针)
void tTaskInit(tTask *task, void (*entry)(void*), void *param, tTaskStack *stack, uint32_t prio){
		*(--stack) = (unsigned long)(1 << 24);
		*(--stack) = (unsigned long)entry;
		*(--stack) = (unsigned long)0x14;
		*(--stack) = (unsigned long)0x12;
	  *(--stack) = (unsigned long)0x3;
	  *(--stack) = (unsigned long)0x2;
		*(--stack) = (unsigned long)0x1;
	  *(--stack) = (unsigned long)param;
	
	  *(--stack) = (unsigned long)0x11;
		*(--stack) = (unsigned long)0x10;
	  *(--stack) = (unsigned long)0x9;
	  *(--stack) = (unsigned long)0x8;
		*(--stack) = (unsigned long)0x7;
	  *(--stack) = (unsigned long)0x6;
	  *(--stack) = (unsigned long)0x5;
		*(--stack) = (unsigned long)0x4;
	
		task->stack = stack;
		task->delayTicks = 0;
	  task->prio = prio;
	  task->state = TINYOS_TASK_STATE_RDY;  //对延时状态初始化
	  task->slice = TINYOS_SLICE_MAX;
	  task->suspendCount = 0;
	  task->clean = NULL;
	  task->cleanParam = NULL;
	  task->requestDeleteFlag = 0;
	  
	  tNodeInit(&(task->delayNode));        //对延时结点进行初始化
	  tNodeInit(&(task->linkNode));         //对同优先级任务结点进行初始化

	  /*tListAddFirst(taskTable[prio], &task->linkNode);  //将该任务结点加入到相应的优先级任务链表
		
		//传入优先级后对相应位图置1
		tBitmapSet(&taskPrioBitmap, prio);*/
		tTaskSchedRdy(task);
}

void tTaskSuspend(tTask *task){
	uint32_t status = tTaskEnterCritical();

	if(!(task->state & TINYOS_TASK_STATE_DELAYED)){   //看是否处于延时状态，如果处于，不能执行任务挂起，因为此时任务已经从任务列表中删除
		if(++task->suspendCount ==1){                 //看是否是第一次被挂起
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedUnRdy(task);
			if(task == currentTask){
				tTaskSched();
			}
		}
	}

	tTaskExitCritical(status);
}

void tTaskWakeUP(tTask *task){
	uint32_t status = tTaskEnterCritical();

	if(task->state & TINYOS_TASK_STATE_SUSPEND){
		if(--task->suspendCount == 0){
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedRdy(task);
			tTaskSched();
		}
	}

	tTaskExitCritical(status);
}

//设置清理的回调函数
void tTaskSetCleanCallFunc(tTask *task, void (*clean)(void *param), void *param){
	task->clean = clean;
	task->cleanParam = param;
}

//强制删除任务
void tTaskForceDelete(tTask *task){	
	uint32_t status = tTaskEnterCritical();

	//看任务是否处于延时状态，如果是，从延时列表中直接删除
	if(task->state & TINYOS_TASK_STATE_DELAYED){
		tTimeTaskRemove(task);
	}
	//看是否处于挂起状态，如果不是，从任务列表中删除
	else if(!(task->state) & TINYOS_TASK_STATE_SUSPEND){
		tTaskSchedRemove(task);		
	}

	if(task->clean){
		task->clean(task->cleanParam);
	}

	if(currentTask == task){
		tTaskSched();
	}
	
	tTaskExitCritical(status);
}

//请求删除接口
void tTaskRequestDelete(tTask *task){
	uint32_t status = tTaskEnterCritical();

	task->requestDeleteFlag = 1;
	
	tTaskExitCritical(status);
}

//检查任务是否被请求删除
uint8_t tTaskIsRequestedDelated(void){
	uint8_t delete;
	
	uint32_t status = tTaskEnterCritical();

	delete = currentTask->requestDeleteFlag;

	tTaskExitCritical(status);

	return delete;
}

//删除自己的任务
void tTaskDeleteSelf(void){
	uint32_t status = tTaskEnterCritical();

	tTaskSchedRemove(currentTask);

	//调用清理函数
	if(currentTask->clean){
		currentTask->clean(currentTask->cleanParam);
	}

	tTaskSched();

	tTaskExitCritical(status);
}
