#include ".\pgeplayer.h"
#include "pgemaps.h"
#include <string.h>
WORLD_CLASS
USE_LUA

CREATE_CLASS(CPGEPlayer)
DELETE_CLASS(CPGEPlayer)

BEGIN_DEFINE_CLASS_FUNC(CPGEPlayer, CreatePlayer)
	char* caFile = (char*)lua_tostring(L, 3); 
	int iType = (int)lua_tonumber(L, 2); 
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEPlayer*)dwCl)->CreatePlayer(iType, GetResFile(caFile)); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_DEFINE_CLASS_FUNC(CPGEPlayer, SetColRow)
	int iRow = (int)lua_tonumber(L, 3); 
	int iCol = (int)lua_tonumber(L, 2); 
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEPlayer*)dwCl)->SetColRow(iCol, iRow); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_DEFINE_CLASS_FUNC(CPGEPlayer, SetDirection)
	int iDir[9];
	for(int i=2; i<11; i++)
		iDir[i-2] = (int) lua_tonumber(L, i);
	int iCurDir = (int) lua_tonumber(L, 11);
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEPlayer*)dwCl)->SetDirection(iDir, iCurDir); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_DEFINE_CLASS_FUNC(CPGEPlayer, MoveTo)
	int ptp = (int)lua_tonumber(L, 5); 
	int mtp = (int)lua_tonumber(L, 4); 
	int y = (int)lua_tonumber(L, 3); 
	int x = (int)lua_tonumber(L, 2); 
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEPlayer*)dwCl)->MoveTo(x, y, mtp, ptp); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_DEFINE_CLASS_FUNC(CPGEPlayer, SetCtrl)
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEPlayer*)dwCl)->SetCtrl(); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_REGIST_FUNC(CPGEPlayer)
	REGIST_CREATE("newCPGEPlayer", CPGEPlayer)
	REGIST_DELETE("delCPGEPlayer", CPGEPlayer)
	REGIST_FUNC("CPGEPlayerCreatePlayer", CPGEPlayer, CreatePlayer)
	REGIST_FUNC("CPGEPlayerSetDirection", CPGEPlayer, SetDirection)
	REGIST_FUNC("CPGEPlayerSetColRow", CPGEPlayer, SetColRow)
	REGIST_FUNC("CPGEPlayerMoveTo", CPGEPlayer, MoveTo)
	REGIST_FUNC("CPGEPlayerSetCtrl", CPGEPlayer, SetCtrl)
END_REGIST_FUNC

void CreateSurfaceFormIndex(CPGESurface* pSuf, FILE* fp, FILE_OF_SIZE* pszFile,
							int index, DWORD dwBaseOffset);
CPGEPlayer::CPGEPlayer(void)
:CPGEGameObj(OBJ_PLAYER)
{
	m_pPicSurf = NULL;
	m_iCurDirection = 1;
	m_iPicCount = 0;
	m_iColCt = 0;
	memset(m_Direction, 0, sizeof(m_Direction));
	m_bCtrl = 0;

	m_ptCur.wx = m_ptCur.wy = 0;
	m_btWalkStat = 0;
	m_iDrawSurfaceIndex = 0;
	m_iWalkStepCt = 0;
	world.push_back((CPGEGameObj*)this);
}

CPGEPlayer::~CPGEPlayer(void)
{
	for(int i=0; i<m_iPicCount; i++)
	{
		if(m_pPicSurf[i]) delete (m_pPicSurf[i]);// = new CPGESurface(NULL);
	}
	SAFE_DELETE_ARY(m_pPicSurf);
	world.reMove((CPGEGameObj*)this);
	
}

