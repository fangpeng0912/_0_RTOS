#include <stdlib.h>
#include "tinyos.h"
#include "tLib.h"
#include "tConfig.h"

extern void tSetSysTickPeriod(uint32_t);
extern tTask *tTaskHighestReady(void);

//指向当前任务与下一任务的指针
tTask *currentTask = NULL;
tTask *nextTask = NULL;
tTask *taskTable[TCONFIG_PRIO_COUNT] = {NULL, };
//调度锁计数器
uint8_t schedLockCount;
//优先级位图
tBitmap taskPrioBitmap;

//延时函数
/*void delay(int count){
	while(--count > 0);
}*/

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
		
		//初始化任务列表，传入优先级后对相应位图置1
		taskTable[prio] = task;
		tBitmapSet(&taskPrioBitmap, prio);
}

//定义任务和任务堆栈
tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

//添加空闲任务
tTask tIdleTask;
tTaskStack idleTaskEnv[1024];

//任务1
int task1Flag;
void task1Entry(void *param){
	tSetSysTickPeriod(10);
	while(1){
		task1Flag = 1;
		tTaskDelay(1);
		task1Flag = 0;
		tTaskDelay(1);
	}
}

//任务2
int task2Flag;
void task2Entry(void *param){
	while(1){
		task2Flag = 1;
		tTaskDelay(2);
		task2Flag = 0;
		tTaskDelay(2);
	}
}

//空闲任务
void IdleTaskEntry(void *param){
	while(1){
		//do nothing
	}
}

//主函数
int main(void){
	//任务调度初始化
	tTaskSchedInit();
	//任务初始化
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024], 0);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024], 1);		
	tTaskInit(&tIdleTask, IdleTaskEntry, (void*)0x0, &idleTaskEnv[1024], TCONFIG_PRIO_COUNT - 1);		 
	//定义初始任务
	nextTask = tTaskHighestReady();
	//悬起PendSV异常，运行初始任务
	tTaskRunFirst();
	
	return 0;
}
