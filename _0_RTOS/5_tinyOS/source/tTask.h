#ifndef _TTASK_H_
#define _TTASK_H_

#include <stdint.h>
#include "tLib.h"

struct _tEvent;    //ǰ�����õķ�ʽ����ֹ��"tTask.h"�������      

#define TINYOS_TASK_STATE_RDY                0
#define TINYOS_TASK_STATE_DESTROYED          (1 << 1)
#define TINYOS_TASK_STATE_DELAYED            (1 << 2)
#define TINYOS_TASK_STATE_SUSPEND            (1 << 3)

#define TINYOS_TASK_WAIT_MASK                (0xFF << 16)

typedef uint32_t tTaskStack;

typedef struct{
	tTaskStack *stack;
	tNode linkNode;        //ͬ���ȼ�������
	uint32_t delayTicks;   //�������ʱ���ļ�����
	tNode delayNode;       //������ʱ���
	uint32_t prio;         //��������ȼ�
	uint32_t state;        //�����Ƿ�����ʱ״̬
	uint32_t slice;        //ʱ��Ƭ������, ����ͬ���ȼ���ͬ����ÿ������������һ�����slice���������slice��Ҫ�л�������
	uint32_t suspendCount; //���������

	void (*clean)(void *param);   //������
	void *cleanParam;                  //���ڱ������
	uint8_t requestDeleteFlag;    //ɾ��������

	struct _tEvent *waitEvent;      //�ȴ����¼���
	void *eventMsg;         //�¼���Ϣ
	uint32_t waitEventResult;  //�ȴ��¼��Ľ��
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
