#include "tinyos.h"

//��������������ջ
tTask tTask1;
tTask tTask2;
tTask tTask3;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];

//��ӿ�������
tTask tIdleTask;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];


int task1Flag;

void cleanTask1Func(void *param){
	task1Flag = 0;
}

//����1
void task1Entry(void *param){
	tSetSysTickPeriod(10);
	
	tTaskSetCleanCallFunc(currentTask, cleanTask1Func, NULL);
	
	while(1){
		task1Flag = 1;
		tTaskDelay(1);
		task1Flag = 0;
		tTaskDelay(1);
	}
}

//����2
int task2Flag;
void task2Entry(void *param){
	
	uint8_t task1Deleted = 0;
	
	while(1){
		task2Flag = 1;
		tTaskDelay(2);
		task2Flag = 0;
		tTaskDelay(2);

		if(!task1Deleted){
			tTaskForceDelete(&tTask1);
			task1Deleted = 1;
		}
	}
}

//����3
int task3Flag;
void task3Entry(void *param){
	while(1){
		task3Flag = 1;
		tTaskDelay(3);
		task3Flag = 0;
		tTaskDelay(3);
	}
}

//��������
void IdleTaskEntry(void *param){
	while(1){
		//do nothing
	}
}

void tAppInit(void){
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024], 0);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024], 1);		
	tTaskInit(&tTask3, task3Entry, (void*)0x33333333, &task3Env[1024], 1);	
	tTaskInit(&tIdleTask, IdleTaskEntry, (void*)0x0, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE], TCONFIG_PRIO_COUNT - 1);		 
}
