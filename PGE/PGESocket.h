#if !defined _GMSOCKET_
#define _GMSOCKET_

#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

#define MAXCLIENT 100			//定义最大客户端数目

//返回值为1的表示正确

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
	void releaseclientinfo();				//离开clientinfo临界区
	void useclientinfo();					//进入clientinfo临界区

	void AddClient(GMSocketClientInfo *g);

	void SetDealRecvFunc(int ( *start_address )(GMSocketClientInfo * ));		//接收到数据时时执行
	void SetDealAcceptFunc(int ( *start_address )(GMSocketClientInfo * ));	//一个客户端连接上时执行
	void SetDealLeaveFunc(int ( *start_address )(GMSocketClientInfo * ));	//断开时执行

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
	int socketStarted;						//标志服务端是否打开
	int InitSocket(int port);				//服务器初始化
	int InitSocket(char *inAddr,int port);	//客户端初始化

private:

	CRITICAL_SECTION clientInfoCriticalSection;	//标记clientInfo为临界区资源
	SOCKET sSocket;

	GMSocketFunc	dealDataFunc;		//函数指针,指向处理数据的函数
	GMSocketFunc	dealAcceptFunc;		//函数指针,指向accept一个client时的处理
	GMSocketFunc	dealLeaveFunc;		//函数指针,当一个client离开时处理
};

/*
//返回值，返回给客户自己的函数
struct GMSocketTransPattern{
	GMSocket	*gmsock;					//client信息
	int (*dealDataFunc)(GMSocketTransPattern * p);				//回调函数
	char	*dataBuf;							//收到信息
};

*/

#endif