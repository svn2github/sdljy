#pragma once
#include "pgegameobj.h"
#define PGE_MAX_MAPCOUNT 5

struct mapBuffer
{
	BYTE* pbtPic;
	BYTE* pbtAlpha;
	BYTE* pbtPal;
	BOOL bUse;
};

class CPGEMaps : public CPGEGameObj
{
public:

	CPGEMaps();
	~CPGEMaps();

	void LoadMap(char* caFileName);
	void TranslateScreenToMap(int& x, int& y);
	void MoveMap(int x, int y);
	virtual void Draw(CPGESurface* pMainSuf);
	virtual void Tick(CPGETimer* timer);
	void TranslateMapToScreen(int& x, int& y);
	void Refash();

private:
	CPGESurface** m_pMapSurf;
	BYTE m_btMapWidth;
	BYTE m_btMapHeight;
	RECT m_rMapColRow;
	POINT m_iCurPos;

	RECT* m_prMapDis;
	RECT* m_prMapSrc;
	void CreateSurfaceForMap(CPGESurface* pSuf, int index);
	mapBuffer m_arMapBuf[21];
	BYTE* m_btMapReadBuf[10000];
	FILE* m_pImgFile;
	FILE_OF_SIZE* m_pszFile;
	DWORD m_dwImgFileBase;
	WORD m_wImgCount;

	WORD m_wImgTotleCount;
};

DECLEAR_CREATE_CLASS(CPGEMaps);
DECLEAR_DELETE_CLASS(CPGEMaps);
DECLEAR_REGIST_FUNC(CPGEMaps);
DECLEAR_DEFINE_CLASS_FUNC(CPGEMaps, LoadMap);
DECLEAR_DEFINE_CLASS_FUNC(CPGEMaps, MoveMap);