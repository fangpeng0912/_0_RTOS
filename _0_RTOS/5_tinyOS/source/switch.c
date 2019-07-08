#include "tinyos.h"
#include "ARMCM3.h"

void tTaskDelayedListInit(void);
void tTimeTaskWait(tTask *task, uint32_t ticks);
void tTimeTaskWakeUp(tTask *task);
void tTaskSchedRdy(tTask *task);
void tTaskSchedUnRdy(tTask *task);

//�궨��NVIC�Ĵ�����ص�ַ������
#define NVIC_ICSR_REG         0xE000ED04     //�жϿ��Ƽ�״̬�Ĵ�������Ҫ��������PENSV�쳣��ϵͳ�쳣����ʹ�ܣ�ֻ���ⲿ�ж���Ҫʹ�ܣ�
#define NVIC_PENDSV_SET       0x10000000     //bit28д1������PendSV
#define NVIC_PENDSV_PRI_REG   0xE000ED22	   //PENDSV���ȼ��Ĵ�������Ҫ��������PENDSV���ȼ���������ϵͳ���ã�����Ϊ������ȼ���ע����SVC����SVCΪϵͳ���ã����������ұ�������ִ�У�����޷�����ִ�У����Ϸó�fault�쳣�� 
#define NVIC_PENDSV_PRI_SET   0xFF           //�����������۸��ں��ö���λ������ȼ������ȼ�������ʲô���ģ����ȼ�������͵�

//����ַת��Ϊʵ�ʵ��ڴ�ռ�
#define MEM8(addr)  *(volatile unsigned char*)(addr)
#define MEM32(addr) *(volatile unsigned long*)(addr)

//������ȳ�ʼ��
void tTaskSchedInit(void){
	int i;
	//����������������0
	schedLockCount = 0;
	//���������ȼ�λͼ����
	tBitmapInit(&taskPrioBitmap);
	//��ͬ���ȼ��б���г�ʼ��
	for(i = 0; i < TINYOS_PRO_COUNT; ++i){
		taskTable[i] = &similar_prio_task_head_node[i];
		tListInit(taskTable[i]);
	}
}

void tTaskRunFirst(void){
	//������һ����ǣ��Ȳ��ùܣ�������tTaskSwitch���֣�������PEND_SV���жϵ�ǰ�л����������л������Ѿ���������ִ�е��л�
	__set_PSP(0);   
	
	//�������ȼ� 
	MEM8(NVIC_PENDSV_PRI_REG) = NVIC_PENDSV_PRI_SET;
	//����PENDSV�ж�
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

void tTaskSwitch(void){
	//����PENDSV�ж�
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

__asm void PendSV_Handler(void){
	
	IMPORT currentTask
	IMPORT nextTask

	MRS R0, PSP                       //��PSP����R0�У���ʱPSP���FF8����1FF8��Ϊʲô����1004����2004��ԭ��Ӧ���ǽ���PendSV�жϻ��Զ�����xpsr/PC/LR/R12/R3->R0��ֵ
	CBZ R0, PendSVHandler_nosave      //�ж�PSP�Ƿ�Ϊ0��Ϊ0����ת
	
/*���ݱ���*/
	STMDB R0!, {R4-R11}               //��R4��R11�Ĵ��������ݱ��浽����ջ��   STMDB����ַ��-1����д��R0��Ӧ��ַ����дR11

/*��¼��ǰ����ջָ��*/
	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]                      //tTask1����tTask2�е�ջָ���Ѿ��仯�����������Ȼ����FE4����1FE4�������ܹ������ָ�����
	
/*���ݻָ�*/
PendSVHandler_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]                      //currentTask=nextTask
	
	LDR R0, [R2]                      //��currentTask�ڵ�ջָ�����R0�Ĵ���
	LDMIA R0!, {R4-R11}               //������ջ�����ݻָ���R4��R11�Ĵ���  LDMIA���ȴ�R0��Ӧ��ַ�����ȶ�R4����ַ��+1

/*�л�����һ�����ջ*/
	MSR PSP, R0                       //��R0�Ĵ������ݣ���currentTask��ջ�ָ����ջָ�룩����PSP

/*�쳣����*/
	ORR LR, LR, #04                   //�쳣����ʱ������CR�Ĵ�����LR[2]=1��ʹ��PSPָ��
	BX LR                             //�˳��쳣ʱ��Ӳ���Զ��ָ�R0-R3 -> R12 -> LR -> PC -> xPSR
}

