#ifndef _TMUTEX_H_
#define _TMUTEX_H_

#include "tEvent.h"

// �����ź�������
typedef struct  _tMutex
{
    // �¼����ƿ�
    tEvent event;

    // �ѱ������Ĵ���
    uint32_t lockedCount;

    // ӵ����
    tTask * owner;

    // ӵ����ԭʼ�����ȼ�
    uint32_t ownerOriginalPrio;
}tMutex;

// �����ź�����ѯ�ṹ
typedef struct  _tMutexInfo
{
    // �ȴ�����������
    uint32_t taskCount;

    // ӵ������������ȼ�
    uint32_t ownerPrio;

    // �̳����ȼ�
    uint32_t inheritedPrio;

    // ��ǰ�ź�����ӵ����
    tTask * owner;

    // ��������
    uint32_t lockedCount;
}tMutexInfo;

void tMutexInit (tMutex *mutex);
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet (tMutex * mutex);
uint32_t tMutexNotify (tMutex * mutex);
uint32_t tMutexDestroy (tMutex * mutex);
void tMutexGetInfo (tMutex * mutex, tMutexInfo * info);







#endif

