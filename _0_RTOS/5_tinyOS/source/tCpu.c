#include "tinyos.h"
#include "ARMCM3.h"

//////////////SysTick初始化
void tSetSysTickPeriod(uint32_t ms){
	SysTick->LOAD = ms * SystemCoreClock / 1000;  //减到0溢出
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

/////////////SysTick定时中断
void SysTick_Handler(){
	tTaskSystemTickHandler();
}
