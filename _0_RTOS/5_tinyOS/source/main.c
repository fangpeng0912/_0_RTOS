#include "tinyos.h"

//ָ��ǰ��������һ�����ָ��
tTask *currentTask = NULL;
tTask *nextTask = NULL;
//ͬ���ȼ���������
tHeadNode similar_prio_task_head_node[TINYOS_PRO_COUNT];
tList taskTable[TINYOS_PRO_COUNT] = {NULL, };
//������������
uint8_t schedLockCount;
//���ȼ�λͼ
tBitmap taskPrioBitmap;
//��ʱ����
tHeadNode task_delay_head_node;
tList tTaskDelayedList = &task_delay_head_node;  //�������벻�˶ѣ������������⣿

//������
int main(void){
	//������ȳ�ʼ��
	tTaskSchedInit();
	//��������ʱ���н��г�ʼ��
	tTaskDelayedListInit();
	//�����ʼ��
	tAppInit();
	//�����ʼ����
	nextTask = tTaskHighestReady();
	//����PendSV�쳣�����г�ʼ����
	tTaskRunFirst();
	
	return 0;
}
