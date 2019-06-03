__asm void PendSV_Handler(void){
	
	IMPORT block_ptr     //相当于C语言extern，导入这个变量
	
	LDR R0 ,=block_ptr
	LDR R0 ,[R0]
	LDR R0 ,[R0]         //R0内存放的是栈指针

	STMDB R0! ,{R4-R11}  //从栈指针处连续递减地址存放R4-R11
	
	LDR R1 ,=block_ptr
	LDR R1 ,[R1]
	STR R0 ,[R1]         //更新栈指针，指向新栈
	
	LDMIA R0! ,{R4-R11}  //从新栈指针处连续多次递增地址读取数据放入R4-R11
	
	BX LR
}