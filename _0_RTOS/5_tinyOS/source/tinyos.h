#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>
#include "tLib.h"

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
	uint32_t delayTicks;   //添加软延时器的计数器
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;
extern tTask *idleTask;
extern tTask *taskTable[];
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
