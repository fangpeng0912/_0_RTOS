#include "tinyos.h"

void tEventInit(tEvent *event, tEventType type){
	event->type = tEventTypeUnknow;
	event->waitList = (tHeadNode*)malloc(sizeof(tHeadNode));
	tListInit(event->waitList);
}

//�¼��ȴ��ӿ�
void tEventWait(tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout){
	uint32_t status = tTaskEnterCritical();

	task->state |= state;
	task->waitEvent = event;
	task->eventMsg = msg;
	task->waitEventResult = tErrorNoError;

	//�Ӿ����б����Ƴ������뵽�¼��б���
	tTaskSchedUnRdy(task);
	tListAddLast(event->waitList, &task->linkNode);

	//�������Ƴ�ʱ�����뵽��ʱ������
	if(timeout){
		tTimeTaskWait(task, timeout);
	}
	
	tTaskExitCritical(status);
}

//���¼����ƿ��л���
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

		//���Ƿ�����ʱ������(��û�г�ʱ)������ڣ�ֱ�Ӵ���ʱ�����л���
		if(task->delayTicks != 0){
			tTimeTaskWakeUp(task);
		}
		
		//�������������б�
		tTaskSchedRdy(task);
	}
	
	tTaskExitCritical(status);
	
	return task;
}

//���¼������Ƴ�����
void tEventRemoveTask(tTask *task, void *msg, uint32_t result){
	uint32_t status = tTaskEnterCritical();

	tListRemove(task->waitEvent->waitList, &task->linkNode);
	task->waitEvent = NULL;
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~TINYOS_TASK_WAIT_MASK;

	/*���ﲻ��Ҫ��鳬ʱʱ�䣬��Ϊ�ú�������systick�ж���ֱ�ӵ������������֮���Լ����ж�delayTicks�Ƿ�Ϊ��������ʱ�����л���*/
	/*Ҳ����˵��ʹ�¼����е�����ɾ����Ҳ����Ӱ������ʱ�����е�����*/
	tTaskExitCritical(status);
}

