#include "tinyos.h"
#include "ARMCM3.h"

void tTaskDelayedListInit(void);
void tTimeTaskWait(tTask *task, uint32_t ticks);
void tTimeTaskWakeUp(tTask *task);
void tTaskSchedRdy(tTask *task);
void tTaskSchedUnRdy(tTask *task);

//宏定义NVIC寄存器相关地址和内容
#define NVIC_ICSR_REG         0xE000ED04     //中断控制及状态寄存器，主要用来悬起PENSV异常（系统异常无需使能，只有外部中断需要使能）
#define NVIC_PENDSV_SET       0x10000000     //bit28写1以悬起PendSV
#define NVIC_PENDSV_PRI_REG   0xE000ED22	   //PENDSV优先级寄存器，主要用来设置PENDSV优先级（可悬起系统调用，配置为最低优先级，注意与SVC区别：SVC为系统调用，不可悬起，且必须立即执行，如果无法立即执行，将上访成fault异常） 
#define NVIC_PENDSV_PRI_SET   0xFF           //这样设置无论该内核用多少位表达优先级、优先级分组是什么样的，优先级都是最低的

//将地址转化为实际的内存空间
#define MEM8(addr)  *(volatile unsigned char*)(addr)
#define MEM32(addr) *(volatile unsigned long*)(addr)

//任务调度初始化
void tTaskSchedInit(void){
	int i;
	//将调度锁计数器清0
	schedLockCount = 0;
	//将任务优先级位图清零
	tBitmapInit(&taskPrioBitmap);
	//对同优先级列表进行初始化
	for(i = 0; i < TINYOS_PRO_COUNT; ++i){
		taskTable[i] = &similar_prio_task_head_node[i];
		tListInit(taskTable[i]);
	}
}

void tTaskRunFirst(void){
	//设置了一个标记，先不用管，用于与tTaskSwitch区分，用于在PEND_SV中判断当前切换是首任务切换还是已经跑起来后执行的切换
	__set_PSP(0);   
	
	//设置优先级 
	MEM8(NVIC_PENDSV_PRI_REG) = NVIC_PENDSV_PRI_SET;
	//悬起PENDSV中断
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

void tTaskSwitch(void){
	//悬起PENDSV中断
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

__asm void PendSV_Handler(void){
	
	IMPORT currentTask
	IMPORT nextTask

	MRS R0, PSP                       //将PSP放入R0中，此时PSP变成FF8或者1FF8，为什么不是1004或者2004？原因：应该是进入PendSV中断会自动保存xpsr/PC/LR/R12/R3->R0的值
	CBZ R0, PendSVHandler_nosave      //判断PSP是否为0，为0则跳转
	
/*数据保存*/
	STMDB R0!, {R4-R11}               //将R4至R11寄存器的内容保存到任务栈中   STMDB：地址先-1，再写入R0对应地址，先写R11

/*记录当前任务栈指针*/
	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]                      //tTask1或者tTask2中的栈指针已经变化，因此这里虽然不是FE4或者1FE4，依旧能够正常恢复数据
	
/*数据恢复*/
PendSVHandler_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]                      //currentTask=nextTask
	
	LDR R0, [R2]                      //将currentTask内的栈指针放入R0寄存器
	LDMIA R0!, {R4-R11}               //将任务栈中内容恢复至R4至R11寄存器  LDMIA：先从R0对应地址读，先读R4，地址再+1

/*切换至下一任务的栈*/
	MSR PSP, R0                       //将R0寄存器内容（即currentTask的栈恢复后的栈指针）放入PSP

/*异常返回*/
	ORR LR, LR, #04                   //异常返回时，设置CR寄存器，LR[2]=1，使用PSP指针
	BX LR                             //退出异常时，硬件自动恢复R0-R3 -> R12 -> LR -> PC -> xPSR
}

///////////systick中断处理，并检测软延时计数器是否为零
void tTaskSystemTickHandler(){
	tNode *node = NULL;
	tTask *task = NULL;
	
	uint32_t status = tTaskEnterCritical();

	for(node = tTaskDelayedList->firstNode; node != &(tTaskDelayedList->headNode); node = node->nextNode){    
		task = tNodeParent(node, tTask, delayNode);
		if(--task->delayTicks == 0){
			/*看任务是否在等待事件*/
			if(task->waitEvent){
				tEventRemoveTask(task, NULL, tErrorTimeout);
			}
			/*看任务是否在等待事件*/
			
			tTimeTaskWakeUp(task);
			tTaskSchedRdy(task);
		}
	}

	//当前任务时间片为0时，同优先级任务链表的结点数大于零，将当前任务结点放至链表末尾，也就是说同优先级间切换任务的最高时间间隔为TINYOS_SLICE_MAX个systemTicks
	if(--currentTask->slice == 0){
		if(tListCount(taskTable[currentTask->prio]) > 0){
			tListRemoveFirst(taskTable[currentTask->prio]);
			tListAddLast(taskTable[currentTask->prio], &(currentTask->linkNode));
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	}
	
	tTaskExitCritical(status);
	
	//任务调度
	tTaskSched();
}






////////////////对任务延时队列进行初始化
void tTaskDelayedListInit(void){
	tListInit(tTaskDelayedList);
}

////////////////将delayNode插入延时队列
void tTimeTaskWait(tTask *task, uint32_t ticks){
	task->delayTicks = ticks;
	tListAddLast(tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

////////////////从延时列表中移除delayNode
void tTimeTaskWakeUp(tTask *task){
	tListRemove(tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;                   //后面有其他用处，所以不要和TINYOS_TASK_STATE_RDY &
}

////////////////将任务插入就绪列表
void tTaskSchedRdy(tTask *task){
	tListAddFirst(taskTable[task->prio], &(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

////////////////将任务从就绪列表中移除
void tTaskSchedUnRdy(tTask *task){
	tListRemove(taskTable[task->prio], &(task->linkNode));
	//当同优先级任务列表中没有其他任务，将对应优先级位图清零
	if(tListCount(taskTable[task->prio]) == 0){
		tBitmapClear(&taskPrioBitmap, task->prio);
	}	
}

//将任务从优先级队列中移除
void tTaskSchedRemove(tTask *task){
	tListRemove(taskTable[task->prio], &(task->linkNode));
	//当同优先级任务列表中没有其他任务，将对应优先级位图清零
	if(tListCount(taskTable[task->prio]) == 0){
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

//将任务从延时队列中移除
void tTimeTaskRemove(tTask *task){
	tListRemove(tTaskDelayedList, &(task->delayNode));
}

///////////获取最高优先级任务
tTask *tTaskHighestReady(void){
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode *node = tListFirst(taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}

////////////任务调度
void tTaskSched(void){
	tTask *tempTask;
	uint32_t status = tTaskEnterCritical();
	
	//检查是否上锁
	if(schedLockCount > 0){
		tTaskExitCritical(status);
		return;
	}
	
	//获得最高优先级任务
	tempTask = tTaskHighestReady();
	if(tempTask != currentTask){
		nextTask = tempTask;
		tTaskSwitch();
	}

	tTaskExitCritical(status);
}







/////////////临界区保护函数
uint32_t tTaskEnterCritical(void){
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void tTaskExitCritical(uint32_t status){
	__set_PRIMASK(status);
}

/////////////任务调度失能、使能(用于调度锁)
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



