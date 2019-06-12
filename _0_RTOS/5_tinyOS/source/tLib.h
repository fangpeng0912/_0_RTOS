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
    struct _tNode * preNode;
    struct _tNode * nextNode;
}tNode;

//双向链表头结点，元素统计结点个数
typedef struct _tList
{   
    tNode headNode;
    uint32_t nodeCount;
}tList;


#endif
