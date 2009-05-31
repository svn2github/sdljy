#include <stdlib.h>
#include <string.h>
#include "pgemaps.h"
WORLD_CLASS
USE_LUA

CREATE_CLASS(CPGEMaps)
DELETE_CLASS(CPGEMaps)

BEGIN_DEFINE_CLASS_FUNC(CPGEMaps, LoadMap)
	char* caFile = (char*)lua_tostring(L, 2); 
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEMaps*)dwCl)->LoadMap(GetResFile(caFile)); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_DEFINE_CLASS_FUNC(CPGEMaps, MoveMap)
	int x = (int)lua_tonumber(L, 2); 
	int y = (int)lua_tonumber(L, 3); 
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); 
	((CPGEMaps*)dwCl)->MoveMap(x,y); 
	return 0; 
END_DEFINE_CLASS_FUNC

BEGIN_REGIST_FUNC(CPGEMaps)
	REGIST_CREATE("newCPGEMaps", CPGEMaps)
	REGIST_DELETE("delCPGEMaps", CPGEMaps)
	REGIST_FUNC("CPGEMapsLoadMap", CPGEMaps, LoadMap)
	REGIST_FUNC("CPGEMapsMoveMap", CPGEMaps, MoveMap)
END_REGIST_FUNC

CPGEMaps::CPGEMaps()
:CPGEGameObj(OBJ_MAP)
{
	if (!world.map)
	{
		world.map = (CPGEGameObj*)this;
	}

	m_pMapSurf = NULL;
	m_rMapColRow = FiRect(0,0,0,0);
	m_iCurPos.x = 0;
	m_iCurPos.y = 0;
	m_pImgFile = 0;
	m_wImgCount = 0;
	m_pszFile = NULL;
	memset(m_arMapBuf, 0, sizeof(m_arMapBuf));
	m_prMapDis = 0;
	m_prMapSrc = 0;
	m_wImgTotleCount = 0;
}

CPGEMaps::~CPGEMaps()
{
	if (m_pMapSurf)
	{
		for (int i=0; i<m_wImgCount; i++)
		{
			SAFE_DELETE(m_pMapSurf[i]);
		}
		delete [] (char*)m_pMapSurf;
		m_pMapSurf = 0;
	}

	SAFE_DELETE_ARY(m_prMapDis);
	SAFE_DELETE_ARY(m_prMapSrc);


	if (!m_arMapBuf[0].pbtPic)
	{
		for (int i=1;i<21; i++)
		{
			SAFE_DELETE_ARY(m_arMapBuf[i].pbtAlpha);
			SAFE_DELETE_ARY(m_arMapBuf[i].pbtPic);
			SAFE_DELETE_ARY(m_arMapBuf[i].pbtPal);
		}
	}
	fclose(m_pImgFile);
	m_pImgFile = NULL;
	SAFE_DELETE_ARY(m_pszFile);

}

