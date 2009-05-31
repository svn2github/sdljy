/*#include "PGESocket.h"

//传给监听线程的参数列表

DWORD socketListenThread(GMSocket *s);

//初始化client socket
int GMSocket::InitSocket(char *ipAddr, int port)
{
	WSADATA wsaData;
	sockaddr_in SockAddr;
	HANDLE hThread;
	DWORD  *dwThreadID=new DWORD;

	if(WSAStartup(0x0101,&wsaData))
		return -1;
	
	if((sSocket=socket(PF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
		return -1;
		
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_port=htons(port);
	SockAddr.sin_addr.S_un.S_addr=inet_addr(ipAddr);
	if(connect(sSocket,(LPSOCKADDR)&SockAddr,sizeof(SockAddr)))
	{
		closesocket(sSocket);
		return -1;
	}

	useclientinfo();
	clientInfo =new GMSocketClientInfo;
	clientInfoHead=clientInfo;
	
	clientInfo->clientSock=sSocket;

	if(dealDataFunc!=NULL)
	{
	hThread=CreateThread(NULL,
				0,
				(LPTHREAD_START_ROUTINE)socketListenThread,
				this,
				0,
				dwThreadID);
	}
	//设置客户端信息
	clientInfo->dwThreadID=*dwThreadID;
	clientInfo->hThread=hThread;
	clientInfo->ipAddr=NULL;
	clientInfo->next=NULL;

	if (dealAcceptFunc!=NULL)
		(*dealAcceptFunc)(clientInfo);
	return 0;
}

//初始化server socket
int GMSocket::InitSocket(int port)
{
	WSADATA wsaData;
	sockaddr_in SockAddr;
	int len;
	SOCKET	clientSocket;
	HANDLE hThread;
	DWORD  *dwThreadID=new DWORD;
	sockaddr clientAddr;

	if(WSAStartup(0x0101,&wsaData))
		return -1;
		
	if((sSocket=socket(PF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
		return -1;
		
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_port=htons(port);
	SockAddr.sin_addr.S_un.S_addr=INADDR_ANY;
	
	if (bind(sSocket,(struct sockaddr *)&SockAddr,sizeof(SockAddr))<0)
		return -1;
	
	if (listen(sSocket,5)==SOCKET_ERROR)
		return -1;
	len=sizeof(clientAddr);
	
	while(socketStarted)
	{
		if ((clientSocket=accept(sSocket,(struct sockaddr *)&clientAddr,&len))==INVALID_SOCKET)
			return -1;
		else
		{
			printf ("new comer!\n");
			
			//为新加入的client生成一个信息节点,并将其插入客户表
			useclientinfo();
			clientInfo =new GMSocketClientInfo;
			AddClient(clientInfo);
						
			clientInfo->clientSock=clientSocket;
		
			if (dealDataFunc!=NULL)
			{
			hThread=CreateThread(NULL,
				0,
				(LPTHREAD_START_ROUTINE)socketListenThread,
				this,
				0,
				dwThreadID);
			}
			//设置客户端信息
			
			clientInfo->dwThreadID=*dwThreadID;
			clientInfo->hThread=hThread;
			clientInfo->ipAddr=(char *)malloc(4);
			clientInfo->ipAddr[0]=clientAddr.sa_data[2];
			clientInfo->ipAddr[1]=clientAddr.sa_data[3];
			clientInfo->ipAddr[2]=clientAddr.sa_data[4];
			clientInfo->ipAddr[3]=clientAddr.sa_data[5];
			clientInfo->next=NULL;
			if (dealAcceptFunc!=NULL)
				(*dealAcceptFunc)(clientInfo);
			if (hThread!=NULL)
				clientNumber++;

		}
	}
	delete dwThreadID;
	return 0;
}

GMSocket::GMSocket()
{
	clientNumber=0;
	dealDataFunc=NULL;
	dealAcceptFunc=NULL;
	dealLeaveFunc=NULL;
	socketStarted=1;

	//初始化临界区
	InitializeCriticalSection(&clientInfoCriticalSection);
	//hClientInfoMutex=CreateMutex(NULL,false,NULL);

}

GMSocket::~GMSocket()
{
	KillAllClient();

	//删除临界区
	DeleteCriticalSection(&clientInfoCriticalSection);

}

//根据dwThreadID查找用户节点
GMSocketClientInfo* GMSocket::FindClient(DWORD dwThreadID)
{
	GMSocketClientInfo* temp;
	temp=clientInfoHead;
	while (temp->next!=NULL)
	{
		if (temp->dwThreadID==dwThreadID)
			return temp;
		temp=temp->next;
	}
	if (temp->dwThreadID==dwThreadID)
		return temp;
	return NULL;
}


//根据dwThreadID删掉用户节点
int GMSocket::KillClient(DWORD dwThreadID)
{
	GMSocketClientInfo *temp,*t;
	temp=clientInfoHead;
	HANDLE hThread=NULL;	
	useclientinfo();
	
	//判断第一个节点是否为要删除的
	if (temp->dwThreadID==dwThreadID)
		{
			hThread=temp->hThread;
			clientInfoHead=temp->next;
			closesocket(temp->clientSock);
			if(temp->ipAddr!=NULL)	delete temp->ipAddr;
			delete temp;
			clientNumber--;
			releaseclientinfo();
			return	1;
		}
	t=temp;
	temp=temp->next;
	
	do 
	{

		if (temp->dwThreadID==dwThreadID)
		{
			hThread=temp->hThread;
			if (temp->next!=NULL)	
				t->next=temp->next;
			else
				t->next=NULL;
			closesocket(temp->clientSock);
			if(temp->ipAddr!=NULL)	delete temp->ipAddr;
			delete temp;
			clientNumber--;
			releaseclientinfo();
			return	1;
		}
		t=temp;
		temp=temp->next;
	}while(temp!=NULL);
	releaseclientinfo();
	if (hThread!=NULL)	CloseHandle(hThread);
	return 0;
}


//删掉所有用户节点
int GMSocket::KillAllClient()
{
	GMSocketClientInfo *temp,*t;
	HANDLE	hTread=NULL;
	temp=clientInfoHead;
	useclientinfo();
	do 
	{
		hTread=temp->hThread;
		t=temp->next;
		closesocket(temp->clientSock);
		if (temp->ipAddr!=NULL)	delete temp->ipAddr ;
		delete temp;
		CloseHandle(hTread);
		temp=t;
	}while (temp!=NULL);
	releaseclientinfo();
	clientInfoHead=NULL;
	releaseclientinfo();
	return 1;
}

int GMSocket::SendData(char *data,int length)
{
	int len;
	if (length>0)
		len=send(sSocket,data,length,0);
	else
		len=send(sSocket,data,strlen(data),0);
	if (len==SOCKET_ERROR)
		return 0;
	return len;
}

//向所有用户发消息，返回发信息的用户数
int GMSocket::SendDataAll(char *data,int length)
{
	int number=0;
	int dataLen;
	GMSocketClientInfo *temp;
	temp=clientInfoHead;
	if (length>0)
		dataLen=length;
	else
		dataLen=strlen(data);
	int len;
	do 
	{
		if ((len=send(temp->clientSock,data,dataLen,0))!=SOCKET_ERROR)
			number++;
		temp=temp->next;	
	}while (temp!=NULL);
	return number;
}

int GMSocket::SendData(DWORD dwThreadID, char *data,int length)
{
	int len;
	GMSocketClientInfo *s;
	if ((s=FindClient(dwThreadID))==NULL)
		return 0;
	if (length>0)
		len=send(s->clientSock ,data,length,0);
	else
		len=send(s->clientSock ,data,strlen(data),0);
	if (len==SOCKET_ERROR)
		return 0;
	return len;
}

//每有一个连接就启用此线程，调用外部函数
DWORD socketListenThread(GMSocket *s)
{
	SOCKET sock=s->clientInfo->clientSock;
	GMSocketClientInfo *clientInfo;
	clientInfo=s->clientInfo;
	s->releaseclientinfo();
	char buf[4096];
	int dataLen=1;
//	send (sock,"connected",sizeof("connected\n"),MSG_OOB);
	while (1)
	{
		dataLen=0;
//		ZeroMemory(clientInfo->dataBuf,4096);
		dataLen=(recv(sock,buf,sizeof(buf),0));
		if (dataLen==SOCKET_ERROR)		//接收错误或者是连接断开
			break;
		//dataLen为传回字节数
		clientInfo->RecvDataLen=dataLen;
//		if (strlen(buf)!=0)				//将接收到的数据传给外部函数
//		{			
		memcpy(clientInfo->dataBuf,buf,dataLen);
		clientInfo->dataBuf[dataLen]=0;
		(*s->GetDealRecvFunc())(clientInfo);		//将客户信息传送给客户程序
//		}
	}
	if (s->GetDealLeaveFunc()!=NULL)
		(*s->GetDealLeaveFunc())(clientInfo);
	//线程结束处理
	s->KillClient(clientInfo->dwThreadID);
	return 0;
}

void GMSocket::SetDealRecvFunc(int ( *start_address )(GMSocketClientInfo * ))
{
	dealDataFunc=start_address;
}

void GMSocket::SetDealAcceptFunc(int ( *start_address )(GMSocketClientInfo * ))
{
	dealAcceptFunc=start_address;
}

void GMSocket::SetDealLeaveFunc(int ( *start_address )(GMSocketClientInfo * ))
{
	dealLeaveFunc=start_address;
}

void GMSocket::AddClient(GMSocketClientInfo *g)
{
	GMSocketClientInfo *temp;
	useclientinfo();
	if(clientNumber)
	{
		temp=clientInfoHead;
		while(temp->next!=NULL)
		{
			temp=temp->next;
		}
		temp->next=g;
	}
	else
		clientInfoHead=g;
	releaseclientinfo();
}

void GMSocket::useclientinfo()
{
	EnterCriticalSection(&clientInfoCriticalSection);
}

void GMSocket::releaseclientinfo()
{
	LeaveCriticalSection(&clientInfoCriticalSection);
}

GMSocketFunc GMSocket::GetDealRecvFunc()
{
	return dealDataFunc;
}

GMSocketFunc GMSocket::GetDealAcceptFunc()
{
	return dealAcceptFunc;
}

GMSocketFunc GMSocket::GetDealLeaveFunc()
{
	return dealLeaveFunc;
}*/
