#include "tinyos.h"
#include "ARMCM3.h"

//宏定义NVIC寄存器相关地址和内容
#define NVIC_ICSR_REG         0xE000ED04     //中断控制及状态寄存器，主要用来悬起PENSV异常（系统异常无需使能，只有外部中断需要使能）
#define NVIC_PENDSV_SET       0x10000000     //bit28写1以悬起PendSV
#define NVIC_PENDSV_PRI_REG   0xE000ED22	   //PENDSV优先级寄存器，主要用来设置PENDSV优先级（可悬起系统调用，配置为最低优先级，注意与SVC区别：SVC为系统调用，不可悬起，且必须立即执行，如果无法立即执行，将上访成fault异常） 
#define NVIC_PENDSV_PRI_SET   0xFF           //这样设置无论该内核用多少位表达优先级、优先级分组是什么样的，优先级都是最低的
  
//将地址转化为实际的内存空间
#define MEM8(addr)  *(volatile unsigned char*)(addr)
#define MEM32(addr) *(volatile unsigned long*)(addr)
	
__asm void PendSV_Handler(void){
	
	IMPORT currentTask
	IMPORT nextTask

	MRS R0, PSP                       //将PSP放入R0中，此时PSP变成FF8或者1FF8，为什么不是1004或者2004？原因：应该是进入PendSV中断会自动保存xpsr/PC/LR/R12/R3->R0的值
	CBZ R0, PendSVHandler_nosave;     //判断PSP是否为0，为0则跳转
	
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

//任务调度
void tTaskSched(void){
	if(currentTask == taskTable[0]){     //这里为什么PSP会变，从1024->101C?原因：进入了一个新的函数，总得保存下一个要执行的指令地址什么的，由程序自动管理，无需认为干涉
		nextTask = taskTable[1];           //认为干涉的只有R11->R4的值
	}
	else{
		nextTask = taskTable[0];
	}
	
	tTaskSwitch();
}

void tSetSysTickPeriod(uint32_t ms){
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(){
	tTaskSched();
}
