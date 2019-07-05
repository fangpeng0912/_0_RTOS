#ifndef _TMBOX_H_
#define _TMBOX_H_

#include "tEvent.h"

#define tMBOXSendNormal                0x00
#define tMBOXSendFront                 0x1   //按优先级高写

typedef struct _tMbox{
	tEvent event;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t maxCount;
	void **msgBuffer;
}tMbox;

typedef struct _tMoxInfo{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;
}tMboxInfo;

void tMboxInit(tMbox *mbox, void **msgBuffer, uint32_t maxCount);
uint32_t tMboxWait (tMbox *mbox, void **msg, uint32_t waitTicks);
uint32_t tMboxNoWaitGet (tMbox *mbox, void **msg);
  uint32_t tMboxNotify (tMbox *mbox, void *msg, uint32_t notifyOption);
void tMboxFlush(tMbox *mbox);
uint32_t tMboxDestroy(tMbox *mbox);
void tMboxGetInfo(tMbox *mbox, tMboxInfo *info);

#endif

