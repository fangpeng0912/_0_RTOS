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

	  tNodeInit(&(task->delayNode));        //对延时结点进行初始化
	  tNodeInit(&(task->linkNode));         //对同优先级任务结点进行初始化

	  tListAddFirst(taskTable[prio], &task->linkNode);  //将该任务结点加入到相应的优先级任务链表
		
		//传入优先级后对相应位图置1
		tBitmapSet(&taskPrioBitmap, prio);
}

void tTaskSuspend(tTask *task){
	uint32_t status = tTaskEnterCritical();

	if(!(task->state & TINYOS_TASK_STATE_DELAYED)){   //看是否处于延时状态，如果处于，不能执行任务挂起，因为此时任务已经从任务列表中删除
		if(++task->suspendCount <=1){                 //看是否是第一次被挂起
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
