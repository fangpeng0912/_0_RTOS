#include <stdlib.h>
#include "tinyos.h"
#include "tLib.h"
#include "tConfig.h"

extern void tSetSysTickPeriod(uint32_t);
extern tTask *tTaskHighestReady(void);

//ָ��ǰ��������һ�����ָ��
tTask *currentTask = NULL;
tTask *nextTask = NULL;
tTask *taskTable[TCONFIG_PRIO_COUNT] = {NULL, };
//������������
uint8_t schedLockCount;
//���ȼ�λͼ
tBitmap taskPrioBitmap;

//��ʱ����
/*void delay(int count){
	while(--count > 0);
}*/

//�����ʼ��(����ָ�룬������ָ�룬����������ָ��(����R0�Ĵ���)����ջ������βָ��)
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
		
		//��ʼ�������б��������ȼ������Ӧλͼ��1
		taskTable[prio] = task;
		tBitmapSet(&taskPrioBitmap, prio);
}

//��������������ջ
tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

//��ӿ�������
tTask tIdleTask;
tTaskStack idleTaskEnv[1024];

//����1
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

//����2
int task2Flag;
void task2Entry(void *param){
	while(1){
		task2Flag = 1;
		tTaskDelay(2);
		task2Flag = 0;
		tTaskDelay(2);
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
	//������ȳ�ʼ��
	tTaskSchedInit();
	//�����ʼ��
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024], 0);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024], 1);		
	tTaskInit(&tIdleTask, IdleTaskEntry, (void*)0x0, &idleTaskEnv[1024], TCONFIG_PRIO_COUNT - 1);		 
	//�����ʼ����
	nextTask = tTaskHighestReady();
	//����PendSV�쳣�����г�ʼ����
	tTaskRunFirst();
	
	return 0;
}
