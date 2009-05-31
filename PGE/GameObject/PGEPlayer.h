#pragma once
#include "pgegameobj.h"

enum PLAYER_DIR		// 方向	
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

enum PIC_TYPE	// 图像类型
{
	PIC_ARRAY = 0,	// 动画序列，单张图片多区
	FILE_ARRAY,	// 文件序列，多张图片单区
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

	// 图像相关
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
	int m_iCol;		// 动画序列
	int m_iRow;		// 方向序列
	char m_Direction[9];
	int m_iColCt;

	BYTE m_bCtrl;

	BYTE m_btWalkStat;
	WPOINT m_wptWalk[320];
	int m_iWalkStepCt;
	int m_iWalkStep;
	int m_iDrawSurfaceIndex;


	// 人物属性
};

DECLEAR_CREATE_CLASS(CPGEPlayer);
DECLEAR_DELETE_CLASS(CPGEPlayer);
DECLEAR_REGIST_FUNC(CPGEPlayer);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, CreatePlayer);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, SetColRow);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, SetDirection);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, MoveTo);
DECLEAR_DEFINE_CLASS_FUNC(CPGEPlayer, SetCtrl);
