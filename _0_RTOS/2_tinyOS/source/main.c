#include <stdlib.h>
#include "tinyos.h"

//定义任务数量
#define TASK_NUM 2
//指向当前任务与下一任务的指针
tTask *currentTask = NULL;
tTask *nextTask = NULL;
tTask *taskTable[TASK_NUM];

//延时函数
void delay(int count){
	while(--count > 0);
}

//任务初始化(任务指针，任务函数指针，任务函数参数指针(放入R0寄存器)，堆栈缓冲区尾指针)
void tTaskInit(tTask *task, void (*entry)(void*), void *param, tTaskStack *stack){
		*(--stack) = (unsigned long)(1 << 24);
		*(--stack) = (unsigned long)entry;
		*(--stack) = (unsigned long)0x14;
		*(--stack) = (unsigned long)0x12;
	  *(--stack) = (unsigned long)0x3;
	  *(--stack) = (unsigned long)0x2;
		*(--stack) = (unsigned long)0x1;
	  *(--stack) = (unsigned long)param;
	
	  *(--stack) = (unsigned long)0x11;
		*(--stack) = (unsigned long)0x10;
	  *(--stack) = (unsigned long)0x9;
	  *(--stack) = (unsigned long)0x8;
		*(--stack) = (unsigned long)0x7;
	  *(--stack) = (unsigned long)0x6;
	  *(--stack) = (unsigned long)0x5;
		*(--stack) = (unsigned long)0x4;
	
		task->stack = stack;
}

//任务调度
void tTaskSched(void){
	if(currentTask == taskTable[0]){     //这里为什么PSP会变，从1024->101C?
		nextTask = taskTable[1];
	}
	else{
		nextTask = taskTable[0];
	}
	
	tTaskSwitch();
}

//定义任务和任务堆栈
tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

//任务1
int task1Flag;
void task1Entry(void *param){
	while(1){
		task1Flag = 1;
		delay(100);
		task1Flag = 0;
		delay(100);
		
		tTaskSched();
	}
}

//任务2
int task2Flag;
void task2Entry(void *param){
	while(1){
		task2Flag = 1;
		delay(100);
		task2Flag = 0;
		delay(100);
		
		tTaskSched();
	}
}

//主函数
int main(void){
	//任务初始化
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[1024]);		
	//初始化任务列表
	taskTable[0] = &tTask1, 
	taskTable[1] = &tTask2, 
	//定义初始任务
	nextTask = taskTable[0];
	//悬起PendSV异常，运行初始任务
	tTaskRunFirst();
	
	return 0;
}