void CPGEMaps::LoadMap(char* caFileName)
{
	if (m_pImgFile)
	{
		fclose(m_pImgFile);
		m_pImgFile = NULL;
	}
	
	m_pImgFile = PGEFileOpen(caFileName, "rb");
	if (m_pImgFile == NULL) 
	{
		return;
	}

	PGE_PAK_IMAGE_HEAD tHead;
	fread(&tHead, sizeof(PGE_PAK_IMAGE_HEAD), 1, m_pImgFile);

	if (tHead.dwImageCount > m_wImgTotleCount)
	{
		m_wImgTotleCount = tHead.dwImageCount;
		if (m_pMapSurf)
		{
			for (int i=0; i<m_wImgCount; i++)
			{
				m_pMapSurf[i]->m_btUserData = 255;
				SAFE_DELETE(m_pMapSurf[i]);
			}
			delete [] (char*)m_pMapSurf;
			m_pMapSurf = 0;
		}
	
		SAFE_DELETE_ARY(m_pszFile);
		m_pszFile = new FILE_OF_SIZE[tHead.dwImageCount];

		SAFE_DELETE_ARY(m_prMapDis);
		SAFE_DELETE_ARY(m_prMapSrc);
		m_prMapDis = new RECT[tHead.dwImageCount];
		m_prMapSrc = new RECT[tHead.dwImageCount];
	}

	fread(m_pszFile, tHead.dwImageCount*sizeof(FILE_OF_SIZE), 1, m_pImgFile);

	m_dwImgFileBase = tHead.dwImageCount*sizeof(FILE_OF_SIZE) + sizeof(PGE_PAK_IMAGE_HEAD);
	m_wImgCount = tHead.dwImageCount;
	m_btMapWidth = tHead.btWidth;
	m_btMapHeight = tHead.btHeight;
	
	if(!m_pMapSurf)
	{
		m_pMapSurf = (CPGESurface**) new char[m_wImgCount*4];
		memset(m_pMapSurf, 0, m_wImgCount*4);
		for (int i=0; i<m_wImgCount; i++)
		{
			m_pMapSurf[i] = new CPGESurface(NULL);
			m_pMapSurf[i]->CreateSurface(100, 100, PGE_PGP_SURFACE);
		}
	}
	for (int i=0; i<m_wImgCount; i++)
		m_pMapSurf[i]->m_btUserData = 0;

	if (!m_arMapBuf[0].pbtPic)
	{
		m_arMapBuf[0].pbtPic = new BYTE[1];
		for (int i=1;i<21; i++)
		{
			m_arMapBuf[i].pbtAlpha = new BYTE[10000];
			m_arMapBuf[i].pbtPic = new BYTE[10000];
			m_arMapBuf[i].pbtPal = new BYTE[256];
			m_arMapBuf[i].bUse = 0;
		}
	}
	else
	{
		for (int i=1;i<21; i++)
		{
			m_arMapBuf[i].bUse = 0;
		}
	}

}

void CPGEMaps::TranslateScreenToMap(int& x, int& y)
{
	x = m_iCurPos.x + x - 160;
	y = m_iCurPos.y + y - 120;
}

void CPGEMaps::TranslateMapToScreen(int& x, int& y)
{
	x = x - m_iCurPos.x + 160;
	y = y - m_iCurPos.y + 120;
}

void CPGEMaps::MoveMap(int x, int y)
{
	RECT rMapShow = {x-160, y-120, x+160, y+120};
	RECT rMapColRow = {rMapShow.left/100, rMapShow.top/100, rMapShow.right/100, rMapShow.bottom/100};
	m_iCurPos.x = x;
	m_iCurPos.y = y;
	int iy = 0, ix = 0, iPos = 0;
	if (!RectEqRect(m_rMapColRow, rMapColRow)){
		m_rMapColRow = rMapColRow;
		for(ix=rMapColRow.left-1; ix<=rMapColRow.right+1; ix++)
		{
			iPos = (rMapColRow.top-1)*m_btMapWidth+ix;
			if (iPos >=0 && iPos < m_wImgCount)
			{
				m_arMapBuf[m_pMapSurf[iPos]->m_btUserData].bUse = 0;
				m_pMapSurf[iPos]->m_btUserData = 0;
			}

			iPos = (rMapColRow.bottom+1)*m_btMapWidth+ix;
			if (iPos >=0 && iPos < m_wImgCount)
			{
				m_arMapBuf[m_pMapSurf[iPos]->m_btUserData].bUse = 0;
				m_pMapSurf[iPos]->m_btUserData = 0;
			}
		}

		for(iy=rMapColRow.top-1; iy<=rMapColRow.bottom+1; iy++)
		{
			iPos = iy*m_btMapWidth+rMapColRow.left-1;
			if (iPos >=0 && iPos < m_wImgCount)
			{
				m_arMapBuf[m_pMapSurf[iPos]->m_btUserData].bUse = 0;
				m_pMapSurf[iPos]->m_btUserData = 0;
			}

			iPos = iy*m_btMapWidth+rMapColRow.right+1;
			if (iPos >=0 && iPos < m_wImgCount)
			{
				m_arMapBuf[m_pMapSurf[iPos]->m_btUserData].bUse = 0;
				m_pMapSurf[iPos]->m_btUserData = 0;
			}
		}
	}
	for (iy = rMapColRow.top; iy<=rMapColRow.bottom; iy++)
	{
		for (ix=rMapColRow.left; ix<=rMapColRow.right; ix++)
		{
			iPos = iy*m_btMapWidth+ix;
			if (iPos<0 || iPos>m_wImgCount-1) continue;
			if (!m_pMapSurf[iPos]->m_btUserData)
			{
				CreateSurfaceForMap(m_pMapSurf[iPos], iPos);
			}
			RECT rTitle = {
				ix*100,
					iy*100,
					ix*100+100,
					iy*100+100
			};
			RECT rBuf = RectAndRect(rTitle, rMapShow);
			m_prMapSrc[iPos] = SubRect(rBuf, rTitle);
			m_prMapSrc[iPos].bottom+=100;
			m_prMapSrc[iPos].right+=100;
			m_prMapDis[iPos].left = rBuf.left-rMapShow.left;
			m_prMapDis[iPos].top = rBuf.top-rMapShow.top;
			m_prMapDis[iPos].right = rBuf.right-rMapShow.left;
			m_prMapDis[iPos].bottom = rBuf.bottom-rMapShow.top;

		}
	}


}

