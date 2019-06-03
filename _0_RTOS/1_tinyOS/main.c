#include <stdlib.h>

//�궨��NVIC�Ĵ�����ص�ַ������
#define NVIC_ICSR_REG         0xE000ED04     //�жϿ��Ƽ�״̬�Ĵ�������Ҫ��������PENSV�쳣��ϵͳ�쳣����ʹ�ܣ�ֻ���ⲿ�ж���Ҫʹ�ܣ�
#define NVIC_PENDSV_SET       0x10000000     //bit28д1������PendSV
#define NVIC_PENDSV_PRI_REG   0xE000ED22	   //PENDSV���ȼ��Ĵ�������Ҫ��������PENDSV���ȼ���������ϵͳ���ã�����Ϊ������ȼ���ע����SVC����SVCΪϵͳ���ã����������ұ�������ִ�У�����޷�����ִ�У����Ϸó�fault�쳣�� 
#define NVIC_PENDSV_PRI_SET   0xFF           //�����������۸��ں��ö���λ������ȼ������ȼ�������ʲô���ģ����ȼ�������͵�
  

//����ַת��Ϊʵ�ʵ��ڴ�ռ�
#define MEM8(addr)  *(volatile unsigned char*)(addr)
#define MEM32(addr) *(volatile unsigned long*)(addr)

//����һ���ṹ���������ڴ�������ջָ��
typedef struct{
	unsigned long *stackptr;
}BlockType_t;

//��ʱ����
void delay(int count){
	while(--count > 0);
}

//����PENDSV�쳣��������
void trigglePendSV(void){
	//�������ȼ� 
	MEM8(NVIC_PENDSV_PRI_REG) = NVIC_PENDSV_PRI_SET;
	//����PENDSV�ж�
	MEM32(NVIC_ICSR_REG) = NVIC_PENDSV_SET;
}

int flag;
//����һ��ջbuffer
unsigned long stack_buffer[1024];
//���������ṹ������
BlockType_t block;
BlockType_t *block_ptr = NULL;


int main(){
	block_ptr = &block;
	block_ptr->stackptr = &stack_buffer[1024]; //ջָ��ָ��ջ��
	while(1){
		flag=0;
		delay(100);
		flag=1;
		delay(100);
		
		trigglePendSV();
	}
}
