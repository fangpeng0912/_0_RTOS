#include "tLib.h"

//结点初始化
void tNodeInit(tNode *node){
    node->nextNode = node;
    node->preNode = node;
}

//双向链表初始化
void tListInit(tList list){
	list->firstNode = &(list->headNode);
  list->lastNode = &(list->headNode);
  list->nodeCount = 0;
}

//返回结点数量
uint32_t tListCount(tList list){
	return list->nodeCount;
}

//返回第一个结点
tNode *tListFirst(tList list){
	tNode *node = (tNode*)0;
	
	if(list->nodeCount != 0){
		node = list->firstNode;
	}    
    return node;
}

//返回最后一个结点
tNode *tListLast(tList list){
  tNode *node = (tNode*)0;
	
	if(list->nodeCount != 0){
		node = list->lastNode;
	}    
    return node;
}

//返回链表中指定结点的前一结点
tNode *tListPre (tList list, tNode *node){
	if (node->preNode == node){
		return (tNode *)0;
	} 
	else{
		return node->preNode;
	}
}

//返回链表中指定结点的后一结点
tNode *tListNext (tList list, tNode * node){
	if (node->nextNode == node){
		return (tNode *)0;
	}
	else{
		return node->nextNode;
	}
}

//移除链表中的所有结点
void tListRemoveAll (tList list){
	uint32_t count;
  tNode *nextNode;
        
  //遍历所有结点
	nextNode = list->firstNode;
  for (count = list->nodeCount; count != 0; --count){
		//先记录下当前结点，和下一个结点
    //必须记录下一结点位置，因为在后面的代码中当前结点的next会被重置
    tNode *currentNode = nextNode;
    nextNode = nextNode->nextNode;
        
    //重置结点自己的信息
    currentNode->nextNode = currentNode;
    currentNode->preNode = currentNode;
	}
    
  list->firstNode = &(list->headNode);
  list->lastNode = &(list->headNode);
  list->nodeCount = 0;
}

//将指定结点添加到链表头部
void tListAddFirst (tList list, tNode *node){
	node->preNode = list->firstNode->preNode;
  node->nextNode = list->firstNode;

  list->firstNode->preNode = node;
  list->firstNode = node;
  list->nodeCount++;
}

//将指定结点添加到链表的末尾
void tListAddLast (tList list, tNode *node){
	node->nextNode = &(list->headNode);
  node->preNode = list->lastNode;

  list->lastNode->nextNode = node;
  list->lastNode = node;
  list->nodeCount++;
}

//移除链表中的第一个结点
tNode *tListRemoveFirst (tList list){
  tNode *node = (tNode *)0;

	if(list->nodeCount != 0){
		node = list->firstNode;

    node->nextNode->preNode = &(list->headNode);
    list->firstNode = node->nextNode;
    list->nodeCount--;
	}
  return  node;
}

//将指定结点插入到某个结点后面
void tListInsertAfter (tList list, tNode *nodeAfter, tNode *nodeToInsert){
	nodeToInsert->preNode = nodeAfter;
  nodeToInsert->nextNode = nodeAfter->nextNode;

  nodeAfter->nextNode->preNode = nodeToInsert;
  nodeAfter->nextNode = nodeToInsert;

  list->nodeCount++;
}

//将指定结点从链表中删除
void tListRemove (tList list, tNode *node){
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount--;
}
