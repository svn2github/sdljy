#include "PGELOG.h"
#include <string.h>
CPGELog g_log;

CPGELog::CPGELog()
{
	m_dwClassID = PGE_LOG;
	m_fpLog = NULL;
	m_dwFileSize = 0;
	m_dwFileLine = 0;
}

CPGELog::~CPGELog()
{
	if (m_fpLog)
		fclose(m_fpLog);
}

void CPGELog::WriteLog(char* caLog)
{
	m_dwFileSize += strlen(caLog);
	m_dwFileLine +=  1;
	if (m_dwFileLine > PGE_LOG_MAXLINE || m_dwFileSize > PGE_LOG_MAXSIZE || (m_fpLog == NULL))
	{
		m_dwFileSize = strlen(caLog);
		m_dwFileLine =  1;
		fclose(m_fpLog);

		char caTime[60];
		char caFileName[200];
		strcpy(caFileName, "log\\");
//		GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_USE_ALT_CALENDAR, NULL,
//			ChartoTChar("yyyy'-'MM'-'dd "), caTime, 60);
//		strcat(caFileName, TCharToChar(caTime, 60));
//		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, NULL, 
//			ChartoTChar("hh':'mm':'ss"), caTime, 60);
//		strcat(caFileName, TCharToChar(caTime, 60));
		strcat(caFileName, ".log");
		m_fpLog = fopen(GetResFile(caFileName), "wb");
	}
	

	fwrite(caLog, 1, strlen(caLog), m_fpLog);
	
	
}
