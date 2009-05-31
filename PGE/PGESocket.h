#if !defined _GMSOCKET_
#define _GMSOCKET_

#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

#define MAXCLIENT 100			//�������ͻ�����Ŀ

//����ֵΪ1�ı�ʾ��ȷ

struct GMSocketClientInfo{
	SOCKET	clientSock;
	HANDLE	hThread;
	DWORD	dwThreadID;
	int		id;
//	char*	cName;
	char*	ipAddr;
	char	dataBuf[4096];
	GMSocketClientInfo* next;
public:
	int RecvDataLen;
};


typedef int  ( *GMSocketFunc)(GMSocketClientInfo *);

class GMSocket
{
public:
	
	HANDLE hClientInfoMutex;
	void releaseclientinfo();				//�뿪clientinfo�ٽ���
	void useclientinfo();					//����clientinfo�ٽ���

	void AddClient(GMSocketClientInfo *g);

	void SetDealRecvFunc(int ( *start_address )(GMSocketClientInfo * ));		//���յ�����ʱʱִ��
	void SetDealAcceptFunc(int ( *start_address )(GMSocketClientInfo * ));	//һ���ͻ���������ʱִ��
	void SetDealLeaveFunc(int ( *start_address )(GMSocketClientInfo * ));	//�Ͽ�ʱִ��

	GMSocketFunc GetDealRecvFunc();
	GMSocketFunc GetDealAcceptFunc();
	GMSocketFunc GetDealLeaveFunc();

	int SendData(DWORD dwThreadID,char *data,int length=0);
	int SendDataAll(char *data,int length=0);
	int SendData(char *data,int length=0);

	int KillAllClient();
	int KillClient(DWORD dwThreadId);
	GMSocketClientInfo *FindClient(DWORD dwThreadId);
	GMSocketClientInfo *clientInfo;
	GMSocketClientInfo *clientInfoHead;
	
	int clientNumber;

	GMSocket();
	~GMSocket();
	int socketStarted;						//��־������Ƿ��
	int InitSocket(int port);				//��������ʼ��
	int InitSocket(char *inAddr,int port);	//�ͻ��˳�ʼ��

private:

	CRITICAL_SECTION clientInfoCriticalSection;	//���clientInfoΪ�ٽ�����Դ
	SOCKET sSocket;

	GMSocketFunc	dealDataFunc;		//����ָ��,ָ�������ݵĺ���
	GMSocketFunc	dealAcceptFunc;		//����ָ��,ָ��acceptһ��clientʱ�Ĵ���
	GMSocketFunc	dealLeaveFunc;		//����ָ��,��һ��client�뿪ʱ����
};

/*
//����ֵ�����ظ��ͻ��Լ��ĺ���
struct GMSocketTransPattern{
	GMSocket	*gmsock;					//client��Ϣ
	int (*dealDataFunc)(GMSocketTransPattern * p);				//�ص�����
	char	*dataBuf;							//�յ���Ϣ
};

*/

#endif