#include "tinyos.h"

void tMutexInit (tMutex * mutex)
{
    tEventInit(&mutex->event, tEventTypeMutex);

    mutex->lockedCount = 0;
    mutex->owner = NULL;
    mutex->ownerOriginalPrio = TINYOS_PRO_COUNT;
}

//�����ź����ĵȴ�
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();

    if (mutex->lockedCount <= 0)   
    {
        // ���û����������ʹ�õ�ǰ��������
        mutex->owner = currentTask;
        mutex->ownerOriginalPrio = currentTask->prio;
        mutex->lockedCount++;

        tTaskExitCritical(status);
        return tErrorNoError;
    }
    else
    {
        // �ź����Ѿ�������
        if (mutex->owner == currentTask)
        {
            // ������ź�����ӵ�����ٴ�wait�������Ӽ���
            mutex->lockedCount++;

            tTaskExitCritical(status);
            return tErrorNoError;
        }
        else
        {
            // ������ź���ӵ����֮�������wait����Ҫ������Ƿ���Ҫʹ��
            // ���ȼ��̳з�ʽ����
            if (currentTask->prio < mutex->owner->prio)
            {
                tTask * owner = mutex->owner;

                // �����ǰ��������ȼ���ӵ�������ȼ����ߣ���ʹ�����ȼ��̳�
                // ����ԭӵ���ߵ�����
                if (owner->state == TINYOS_TASK_STATE_RDY)
                {
                    // �����ھ���״̬ʱ�����������ھ������е�λ��
                    tTaskSchedUnRdy(owner);
                    owner->prio = currentTask->prio;   
                    tTaskSchedRdy(owner);
                }
                else
                {
                    // ����״̬��ֻ��Ҫ�޸����ȼ�
                    owner->prio = currentTask->prio;
                }
            }

            // ��ǰ�������ȴ�������
            tEventWait(&mutex->event, currentTask, (void *)0, tEventTypeMutex, waitTicks);
            tTaskExitCritical(status);

            // ִ�е��ȣ� �л�����������
            tTaskSched();
            return currentTask->waitEventResult;
        }
    }
}

//�����źŵ��޵ȴ���ȡ
uint32_t tMutexNoWaitGet (tMutex * mutex)
{
    uint32_t status = tTaskEnterCritical();

    if (mutex->lockedCount <= 0)
    {
        // ���û����������ʹ�õ�ǰ��������
        mutex->owner = currentTask;
        mutex->ownerOriginalPrio = currentTask->prio;
        mutex->lockedCount++;

        tTaskExitCritical(status);
        return tErrorNoError;
    }
    else
    {
        // �ź����Ѿ�������
        if (mutex->owner == currentTask)
        {
            // ������ź�����ӵ�����ٴ�wait�������Ӽ���
            mutex->lockedCount++;

            tTaskExitCritical(status);
            return tErrorNoError;
        }

        tTaskExitCritical(status);
        return tErrorResourceUnavailable;
    }
}

//�����ź�����֪ͨ
uint32_t tMutexNotify (tMutex * mutex)
{
    uint32_t status = tTaskEnterCritical();

    if (mutex->lockedCount <= 0)
    {
        // ��������Ϊ0���ź���δ��������ֱ���˳�
        tTaskExitCritical(status);
        return tErrorNoError;
    }

    if (mutex->owner != currentTask)
    {
        // ����ӵ�����ͷţ���Ϊ�ǷǷ�
        tTaskExitCritical(status);
        return tErrorOwner;
    }

    if (--mutex->lockedCount > 0)
    {
        // ��1������Բ�Ϊ0, ֱ���˳�������Ҫ���ѵȴ�������
        tTaskExitCritical(status);
        return tErrorNoError;
    }

    // �Ƿ��з������ȼ��̳�
    if (mutex->ownerOriginalPrio != mutex->owner->prio)
    {
        // �з������ȼ��̳У��ָ�ӵ���ߵ����ȼ�
        if (mutex->owner->state == TINYOS_TASK_STATE_RDY)
        {
            // �����ھ���״̬ʱ�����������ھ������е�λ��
            tTaskSchedUnRdy(mutex->owner);
            currentTask->prio = mutex->ownerOriginalPrio;
            tTaskSchedRdy(mutex->owner);
        }
        else
        {
            // ����״̬��ֻ��Ҫ�޸����ȼ�
            currentTask->prio = mutex->ownerOriginalPrio;
        }
    }

    // ����Ƿ�������ȴ�
    if (tEventWaitCount(&mutex->event) > 0)
    {
        // ����еĻ�����ֱ�ӻ���λ�ڶ����ײ������ȵȴ���������        /*�����ǲ���Ӧ�û������ȼ����ߵ����񣬱���˵У���ͽ����ֳ������Ŷӣ�����У����˵���ǽ����ֳ����϶��ǽ����ֳ�*/
        tTask * task = tEventWakeUp(&mutex->event, (void *)0, tErrorNoError);

        mutex->owner = task;
        mutex->ownerOriginalPrio = task->prio;
        mutex->lockedCount++;

        // ��������������ȼ����ߣ���ִ�е��ȣ��л���ȥ
        if (task->prio < currentTask->prio)
        {
            tTaskSched();
        }
    }
    tTaskExitCritical(status);
    return tErrorNoError;
}

//�����ź�����ɾ��
uint32_t tMutexDestroy (tMutex * mutex)
{
    uint32_t count = 0;
    uint32_t status = tTaskEnterCritical();

    // �ź����Ƿ��Ѿ���������δ����ʱû������ȴ������ش���
    if (mutex->lockedCount > 0)
    {
        // �Ƿ��з������ȼ��̳�?�����������Ҫ�ָ�ӵ���ߵ�ԭ���ȼ�
        if (mutex->ownerOriginalPrio != mutex->owner->prio)
        {
            // �з������ȼ��̳У��ָ�ӵ���ߵ����ȼ�
            if (mutex->owner->state == TINYOS_TASK_STATE_RDY)
            {
                // �����ھ���״̬ʱ�����������ھ������е�λ��
                tTaskSchedUnRdy(mutex->owner);
                mutex->owner->prio = mutex->ownerOriginalPrio;
                tTaskSchedRdy(mutex->owner);
            }
            else
            {
                // ����״̬��ֻ��Ҫ�޸����ȼ�
                mutex->owner->prio = mutex->ownerOriginalPrio;
            }
        }

        // Ȼ������¼����ƿ��е�����
        count = tEventRemoveAll(&mutex->event, (void *)0, tErrorDel);

        // ��չ����п��������������ִ��һ�ε���
        if (count > 0)
        {
            tTaskSched();
        }
    }

    tTaskExitCritical(status);
    return count;
}

//�����ź�����ȡ״̬
void tMutexGetInfo (tMutex * mutex, tMutexInfo * info)
{
    uint32_t status = tTaskEnterCritical();

    // ������Ҫ����Ϣ
    info->taskCount = tEventWaitCount(&mutex->event);
    info->ownerPrio = mutex->ownerOriginalPrio;
    if (mutex->owner != (tTask *)0)
    {
        info->inheritedPrio = mutex->owner->prio;
    }
    else
    {
        info->inheritedPrio = TINYOS_PRO_COUNT;
    }
    info->owner = mutex->owner;
    info->lockedCount = mutex->lockedCount;

    tTaskExitCritical(status);
}




