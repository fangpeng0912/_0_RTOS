#include "tinyos.h"

void tEventInit(tEvent *event, tEventType type){
	event->type = tEventTypeUnknow;
	event->waitList = (tHeadNode*)malloc(sizeof(tHeadNode));
	tListInit(event->waitList);
}

//事件等待接口
void tEventWait(tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout){
	uint32_t status = tTaskEnterCritical();

	task->state |= state;
	task->waitEvent = event;
	task->eventMsg = msg;
	task->waitEventResult = tErrorNoError;

	//从就绪列表中移除，插入到事件列表中
	tTaskSchedUnRdy(task);
	tListAddLast(event->waitList, &task->linkNode);

	//如果有设计超时，插入到延时队列中
	if(timeout){
		tTimeTaskWait(task, timeout);
	}
	
	tTaskExitCritical(status);
}

//从事件控制块中唤醒
tTask *tEventWakeUp(tEvent *event, void *msg, uint32_t result){
	uint32_t status = tTaskEnterCritical();
	
	tNode *node = NULL;
	tTask *task = NULL;
	
	if((node = tListRemoveFirst(event->waitList)) != NULL){
		task = (tTask*)tNodeParent(node, tTask, linkNode);
		task->waitEvent = NULL;
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK;

		//看是否还在延时队列中(即没有超时)，如果在，直接从延时队列中唤醒
		if(task->delayTicks != 0){
			tTimeTaskWakeUp(task);
		}
		
		//将任务插入就绪列表
		tTaskSchedRdy(task);
	}
	
	tTaskExitCritical(status);
	
	return task;
}

//从事件块中移除任务
void tEventRemoveTask(tTask *task, void *msg, uint32_t result){
	uint32_t status = tTaskEnterCritical();

	tListRemove(task->waitEvent->waitList, &task->linkNode);
	task->waitEvent = NULL;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~TINYOS_TASK_WAIT_MASK;

	/*这里不需要检查超时时间，因为该函数会在systick中断中直接调用这里，调用完之后自己会判断delayTicks是否为零来从延时队列中唤醒*/
	/*也就是说即使事件块中的任务被删除，也并不影响在延时队列中的任务*/
	tTaskExitCritical(status);
}

