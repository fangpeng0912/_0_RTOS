#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
	uint32_t delayTicks;   //添加软延时器的计数器
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;
extern tTask *idleTask;
extern tTask *taskTable[];

void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskDelay(uint32_t delay);

#endif