///////////systick�жϴ������������ʱ�������Ƿ�Ϊ��
void tTaskSystemTickHandler(){
	tNode *node = NULL;
	tTask *task = NULL;
	
	uint32_t status = tTaskEnterCritical();

	for(node = tTaskDelayedList->firstNode; node != &(tTaskDelayedList->headNode); node = node->nextNode){    
		task = tNodeParent(node, tTask, delayNode);
		if(--task->delayTicks == 0){
			/*�������Ƿ��ڵȴ��¼�*/
			if(task->waitEvent){
				tEventRemoveTask(task, NULL, tErrorTimeout);
			}
			/*�������Ƿ��ڵȴ��¼�*/
			
			tTimeTaskWakeUp(task);
			tTaskSchedRdy(task);
		}
	}

	//��ǰ����ʱ��ƬΪ0ʱ��ͬ���ȼ���������Ľ���������㣬����ǰ�������������ĩβ��Ҳ����˵ͬ���ȼ����л���������ʱ����ΪTINYOS_SLICE_MAX��systemTicks
	if(--currentTask->slice == 0){
		if(tListCount(taskTable[currentTask->prio]) > 0){
			tListRemoveFirst(taskTable[currentTask->prio]);
			tListAddLast(taskTable[currentTask->prio], &(currentTask->linkNode));
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	}
	
	tTaskExitCritical(status);
	
	//�������
	tTaskSched();
}






////////////////��������ʱ���н��г�ʼ��
void tTaskDelayedListInit(void){
	tListInit(tTaskDelayedList);
}

////////////////��delayNode������ʱ����
void tTimeTaskWait(tTask *task, uint32_t ticks){
	task->delayTicks = ticks;
	tListAddLast(tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

////////////////����ʱ�б����Ƴ�delayNode
void tTimeTaskWakeUp(tTask *task){
	tListRemove(tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;                   //�����������ô������Բ�Ҫ��TINYOS_TASK_STATE_RDY &
}

////////////////�������������б�
void tTaskSchedRdy(tTask *task){
	tListAddFirst(taskTable[task->prio], &(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

////////////////������Ӿ����б����Ƴ�
void tTaskSchedUnRdy(tTask *task){
	tListRemove(taskTable[task->prio], &(task->linkNode));
	//��ͬ���ȼ������б���û���������񣬽���Ӧ���ȼ�λͼ����
	if(tListCount(taskTable[task->prio]) == 0){
		tBitmapClear(&taskPrioBitmap, task->prio);
	}	
}

//����������ȼ��������Ƴ�
void tTaskSchedRemove(tTask *task){
	tListRemove(taskTable[task->prio], &(task->linkNode));
	//��ͬ���ȼ������б���û���������񣬽���Ӧ���ȼ�λͼ����
	if(tListCount(taskTable[task->prio]) == 0){
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

//���������ʱ�������Ƴ�
void tTimeTaskRemove(tTask *task){
	tListRemove(tTaskDelayedList, &(task->delayNode));
}

///////////��ȡ������ȼ�����
tTask *tTaskHighestReady(void){
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode *node = tListFirst(taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}

////////////�������
void tTaskSched(void){
	tTask *tempTask;
	uint32_t status = tTaskEnterCritical();
	
	//����Ƿ�����
	if(schedLockCount > 0){
		tTaskExitCritical(status);
		return;
	}
	
	//���������ȼ�����
	tempTask = tTaskHighestReady();
	if(tempTask != currentTask){
		nextTask = tempTask;
		tTaskSwitch();
	}

	tTaskExitCritical(status);
}







/////////////�ٽ�����������
uint32_t tTaskEnterCritical(void){
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void tTaskExitCritical(uint32_t status){
	__set_PRIMASK(status);
}

/////////////�������ʧ�ܡ�ʹ��(���ڵ�����)
void tTaskSchedDisable(void){
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount < 255){
		++schedLockCount;
	}
	tTaskExitCritical(status);
}

void tTaskSchedEnable(void){
	uint32_t status = tTaskEnterCritical();
	if(--schedLockCount == 0){
		tTaskSched();
	}
	tTaskExitCritical(status);
}



