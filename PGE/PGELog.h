#pragma once
#include "pgestd.h"
#define PGE_LOG_MAXSIZE 100		// ��־�ļ�����ֽ���,��λ: k
#define PGE_LOG_MAXLINE 1000	// ��־�ļ�������� 

class CPGELog : public CPGEBase
{
public:
	CPGELog();
	~CPGELog();

	void WriteLog(char* caLog);

private:
	FILE* m_fpLog;
	DWORD m_dwFileSize;
	DWORD m_dwFileLine;
};

