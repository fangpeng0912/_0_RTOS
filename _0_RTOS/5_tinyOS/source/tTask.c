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

	  tNodeInit(&(task->delayNode));        //����ʱ�����г�ʼ��
	  tNodeInit(&(task->linkNode));         //��ͬ���ȼ���������г�ʼ��

	  tListAddFirst(taskTable[prio], &task->linkNode);  //������������뵽��Ӧ�����ȼ���������
		
		//�������ȼ������Ӧλͼ��1
		tBitmapSet(&taskPrioBitmap, prio);
}

void tTaskSuspend(tTask *task){
	uint32_t status = tTaskEnterCritical();

	if(!(task->state & TINYOS_TASK_STATE_DELAYED)){   //���Ƿ�����ʱ״̬��������ڣ�����ִ�����������Ϊ��ʱ�����Ѿ��������б���ɾ��
		if(++task->suspendCount <=1){                 //���Ƿ��ǵ�һ�α�����
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
