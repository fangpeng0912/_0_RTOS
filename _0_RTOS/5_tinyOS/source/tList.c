#include "tLib.h"

//����ʼ��
void tNodeInit(tNode *node){
    node->nextNode = node;
    node->preNode = node;
}

//˫�������ʼ��
void tListInit(tList list){
	list->firstNode = &(list->headNode);
  list->lastNode = &(list->headNode);
  list->nodeCount = 0;
}

//���ؽ������
uint32_t tListCount(tList list){
	return list->nodeCount;
}

//���ص�һ�����
tNode *tListFirst(tList list){
	tNode *node = (tNode*)0;
	
	if(list->nodeCount != 0){
		node = list->firstNode;
	}    
    return node;
}

//�������һ�����
tNode *tListLast(tList list){
  tNode *node = (tNode*)0;
	
	if(list->nodeCount != 0){
		node = list->lastNode;
	}    
    return node;
}

//����������ָ������ǰһ���
tNode *tListPre (tList list, tNode *node){
	if (node->preNode == node){
		return (tNode *)0;
	} 
	else{
		return node->preNode;
	}
}

//����������ָ�����ĺ�һ���
tNode *tListNext (tList list, tNode * node){
	if (node->nextNode == node){
		return (tNode *)0;
	}
	else{
		return node->nextNode;
	}
}

//�Ƴ������е����н��
void tListRemoveAll (tList list){
	uint32_t count;
  tNode *nextNode;
        
  //�������н��
	nextNode = list->firstNode;
  for (count = list->nodeCount; count != 0; --count){
		//�ȼ�¼�µ�ǰ��㣬����һ�����
    //�����¼��һ���λ�ã���Ϊ�ں���Ĵ����е�ǰ����next�ᱻ����
    tNode *currentNode = nextNode;
    nextNode = nextNode->nextNode;
        
    //���ý���Լ�����Ϣ
    currentNode->nextNode = currentNode;
    currentNode->preNode = currentNode;
	}
    
  list->firstNode = &(list->headNode);
  list->lastNode = &(list->headNode);
  list->nodeCount = 0;
}

//��ָ�������ӵ�����ͷ��
void tListAddFirst (tList list, tNode *node){
	node->preNode = list->firstNode->preNode;
  node->nextNode = list->firstNode;

  list->firstNode->preNode = node;
  list->firstNode = node;
  list->nodeCount++;
}

//��ָ�������ӵ������ĩβ
void tListAddLast (tList list, tNode *node){
	node->nextNode = &(list->headNode);
  node->preNode = list->lastNode;

  list->lastNode->nextNode = node;
  list->lastNode = node;
  list->nodeCount++;
}

//�Ƴ������еĵ�һ�����
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

//��ָ�������뵽ĳ��������
void tListInsertAfter (tList list, tNode *nodeAfter, tNode *nodeToInsert){
	nodeToInsert->preNode = nodeAfter;
  nodeToInsert->nextNode = nodeAfter->nextNode;

  nodeAfter->nextNode->preNode = nodeToInsert;
  nodeAfter->nextNode = nodeToInsert;

  list->nodeCount++;
}

//��ָ������������ɾ��
void tListRemove (tList list, tNode *node){
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount--;
}
