#include "tSem.h"

void tSemInit(tSem *sem, uint32_t startCount, uint32_t maxCount){
	tEventInit(&sem->event, tEventTypeSem);

	sem->maxCount = maxCount;

	if(maxCount == 0){
		sem->count = startCount;
	}
	else{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}

//�ź����ĵȴ���������������Ϊ�㣬���������ȴ�����
uint32_t tSemWait(tSem *sem, uint32_t waitTicks){
	uint32_t status = tTaskEnterCritical();

	if(sem->count > 0){
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else{
		tEventWait(&sem->event, currentTask, NULL, tEventTypeSem, waitTicks);
		tTaskExitCritical(status);

		tTaskSched();

		return currentTask->waitEventResult;
	}
}

//�ź������޵ȴ���ȡ������ʹ������Ϊ�㣬����Ҳ��������¼�������
uint32_t tSemNoWaitGet(tSem *sem){
	uint32_t status = tTaskEnterCritical();

	if(sem->count > 0){
		--sem->count;
		tTaskExitCritical(status);
	    return tErrorNoError;
	}
	else{
		tTaskExitCritical(status);
		return tErrorResourceUnavailable;
	}
		
}

void tSemNotify(tSem *sem){
	uint32_t status = tTaskEnterCritical();

	if(tEventWaitCount(&sem->event) > 0){
		tTask *task = tEventWakeUp(&sem->event, NULL, tErrorNoError);
		if(task->prio < currentTask->prio){
			tTaskSched();
		}
	}
	else{
		++sem->count;
		if((sem->maxCount != 0) && (sem->count > sem->maxCount)){
			sem->count = sem->maxCount;
		}
	}

	tTaskExitCritical(status);
}

//��ȡ�����ź�����Ϣ
void tSemGetInfo(tSem *sem, tSeminfo *info){
	uint32_t status = tTaskEnterCritical(); 

	info->count = sem->count;
	info->maxCount = sem->maxCount;
	info->taskCount = tEventWaitCount(&sem->event);

	tTaskExitCritical(status);
}

//�����ź�����ɾ��
uint32_t tSemDestroy(tSem *sem){
	uint32_t status = tTaskEnterCritical(); 

	uint32_t count = tEventRemoveAll(&sem->event, NULL, tErrorDel);
	sem->count = 0;

	tTaskExitCritical(status);

	//�����ź�����������
	if(count > 0){
		tTaskSched();
	}

	return count;
}

