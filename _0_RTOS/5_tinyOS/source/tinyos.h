#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>
#include <stdlib.h>
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY      0
#define TINYOS_TASK_STATE_DELAYED  1

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
	tNode linkNode;        //ͬ���ȼ�������
	uint32_t delayTicks;   //�������ʱ���ļ�����
	tNode delayNode;       //������ʱ���
	uint32_t prio;         //��������ȼ�
	uint32_t state;        //�����Ƿ�����ʱ״̬
	uint32_t slice;        //ʱ��Ƭ������
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

#endif
