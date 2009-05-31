#include ".\pgeime.h"
#include <string.h>
CPGEIme::CPGEIme(void)
{
	m_pPGEImeObj = NULL;
	m_dwClassID = PGE_IME;
}

CPGEIme::~CPGEIme(void)
{
	for(int i=0; i<PGE_IME_OBJCOUNT; i++)
	{
		SAFE_DELETE_ARY(m_pPGEImeObj[i].pData);
	}
	SAFE_DELETE_ARY(m_pPGEImeObj);
}

PGE_RESULT CPGEIme::LoadIme(char* caFileName)
{
	SRESULT;
	if (m_pPGEImeObj == NULL)
	{
		m_pPGEImeObj = new PGEImeObject[PGE_IME_OBJCOUNT];
	}

	memset(m_pPGEImeObj, 0, sizeof(PGEImeObject)*PGE_IME_OBJCOUNT);

	FILE* fp = PGEFileOpen(caFileName, "rb");
	if (!fp) RET;
	int j = 0;
	for (int i=0; i<PGE_IME_OBJCOUNT; i++)
	{
		fread(&(m_pPGEImeObj[i].iSize), sizeof(WORD), 1, fp);
		m_pPGEImeObj[i].pData = new char[m_pPGEImeObj[i].iSize];
		fread(m_pPGEImeObj[i].InputKey, sizeof(m_pPGEImeObj[i].InputKey), 1, fp);
		fread(m_pPGEImeObj[i].pData, m_pPGEImeObj[i].iSize, 1, fp);
	}
	fclose(fp);
	OKRESULT;
	RET;
}

