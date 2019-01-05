__asm void PendSV_Handler(void){
	
	IMPORT block_ptr   //相当于C语言extern，导入这个变量
	
	LDR R0 ,=block_ptr
	LDR R0 ,[R0]
	LDR R0 ,[R0]       //R0内存放的是栈指针

	STMDB R0! ,{R4-R11}
	
	LDR R1 ,=block_ptr
	LDR R1 ,[R1]
	STR R0 ,[R1]     //更新栈指针,将栈指针指向新栈顶
	
	LDMIA R0! ,{R4-R11}
	
	BX LR
}