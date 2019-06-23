#ifndef _TINYOS_H_
#define _TINYOS_H_

#include <stdint.h>
#include <stdlib.h>
#include "tLib.h"
#include "tConfig.h"
#include "tEvent.h"

typedef enum{
	tErrorNoError = 0,    //等待事件的结果
}tError;

#define TINYOS_TASK_STATE_RDY                0
#define TINYOS_TASK_STATE_DESTROYED          (1<<1)
#define TINYOS_TASK_STATE_DELAYED            (1<<2)
#define TINYOS_TASK_STATE_SUSPEND            (1<<3)

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
	tNode linkNode;        //同优先级任务结点
	uint32_t delayTicks;   //添加软延时器的计数器
	tNode delayNode;       //任务延时结点
	uint32_t prio;         //任务的优先级
	uint32_t state;        //任务是否处于延时状态
	uint32_t slice;        //时间片计数器, 用于同优先级不同任务，每个任务运行有一个最大slice，超过这个slice就要切换任务了
	uint32_t suspendCount; //挂起计数器

	void (*clean)(void *param);   //清理函数
	void *cleanParam;                  //用于保存参数
	uint8_t requestDeleteFlag;    //删除请求标记

	tEvent *waitEvent;      //等待的事件块
	void *eventMsg;         //事件消息
	uint32_t waitEventResult;  //等待事件的结果
}tTask;

extern tTask *currentTask;
extern tTask *nextTask;
extern tTask *idleTask;
extern tList taskTable[];
extern uint8_t schedLockCount;

void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskDelay(uint32_t delay);
uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);
void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);
void tTaskSched(void);

void tSetSysTickPeriod(uint32_t);
tTask *tTaskHighestReady(void);
void tTaskDelayedListInit(void);
void tTaskSystemTickHandler(void);
void tTimeTaskWait(tTask *task, uint32_t ticks);
void tTaskSchedUnRdy(tTask *task);
void tTaskSchedRdy(tTask *task);

typedef struct{
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}TtaskInfo;

void tTaskInit(tTask *task, void (*entry)(void*), void *param, tTaskStack *stack, uint32_t prio);
void tTaskSuspend(tTask *task);
void tTaskWakeUP(tTask *task);
void tTaskSchedRemove(tTask *task);
void tTimeTaskRemove(tTask *task);
void tTaskSetCleanCallFunc(tTask *task, void (*clean)(void *param), void *param);
void tTaskForceDelete(tTask *task);
void tTaskRequestDelete(tTask *task);
uint8_t tTaskIsRequestedDelated(void);
void tTaskDeleteSelf(void);
void tTaskGetInfo(tTask *task, TtaskInfo *info);

void tAppInit(void);



#endif
