#pragma once
#include "pgestd.h"
#define PGE_LOG_MAXSIZE 100		// 日志文件最大字节数,单位: k
#define PGE_LOG_MAXLINE 1000	// 日志文件最大行数 

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

