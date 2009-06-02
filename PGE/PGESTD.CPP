#include "pgestd.h"
#include <string.h>
#include <stdlib.h>
lua_State* L_stat = NULL;

CPGEBase::CPGEBase()
{
	m_dwClassID = PGE_UNDEFINE;
}

CPGEBase::~CPGEBase()
{
	
}

int itoa(WORD* caOut, DWORD dwNum)
{
	char caBuff[50];
	sprintf(caBuff, "%d", dwNum);
	WORD iCount = strlen(caBuff);
	WORD* wBuff = new WORD[iCount];
	for(WORD i=0; i<iCount; i++)
	{
		wBuff[i] = caBuff[i];
	}
	memcpy(caOut, wBuff, iCount*sizeof(WORD));
	SAFE_DELETE_ARY(wBuff);
	return iCount;
}

// RECT操作函数
RECT FiRect(int iLeft, int iTop, int iRight, int iBottom)
{
	RECT rBuf = {iLeft, iTop, iRight, iBottom};
	return rBuf;
}

RECT AdRect(RECT r1, RECT r2)
{
	RECT rBuf = {
		r1.left + r2.left,
		r1.top + r2.top,
		r1.right + r2.right,
		r1.bottom + r2.bottom
	};
	return rBuf;
}

RECT MuRect(RECT r1, RECT r2)
{
	RECT rBuf = {
		r1.left * r2.left,
		r1.top * r2.top,
		r1.right * r2.right,
		r1.bottom * r2.bottom
	};
	return rBuf;
}

RECT MuRect(RECT r1, float fValue)
{
	RECT rBuf = {
		r1.left * fValue,
		r1.top * fValue,
		r1.right * fValue,
		r1.bottom * fValue
	};
	return rBuf;
}

BOOL PointInRect(POINT pt, RECT r)
{ 
	return (pt.x > r.left) && (pt.y > r.top) && (pt.x < r.right) && (pt.y < r.bottom);
}

BOOL RectEqZero(RECT r)
{
	return (r.left == 0) && (r.right == 0) && (r.top == 0) && (r.bottom == 0);
}

RECT FiRectWithStr(char* caStr)
{
	char caBuf[20];
	WORD iBuf[4];
	memset(iBuf, 0, sizeof(iBuf));
	memset(caBuf, 0, sizeof(caBuf));

	int k = 0, j = 0;
	for (int i=0;i<strlen(caStr);i++)
	{
		if (caStr[i] != ',')
		{
			caBuf[k] = caStr[i];
			k++;
		}
		else
		{
			k = 0;
			if (j>2) return FiRect(0,0,0,0);
			iBuf[j] = atoi(caBuf);
			j++;
			memset(caBuf, 0, sizeof(caBuf));
		}
	}
	iBuf[j] = atoi(caBuf);
	return FiRect(iBuf[0], iBuf[1], iBuf[2], iBuf[3]);
}

RECT TrunRect(RECT& r, const int iValue)
{
	r.top = r.top>iValue ? iValue : r.top;
	r.left = r.left>iValue ? iValue : r.left;
	r.right = r.right>iValue ? iValue : r.right;
	r.bottom = r.bottom>iValue ? iValue : r.bottom;

	r.top = r.top<0 ? -r.top : r.top;
	r.left = r.left<0 ? -r.left : r.left;
	r.right = r.right<0 ? -r.right : r.right;
	r.bottom = r.bottom<0 ? -r.bottom : r.bottom;

	return r;
}

BOOL PointInRect(int x, int y, RECT r)
{
	return (x > r.left) && (y > r.top) && (x < r.right) && (y < r.bottom);
}

RECT RectAndRect(RECT r1, RECT r2)
{
	RECT rBuf = {
		r1.left>r2.left ? r1.left : r2.left,
		r1.top > r2.top ? r1.top : r2.top,
		r1.right > r2.right ? r2.right : r1.right,
		r1.bottom > r2.bottom ? r2.bottom : r1.bottom
	};
	return rBuf;
}

RECT SubRect(RECT r1, RECT r2)
{
	RECT rBuf = {
		r1.left - r2.left,
			r1.top - r2.top,
			r1.right - r2.right,
			r1.bottom - r2.bottom
			
	};
	return rBuf;
}
 
char m_caResFile[200];
char* GetResFile(char* caFileName)
{
	/*if (caFileName == NULL) return NULL;
	char caBuf[200];
	memset(caBuf, 0, sizeof(caBuf));
	int iLen = GetModuleFileName(NULL, caBuf, sizeof(caBuf));
	params[0];
	char caFilePath[200];
#ifdef _WIN32_WCE
	WideCharToMultiByte(CP_ACP, 0, caBuf, sizeof(caBuf), caFilePath, sizeof(caFilePath),0,0);
#else
	strcpy(caFilePath, caBuf);
#endif

	//char* caFilePath = TCharToChar(caBuf, sizeof(caBuf));
	if (iLen > 0)
	{
		// 查找最后一个\, 将文件名去掉, 从而获得文件所在的路径
		int iCount = strlen(caFilePath);
		for (int i = iCount - 2; i >= 0; i--)	// 从右向左查找, 最后一个字符肯定不是\,因此跳过
		{
			if ('\\' == caFilePath[i])	// 找到最右的一个
			{
				caFilePath[i + 1 ] = '\0';	// 去掉文件名
				break;
			}
		}
	}

	// 设置配置文件名
	strcat(caFilePath, "Resource\\");
	strcat(caFilePath, caFileName);


	strcpy(m_caResFile, caFilePath);
//	SAFE_DELETE(caFilePath);
	return m_caResFile;
	*/
}


FILE* PGEFileOpen(char* fileName, char* tp)
{	
	FILE* fp = fopen(fileName, tp);
	if (!fp)
	{
		char caBuf[200];
		char wBuf[100];
		sprintf(caBuf, "找不到%s", fileName);

	}
	return fp;
}
