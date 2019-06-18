#include "tinyos.h"
#include "ARMCM3.h"

//////////////SysTick��ʼ��
void tSetSysTickPeriod(uint32_t ms){
	SysTick->LOAD = ms * SystemCoreClock / 1000;  //����0���
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

/////////////SysTick��ʱ�ж�
void SysTick_Handler(){
	tTaskSystemTickHandler();
}
