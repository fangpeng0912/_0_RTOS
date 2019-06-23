#ifndef _TEVENT_H_
#define _TEVENT_H_

#include "tLib.h"

typedef enum{
	tEventTypeUnknow,
}tEventType;

typedef struct{
	tEventType type;
	tList waitList;
}tEvent;

void tEventInit(tEvent *event, tEventType type);

#endif
