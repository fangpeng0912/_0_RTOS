#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;
extern tTask *taskTable[];

void tTaskRunFirst(void);
void tTaskSwitch(void);

#endif
