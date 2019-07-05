#include "tinyOS.h"

//�¼���־���ʼ��
void tFlagGroupInit (tFlagGroup *flagGroup, uint32_t flags){
	tEventInit(&flagGroup->event, tEventTypeFlagGroup);
	flagGroup->flags = flags;
}

//�¼���־��ĵȴ�
uint32_t tFlagGroupWait (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requestFlag,
						uint32_t *resultFlag, uint32_t waitTicks){
	uint32_t result;
    uint32_t flags = requestFlag;

    uint32_t status = tTaskEnterCritical();
    result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
    if (result != tErrorNoError){
	    // ����¼���־����������������뵽�ȴ�������
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		tEventWait(&flagGroup->event, currentTask, NULL,  tEventTypeFlagGroup, waitTicks);

        tTaskExitCritical(status);

		// ��ִ��һ���¼����ȣ��Ա����л�����������
		tTaskSched();

        *resultFlag = currentTask->eventFlags;
        result = currentTask->waitEventResult;
	}
    else{
        *resultFlag = flags;
        tTaskExitCritical(status);
    }

	return result;
}

//�޵ȴ���ȡ
uint32_t tFlagGroupNoWaitGet (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requstFlag, uint32_t *requestFlag){
	uint32_t flags = requestFlag;

    uint32_t status = tTaskEnterCritical();
    uint32_t result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	tTaskExitCritical(status);

	*resultFlag = flags;
	return status;
}

//�¼���־���֪ͨ
void tFlagGroupNotify (tFlagGroup *flagGroup, uint8_t isSet, uint32_t flags){
	tList *waitList;
    tNode * node;
    tNode * nextNode;
    uint8_t sched = 0;

    uint32_t status = tTaskEnterCritical();

    if (isSet) {
        flagGroup->flags |= flags;     // ��1�¼�
    } else {
        flagGroup->flags &= ~flags;    // ��0�¼�
    }

    // �������еĵȴ�����, ��ȡ�������������񣬼��뵽���Ƴ��б���
    waitList = &flagGroup->event.waitList;
    for (node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode) {
        uint32_t result;
        tTask *task = tNodeParent(node, tTask, linkNode);
        uint32_t flags = task->eventFlags;
        nextNode = node->nextNode;

        // ����־
        result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
        if (result == tErrorNoError) {
            // ��������
            task->eventFlags = flags;
            tEventWakeUpTask(&flagGroup->event, task, (void *)0, tErrorNoError);
            sched = 1;
        }
    }

    // ����������������ִ��һ�ε���
    if (sched)
    {
        tTaskSched();
    }

    tTaskExitCritical(status);

}

//�¼���־��ļ��������
static uint32_t tFlagGroupCheckAndConsume (tFlagGroup *flagGroup, uint32_t type, uint32_t *flags){
	uint32_t srcFlags = *flags;
	uint32_t isSet = type & TFLAGGROUP_SET;
	uint32_t isAll = type & TFLAGGROUP_ALL;
	uint32_t isConsume = type & TFLAGGROUP_CONSUME;

	// ����Щ���͵ı�־λ����
	// flagGroup->flags & flags���������ЩλΪ1
	// ~flagGroup->flags & flags:�������λΪ0
	uint32_t calcFlag = isSet ? (flagGroup->flags & srcFlags) : (~flagGroup->flags & srcFlags);

	// ���б�־λ����, �����������־λ���֣���������
	if (((isAll != 0) && (calcFlag == srcFlags)) || ((isAll == 0) && (calcFlag != 0))){
		// �Ƿ����ĵ���־λ
		if (isConsume){
			if (isSet){
				// ���Ϊ1�ı�־λ�����0
				flagGroup->flags &= ~srcFlags;
			}
			else {
				// ���Ϊ0�ı�־λ�����1
				flagGroup->flags |= srcFlags;
			}
		}
		*flags = calcFlag;
		return tErrorNoError;
	}

	*flags = calcFlag;
	return tErrorResourceUnavailable;
}

void tFlagGroupGetInfo (tFlagGroup * flagGroup, tFlagGroupInfo * info){
	
}


uint32_t tFlagGroupDestroy (tFlagGroup * flagGroup);