char* CPGEIme::GetWords(char* caPy, int& iOutCount)
{
	if (caPy[0] == 0)
	{
		iOutCount = 0;
		return NULL;
	}
	int i = 0;
	int iOffset = 0;
	int iKeyOff = 0;
	if ((caPy[0] == 'a') || (caPy[0] == 'e') 
		|| (caPy[0] == 'u') || (caPy[0] == 'o') || (caPy[0] == 'i'))
	{
		iOffset = 0;
	}
	else
	{
		iKeyOff = 1;
		iOffset = (caPy[0]-'a') * 33;
		if (caPy[0] == 'z' && caPy[1]=='h') iOffset = 8*33;
		if (caPy[0] == 'c' && caPy[1]=='h') iOffset = 4*33;
		if (caPy[0] == 's' && caPy[1]=='h') iOffset = 20*33;
		if (caPy[1]=='h') iKeyOff = 2;

	}

	int ib = 0;
	if (caPy[iKeyOff] > m_pPGEImeObj[iOffset+16].InputKey[iKeyOff])
	{
		if (caPy[iKeyOff] > m_pPGEImeObj[iOffset+16+8].InputKey[iKeyOff])
		{
			for (i=iOffset+16+8+1; i<iOffset+33; i++)
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[i].InputKey))
				{
					iOutCount = (m_pPGEImeObj[i].iSize-2) >> 1;
					return m_pPGEImeObj[i].pData;
				}
			}
			iOutCount = (m_pPGEImeObj[i-1].iSize-2) >> 1;
			return m_pPGEImeObj[i-1].pData;
		}
		else if (caPy[iKeyOff] < m_pPGEImeObj[iOffset+16+8].InputKey[iKeyOff])
		{
			for (i=iOffset+16+1; i<iOffset+16+8-1; i++)
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[i].InputKey))
				{
					iOutCount = (m_pPGEImeObj[i].iSize-2) >> 1;
					return m_pPGEImeObj[i].pData;
				}
			}
			iOutCount = (m_pPGEImeObj[i-1].iSize-2) >> 1;
			return m_pPGEImeObj[i-1].pData;
		}
		else
		{
			while (caPy[iKeyOff] == m_pPGEImeObj[iOffset+16+8+ib].InputKey[iKeyOff])
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[iOffset+16+8+ib].InputKey))
				{
					iOutCount = (m_pPGEImeObj[iOffset+16+8+ib].iSize-2) >> 1;
					return m_pPGEImeObj[iOffset+16+8+ib].pData;
				}
				ib++;
			}

			ib = -1;
			while (caPy[iKeyOff] == m_pPGEImeObj[iOffset+16+8+ib].InputKey[iKeyOff])
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[iOffset+16+8+ib].InputKey))
				{
					iOutCount = (m_pPGEImeObj[iOffset+16+8+ib].iSize-2) >> 1;
					return m_pPGEImeObj[iOffset+16+8+ib].pData;
				}
				ib--;
			}
			iOutCount = (m_pPGEImeObj[iOffset+16+8+ib].iSize-2) >> 1;
			return m_pPGEImeObj[iOffset+16+8+ib].pData;		
		}
	}
	else if (caPy[iKeyOff] < m_pPGEImeObj[iOffset+16].InputKey[iKeyOff])
	{
		if (caPy[iKeyOff] > m_pPGEImeObj[iOffset+16-8].InputKey[iKeyOff])
		{
			for (i=iOffset+16-8+1; i<iOffset+16; i++)
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[i].InputKey))
				{
					iOutCount = (m_pPGEImeObj[i].iSize-2) >> 1;
					return m_pPGEImeObj[i].pData;
				}
			}
			iOutCount = (m_pPGEImeObj[i-1].iSize-2) >> 1;
			return m_pPGEImeObj[i-1].pData;
		}
		else if (caPy[iKeyOff] < m_pPGEImeObj[iOffset+16-8].InputKey[iKeyOff])
		{
			for (i=iOffset; i<iOffset+16-8-1; i++)
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[i].InputKey))
				{
					iOutCount = (m_pPGEImeObj[i].iSize-2) >> 1;
					return m_pPGEImeObj[i].pData;
				}
			}
			iOutCount = (m_pPGEImeObj[i-1].iSize-2) >> 1;
			return m_pPGEImeObj[i-1].pData;
		}
		else
		{
			while (caPy[iKeyOff] == m_pPGEImeObj[iOffset+16-8+ib].InputKey[iKeyOff])
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[iOffset+16-8+ib].InputKey))
				{
					iOutCount = (m_pPGEImeObj[iOffset+16-8+ib].iSize-2) >> 1;
					return m_pPGEImeObj[iOffset+16-8+ib].pData;
				}
				ib++;
			}

			ib = -1;
			while (caPy[iKeyOff] == m_pPGEImeObj[iOffset+16-8+ib].InputKey[iKeyOff])
			{
				if (0 == strcmp(caPy, m_pPGEImeObj[iOffset+16-8+ib].InputKey))
				{
					iOutCount = (m_pPGEImeObj[iOffset+16-8+ib].iSize-2) >> 1;
					return m_pPGEImeObj[iOffset+16-8+ib].pData;
				}
				ib--;
			}		
			iOutCount = (m_pPGEImeObj[iOffset+16-8+ib].iSize-2) >> 1;
			return m_pPGEImeObj[iOffset+16-8+ib].pData;
		}
	}
	else
	{
		while (caPy[iKeyOff] == m_pPGEImeObj[iOffset+16+ib].InputKey[iKeyOff])
		{
			if (0 == strcmp(caPy, m_pPGEImeObj[iOffset+16+ib].InputKey))
			{
				iOutCount = (m_pPGEImeObj[iOffset+16+ib].iSize-2) >> 1;
				return m_pPGEImeObj[iOffset+16+ib].pData;
			}
			ib++;
		}

		ib = -1;
		while (caPy[iKeyOff] == m_pPGEImeObj[iOffset+16+ib].InputKey[iKeyOff])
		{
			if (0 == strcmp(caPy, m_pPGEImeObj[iOffset+16+ib].InputKey))
			{
				iOutCount = (m_pPGEImeObj[iOffset+16+ib].iSize-2) >> 1;
				return m_pPGEImeObj[iOffset+16+ib].pData;				
			}
			ib--;
		}
		iOutCount = (m_pPGEImeObj[iOffset+16+ib].iSize-2) >> 1;
		return m_pPGEImeObj[iOffset+16+ib].pData;
	}
}
