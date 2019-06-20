#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>
#include <stdlib.h>
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY                0
#define TINYOS_TASK_STATE_DESTROYED          (1<<1)
#define TINYOS_TASK_STATE_DELAYED            (1<<2)
#define TINYOS_TASK_STATE_SUSPEND            (1<<3)

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
	tNode linkNode;        //ͬ���ȼ�������
	uint32_t delayTicks;   //�������ʱ���ļ�����
	tNode delayNode;       //������ʱ���
	uint32_t prio;         //��������ȼ�
	uint32_t state;        //�����Ƿ�����ʱ״̬
	uint32_t slice;        //ʱ��Ƭ������
	uint32_t suspendCount; //���������

	void (*clean)(void *param);   //������
	void *cleanParam;                  //���ڱ������
	uint8_t requestDeleteFlag;    //ɾ��������
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;
extern tTask *idleTask;
extern tList taskTable[];
extern uint8_t schedLockCount;

void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskDelay(uint32_t delay);
uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);
void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);
void tTaskSched(void);

void tSetSysTickPeriod(uint32_t);
tTask *tTaskHighestReady(void);
void tTaskDelayedListInit(void);
void tTaskSystemTickHandler(void);
void tTimeTaskWait(tTask *task, uint32_t ticks);
void tTaskSchedUnRdy(tTask *task);
void tTaskSchedRdy(tTask *task);

void tTaskInit(tTask *task, void (*entry)(void*), void *param, tTaskStack *stack, uint32_t prio);
void tTaskSuspend(tTask *task);
void tTaskWakeUP(tTask *task);
void tTaskSchedRemove(tTask *task);
void tTimeTaskRemove(tTask *task);
void tTaskSetCleanCallFunc(tTask *task, void (*clean)(void *param), void *param);
void tTaskForceDelete(tTask *task);
void tTaskRequestDelete(tTask *task);
uint8_t tTaskIsRequestedDelated(void);
void tTaskDeleteSelf(void);

void tAppInit(void);



#endif