PGE_RESULT CPGEPlayer::CreatePlayer(int iPicType, char* caFileName)
{
	if (iPicType)
	{
		FILE* fp = PGEFileOpen(caFileName, "rb");
		if (fp == NULL) 
		{
			return PGE_FAIL;
		}

		PGE_PAK_IMAGE_HEAD tHead;
		fread(&tHead, sizeof(PGE_PAK_IMAGE_HEAD), 1, fp);

		FILE_OF_SIZE* pszFile = new FILE_OF_SIZE[tHead.dwImageCount];

		fread(pszFile, tHead.dwImageCount*sizeof(FILE_OF_SIZE), 1, fp);

		DWORD dwImgFileBase = tHead.dwImageCount*sizeof(FILE_OF_SIZE) + sizeof(PGE_PAK_IMAGE_HEAD);

		m_pPicSurf = (CPGESurface**) new char[tHead.dwImageCount*sizeof(void*)];
		
		for(int i=0; i<tHead.dwImageCount; i++)
		{
			m_pPicSurf[i] = new CPGESurface(NULL);
			CreateSurfaceFormIndex(m_pPicSurf[i], fp, pszFile, i, dwImgFileBase);
		}
		m_iPicCount = tHead.dwImageCount;
		fclose(fp);
		fp = 0;
		SAFE_DELETE_ARY(pszFile);

	}
	return PGE_OK;
}

void CPGEPlayer::SetCtrl()
{
	world.player = (CPGEGameObj*)this;
	m_bCtrl = 1;
	CPGEGameObj* pobj = NULL;
	for(int i=world.begin(); i!=world.end(); i++)
	{
		pobj = world.GetPos(i);
		if (pobj)
		{
			if (pobj->m_btType == OBJ_PLAYER && pobj != (CPGEGameObj*)this)
				((CPGEPlayer*)pobj)->m_bCtrl = 0;
		}
	}
}

void CPGEPlayer::SetColRow(int iCol, int iRow)
{
	m_iCol = iCol;
	m_iRow = iRow;
}

void CPGEPlayer::SetDirection(int* iarDir, int iCurDir)
{
	m_Direction[DIR_EAST] = iarDir[0];
	m_Direction[DIR_SOUTH] = iarDir[1];
	m_Direction[DIR_WEST] = iarDir[2];
	m_Direction[DIR_NORTH] = iarDir[3];
	m_Direction[DIR_NE] = iarDir[4];
	m_Direction[DIR_NW] = iarDir[5];
	m_Direction[DIR_SW]= iarDir[6];
	m_Direction[DIR_SE] = iarDir[7];
	m_Direction[DIR_STAND] = iarDir[8];
	m_iCurDirection = iCurDir;
}

void CPGEPlayer::FileWalkArry(int x1, int y1, int x2, int y2)
{
	register int t;
	int distance;
	int x=0,y=0,delta_x,delta_y,incx,incy;
	delta_x = x2-x1;
	delta_y = y2-y1;
	//根据x和y的变化量设置x和y的增长方式(每次加1还是减1或者是不变)
	if(delta_x>0) 
	{
		incx=1;
	}
	else if (delta_x==0) 
	{
		incx=0;
	}
	else 
	{
		delta_x=-delta_x; 
		incx=-1;
	}


	if (delta_y>0) 
	{
		incy=1;
	}
	else if (delta_y==0) 
	{	
		incy=0;
	}
	else 
	{
		delta_y=-delta_y; 
		incy=-1;
	}


	if(delta_x>delta_y)
	{
		distance=delta_x;
	}
	else
	{
		distance=delta_y;
	}

	//开始画线了	
	//一样的Bresenham算法
	//看看右边的图先

	//两个端点也要画
	if (distance > 318) distance = 318;
	for (t=0; t<distance+2; t++)
	{
		//画点
		m_wptWalk[t].wx = x1;
		m_wptWalk[t].wy = y1; 
		x+=delta_x;					   
		y+=delta_y;						
		if(x>distance)					 
		{								 
			x-=distance;				 
			x1+=incx;					
		}								
		if(y>distance)					
		{								
			y-=distance;				
			y1+=incy;					
		}								
	}
	m_iWalkStep = distance+2;
	m_iWalkStepCt = 0;
}										


