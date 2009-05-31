#pragma once
#include "pgestd.h"

typedef struct tgaPGECnInput
{
	WORD iSize;
	char InputKey[8];
	char* pData;
}PGEImeObject;

#define PGE_IME_OBJCOUNT 858

class CPGEIme :
	public CPGEBase
{
public:
	CPGEIme(void);
	virtual ~CPGEIme(void);

	PGE_RESULT LoadIme(char* caFileName);
	char* GetWords(char* caPy, int& iOutCount);

private:
	PGEImeObject* m_pPGEImeObj;
};
