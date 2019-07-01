#include "tinyos.h"

void tMboxInit(tMbox *mbox, void **msgBuffer, uint32_t maxCount){
	tEventInit(&mbox->event, tEventTypeMbox);
	
	mbox->msgBuffer = msgBuffer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}