void CPGEPlayer::MoveTo(int x, int y, int iMType, int iPType)
{
	if (iPType == MOVETO_SCREEN) ((CPGEMaps*)world.map)->TranslateScreenToMap(x, y);
	if (iMType == MOVETO_IMM)
	{
		if (m_bCtrl)
			((CPGEMaps*)world.map)->MoveMap(x, y);
		m_ptCur.wx = x;
		m_ptCur.wy = y;
		return;
	}
	else
	{
		FileWalkArry(m_ptCur.wx, m_ptCur.wy, x, y);
	}
	m_btWalkStat = 4;
	//e -0.24 0.24
	//	ne 0.24 2.24
	//	n >2.24 <-2.75
	//	nw -0.29 -2.75
	//	w -0.24 0.24
	//	sw 0.24 2.24
	//	s >2.24 <-2.75
	//	se -0.29 -2.75
	((CPGEMaps*)world.map)->TranslateMapToScreen(x, y);
	float k = (120.f-y) / (x - 160.f);
	if(x > 160)
	{
		if (k > -0.24f && k < 0.24f) m_iCurDirection = DIR_EAST;
		if (k > 0.24f && k < 2.24f) m_iCurDirection = DIR_NE;
		if (k < -0.29f && k > -2.75f) m_iCurDirection = DIR_SE;
	} // 大方向：东
	else if(x < 160)
	{
		if (k > -0.24f && k < 0.24f) m_iCurDirection = DIR_WEST;
		if (k > 0.24f && k < 2.24f) m_iCurDirection = DIR_SW;
		if (k < -0.29f && k > -2.75f) m_iCurDirection = DIR_NW;
	}// 西

	if(y > 120)
	{
		if (k>2.24f || k<-2.75f) m_iCurDirection = DIR_SOUTH;
	}// 南
	else if(y < 120)
	{
		if (k>2.24f || k<-2.75f) m_iCurDirection = DIR_NORTH;
	}// 北

	//m_iCurDirection = DIR_EAST;
	//m_iDrawSurfaceIndex = 0;

}

void CPGEPlayer::Tick(CPGETimer* timer)
{
	// 行走、碰撞
	for (int i=0; i<m_btWalkStat; i++)
	{
		if (m_iWalkStepCt < m_iWalkStep)
		{
			if (m_bCtrl)
				((CPGEMaps*)world.map)->MoveMap(m_wptWalk[m_iWalkStepCt].wx, m_wptWalk[m_iWalkStepCt].wy);
			m_ptCur.wx = m_wptWalk[m_iWalkStepCt].wx;
			m_ptCur.wy = m_wptWalk[m_iWalkStepCt].wy;
			m_iWalkStepCt++;
		}
		else
		{
			m_btWalkStat = 0;
		}
	}

	// 动画
	if (m_btWalkStat)
	{
		m_iColCt++;
		if (m_iColCt == m_iCol) m_iColCt=0;
		// 8+的目的是跑，因该根据m_btWalkStat来确定
		m_iDrawSurfaceIndex = m_iColCt+(((m_btWalkStat-2)<<2)+m_Direction[m_iCurDirection])*m_iCol;
	}
	else
	{
		m_iDrawSurfaceIndex = m_Direction[m_iCurDirection]-1+m_Direction[DIR_STAND]*m_iCol;
	}
	if (m_bCtrl)
	{
		FILLRECT(m_rDis, 160-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wWiedth/2, 
			120-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wHeigh/2, 
			160-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wWiedth/2 + m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wWiedth,
			120-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wHeigh/2 + m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wHeigh);
	}
	else
	{
		int x = m_ptCur.wx;
		int y = m_ptCur.wy;
		((CPGEMaps*)world.map)->TranslateMapToScreen(x, y);
		FILLRECT(m_rDis, x-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wWiedth/2,
			y-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wHeigh/2,
			x-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wWiedth/2+m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wWiedth,
			y-m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wHeigh/2+m_pPicSurf[m_iDrawSurfaceIndex]->GetSurfaceInfo()->wHeigh);

	}

	RECT rScreen = {0,0,320,240};
	RECT r = RectAndRect(m_rDis, rScreen);

	r.bottom = r.bottom-r.top;
	r.right = r.right - r.left;
	r.left = r.top = 0;

	if (m_rDis.top < 0)
	{
		r.top = -m_rDis.top;
		r.bottom = r.top+r.bottom;
	}

	if (m_rDis.left < 0)
	{
		r.left = -m_rDis.left;
		r.right = r.left + r.right;
	}

	m_rDis.left = m_rDis.left > 0 ? m_rDis.left : 0;
	m_rDis.top = m_rDis.top > 0 ? m_rDis.top : 0;
	m_rSrc = r;

}

