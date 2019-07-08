#ifndef _TMUTEX_H_
#define _TMUTEX_H_

#include "tEvent.h"

// 互斥信号量类型
typedef struct  _tMutex
{
    // 事件控制块
    tEvent event;

    // 已被锁定的次数
    uint32_t lockedCount;

    // 拥有者
    tTask * owner;

    // 拥有者原始的优先级
    uint32_t ownerOriginalPrio;
}tMutex;

// 互斥信号量查询结构
typedef struct  _tMutexInfo
{
    // 等待的任务数量
    uint32_t taskCount;

    // 拥有者任务的优先级
    uint32_t ownerPrio;

    // 继承优先级
    uint32_t inheritedPrio;

    // 当前信号量的拥有者
    tTask * owner;

    // 锁定次数
    uint32_t lockedCount;
}tMutexInfo;

void tMutexInit (tMutex *mutex);
uint32_t tMutexWait (tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet (tMutex * mutex);
uint32_t tMutexNotify (tMutex * mutex);
uint32_t tMutexDestroy (tMutex * mutex);
void tMutexGetInfo (tMutex * mutex, tMutexInfo * info);







#endif

