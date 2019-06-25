#ifndef _TEVENT_H_
#define _TEVENT_H_

#include "tTask.h"

typedef enum{
	tEventTypeUnknow,
}tEventType;

typedef struct _tEvent{
	tEventType type;
	tList waitList;
}tEvent;

void tEventInit(tEvent *event, tEventType type);
void tEventWait(tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout);
tTask *tEventWakeUp(tEvent *event, void *msg, uint32_t result);
void tEventRemoveTask(tTask *task, void *msg, uint32_t result);

#endif
