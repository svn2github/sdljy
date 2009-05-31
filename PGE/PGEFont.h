// PGEFont.h: interface for the CPGEFont class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "pgestd.h"

#define ENGFONTWIDTH 6
#define CHSEENGWIDTH 4

typedef struct PGE_FONTFILE_HEAD
{
	DWORD dwfileSize;
	BYTE btx;
	BYTE bty;
	BYTE btFontSize;
	char Name[10];
}PGE_FONT_INFO;

class CPGEFont  : public CPGEBase
{
public:
	CPGEFont();
	virtual ~CPGEFont();

	PGE_RESULT CreateFont(char* caFontFile);

	BYTE* GetText(char* caStr);

	BYTE* GetText(BYTE btPage, BYTE btPos);
	
	PGE_FONTFILE_HEAD* GetFontInfo();

private:
	BYTE* m_pbtFontData;
	PGE_FONTFILE_HEAD m_FontInfo;
	BYTE* m_pTxtOutBuf;
	BYTE btFontModelBuf[32];

};

