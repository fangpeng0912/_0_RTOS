#ifndef _TTASK_H_
#define _TTASK_H_

#include <stdint.h>
#include "tLib.h"

struct _tEvent;    //前向引用的方式，防止与"tTask.h"互相包含      

#define TINYOS_TASK_STATE_RDY                0
#define TINYOS_TASK_STATE_DESTROYED          (1 << 1)
#define TINYOS_TASK_STATE_DELAYED            (1 << 2)
#define TINYOS_TASK_STATE_SUSPEND            (1 << 3)

#define TINYOS_TASK_WAIT_MASK                (0xFF << 16)

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

	struct _tEvent *waitEvent;      //等待的事件块
	void *eventMsg;         //事件消息
	uint32_t waitEventResult;  //等待事件的结果
}tTask;

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

#endif
