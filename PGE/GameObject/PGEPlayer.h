#pragma once
#include "pgegameobj.h"

enum PLAYER_DIR		// ����	
{
	DIR_EAST = 0,
	DIR_SOUTH,
	DIR_WEST,
	DIR_NORTH,
	DIR_NE,
	DIR_NW,
	DIR_SW,
	DIR_SE,
	DIR_STAND,
};

enum PIC_TYPE	// ͼ������
{
	PIC_ARRAY = 0,	// �������У�����ͼƬ����
	FILE_ARRAY,	// �ļ����У�����ͼƬ����
};

enum PLAYER_MOVETO_TYPE
{
	MOVETO_IMM = 0,
	MOVETO_DALY,
	MOVETO_SCREEN,
	MOVETO_MAP,
};

class CPGEPlayer :
	public CPGEGameObj
{
public:
	CPGEPlayer(void);
	~CPGEPlayer(void);

	virtual void Tick(CPGETimer* timer);
	virtual void Draw(CPGESurface* pMainSuf);

	// ͼ�����
	CPGESurface** m_pPicSurf;
	PGE_RESULT CreatePlayer(int iPicType, char* caFileName);
	void SetCtrl();
	void SetColRow(int iCol, int iRow);
	void SetDirection(int* iarDir, int iCurDir);
	void MoveTo(int x, int y, int iMType = MOVETO_DALY, int iPType = MOVETO_SCREEN);

	void FileWalkArry(int x1, int y1, int x2, int y2);
	RECT m_rSrc;
	RECT m_rDis;
	int m_iCurDirection;
	int m_iPicCount;
	int m_iCol;		// ��������
	int m_iRow;		// ��������
	char m_Direction[9];
	int m_iColCt;

	BYTE m_bCtrl;

	BYTE m_btWalkStat;
	WPOINT m_wptWalk[320];
	int m_iWalkStepCt;
	int m_iWalkStep;
	int m_iDrawSurfaceIndex;


	// ��������
};

DECLEAR_CREATE_CLASS(CPGEPlayer);
DECLEAR_DELETE_CLASS(CPGEPlayer);
DECLEAR_REGIST_FUNC(CPGEPlayer);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, CreatePlayer);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, SetColRow);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, SetDirection);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, MoveTo);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, SetCtrl);
