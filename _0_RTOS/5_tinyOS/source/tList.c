#include "tLib.h"

//结点初始化
void tNodeInit (tNode *node)
{
    node->nextNode = node;
    node->preNode = node;
}

//第一个结点和最后一个结点
#define firstNode   headNode.nextNode
#define lastNode    headNode.preNode

//双向链表初始化
void tListInit (tList *list)
{
	list->firstNode = &(list->headNode);
  list->lastNode = &(list->headNode);
  list->nodeCount = 0;
}

//返回结点数量
uint32_t tListCount (tList *list)
{
	return list->nodeCount;
}

//返回第一个结点
tNode *tListFirst (tList *list)
{
	tNode *node = (tNode*)0;
	
	if(list->nodeCount != 0) 
	{
		node = list->firstNode;
	}    
    return node;
}