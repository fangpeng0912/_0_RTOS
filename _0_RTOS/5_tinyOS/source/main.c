#include <stdlib.h>
#include "tinyos.h"

extern void tSetSysTickPeriod(uint32_t);

//������������
#define TASK_NUM 2
//ָ��ǰ��������һ�����ָ��
tTask *currentTask = NULL;
tTask *nextTask = NULL;
tTask *taskTable[TASK_NUM];
//��������ָ��
tTask *idleTask;
//������������
uint8_t schedLockCount;

//��ʱ����
/*void delay(int count){
	while(--count > 0);
}*/

//�����ʼ��(����ָ�룬������ָ�룬����������ָ��(����R0�Ĵ���)����ջ������βָ��)
void tTaskInit(tTask *task, void (*entry)(void*), void *param, tTaskStack *stack){
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
}

//��������������ջ
tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

//���ӿ�������
tTask tIdleTask;
tTaskStack idleTaskEnv[1024];

//����1
int task1Flag;
void task1Entry(void *param){
	tSetSysTickPeriod(10);
	while(1){
		task1Flag = 1;
		tTaskDelay(10);
		task1Flag = 0;
		tTaskDelay(10);
	}
}

//����2
int task2Flag;
void task2Entry(void *param){
	while(1){
		task2Flag = 1;
		tTaskDelay(10);
		task2Flag = 0;
		tTaskDelay(10);
	}
}

//��������
void IdleTaskEntry(void *param){
	while(1){
		//do nothing
	}
}

//������
int main(void){
	//��������ʼ��
	tTaskSchedInit();
	//�����ʼ��
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024]);		
	tTaskInit(&tIdleTask, IdleTaskEntry, (void*)0x0, &idleTaskEnv[1024]);		
	//��ʼ�������б�
	taskTable[0] = &tTask1, 
	taskTable[1] = &tTask2, 
	//�����ʼ����
	nextTask = taskTable[0];
	idleTask = &tIdleTask;
	//����PendSV�쳣�����г�ʼ����
	tTaskRunFirst();
	
	return 0;
}