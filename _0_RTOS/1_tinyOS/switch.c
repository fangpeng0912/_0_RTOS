__asm void PendSV_Handler(void){
	
	IMPORT block_ptr     //�൱��C����extern�������������
	
	LDR R0 ,=block_ptr
	LDR R0 ,[R0]
	LDR R0 ,[R0]         //R0�ڴ�ŵ���ջָ��

	STMDB R0! ,{R4-R11}  //��ջָ�봦�����ݼ���ַ���R4-R11
	
	LDR R1 ,=block_ptr
	LDR R1 ,[R1]
	STR R0 ,[R1]         //����ջָ�룬ָ����ջ
	
	LDMIA R0! ,{R4-R11}  //����ջָ�봦������ε�����ַ��ȡ���ݷ���R4-R11
	
	BX LR
}