void CPGEPlayer::Draw(CPGESurface* pMainSuf)
{
	if (m_rDis.right>0 && m_rDis.left <320 && m_rDis.top <240 && m_rDis.bottom >0)
		pMainSuf->Blt(m_pPicSurf[m_iDrawSurfaceIndex], &m_rSrc, &m_rDis, PGE_BLT_ALPHA);

}

void CreateSurfaceFormIndex(CPGESurface* pSuf, FILE* fp, FILE_OF_SIZE* pszFile, int index, DWORD dwBaseOffset)
{
	int inil = 0;

	DWORD dwFileSeek = dwBaseOffset + pszFile[index].dwStart;

	PGEPicBaseHead PicHead;
	fseek(fp, dwFileSeek, SEEK_SET);
	fread(&PicHead, sizeof(PGEPicBaseHead), 1, fp);

	DWORD dwpgpX = PicHead.wWidth;
	DWORD dwpgpY = PicHead.wHeight;
	BOOL bAlpha = PicHead.bBitCount & 0x80;
	BYTE btCount = PicHead.bBitCount & 0x7f;
	BOOL bCompress = PicHead.bCompess;

	pSuf->CreateSurface(dwpgpY, dwpgpX, PGE_PGP_SURFACE);
	if (PicHead.wPalPiexlCount)
	{
		WORD* pwPal = new WORD[PicHead.wPalPiexlCount];
		fseek(fp, dwFileSeek + PicHead.dwPalOffset, SEEK_SET);
		fread(pwPal, PicHead.dwPalSize, 1, fp);
		pSuf->SetPal(pwPal, PicHead.wPalPiexlCount);
	}

	DWORD dwSize = dwpgpX * dwpgpY;

	BYTE* pbtMapReadBuf = new BYTE[PicHead.dwDeComSize];
	BYTE*  pbtPic = new BYTE[PicHead.dwDeComSize];
	fseek(fp, dwFileSeek + PicHead.dwDataOffset, SEEK_SET);
	fread(pbtMapReadBuf, PicHead.dwDataSize, 1, fp);
	decompress(pbtMapReadBuf, PicHead.dwDataSize, pbtPic);


	BYTE* pbtAlpha = NULL;
	if(bAlpha) 
	{
		pbtAlpha = new BYTE[PicHead.dwAlphaDeComSize];
		fseek(fp, dwFileSeek + PicHead.dwAlphaOffset, SEEK_SET);
		fread(pbtMapReadBuf, PicHead.dwAlphaSize, 1, fp);
		decompress(pbtMapReadBuf, PicHead.dwAlphaSize, pbtAlpha);
		// pSuf->EnableAlphaChanle(); 如需动态alpha则必调用
	}

	pSuf->SetPgpSurface((WORD*)pbtPic, pbtAlpha);
	pSuf->m_btUserData = inil;
	SAFE_DELETE_ARY(pbtMapReadBuf);
}
