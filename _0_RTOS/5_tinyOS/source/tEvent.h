#ifndef _TEVENT_H_
#define _TEVENT_H_

#include "tTask.h"

typedef enum{
	tEventTypeUnknow,
	tEventTypeSem,                               //计数信号量
	tEventTypeMbox,                              //邮箱
	tEventTypeMemBlock,                          //存储块
	tEventTypeFlagGroup,                         //标志组
}tEventType;

typedef struct _tEvent{
	tEventType type;
	tList waitList;
}tEvent;

void tEventInit(tEvent *event, tEventType type);
void tEventWait(tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout);
tTask *tEventWakeUp(tEvent *event, void *msg, uint32_t result);
void tEventRemoveTask(tTask *task, void *msg, uint32_t result);
uint32_t tEventRemoveAll(tEvent *event, void *msg, uint32_t result);
uint32_t tEventWaitCount(tEvent *event);

#endif

