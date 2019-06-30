#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>
#include <stdlib.h>
#include "tLib.h"
#include "tConfig.h"
#include "tTask.h"
#include "tEvent.h"
#include "tSem.h"

typedef enum{
	tErrorNoError = 0,                           //等待事件的结果
	tErrorTimeout,                               //超时
	tErrorResourceUnavailable,                   //资源不可用
	tErrorDel,                                   //计数信号量删除
}tError;

extern tTask *currentTask;
extern tTask *nextTask;
extern tTask *idleTask;
extern tList taskTable[];
extern uint8_t schedLockCount;
extern tBitmap taskPrioBitmap;
extern tList tTaskDelayedList;
extern tHeadNode similar_prio_task_head_node[TCONFIG_PRIO_COUNT];

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
void tTimeTaskWakeUp(tTask *task);
void tTaskSchedUnRdy(tTask *task);
void tTaskSchedRdy(tTask *task);

void tAppInit(void);



#endif