void CPGEMaps::Draw(CPGESurface* pMainSuf)
{
	for (int i=0; i<m_wImgCount; i++){
		if (m_pMapSurf[i]->m_btUserData){
			pMainSuf->Blt(m_pMapSurf[i], m_prMapSrc+i, m_prMapDis+i);
		}
	}
}

void CPGEMaps::Refash()
{
	MoveMap(m_iCurPos.x, m_iCurPos.y);
}

void CPGEMaps::CreateSurfaceForMap(CPGESurface* pSuf, int index)
{
	int inil = 0;
	for(int i=1;i<21;i++)
	{
		inil = i;
		if(!m_arMapBuf[i].bUse) break;
	}

	DWORD dwFileSeek = m_dwImgFileBase + m_pszFile[index].dwStart;

	PGEPicBaseHead PicHead;
	fseek(m_pImgFile, dwFileSeek, SEEK_SET);
	fread(&PicHead, sizeof(PGEPicBaseHead), 1, m_pImgFile);

	DWORD dwpgpX = PicHead.wWidth;
	DWORD dwpgpY = PicHead.wHeight;
	BOOL bAlpha = PicHead.bBitCount & 0x80;
	BYTE btCount = PicHead.bBitCount & 0x7f;
	BOOL bCompress = PicHead.bCompess;

	if (PicHead.wPalPiexlCount)
	{
		fseek(m_pImgFile, dwFileSeek + PicHead.dwPalOffset, SEEK_SET);
		fread(m_arMapBuf[inil].pbtPal, PicHead.dwPalSize, 1, m_pImgFile);
		pSuf->SetPal((WORD*)m_arMapBuf[inil].pbtPal, PicHead.wPalPiexlCount);
	}

	DWORD dwSize = dwpgpX * dwpgpY;

	fseek(m_pImgFile, dwFileSeek + PicHead.dwDataOffset, SEEK_SET);

	fread(m_btMapReadBuf, PicHead.dwDataSize, 1, m_pImgFile);
	decompress(m_btMapReadBuf, PicHead.dwDataSize, m_arMapBuf[inil].pbtPic);

	if(bAlpha) 
	{
		fseek(m_pImgFile, dwFileSeek + PicHead.dwAlphaOffset, SEEK_SET);
		fread(m_btMapReadBuf, PicHead.dwAlphaSize, 1, m_pImgFile);
		decompress(m_btMapReadBuf, PicHead.dwAlphaSize, m_arMapBuf[inil].pbtAlpha);
	}


	pSuf->SetPgpSurface((WORD*)m_arMapBuf[inil].pbtPic, NULL);
	pSuf->m_btUserData = inil;
	m_arMapBuf[inil].bUse = 1;
}

void CPGEMaps::Tick(CPGETimer* timer)
{

}















