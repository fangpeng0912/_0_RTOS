//宏定义NVIC寄存器相关地址和内容
#define NVIC_PENDSVPRI        0xE000ED22		 //PENDSV优先级寄存器，主要用来设置PENDSV优先级（可悬起系统调用，配置为最低优先级，注意与SVC区别：SVC为系统调用，不可悬起，且必须立即执行，如果无法立即执行，将上访成fault异常） 
#define NVIC_ICSR             0xE000ED04     //中断控制及状态寄存器，主要用来悬起PENSV异常（系统异常无需使能，只有外部中断需要使能）
#define NVIC_PENDSV_PRISET    0x000000FF     //这样设置无论该内核用多少位表达优先级、优先级分组是什么样的，优先级都是最低的
#define NVIC_PENDSVSET        0x10000000  

//将地址转化为实际的内存空间
#define MEM8(addr)  *(volatile unsigned char*)(addr)
#define MEM32(addr) *(volatile unsigned long*)(addr)

//定义一个结构体类型用于存放任务块栈指针
typedef struct{
	unsigned long *stackptr;
}BlockType_t;

//延时函数
void delay(int count){
	while(--count > 0);
}

//悬起PENDSV异常函数定义
void trigglePendSV(void){
	//设置优先级
	MEM8(NVIC_PENDSVPRI)=NVIC_PENDSV_PRISET;
	//悬起PENDSV中断
	MEM32(NVIC_ICSR)=NVIC_PENDSVSET;
}

int flag;
//定义一个栈buffer
unsigned long stack_buffer[1024];
//定义任务块结构体类型
BlockType_t *block_ptr;
BlockType_t block;

int main(){
	block.stackptr=&stack_buffer[1024]; //栈指针指向栈底
	block_ptr=&block;
	while(1){
		flag=0;
		delay(100);
		flag=1;
		delay(100);
		
		trigglePendSV();
	}
}
