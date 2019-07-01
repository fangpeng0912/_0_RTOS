#ifndef _TMBOX_H_
#define _TMBOX_H_

#include "tEvent.h"

typedef struct _tMbox{
	tEvent event;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t maxCount;
	void **msgBuffer;
}tMbox;

void tMboxInit(tMbox *mbox, void **msgBuffer, uint32_t maxCount);





#endif
