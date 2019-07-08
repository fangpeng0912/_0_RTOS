#include "tinyos.h"

//定义任务和任务堆栈
tTask tTask1;
tTask tTask2;
tTask tTask3;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];

//添加空闲任务
tTask tIdleTask;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];

tSem sem;

int task1Flag;
//任务1
void task1Entry(void *param){
	
	tSetSysTickPeriod(10);

	tSemInit(&sem, 1, 10);
	while(1){
		tSemNoWaitGet(&sem);
		tSemWait(&sem, 0);
		
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
		
		tSemNotify(&sem);
	}
}

//任务3
int task3Flag;
void task3Entry(void *param){
	while(1){
		task3Flag = 1;
		tTaskDelay(3);
		task3Flag = 0;
		tTaskDelay(3);
	}
}

//空闲任务
void IdleTaskEntry(void *param){
	while(1){
		//do nothing
	}
}

void tAppInit(void){
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024], 0);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024], 1);		
	tTaskInit(&tTask3, task3Entry, (void*)0x33333333, &task3Env[1024], 1);	
	tTaskInit(&tIdleTask, IdleTaskEntry, (void*)0x0, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE], TINYOS_PRO_COUNT - 1);		 
}
