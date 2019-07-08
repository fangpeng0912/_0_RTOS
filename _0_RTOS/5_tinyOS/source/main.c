#include "tinyos.h"

//指向当前任务与下一任务的指针
tTask *currentTask = NULL;
tTask *nextTask = NULL;
//同优先级任务链表
tHeadNode similar_prio_task_head_node[TINYOS_PRO_COUNT];
tList taskTable[TINYOS_PRO_COUNT] = {NULL, };
//调度锁计数器
uint8_t schedLockCount;
//优先级位图
tBitmap taskPrioBitmap;
//延时队列
tHeadNode task_delay_head_node;
tList tTaskDelayedList = &task_delay_head_node;  //这里申请不了堆，编译器的问题？

//主函数
int main(void){
	//任务调度初始化
	tTaskSchedInit();
	//对任务延时队列进行初始化
	tTaskDelayedListInit();
	//任务初始化
	tAppInit();
	//定义初始任务
	nextTask = tTaskHighestReady();
	//悬起PendSV异常，运行初始任务
	tTaskRunFirst();
	
	return 0;
}
