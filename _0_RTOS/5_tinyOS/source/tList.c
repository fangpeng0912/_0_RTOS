#include "tLib.h"

//����ʼ��
void tNodeInit (tNode *node)
{
    node->nextNode = node;
    node->preNode = node;
}

//��һ���������һ�����
#define firstNode   headNode.nextNode
#define lastNode    headNode.preNode

//˫�������ʼ��
void tListInit (tList *list)
{
	list->firstNode = &(list->headNode);
  list->lastNode = &(list->headNode);
  list->nodeCount = 0;
}

//���ؽ������
uint32_t tListCount (tList *list)
{
	return list->nodeCount;
}

//���ص�һ�����
tNode *tListFirst (tList *list)
{
	tNode *node = (tNode*)0;
	
	if(list->nodeCount != 0) 
	{
		node = list->firstNode;
	}    
    return node;
}