// PGEFont.cpp: implementation of the CPGEFont class.
//
//////////////////////////////////////////////////////////////////////

#include "PGEFont.h"
#include <string.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPGEFont::CPGEFont()
{
	m_pbtFontData = NULL;
	m_pTxtOutBuf = NULL;
	memset(&m_FontInfo, 0, sizeof(PGE_FONTFILE_HEAD));
	m_dwClassID = PGE_FONT;
}

CPGEFont::~CPGEFont()
{
	SAFE_DELETE_ARY(m_pbtFontData);
	SAFE_DELETE_ARY(m_pTxtOutBuf);

}

PGE_RESULT CPGEFont::CreateFont(char* caFontFile)
{
	FILE* fp;
	fp = PGEFileOpen(caFontFile, "rb");
	if (!fp) return PGE_FAIL;
	if (m_pbtFontData != NULL) return PGE_FAIL;

	fread(&m_FontInfo, 1, sizeof(PGE_FONTFILE_HEAD), fp);

	m_pbtFontData = new BYTE[m_FontInfo.dwfileSize];

	fread(m_pbtFontData, 1, m_FontInfo.dwfileSize, fp);

	fclose(fp);
	
	m_pTxtOutBuf = new BYTE[m_FontInfo.btx*m_FontInfo.bty];


	return PGE_OK;

	
}

PGE_FONTFILE_HEAD* CPGEFont::GetFontInfo()
{
	return &m_FontInfo;
}

BYTE* CPGEFont::GetText(BYTE btPage, BYTE btPos)
{
	// memset(m_pTxtOutBuf, 0, m_FontInfo.btx*m_FontInfo.bty*sizeof(WORD));

	BYTE by = btPage;
	BYTE bx = btPos;
	DWORD lOff = 0;
	if (by>=161 && bx>=161)
	{
		lOff = ((by-0xa1) * 94 + (bx-0xa1))*32;
	}
	else
	{
		lOff = (by + 155)*32;
	}
	

	BYTE btFontModelBuf[32];
	memcpy(btFontModelBuf, m_pbtFontData+lOff, 32);
	
	int iCount = 0;
	int ip, i;
	for (ip = 0; ip<32; ip++)
	{
		for (i=0; i<8; i++)
		{
			BYTE cBit = btFontModelBuf[ip]<<i;
			m_pTxtOutBuf[iCount] = cBit;
			iCount++;
		}
	}
	return m_pTxtOutBuf;
}

BYTE* CPGEFont::GetText(char* caStr)
{
	// memset(m_pTxtOutBuf, 0, m_FontInfo.btx*m_FontInfo.bty*sizeof(WORD));
	
	if (caStr[0] == 0) return NULL;

	BYTE by=caStr[0];
	BYTE bx = caStr[1];
	DWORD lOff = 0;
	
	if (by > 0x80)
	{
		by -= 0x81;
		bx -= 0x40;
		lOff =(by*0xbf+bx+0x81)<<5;
	}
	else
	{
		lOff = by<<5;
	}
	
	memcpy(btFontModelBuf, m_pbtFontData+lOff, 32);
	
	lOff = 0;
	for (by = 0; by<32; by++)
	{
		for (bx=0; bx<8; bx++)
		{
			m_pTxtOutBuf[lOff] = (btFontModelBuf[by]<<bx) & 0x80;
			lOff++;
		}
	}
	return m_pTxtOutBuf;
}
