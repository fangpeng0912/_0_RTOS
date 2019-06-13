#ifndef _TLIB_H_
#define _TLIB_H_

#include <stdint.h>

typedef struct{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit(tBitmap *bitmap);
uint32_t tBitmapPosCount(void);   //返回所能支持的标志位的数量
void tBitmapSet(tBitmap *bitmap, uint32_t pos);
void tBitmapClear(tBitmap *bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap *bitmap);

//双向链表结点类型
typedef struct _tNode
{
    struct _tNode *preNode;
    struct _tNode *nextNode;
}tNode;

//双向链表
typedef struct
{   
    tNode headNode;
    uint32_t nodeCount;
}tHeadNode;
typedef tHeadNode *tList;

//第一个结点和最后一个结点
#define firstNode   headNode.nextNode
#define lastNode    headNode.preNode

#define tNodeParent(childNode_addr, parentNode_type, childNode_name) \
				(parent*)((uint32_t)childNode_addr - (uint32_t)&((parentNode_type*)0)->childNode_name)

void tNodeInit(tNode *node);
void tListInit(tList list);
uint32_t tListCount(tList list);
tNode *tListFirst(tList list);
tNode *tListLast(tList list);
tNode *tListPre (tList list, tNode *node);
tNode * tListNext (tList list, tNode * node);
void tListRemoveAll (tList list);
void tListAddFirst (tList list, tNode *node);
void tListAddLast (tList list, tNode * node);
tNode * tListRemoveFirst (tList list);
void tListInsertAfter (tList list, tNode *nodeAfter, tNode *nodeToInsert);
void tListRemove (tList list, tNode *node);

#endif
