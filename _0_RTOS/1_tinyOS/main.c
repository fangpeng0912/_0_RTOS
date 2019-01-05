//�궨��NVIC�Ĵ�����ص�ַ������
#define NVIC_PENDSVPRI        0xE000ED22		 //PENDSV���ȼ��Ĵ�������Ҫ��������PENDSV���ȼ���������ϵͳ���ã�����Ϊ������ȼ���ע����SVC����SVCΪϵͳ���ã����������ұ�������ִ�У�����޷�����ִ�У����Ϸó�fault�쳣�� 
#define NVIC_ICSR             0xE000ED04     //�жϿ��Ƽ�״̬�Ĵ�������Ҫ��������PENSV�쳣��ϵͳ�쳣����ʹ�ܣ�ֻ���ⲿ�ж���Ҫʹ�ܣ�
#define NVIC_PENDSV_PRISET    0x000000FF     //�����������۸��ں��ö���λ������ȼ������ȼ�������ʲô���ģ����ȼ�������͵�
#define NVIC_PENDSVSET        0x10000000  

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
	MEM8(NVIC_PENDSVPRI)=NVIC_PENDSV_PRISET;
	//����PENDSV�ж�
	MEM32(NVIC_ICSR)=NVIC_PENDSVSET;
}

int flag;
//����һ��ջbuffer
unsigned long stack_buffer[1024];
//���������ṹ������
BlockType_t *block_ptr;
BlockType_t block;

int main(){
	block.stackptr=&stack_buffer[1024]; //ջָ��ָ��ջ��
	block_ptr=&block;
	while(1){
		flag=0;
		delay(100);
		flag=1;
		delay(100);
		
		trigglePendSV();
	}
}
