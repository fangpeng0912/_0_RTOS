#include "tinyos.h"

extern tBitmap taskPrioBitmap;

//�����ʼ��(����ָ�룬������ָ�룬����������ָ��(����R0�Ĵ���)����ջ������βָ��)
void tTaskInit(tTask *task, void (*entry)(void*), void *param, tTaskStack *stack, uint32_t prio){
		*(--stack) = (unsigned long)(1 << 24);
		*(--stack) = (unsigned long)entry;
		*(--stack) = (unsigned long)0x14;
		*(--stack) = (unsigned long)0x12;
	  *(--stack) = (unsigned long)0x3;
	  *(--stack) = (unsigned long)0x2;
		*(--stack) = (unsigned long)0x1;
	  *(--stack) = (unsigned long)param;
	
	  *(--stack) = (unsigned long)0x11;
		*(--stack) = (unsigned long)0x10;
	  *(--stack) = (unsigned long)0x9;
	  *(--stack) = (unsigned long)0x8;
		*(--stack) = (unsigned long)0x7;
	  *(--stack) = (unsigned long)0x6;
	  *(--stack) = (unsigned long)0x5;
		*(--stack) = (unsigned long)0x4;
	
		task->stack = stack;
		task->delayTicks = 0;
	  task->prio = prio;
	  task->state = TINYOS_TASK_STATE_RDY;  //����ʱ״̬��ʼ��
	  task->slice = TINYOS_SLICE_MAX;
	  task->suspendCount = 0;
	  task->clean = NULL;
	  task->cleanParam = NULL;
	  task->requestDeleteFlag = 0;
	  
	  tNodeInit(&(task->delayNode));        //����ʱ�����г�ʼ��
	  tNodeInit(&(task->linkNode));         //��ͬ���ȼ���������г�ʼ��

	  /*tListAddFirst(taskTable[prio], &task->linkNode);  //������������뵽��Ӧ�����ȼ���������
		
		//�������ȼ������Ӧλͼ��1
		tBitmapSet(&taskPrioBitmap, prio);*/
		tTaskSchedRdy(task);
}

void tTaskSuspend(tTask *task){
	uint32_t status = tTaskEnterCritical();

	if(!(task->state & TINYOS_TASK_STATE_DELAYED)){   //���Ƿ�����ʱ״̬��������ڣ�����ִ�����������Ϊ��ʱ�����Ѿ��������б���ɾ��
		if(++task->suspendCount ==1){                 //���Ƿ��ǵ�һ�α�����
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedUnRdy(task);
			if(task == currentTask){
				tTaskSched();
			}
		}
	}

	tTaskExitCritical(status);
}

void tTaskWakeUP(tTask *task){
	uint32_t status = tTaskEnterCritical();

	if(task->state & TINYOS_TASK_STATE_SUSPEND){
		if(--task->suspendCount == 0){
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedRdy(task);
			tTaskSched();
		}
	}

	tTaskExitCritical(status);
}

//��������Ļص�����
void tTaskSetCleanCallFunc(tTask *task, void (*clean)(void *param), void *param){
	task->clean = clean;
	task->cleanParam = param;
}

//ǿ��ɾ������
void tTaskForceDelete(tTask *task){	
	uint32_t status = tTaskEnterCritical();

	//�������Ƿ�����ʱ״̬������ǣ�����ʱ�б���ֱ��ɾ��
	if(task->state & TINYOS_TASK_STATE_DELAYED){
		tTimeTaskRemove(task);
	}
	//���Ƿ��ڹ���״̬��������ǣ��������б���ɾ��
	else if(!(task->state) & TINYOS_TASK_STATE_SUSPEND){
		tTaskSchedRemove(task);		
	}

	if(task->clean){
		task->clean(task->cleanParam);
	}

	if(currentTask == task){
		tTaskSched();
	}
	
	tTaskExitCritical(status);
}

//����ɾ���ӿ�
void tTaskRequestDelete(tTask *task){
	uint32_t status = tTaskEnterCritical();

	task->requestDeleteFlag = 1;
	
	tTaskExitCritical(status);
}

//��������Ƿ�����ɾ��
uint8_t tTaskIsRequestedDelated(void){
	uint8_t delete;
	
	uint32_t status = tTaskEnterCritical();

	delete = currentTask->requestDeleteFlag;

	tTaskExitCritical(status);

	return delete;
}

//ɾ���Լ�������
void tTaskDeleteSelf(void){
	uint32_t status = tTaskEnterCritical();

	tTaskSchedRemove(currentTask);

	//����������
	if(currentTask->clean){
		currentTask->clean(currentTask->cleanParam);
	}

	tTaskSched();

	tTaskExitCritical(status);
}
