#ifndef _TFLAGGROUP_H_
#define _TFLAGGROUP_H_

typedef struct _tFlagGroup
{
	// �¼����ƿ�
    tEvent event;

    // ��ǰ�¼���־
    uint32_t flags;
}tFlagGroup;

//���������������¼�����
#define TFLAGGROUP_CLEAR (0x0 << 0)       //�������
#define TFLAGGROUP_SET   (0x1 << 0)       //��־λ����
#define TFLAGGROUP_ANY   (0x0 << 1)       //�����־λ���û������� 
#define TFLAGGROUP_ALL   (0x1 << 1)       //���б�־λ���û�������   

#define TFLAGFROUP_SET_ALL    (TFLAGGROUP_SET | TFLAGGROUP_ALL)      //���б�־λ����
#define TFLAGGROUP_SET_ANY    (TFLAGGROUP_SET | TFLAGGROUP_ANY)      //�����־λ����
#define TFLAGFROUP_CLEAR_ALL  (TFLAGGROUP_CLEAR | TFLAGGROUP_ALL)    //���б�־λ�����
#define TFLAGFROUP_CLEAR_ANY  (TFLAGGROUP_CLEAR | TFLAGGROUP_ANY)    //�����־λ�����

#define TFLAGGROUP_CONSUME    (0x1 << 7)       //����ȴ��¼������������鵽��Ӧ��־λ�����ʱ���ǲ���Ҫ�ѱ�־λ���������1�����ݵ�ǰ�ȴ���־�����;���

void tFlagGroupInit (tFlagGroup * flagGroup, uint32_t flags);
uint32_t tFlagGroupWait (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requestFlag,
						uint32_t *resultFlag, uint32_t waitTicks);
uint32_t tFlagGroupNoWaitGet (tFlagGroup *flagGroup, uint32_t waitType, uint32_t requstFlag, uint32_t *requestFlag);
void tFlagGroupNotify (tFlagGroup *flagGroup, uint8_t isSet, uint32_t flags);
static uint32_t tFlagGroupCheckAndConsume (tFlagGroup *flagGroup, uint32_t type, uint32_t *flags);
void tFlagGroupGetInfo (tFlagGroup * flagGroup, tFlagGroupInfo * info)
uint32_t tFlagGroupDestroy (tFlagGroup * flagGroup);


#endif

