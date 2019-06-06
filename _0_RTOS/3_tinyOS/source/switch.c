#include "tinyos.h"
#include "ARMCM3.h"

//�궨��NVIC�Ĵ�����ص�ַ������
#define NVIC_ICSR_REG         0xE000ED04     //�жϿ��Ƽ�״̬�Ĵ�������Ҫ��������PENSV�쳣��ϵͳ�쳣����ʹ�ܣ�ֻ���ⲿ�ж���Ҫʹ�ܣ�
#define NVIC_PENDSV_SET       0x10000000     //bit28д1������PendSV
#define NVIC_PENDSV_PRI_REG   0xE000ED22	   //PENDSV���ȼ��Ĵ�������Ҫ��������PENDSV���ȼ���������ϵͳ���ã�����Ϊ������ȼ���ע����SVC����SVCΪϵͳ���ã����������ұ�������ִ�У�����޷�����ִ�У����Ϸó�fault�쳣�� 
#define NVIC_PENDSV_PRI_SET   0xFF           //�����������۸��ں��ö���λ������ȼ������ȼ�������ʲô���ģ����ȼ�������͵�
  
//����ַת��Ϊʵ�ʵ��ڴ�ռ�
#define MEM8(addr)  *(volatile unsigned char*)(addr)
#define MEM32(addr) *(volatile unsigned long*)(addr)
	
__asm void PendSV_Handler(void){
	
	IMPORT currentTask
	IMPORT nextTask

	MRS R0, PSP                       //��PSP����R0�У���ʱPSP���FF8����1FF8��Ϊʲô����1004����2004��ԭ��Ӧ���ǽ���PendSV�жϻ��Զ�����xpsr/PC/LR/R12/R3->R0��ֵ
	CBZ R0, PendSVHandler_nosave;     //�ж�PSP�Ƿ�Ϊ0��Ϊ0����ת
	
/*���ݱ���*/
	STMDB R0!, {R4-R11}               //��R4��R11�Ĵ��������ݱ��浽����ջ��   STMDB����ַ��-1����д��R0��Ӧ��ַ����дR11

/*��¼��ǰ����ջָ��*/
	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]                      //tTask1����tTask2�е�ջָ���Ѿ��仯�����������Ȼ����FE4����1FE4�������ܹ������ָ�����
	
/*���ݻָ�*/
PendSVHandler_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]                      //currentTask=nextTask
	
	LDR R0, [R2]                      //��currentTask�ڵ�ջָ�����R0�Ĵ���
	LDMIA R0!, {R4-R11}               //������ջ�����ݻָ���R4��R11�Ĵ���  LDMIA���ȴ�R0��Ӧ��ַ�����ȶ�R4����ַ��+1

/*�л�����һ�����ջ*/
	MSR PSP, R0                       //��R0�Ĵ������ݣ���currentTask��ջ�ָ����ջָ�룩����PSP

/*�쳣����*/
	ORR LR, LR, #04                   //�쳣����ʱ������CR�Ĵ�����LR[2]=1��ʹ��PSPָ��
	BX LR                             //�˳��쳣ʱ��Ӳ���Զ��ָ�R0-R3 -> R12 -> LR -> PC -> xPSR
}

void tTaskRunFirst(void){
	//������һ����ǣ��Ȳ��ùܣ�������tTaskSwitch���֣�������PEND_SV���жϵ�ǰ�л����������л������Ѿ���������ִ�е��л�
	__set_PSP(0);   
	
	//�������ȼ� 
	MEM8(NVIC_PENDSV_PRI_REG) = NVIC_PENDSV_PRI_SET;
	//����PENDSV�ж�
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

void tTaskSwitch(void){
	//����PENDSV�ж�
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

//�������
void tTaskSched(void){
	if(currentTask == taskTable[0]){     //����ΪʲôPSP��䣬��1024->101C?ԭ�򣺽�����һ���µĺ������ܵñ�����һ��Ҫִ�е�ָ���ַʲô�ģ��ɳ����Զ�����������Ϊ����
		nextTask = taskTable[1];           //��Ϊ�����ֻ��R11->R4��ֵ
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
