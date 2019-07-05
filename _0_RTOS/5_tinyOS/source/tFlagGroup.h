#ifndef _TFLAGGROUP_H_
#define _TFLAGGROUP_H_

typedef struct _tFlagGroup
{
	// 事件控制块
    tEvent event;

    // 当前事件标志
    uint32_t flags;
}tFlagGroup;

//任务想请求哪种事件类型
#define TFLAGGROUP_CLEAR (0x0 << 0)       //清除类型
#define TFLAGGROUP_SET   (0x1 << 0)       //标志位设置
#define TFLAGGROUP_ANY   (0x0 << 1)       //任意标志位设置或者清零 
#define TFLAGGROUP_ALL   (0x1 << 1)       //所有标志位设置或者清零   

#define TFLAGFROUP_SET_ALL    (TFLAGGROUP_SET | TFLAGGROUP_ALL)      //所有标志位设置
#define TFLAGGROUP_SET_ANY    (TFLAGGROUP_SET | TFLAGGROUP_ANY)      //任意标志位设置
#define TFLAGFROUP_CLEAR_ALL  (TFLAGGROUP_CLEAR | TFLAGGROUP_ALL)    //所有标志位被清空
#define TFLAGFROUP_CLEAR_ANY  (TFLAGGROUP_CLEAR | TFLAGGROUP_ANY)    //任意标志位被清空

#define TFLAGGROUP_CONSUME    (0x1 << 7)       //任务等待事件发生，任务检查到相应标志位满足的时候，是不是要把标志位清零或者置1，根据当前等待标志的类型决定

void tFlagGroupInit (tFlagGroup * flagGroup, uint32_t flags);
uint32_t tFlagGroupWait (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requestFlag,
						uint32_t *resultFlag, uint32_t waitTicks);
uint32_t tFlagGroupNoWaitGet (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requstFlag, uint32_t *requestFlag);
void tFlagGroupNotify (tFlagGroup *flagGroup, uint8_t isSet, uint32_t flags);
static uint32_t tFlagGroupCheckAndConsume (tFlagGroup *flagGroup, uint32_t type, uint32_t *flags);
void tFlagGroupGetInfo (tFlagGroup * flagGroup, tFlagGroupInfo * info)
uint32_t tFlagGroupDestroy (tFlagGroup * flagGroup);


#endif

