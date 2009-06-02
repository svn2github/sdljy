// PGESurface.cpp: implementation of the CPGESurface class.
//
//////////////////////////////////////////////////////////////////////

#include "PGESurface.h"
#include "math.h"
#include <string.h>
extern DWORD g_dwDrawCmds;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

__inline WORD RGBto16bit555(unsigned char r,unsigned char g,unsigned char b)
{	
	return (((WORD)r<<7)&0x7c00)|(((WORD)g<<2)&0x03e0)|((WORD)b>>3);
}
__inline void MakeAlpha(WORD* wpSrc, WORD* wpDes, BYTE btAlpha);
__inline void MakeADD(WORD* wpSrc, WORD* wpDes);

#define PI 3.1415926
int farSin[360];		// sin函数值表
int farCos[360];		// cos函数值表

typedef struct tagBITMAPFILEHEADER
{
WORD bfType; // 位图文件的类型，必须为BM
DWORD bfSize; // 位图文件的大小，以字节为单位
WORD bfReserved1; // 位图文件保留字，必须为0
WORD bfReserved2; // 位图文件保留字，必须为0
DWORD bfOffBits; // 位图数据的起始位置，以相对于位图
// 文件头的偏移量表示，以字节为单位
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
DWORD biSize; // 本结构所占用字节数
LONG biWidth; // 位图的宽度，以像素为单位
LONG biHeight; // 位图的高度，以像素为单位
WORD biPlanes; // 目标设备的级别，必须为1
WORD biBitCount;// 每个像素所需的位数，必须是1(双色),
// 4(16色)，8(256色)或24(真彩色)之一
DWORD biCompression; // 位图压缩类型，必须是 0(不压缩),
// 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
DWORD biSizeImage; // 位图的大小，以字节为单位
LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数
LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数
DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数
DWORD biClrImportant;// 位图显示过程中重要的颜色数
} BITMAPINFOHEADER;
typedef struct tagRGBQUAD {
BYTE rgbBlue;// 蓝色的亮度(值范围为0-255)
BYTE rgbGreen; // 绿色的亮度(值范围为0-255)
BYTE rgbRed; // 红色的亮度(值范围为0-255)
BYTE rgbReserved;// 保留，必须为0
} RGBQUAD;

CPGESurface::CPGESurface(CPGEDevice* pDevice)
{
	m_dwClassID = PGE_SURFACE;
	m_pwMainData = NULL;
	m_bLocked = FALSE;
	m_pDevice = pDevice;
	memset(&m_SurfaceInfo, 0, sizeof(m_SurfaceInfo));
	m_SurfaceInfo.wSurfaceTp = PGE_NOMARL_SURFACE;
	m_dwMaskColor = PGE_NONECOLOR;
	m_dwDataSize = 0;
	m_pFont = NULL;
	m_pbtAlphaChanle = NULL;
	m_pbtAlphaChanleStatic = NULL;
	m_bAlphaChanle = FALSE;
	m_btAlpha = 16;
	m_pwPal = NULL;
	m_btSurfaceBit = 16;
	m_btUserData = 0;

	if (farSin[0])
	for (int i=0; i<360; i++)
	{
		farSin[i] = sin((float)(PI *(float)i)/180)*1024;
		farCos[i] = cos((float)(PI *(float)i)/180)*1024;

		if(i==0)
		{
			farSin[i] = 0;
			farCos[i] = 1*1024;
		}
		else if (i==90)
		{
			farSin[i] = 1*1024;
			farCos[i] = 0*1024;
		}
		else if (i==180)
		{
			farSin[i] = 0*1024;
			farCos[i] = -1*1024;
		}
		else if (i==270)
		{
			farSin[i] = -1*1024;
			farCos[i] = 0*1024;
		}
		else if (i==360)
		{
			farSin[i] = 0;
			farCos[i] = 1*1024;
		}

	}
	m_dwScale = (1<<14);
	m_iRot = 0;

}

CPGESurface::~CPGESurface()
{
	if (m_btUserData<255)
	{
		SAFE_DELETE_ARY(m_pwMainData);
		SAFE_DELETE(m_pFont);
		SAFE_DELETE_ARY(m_pbtAlphaChanle);
		SAFE_DELETE_ARY(m_pbtAlphaChanleStatic);
		SAFE_DELETE_ARY(m_pwPal);
	}

}

void CPGESurface::Clean()
{
	memset(m_pwMainData, 0, m_dwDataSize*sizeof(WORD));
}

PGE_RESULT CPGESurface::CreateSurface(WORD wGrHeight, WORD wGrWide, BYTE btSurfaceTp, DWORD dwMaskColor)
{
	if (NULL != m_pwMainData)
	{
		if (m_SurfaceInfo.wSurfaceTp == PGE_MAIN_SURFACE)
		{
			return PGE_FAIL;
		}
		delete m_pwMainData;
	}
	m_SurfaceInfo.wSurfaceTp = btSurfaceTp;
	m_SurfaceInfo.wHeigh = wGrHeight;
	m_SurfaceInfo.wWiedth = wGrWide;
	m_dwMaskColor = dwMaskColor;

	if (PGE_MAIN_SURFACE == btSurfaceTp)
	{
		DWORD dwDeviceSize = m_pDevice->GetDeviceSize();
		m_dwDataSize = dwDeviceSize;
		m_pwMainData = new WORD[m_dwDataSize];

		return PGE_OK;
	}
	
	m_dwDataSize = wGrWide * wGrHeight;

	if (PGE_PGP_SURFACE == btSurfaceTp)
	{
		return PGE_OK;
		//if (!m_pbtAlphaChanleStatic) m_pbtAlphaChanleStatic = new BYTE[m_dwDataSize];
	}
	else
	{
		m_pwMainData = new WORD[m_dwDataSize];
		if (m_bAlphaChanle) 
		{
			if (!m_pbtAlphaChanle) m_pbtAlphaChanle = new BYTE[m_dwDataSize];
			if (!m_pbtAlphaChanleStatic) m_pbtAlphaChanleStatic = new BYTE[m_dwDataSize];
		}
	}


	return PGE_OK;
}

void CPGESurface::Lock(void** pBuff)
{
	(*pBuff) = m_pwMainData;
	m_bLocked = TRUE;
}

void CPGESurface::unLock()
{
	m_bLocked = FALSE;
}

DWORD CPGESurface::GetMaskColor()
{
	return m_dwMaskColor;
}


void CPGESurface::SetScale(float fScale)
{
	if (fScale <= 0.0f) fScale = 1.0f;
	m_dwScale = fScale*(1<<14);
}

float CPGESurface::GetScale()
{
	return (float)m_dwScale/(1<<14);
}

DWORD CPGESurface::GetScaleOffset()
{
	return m_dwScale;
}

void CPGESurface::SetRot(int iRot)
{
	if (iRot > 359) iRot = 359;
	if (iRot < 0) iRot = 0;
	m_iRot = iRot;
}

int CPGESurface::GetRot()
{
	return m_iRot;
}

DWORD CPGESurface::GetDataSize()
{
	return m_dwDataSize;
}

WORD* CPGESurface::GetPal()
{
	return m_pwPal;
}

void CPGESurface::Blt8(CPGESurface* pSrcSurface, RECT* prSrc, RECT* prDis, WORD wFlag)
{

	WORD wDrawFlag = wFlag & 0xf;
	BOOL bMask = wFlag & 0xf0;
	BOOL bRotScl = wFlag & 0xf00;
	RECT rSrc;// = *prSrc;
	RECT rDis;// = *prDis;
	PGESurfaceInfo* pSrcInfo = pSrcSurface->GetSurfaceInfo();
	// 正式版不提供有效性检查
	if (prSrc == NULL)
	{
		rSrc.left = rSrc.top = 0;
		rSrc.bottom = pSrcInfo->wHeigh;
		rSrc.right = pSrcInfo->wWiedth;
	}
	else
	{
		rSrc = *prSrc;
	}

	if (prDis == NULL)
	{
		rDis.left = rDis.top = 0;
		rDis.bottom = m_SurfaceInfo.wHeigh;
		rDis.right = m_SurfaceInfo.wWiedth;
	}
	else
	{
		rDis = *prDis;
	}

	if (rSrc.top > pSrcInfo->wHeigh) rSrc.top = pSrcInfo->wHeigh;	// 是否超过源图像的边界
	if (rSrc.bottom > pSrcInfo->wHeigh) rSrc.bottom = pSrcInfo->wHeigh;
	if (rSrc.left > pSrcInfo->wWiedth) rSrc.left = pSrcInfo->wWiedth;
	if (rSrc.right > pSrcInfo->wWiedth) rSrc.right = pSrcInfo->wWiedth;

	if ((rSrc.bottom- rSrc.top)+rDis.top > rDis.bottom) rSrc.bottom = rDis.bottom-rDis.top + rSrc.top; // 是否超过目的图像的边界
	if ((rSrc.right- rSrc.left)+rDis.left > rDis.right) rSrc.right = rDis.right-rDis.left + rSrc.left;


	int ix = 0, iy = 0, idx = rDis.left, idy = rDis.top;
	WORD wDisHeight = rDis.bottom - rDis.top;	// 目的矩形高
	WORD wDisWide = rDis.right - rDis.left;	// 目的矩形宽
	WORD wSrcWide = rSrc.right - rSrc.left;
	BYTE* pwSrc = NULL;
	WORD* pwDis = m_pwMainData;
	DWORD dwDisPos = 0;
	DWORD dwSrcPos = 0;	// 目的和源像素的偏移量
	DWORD dwSrcMask = PGE_NONECOLOR;
	
	WORD* pPal = pSrcSurface->GetPal();


	int x,y,nx,ny,iRot;
	DWORD dwScale;

#define PosRotScl	\
	y = (rDis.top + (iy-rSrc.top)) - (rDis.top + ((rSrc.bottom - rSrc.top)>>1));	\
	x = rDis.left - rSrc.left + ix - (rDis.left + ((rSrc.right - rSrc.left)>>1));	\
	dwScale = pSrcSurface->GetScaleOffset();	\
	iRot = pSrcSurface->GetRot();	\
	if (dwScale == 16384)	\
	{	\
	nx = ((x*farCos[iRot])>>10) + ((y*farSin[iRot])>>10)	\
	+(rDis.left+((rSrc.right - rSrc.left)>>1));	\
	ny = ((y*farCos[iRot])>>10) - ((x*farSin[iRot])>>10)	\
	+(rDis.top+((rSrc.bottom - rSrc.top)>>1));	\
	}	\
	else	\
	{	\
	nx = ((((x*farCos[iRot])>>10)*(long)dwScale)>>14) + ((y*farSin[iRot])>>10)	\
	+(rDis.left+((rSrc.right - rSrc.left)>>1));	\
	ny = ((((y*farCos[iRot])>>10)*(long)dwScale)>>14) - ((x*farSin[iRot])>>10)	\
	+(rDis.top+((rSrc.bottom - rSrc.top)>>1));	\
	}	\
	if (nx < 0) continue;	\
	nx = nx > m_SurfaceInfo.wWiedth ? m_SurfaceInfo.wWiedth : nx;	\
	ny = ny > m_SurfaceInfo.wHeigh ? m_SurfaceInfo.wHeigh : ny;	\
	dwDisPos = ny*m_SurfaceInfo.wWiedth+nx;	\
	if (dwDisPos >= m_dwDataSize) continue;	\

#define RotSclFi \
	if (dwDisPos < m_dwDataSize-m_SurfaceInfo.wWiedth && dwDisPos > m_SurfaceInfo.wWiedth) \
	{ \
	*(pwDis + dwDisPos+1) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos-1) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos+m_SurfaceInfo.wWiedth) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos+m_SurfaceInfo.wWiedth + 1) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos+m_SurfaceInfo.wWiedth - 1) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos-m_SurfaceInfo.wWiedth) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos-m_SurfaceInfo.wWiedth + 1) = *(pwDis + dwDisPos); \
	*(pwDis + dwDisPos-m_SurfaceInfo.wWiedth - 1) = *(pwDis + dwDisPos); \
	} \


	if (wDrawFlag == PGE_BLT_ALPHA)	// 如果使用alpha
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		BYTE* pbtSrcAlpha = pSrcSurface->GetAlphaChanle();
		BYTE btSrcAlphaVal = pSrcSurface->GetAlphaVal();
		if (bMask) dwSrcMask = pSrcSurface->GetMaskColor();

		DWORD dwSrcCol = rSrc.top * pSrcInfo->wWiedth;
		DWORD dwDisCol = rDis.top * m_SurfaceInfo.wWiedth;
		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = dwSrcCol + ix;	// 源地址偏移
				if(pPal[pwSrc[dwSrcPos]] == dwSrcMask) continue; // 去掉遮罩颜色

				if (!bRotScl)
				{
					dwDisPos = dwDisCol + idx;	// 目的地址偏移
					if (pbtSrcAlpha == NULL)
					{
						MakeAlpha(pPal+(*(pwSrc + dwSrcPos)), 
							pwDis + dwDisPos, 
							btSrcAlphaVal);	
					}
					else
					{
						if (pbtSrcAlpha[dwSrcPos] == 31)
						{
							*(pwDis + dwDisPos) = pPal[*(pwSrc + dwSrcPos)];
						}
						else if (pbtSrcAlpha[dwSrcPos] != 0)
						{
							MakeAlpha(pPal+(*(pwSrc + dwSrcPos)), 
								pwDis + dwDisPos, 
								pbtSrcAlpha[dwSrcPos]);	
						}
						// 在物体周围画一个框
						//if (dwSrcPos>1 && dwSrcPos < pSrcSurface->GetDataSize())
						//{
						//	if ((pbtSrcAlpha[dwSrcPos] == 0 && pbtSrcAlpha[dwSrcPos+1] !=0)
						//		|| (pbtSrcAlpha[dwSrcPos+1] == 0 && pbtSrcAlpha[dwSrcPos] !=0))
						//	{
						//		*(pwDis + dwDisPos) = PGE_PURPLE;
						//	}

						//}
						//*(pwDis + dwDisPos)=*(pwSrc + dwSrcPos);
					}
				}
				else
				{
					PosRotScl;
					if (pbtSrcAlpha == NULL)
					{
						MakeAlpha(pPal+(*(pwSrc + dwSrcPos)), 
							pwDis + dwDisPos, 
							btSrcAlphaVal);	
					}
					else
					{
						if (pbtSrcAlpha[dwSrcPos] == 31)
						{
							*(pwDis + dwDisPos) = pPal[*(pwSrc + dwSrcPos)];
						}
						else if (pbtSrcAlpha[dwSrcPos] != 0)
						{
							MakeAlpha(pPal+(*(pwSrc + dwSrcPos)), 
								pwDis + dwDisPos, 
								pbtSrcAlpha[dwSrcPos]);	
						}
						//*(pwDis + dwDisPos)=*(pwSrc + dwSrcPos);
					}
					RotSclFi;
				}

				idx++;
			}

			//	idy++;
			dwSrcCol += pSrcInfo->wWiedth; //行
			dwDisCol += m_SurfaceInfo.wWiedth;
		}
		pSrcSurface->unLock();
	}
	else if (wDrawFlag == PGE_BLT_NORMAL)
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面
		dwSrcMask = pSrcSurface->GetMaskColor();
		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = iy * pSrcInfo->wWiedth + ix;	// 源地址偏移

				if (!bRotScl)
				{
					dwDisPos = idy * m_SurfaceInfo.wWiedth + idx;	// 目的地址偏移
					if (dwSrcMask != pPal[pwSrc[dwSrcPos]])
					{
						*(pwDis + dwDisPos)=pPal[*(pwSrc + dwSrcPos)];
					}
				}
				else
				{
					PosRotScl;
					if (dwSrcMask !=  pPal[pwSrc[dwSrcPos]])
					{
						*(pwDis + dwDisPos)=pPal[*(pwSrc + dwSrcPos)];
					}
					RotSclFi;
				}


				idx++;
			}
			idy++;
		}
		pSrcSurface->unLock();
		
	}
	else if (wDrawFlag == PGE_BLT_ADD)
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		if (bMask) dwSrcMask = pSrcSurface->GetMaskColor();

		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = iy * pSrcInfo->wWiedth + ix;	// 源地址偏移

				if (!bRotScl)
				{
					dwDisPos = idy * m_SurfaceInfo.wWiedth + idx;	// 目的地址偏移
					if (pPal[pwSrc[dwSrcPos]] != dwSrcMask)
					{
						MakeADD(pPal+(*(pwSrc + dwSrcPos)), pwDis + dwDisPos);	
					}
				}
				else
				{
					PosRotScl;
					if (pPal[pwSrc[dwSrcPos]] != dwSrcMask)
					{
						MakeADD(pPal+(*(pwSrc + dwSrcPos)), pwDis + dwDisPos);	
					}
					RotSclFi;
				}
				idx++;
			}
			idy++;
		}
		pSrcSurface->unLock();
	}
}

void CPGESurface::Blt(CPGESurface* pSrcSurface, RECT* prSrc, RECT* prDis, WORD wFlag)
{
	if (m_btSurfaceBit == 8)
	{
		return;
	}

	if (pSrcSurface->m_btSurfaceBit == 8)
	{
		Blt8(pSrcSurface, prSrc, prDis, wFlag);
		return;
	}

	WORD wDrawFlag = wFlag & 0xf;
	BOOL bMask = wFlag & 0xf0;
	BOOL bRotScl = wFlag & 0xf00;
	BOOL bAndAlpha = wFlag & 0xf000;
	RECT rSrc;// = *prSrc;
	RECT rDis;// = *prDis;
	PGESurfaceInfo* pSrcInfo = pSrcSurface->GetSurfaceInfo();
	// 正式版不提供有效性检查
	if (prSrc == NULL)
	{
		rSrc.left = rSrc.top = 0;
		rSrc.bottom = pSrcInfo->wHeigh;
		rSrc.right = pSrcInfo->wWiedth;
	}
	else
	{
		rSrc = *prSrc;
	}

	if (prDis == NULL)
	{
		rDis.left = rDis.top = 0;
		rDis.bottom = m_SurfaceInfo.wHeigh;
		rDis.right = m_SurfaceInfo.wWiedth;
	}
	else
	{
		rDis = *prDis;
	}
	
	if (rSrc.top > pSrcInfo->wHeigh) rSrc.top = pSrcInfo->wHeigh;	// 是否超过源图像的边界
	if (rSrc.bottom > pSrcInfo->wHeigh) rSrc.bottom = pSrcInfo->wHeigh;
	if (rSrc.left > pSrcInfo->wWiedth) rSrc.left = pSrcInfo->wWiedth;
	if (rSrc.right > pSrcInfo->wWiedth) rSrc.right = pSrcInfo->wWiedth;

	if ((rSrc.bottom- rSrc.top)+rDis.top > rDis.bottom) rSrc.bottom = rDis.bottom-rDis.top + rSrc.top; // 是否超过目的图像的边界
	if ((rSrc.right- rSrc.left)+rDis.left > rDis.right) rSrc.right = rDis.right-rDis.left + rSrc.left;


	int ix = 0, iy = 0, idx = rDis.left, idy = rDis.top;
	WORD wDisHeight = rDis.bottom - rDis.top;	// 目的矩形高
	WORD wDisWide = rDis.right - rDis.left;	// 目的矩形宽
	WORD wSrcWide = rSrc.right - rSrc.left;
	WORD* pwSrc = NULL;
	WORD* pwDis = m_pwMainData;
	DWORD dwDisPos = 0, dwSrcPos = 0;	// 目的和源像素的偏移量
	DWORD dwSrcMask = PGE_NONECOLOR;



	int x,y,nx,ny,iRot;
	DWORD dwScale;

#define PosRotScl	\
	y = (rDis.top + (iy-rSrc.top)) - (rDis.top + ((rSrc.bottom - rSrc.top)>>1));	\
	x = rDis.left - rSrc.left + ix - (rDis.left + ((rSrc.right - rSrc.left)>>1));	\
	dwScale = pSrcSurface->GetScaleOffset();	\
	iRot = pSrcSurface->GetRot();	\
	if (dwScale == 16384)	\
	{	\
		nx = ((x*farCos[iRot])>>10) + ((y*farSin[iRot])>>10)	\
			+(rDis.left+((rSrc.right - rSrc.left)>>1));	\
		ny = ((y*farCos[iRot])>>10) - ((x*farSin[iRot])>>10)	\
			+(rDis.top+((rSrc.bottom - rSrc.top)>>1));	\
	}	\
	else	\
	{	\
		nx = ((((x*farCos[iRot])>>10)*(long)dwScale)>>14) + ((y*farSin[iRot])>>10)	\
			+(rDis.left+((rSrc.right - rSrc.left)>>1));	\
		ny = ((((y*farCos[iRot])>>10)*(long)dwScale)>>14) - ((x*farSin[iRot])>>10)	\
			+(rDis.top+((rSrc.bottom - rSrc.top)>>1));	\
	}	\
	if (nx < 0) continue;	\
	nx = nx > m_SurfaceInfo.wWiedth ? m_SurfaceInfo.wWiedth : nx;	\
	ny = ny > m_SurfaceInfo.wHeigh ? m_SurfaceInfo.wHeigh : ny;	\
	dwDisPos = ny*m_SurfaceInfo.wWiedth+nx;	\
	if (dwDisPos >= m_dwDataSize) continue;	\

#define RotSclFi \
	if (dwDisPos < m_dwDataSize-m_SurfaceInfo.wWiedth && dwDisPos > m_SurfaceInfo.wWiedth) \
	{ \
		*(pwDis + dwDisPos+1) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos-1) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos+m_SurfaceInfo.wWiedth) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos+m_SurfaceInfo.wWiedth + 1) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos+m_SurfaceInfo.wWiedth - 1) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos-m_SurfaceInfo.wWiedth) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos-m_SurfaceInfo.wWiedth + 1) = *(pwDis + dwDisPos); \
		*(pwDis + dwDisPos-m_SurfaceInfo.wWiedth - 1) = *(pwDis + dwDisPos); \
	} \
	

	if (wDrawFlag == PGE_BLT_ALPHA)	// 如果使用alpha
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		BYTE* pbtSrcAlpha = pSrcSurface->GetAlphaChanle();
		BYTE btSrcAlphaVal = pSrcSurface->GetAlphaVal();
		if (bMask) dwSrcMask = pSrcSurface->GetMaskColor();

		DWORD dwSrcCol = rSrc.top * pSrcInfo->wWiedth;
		DWORD dwDisCol = rDis.top * m_SurfaceInfo.wWiedth;
		BYTE btAlphaVal;
		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = dwSrcCol + ix;	// 源地址偏移
				if(pwSrc[dwSrcPos] == dwSrcMask) continue; // 去掉遮罩颜色

				if (!bRotScl)
				{
					dwDisPos = dwDisCol + idx;	// 目的地址偏移
					if (pbtSrcAlpha == NULL)
					{
						MakeAlpha(pwSrc + dwSrcPos, 
							pwDis + dwDisPos, 
							btSrcAlphaVal);	
					}
					else
					{
						if (pbtSrcAlpha[dwSrcPos] == 31)
						{
							*(pwDis + dwDisPos) = *(pwSrc + dwSrcPos);
						}
						else if (pbtSrcAlpha[dwSrcPos] != 0)
						{
							MakeAlpha(pwSrc + dwSrcPos, 
								pwDis + dwDisPos, 
								pbtSrcAlpha[dwSrcPos]);	
						}
						//*(pwDis + dwDisPos)=*(pwSrc + dwSrcPos);
					}
				}
				else
				{
					PosRotScl;
					if (pbtSrcAlpha == NULL)
					{
						MakeAlpha(pwSrc + dwSrcPos, 
							pwDis + dwDisPos, 
							btSrcAlphaVal);	
					}
					else
					{
						if (pbtSrcAlpha[dwSrcPos] == 31)
						{
							if (bAndAlpha == PGE_BLT_ANDALPHA)
								MakeAlpha(pwSrc + dwSrcPos, 
								pwDis + dwDisPos, 
								m_pbtAlphaChanle[dwDisPos]);
							else
							*(pwDis + dwDisPos) = *(pwSrc + dwSrcPos);
						}
						else if (pbtSrcAlpha[dwSrcPos] != 0)
						{
							MakeAlpha(pwSrc + dwSrcPos, 
							pwDis + dwDisPos,
							pbtSrcAlpha[dwSrcPos]);
						}
						//*(pwDis + dwDisPos)=*(pwSrc + dwSrcPos);
					}
					RotSclFi;
				}

				idx++;
			}

		//	idy++;
			dwSrcCol += pSrcInfo->wWiedth; //行
			dwDisCol += m_SurfaceInfo.wWiedth;
		}
		pSrcSurface->unLock();
	}
	else if (wDrawFlag == PGE_BLT_NORMAL)
	{
		BYTE* pbtSrcAlpha = pSrcSurface->GetAlphaChanle();
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		if (!(bMask || bRotScl))
		{
			WORD wSize = wDisWide > (rSrc.right - rSrc.left) ? wSrcWide : wDisWide; 
			if (rDis.left + wDisWide > m_SurfaceInfo.wWiedth) wSize -= (rDis.left + wDisWide - m_SurfaceInfo.wWiedth);
			DWORD dwDisBuf = 0;
			DWORD dwSrcBuf = rSrc.top * pSrcInfo->wWiedth;
			for(iy=rSrc.top; iy<rSrc.bottom; iy++)
			{
				if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
				dwDisPos = dwDisBuf + rDis.left;
				dwSrcPos = dwSrcBuf + rSrc.left;
				memcpy(pwDis + dwDisPos, pwSrc + dwSrcPos, wSize<<1);
				
				if (bAndAlpha == PGE_CLT_COPYALPHA)
						memcpy(m_pbtAlphaChanle + dwDisPos, pbtSrcAlpha + dwSrcPos, wSize);

				dwDisBuf += m_SurfaceInfo.wWiedth;
				dwSrcBuf += pSrcInfo->wWiedth;
			}
		}
		else
		{
			dwSrcMask = pSrcSurface->GetMaskColor();
			for(iy=rSrc.top; iy<rSrc.bottom; iy++)
			{
				if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
				idx = rDis.left;
				for(ix=rSrc.left; ix<rSrc.right; ix++)
				{
					if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
					dwSrcPos = iy * pSrcInfo->wWiedth + ix;	// 源地址偏移

					if (!bRotScl)
					{
						dwDisPos = idy * m_SurfaceInfo.wWiedth + idx;	// 目的地址偏移
						if (dwSrcMask != pwSrc[dwSrcPos])
						{
							*(pwDis + dwDisPos)=*(pwSrc + dwSrcPos);
						}
					}
					else
					{
						PosRotScl;
						if (dwSrcMask != pwSrc[dwSrcPos])
						{
							*(pwDis + dwDisPos)=*(pwSrc + dwSrcPos);
						}
						RotSclFi;
					}


					idx++;
				}
				idy++;
			}
			pSrcSurface->unLock();
		}
	}
	else if (wDrawFlag == PGE_BLT_ADD)
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		if (bMask) dwSrcMask = pSrcSurface->GetMaskColor();

		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = iy * pSrcInfo->wWiedth + ix;	// 源地址偏移

				if (!bRotScl)
				{
					dwDisPos = idy * m_SurfaceInfo.wWiedth + idx;	// 目的地址偏移
					if (pwSrc[dwSrcPos] != dwSrcMask)
					{
						MakeADD(pwSrc + dwSrcPos, pwDis + dwDisPos);	
					}
				}
				else
				{
					PosRotScl;
					if (pwSrc[dwSrcPos] != dwSrcMask)
					{
						MakeADD(pwSrc + dwSrcPos, pwDis + dwDisPos);	
					}
					RotSclFi;
				}
				idx++;
			}
			idy++;
		}
		pSrcSurface->unLock();
	}
	else if (wDrawFlag == PGE_BLT_AND)
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		if (bMask) dwSrcMask = pSrcSurface->GetMaskColor();

		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = iy * pSrcInfo->wWiedth + ix;	// 源地址偏移

				if (!bRotScl)
				{
					dwDisPos = idy * m_SurfaceInfo.wWiedth + idx;	// 目的地址偏移
					if (pwSrc[dwSrcPos] != dwSrcMask)
					{
						*(pwDis + dwDisPos) &= *(pwSrc + dwSrcPos);	
					}
				}
				else
				{
					PosRotScl;
					if (pwSrc[dwSrcPos] != dwSrcMask)
					{
						*(pwDis + dwDisPos) &= *(pwSrc + dwSrcPos);	
					}
					RotSclFi;
				}

				idx++;
			}
			idy++;
		}
		pSrcSurface->unLock();

	}
	else if (wDrawFlag == PGE_BLT_OR)
	{
		pSrcSurface->Lock((void**)&pwSrc);	// 锁定源表面

		if (bMask) dwSrcMask = pSrcSurface->GetMaskColor();

		for(iy=rSrc.top; iy<rSrc.bottom; iy++)
		{
			if (iy-rSrc.top > wDisHeight) break;	// 如果超出目的矩形的下边缘，则裁剪掉
			idx = rDis.left;
			for(ix=rSrc.left; ix<rSrc.right; ix++)
			{
				if(ix-rSrc.left > wDisWide) break;	// 如果超出目的矩形的右边缘，则裁剪掉
				dwSrcPos = iy * pSrcInfo->wWiedth + ix;	// 源地址偏移

				if (!bRotScl)
				{
					dwDisPos = idy * m_SurfaceInfo.wWiedth + idx;	// 目的地址偏移
					if (pwSrc[dwSrcPos] != dwSrcMask)
					{
						*(pwDis + dwDisPos) |= *(pwSrc + dwSrcPos);	
					}
				}
				else
				{
					PosRotScl;
					if (pwSrc[dwSrcPos] != dwSrcMask)
					{
						*(pwDis + dwDisPos) |= *(pwSrc + dwSrcPos);	
					}
					RotSclFi;
				}


				idx++;
			}
			idy++;
		}
		pSrcSurface->unLock();
	}
	
}

void CPGESurface::Flip()
{	
	// 有时间的话记得验证只更新变化后区域
	//if (m_SurfaceInfo.wSurfaceTp == PGE_MAIN_SURFACE)
	{				
		WORD*pBuff = m_pDevice->BeginDraw();
		memcpy(pBuff, m_pwMainData, (m_dwDataSize<<1));
		m_pDevice->EndDraw();			
	}

}

void CPGESurface::FlipH()
{
	
	//if (m_SurfaceInfo.wSurfaceTp == PGE_MAIN_SURFACE)
	{	
		WORD ix, iy;
		WORD*pBuff = m_pDevice->BeginDraw();
		DWORD dwDisOffset = 0;
		DWORD dwSrcOffset = 0;
		WORD* wpSrc = 0;
		for(ix=0; ix<m_SurfaceInfo.wHeigh; ix++)
		{
			dwDisOffset = 0;
			wpSrc = m_pwMainData+dwSrcOffset;
			for (iy=0; iy<m_SurfaceInfo.wWiedth; iy++)
			{
				*(pBuff + dwDisOffset + ix) = *(wpSrc+m_SurfaceInfo.wWiedth-1-iy);
				dwDisOffset += m_SurfaceInfo.wHeigh;
			}
			dwSrcOffset += m_SurfaceInfo.wWiedth;
		}
		m_pDevice->EndDraw();				
	}

}

PGESurfaceInfo* CPGESurface::GetSurfaceInfo()
{
	return &m_SurfaceInfo;
}


void CPGESurface::SetAlphaVal(BYTE btAlpha)
{
	m_btAlpha = btAlpha;
}

BYTE CPGESurface::GetAlphaVal()
{
	return m_btAlpha;
}

PGE_RESULT CPGESurface::SetSurfaceInfo(PGESurfaceInfo SurfaceInfo, CPGEBase* pPGEBase)
{
	if (pPGEBase == NULL) return PGE_FAIL;
	if (pPGEBase->m_dwClassID != PGE_DEVICE)
	{
		return PGE_FAIL;
	}
	m_SurfaceInfo = SurfaceInfo;
	return PGE_OK;
}

void CPGESurface::DrawText(char* pText, int x, int y, WORD wColor)
{
	if (m_btSurfaceBit < 16) return;
	BYTE* pwFontModel = NULL;
	register int iCharPos = 0, iMuBuf = 0;
	int ix = 0, iy = 0;
	DWORD wBuf, wBuf1;
	for(int i=0; i<strlen(pText); i++)
	{
		if ((BYTE)pText[i] > 0x80)
		{
			pwFontModel = m_pFont->GetText(pText+i);
			i += 1;
			iCharPos += 2;
		}
		else
		{	
			if ((BYTE)pText[i] == '\r' && (BYTE)pText[i+1] == '\n')
			{
				i += 1;
				y += m_btFontH;
				iCharPos = 0;
				continue;
			}
			pwFontModel = m_pFont->GetText(pText+i);
			iCharPos++;
			if ((BYTE)pText[i + 1] > 0x80) iCharPos++;
			
		}
		
		
		for(iy=0; iy<16; iy++)
		{
			for (ix=0; ix<16; ix++)
			{
				if (pwFontModel[(iy<<4)+ix] != 0)
				{
					wBuf = x+ix+iMuBuf;
					if (wBuf > m_SurfaceInfo.wWiedth) continue;
					wBuf1 = (iy+y)*m_SurfaceInfo.wWiedth+wBuf;
					if (wBuf1 >= m_dwDataSize) return;
					m_pwMainData[wBuf1] = wColor;
				}
			}
		}
		iMuBuf = iCharPos*m_btFontW;

	}	
	
}

PGE_RESULT CPGESurface::CreateFont(void* pData, WORD iUseType)
{
	if (iUseType == PGE_FONT_USEMAIN)
	{
		if (m_SurfaceInfo.wSurfaceTp == PGE_MAIN_SURFACE) return PGE_FAIL;
		CPGESurface* pSurf = (CPGESurface*)m_pDevice->GetMainSurface();
		m_pFont = pSurf->GetFont(this);
	}
	else if (iUseType == PGE_FONT_USESURFACE)
	{
		CPGESurface* pSurf = (CPGESurface*)pData;
		m_pFont = pSurf->GetFont(this);	
	}
	else if (iUseType == PGE_FONT_USEFILE)
	{
		char* caFileName = (char*)pData;
		m_pFont = new CPGEFont();
		m_pFont->CreateFont(caFileName);
	}
	if (m_pFont) 
	{
		m_btFontH = m_pFont->GetFontInfo()->btFontSize;
		m_btFontW = m_btFontH / 1.85f;
		return PGE_OK;
	}
	else
	{
		return PGE_FAIL;
	}
}

void CPGESurface::SetPal(WORD* pwPal, int iPalPiexlCount)
{
	m_btSurfaceBit = 8;
//	if (!m_pwPal)
//		m_pwPal = new WORD[iPalPiexlCount];
//	memcpy(m_pwPal, pwPal, iPalPiexlCount*2);
	m_pwPal = pwPal;
}

CPGEFont* CPGESurface::GetFont(CPGEBase* pPGEBase)
{
	if (NULL == pPGEBase) return NULL;
	if (pPGEBase->m_dwClassID != PGE_SURFACE) return NULL;
	return m_pFont;
}
BYTE* CPGESurface::GetAlphaChanle()
{
	if (m_bAlphaChanle) return m_pbtAlphaChanle;
	return NULL;
}

PGE_RESULT CPGESurface::EnableAlphaChanle()
{
	m_bAlphaChanle = TRUE;
	if (PGE_PGP_SURFACE == m_SurfaceInfo.wSurfaceTp)
	{
		if (!m_pbtAlphaChanleStatic)
		{
			m_pbtAlphaChanleStatic = new BYTE[m_dwDataSize];
			if (m_pbtAlphaChanle)
				memcpy(m_pbtAlphaChanleStatic, m_pbtAlphaChanle, m_dwDataSize);

		}
		return PGE_OK;
	}

	if (!m_pbtAlphaChanle) 
	{
		m_pbtAlphaChanle = new BYTE[m_dwDataSize];
	}

	if (!m_pbtAlphaChanleStatic)
	{
		m_pbtAlphaChanleStatic = new BYTE[m_dwDataSize];
	}
	return PGE_OK;
}

void CPGESurface::SetPgpSurface(WORD* pwPicData, BYTE* pbtAlpha)
{
	if (PGE_PGP_SURFACE != m_SurfaceInfo.wSurfaceTp) return;
	m_pwMainData = pwPicData;
	if (pbtAlpha)
	{
		m_bAlphaChanle = 1;
		m_pbtAlphaChanle = pbtAlpha;
		if (m_pbtAlphaChanleStatic)	// 如果PGP面想使用静态alpha通道，必须EnableAlphaChanle
			memcpy(m_pbtAlphaChanleStatic, pbtAlpha, m_dwDataSize);
	}
}
void CPGESurface::SetAlphaChanle(RECT* pRect, BYTE* pbtAlphaData)
{
	if (!m_bAlphaChanle) return;
	if (!pbtAlphaData) return;
	int isx = 0;
	int isy = 0;
	int iStep = pRect->right - pRect->left;
	int ix = 0, iy = 0;
	WORD wPos = 0;
	for (iy=pRect->top; iy<pRect->bottom; iy++)
	{
		isx = 0;
		for (ix=pRect->left; ix<pRect->right; ix++)
		{
			wPos = iy*m_SurfaceInfo.wWiedth + ix;
			if (m_pbtAlphaChanle[wPos] != 0)
				m_pbtAlphaChanle[wPos] = 
					pbtAlphaData[isy*iStep + isx];
			isx++;
		}
		isy++;
	}

	//pgeR(5, 重置Alpha通道标志) = 1;
}

void CPGESurface::RestoreAlphaChanle(RECT* pRect)
{
	// 大量的memcpy同样耗时！
	if (!m_bAlphaChanle) return;
	//if (pgeR(5, 重置Alpha通道标志) != 1) return;
	for (int iy=pRect->top; iy<pRect->bottom; iy++)
	{		
		memcpy(m_pbtAlphaChanle+iy*m_SurfaceInfo.wWiedth, 
			m_pbtAlphaChanleStatic+iy*m_SurfaceInfo.wWiedth, pRect->right);
	}
	//pgeR(5, 重置Alpha通道标志) = 0;

}

BYTE* CPGESurface::GetStaticAlphaChanle(WORD wKey)
{
	if (wKey == 0x7819 && m_bAlphaChanle)
	{
		return m_pbtAlphaChanleStatic;
	}
	return NULL;
}

PGE_RESULT CPGESurface::CreateAlphaWithMaskColor()
{
	if (m_btSurfaceBit < 16) return PGE_FAIL;
	if (m_bAlphaChanle != 0) return PGE_FAIL;
	m_bAlphaChanle = TRUE;
	m_pbtAlphaChanleStatic = new BYTE[m_dwDataSize];
	m_pbtAlphaChanle = new BYTE[m_dwDataSize];
	
	memset(m_pbtAlphaChanle, 0, m_dwDataSize);
	memset(m_pbtAlphaChanleStatic, 0, m_dwDataSize);

	for (int i=0; i<m_dwDataSize; i++)
	{
		if (m_pwMainData[i] != m_dwMaskColor)
		{
			m_pbtAlphaChanleStatic[i] = m_pbtAlphaChanle[i] = 32;
		}
	}

	return PGE_OK;
}

void CPGESurface::SetReplaceColor(WORD wColor)
{
	m_wRplaceColor = wColor;
}

WORD CPGESurface::GetReplaceColor()
{
	return m_wRplaceColor;
}

void CPGESurface::SetPiexl(int x, int y, WORD wColor)
{
	if (m_btSurfaceBit < 16) return;
	int iPos = y*m_SurfaceInfo.wWiedth+x;
	if (iPos < 0 || iPos > m_dwDataSize) return;
	//if(m_btSurfaceBit < 16)
	//{
	//	((BYTE*)m_pwMainData)[iPos] = wColor>>8;
	//	return;
	//}
	m_pwMainData[iPos] = wColor;
}

POINT ppt[500];
void CPGESurface::DrawLine(int x1, int y1, int x2, int y2, WORD wColor)
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
	for (t=0; t<distance+2; t++)
	{
		//画点
		ppt[t].x = x1;
		ppt[t].y = y1;
		SetPiexl(x1, y1, wColor);	 
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
	int pp=0;
}													 

void CPGESurface::DrawRect(int x1, int y1, int x2, int y2, WORD wColor)
{
	DrawLine(x1, y1, x2, y1, wColor);
	DrawLine(x1, y1, x1, y2, wColor);
	DrawLine(x2, y1, x2, y2, wColor);
	DrawLine(x1, y2, x2, y2, wColor);
}


/////////////////////////////////////////////////////////////
/////////////Surface Globe Function//////////////////////////
void CreateSurfaceFromFile(CPGESurface** pSurface, char* caFileName, 
								 CPGEDevice* pDevice, DWORD wMaskColor)
{
	int iSize = strlen(caFileName) - 1;

	char caTmp = caFileName[iSize - 2];
	if (caTmp < 97) caTmp = caTmp + 32;

	if (caTmp == 'p')
	{
		FILE* pf = PGEFileOpen(caFileName, "rb");
		if (pf == NULL) 
		{
			return;
		}
		PGEPicBaseHead PicHead;
		fread(&PicHead, sizeof(PGEPicBaseHead), 1, pf);
		
		if (PicHead.dwHeadFlag != *((DWORD*)"PGEP"))
		{
			return;
		}
		DWORD dwpgpX = PicHead.wWidth;
		DWORD dwpgpY = PicHead.wHeight;
		BOOL bAlpha = PicHead.bBitCount & 0x80;
		BYTE btCount = PicHead.bBitCount & 0x7f;
		BOOL bCompress = PicHead.bCompess;
		
		WORD* pPal = NULL;
		if (PicHead.wPalPiexlCount)
		{
			pPal = new WORD[PicHead.wPalPiexlCount];
			fseek(pf, PicHead.dwPalOffset, SEEK_SET);
			fread(pPal, PicHead.dwPalSize, 1, pf);
		}

		DWORD dwSize = dwpgpX * dwpgpY;

		BYTE* pBmpData = NULL;
		if(bCompress)
			pBmpData = new BYTE[PicHead.dwDeComSize];
		else
			pBmpData = new BYTE[PicHead.dwDataSize];


		fseek(pf, PicHead.dwDataOffset, SEEK_SET);

		BYTE* btCpBuf = NULL;
		if(bCompress)
		{
			btCpBuf = new BYTE[PicHead.dwDataSize];
			fread(btCpBuf, PicHead.dwDataSize, 1, pf);
			decompress(btCpBuf, PicHead.dwDataSize, pBmpData);
			SAFE_DELETE_ARY(btCpBuf);
		}
		else
		{
			fread(pBmpData, PicHead.dwDataSize, 1, pf);
		}


		if ((* pSurface) == NULL)
		{
			(*pSurface) = new CPGESurface(pDevice);
		}
	
		if (btCount == 8)
		{
			(*pSurface)->SetPal(pPal, PicHead.wPalPiexlCount);
		}
		(*pSurface)->CreateSurface(dwpgpY, dwpgpX, PGE_PGP_SURFACE, wMaskColor);

		BYTE* pAlphaBuf = NULL;
		if (bAlpha)	// 带Alpha通道
		{
			if (bCompress)
			{
				BYTE* pAlphaRdBuf = new BYTE[PicHead.dwAlphaSize];
				pAlphaBuf = new BYTE[PicHead.dwAlphaDeComSize];
				fseek(pf, SEEK_SET, PicHead.dwAlphaOffset);
				fread(pAlphaRdBuf, PicHead.dwAlphaSize, 1, pf);
				int a = decompress(pAlphaRdBuf, PicHead.dwAlphaSize, pAlphaBuf);
				SAFE_DELETE_ARY(pAlphaRdBuf);

			}
			else
			{
				pAlphaBuf = new BYTE[PicHead.dwAlphaSize];
				fseek(pf, SEEK_SET, PicHead.dwAlphaOffset);
				fread(pAlphaBuf, PicHead.dwAlphaSize, 1, pf);
			}
		}
		fclose(pf);

		(*pSurface)->SetPgpSurface((WORD*)pBmpData, pAlphaBuf);

	}
	else if (caTmp == 'b')
	{
		FILE* pf = PGEFileOpen(caFileName, "rb");
		if (pf == NULL) 
		{
			return;
		}
		BITMAPFILEHEADER bm;
		fread(&bm, 1, sizeof(BITMAPINFOHEADER), pf);
		if (bm.bfType!=*(WORD*)"BM")
		{		//AfxMessageBox("非位图文件");
			fclose(pf);
			return;
		}
		BITMAPINFOHEADER bmif;
		fread(&bmif, 1, sizeof(BITMAPFILEHEADER), pf);
		int bmx = bmif.biWidth;
		int bmy = bmif.biHeight>0?bmif.biHeight:(-bmif.biHeight);


		if (bmif.biBitCount < 8 || bmif.biCompression != 0) 
		{
			fclose(pf);
			return;
		}

		RGBQUAD *pRgb = NULL;
		if (bmif.biBitCount == 8) // 256色bmp
		{
			pRgb = (RGBQUAD*) new BYTE[bmif.biClrUsed * sizeof(RGBQUAD)];
			fread(pRgb, bmif.biClrUsed * sizeof(RGBQUAD), 1, pf);
		}

		BOOL bAlpha = FALSE;
		if (bmif.biBitCount == 32)	bAlpha = TRUE;// 带Alpha通道

		DWORD dwRealWidth = (((bmx*bmif.biBitCount)+31)>>5)<<2;
		int iBitSteps = bmif.biBitCount>>3;
		DWORD iSize = dwRealWidth*bmy*iBitSteps;

		BYTE* pBmpData = new BYTE[iSize];

		fseek(pf, bm.bfOffBits, SEEK_SET);
		fread(pBmpData, 1, iSize, pf);
		fclose(pf);

		if ((* pSurface) == NULL)
		{
			//ASSERT(pDevice);
			(*pSurface) = new CPGESurface(pDevice);
		}

		(*pSurface)->CreateSurface(bmy, bmx, PGE_NOMARL_SURFACE, wMaskColor);
		if (bAlpha)	// 带Alpha通道
		{
			(*pSurface)->EnableAlphaChanle();
		}

		WORD* pBuff = NULL;
		BYTE* pbtBuff = (*pSurface)->GetAlphaChanle();
		BYTE* pbtBuff1 = (*pSurface)->GetStaticAlphaChanle(0x7819);

		(*pSurface)->Lock((void**)&pBuff);

		int iy,ix;
		BYTE btPil;
		WORD caTmp[3];
		DWORD dwDataOff = 0;
		for(iy=0; iy<bmy; iy++)
		{
			for (ix=0; ix<bmx; ix++)
			{		
				if (pRgb == NULL)
				{			
					caTmp[0] = pBmpData[(iy*(dwRealWidth)+(ix*iBitSteps)) + 2];
					caTmp[1] = pBmpData[(iy*(dwRealWidth)+(ix*iBitSteps)) + 1];
					caTmp[2] = pBmpData[(iy*(dwRealWidth)+(ix*iBitSteps))];
				}
				else
				{
					btPil = pBmpData[(iy*(dwRealWidth)+(ix*iBitSteps))];
					if (bmif.biClrUsed != 0)
					{
						caTmp[0] = pRgb[btPil].rgbRed;
						caTmp[1] = pRgb[btPil].rgbGreen;
						caTmp[2] = pRgb[btPil].rgbBlue;
					}
					else
					{
						caTmp[0] = caTmp[1] = caTmp[2] = btPil;
					}

				}

				if (bmif.biHeight<0)		// 有时bmp图像的高会是负值，这时则不用颠倒 
				{
					dwDataOff = iy*bmx + ix;
				}
				else
				{
					dwDataOff = (bmy-iy-1)*bmx + ix;
				}

				if (bAlpha)
				{
					DWORD dbf=pBmpData[(iy*(dwRealWidth)+(ix*iBitSteps)) + 3];
					pbtBuff1[dwDataOff] = pbtBuff[dwDataOff] = ((float)dbf/255.f)*31;
				}

				pBuff[dwDataOff] = RGBto16bit565(caTmp[0], caTmp[1], caTmp[2]);

			}
		}
		SAFE_DELETE_ARY(pBmpData);
		SAFE_DELETE_ARY(pRgb);
		(*pSurface)->unLock();

	}
	
}

__inline void MakeAlpha(WORD* wpSrc, WORD* wpDes, BYTE btAlpha)
{
	register DWORD d1;
	register DWORD wa = *wpSrc;
	register DWORD wb = *wpDes;
	register DWORD alpha = btAlpha;

	d1 = (((((((wa << 16) | wa) & 0x7e0f81f) - (((wb << 16) | wb) & 0x7e0f81f)) * alpha) >> 5) + (((wb << 16) | wb) & 0x7e0f81f)) & 0x7e0f81f;
	wa = (d1 & 0xffff0000)>>16;
	wb = d1 & 0xffff;
	*wpDes = wa | wb;
//	register BYTE bSrcR, bSrcG,bSrcB, bDisR, bDisG, bDisB;
//	bSrcR = (*wpSrc & 0xf800) >> 8;
//	bSrcG = (*wpSrc & 0x7e0) >> 3;
//	bSrcB = (*wpSrc & 0x1f) << 3; 
//	
//	bDisR = (*wpDes & 0xf800) >> 8;
//	bDisG = (*wpDes & 0x7e0) >> 3;
//	bDisB = (*wpDes & 0x1f) << 3; 
//
//#define apl(c1,c2) (c1*wAlpha+(255-wAlpha)*c2)>>8
//	*wpDes = RGBto16bit565(apl(bSrcR, bDisR), apl(bSrcG, bDisG), apl(bSrcB, bDisB));
}

__inline void MakeADD(WORD* wpSrc, WORD* wpDes)
{
	register DWORD tdest = 0;
	register DWORD t;

	t = (*wpSrc & 0xf800) + (*wpDes & 0xf800);
	if(t > 0xf800) t = 0xf800;
		tdest |= t;

	t = (*wpSrc & 0x7e0) + (*wpDes & 0x7e0);
	if(t > 0x7e0) t = 0x7e0;
		tdest |= t;

	t = (*wpSrc & 0x1f) + (*wpDes & 0x1f);
	if(t > 0x1f) t = 0x1f;
		tdest |= t;


	*wpDes = (WORD)tdest;
}


PGE_RESULT CreateAlphaFromFile(CPGESurface* pSurface, 
							   char* caFileName)
{
	if (pSurface->GetSurfaceInfo()->wSurfaceTp == PGE_PGP_SURFACE) return PGE_FAIL;
	FILE* pf = PGEFileOpen(caFileName, "rb");
	if (pf == NULL) 
	{
		return PGE_FAIL;
	}
	BITMAPFILEHEADER bm;
	fread(&bm, 1, sizeof(BITMAPFILEHEADER), pf);
	if (bm.bfType!=*(WORD*)"BM")
	{		//AfxMessageBox("非位图文件");
		fclose(pf);
		return 0;
	}
	BITMAPINFOHEADER bmif;
	fread(&bmif, 1, sizeof(BITMAPINFOHEADER), pf);
	int bmx = bmif.biWidth;
	int bmy = bmif.biHeight;

	if (bmif.biBitCount != 8) 
	{
		fclose(pf);
		return PGE_FAIL;
	}

	RGBQUAD *pRgb = NULL;
	if (bmif.biBitCount == 8) // 256色bmp
	{
		pRgb = (RGBQUAD*) new BYTE[bmif.biClrUsed * sizeof(RGBQUAD)];
		fread(pRgb, bmif.biClrUsed * sizeof(RGBQUAD), 1, pf);
	}


	DWORD dwRealWidth = (((bmx*bmif.biBitCount)+31)>>5)<<2;
	int iBitSteps = bmif.biBitCount>>3;
	int iSize = dwRealWidth*bmy*iBitSteps;

	BYTE* pBmpData = new BYTE[iSize];

	fseek(pf, bm.bfOffBits, SEEK_SET);
	fread(pBmpData, 1, iSize, pf);
	fclose(pf);

	pSurface->EnableAlphaChanle();
	BYTE* pbtAlpha = pSurface->GetAlphaChanle();
	BYTE* pbtBuff1 = pSurface->GetStaticAlphaChanle(0x7819);

	int iy,ix;
	BYTE btPil;
	WORD caTmp[3];
	for(iy=0; iy<bmy; iy++)
	{
		for (ix=0; ix<bmx; ix++)
		{
			btPil = pBmpData[(iy*(dwRealWidth)+(ix*iBitSteps))];
			//if (bmif.biClrUsed != 0)
			//{
			//	caTmp[0] = pRgb[btPil].rgbRed;
			//	caTmp[1] = pRgb[btPil].rgbGreen;
			//	caTmp[2] = pRgb[btPil].rgbBlue;
			//}
			//else
			{
				caTmp[0] = caTmp[1] = caTmp[2] = btPil;
			}

			BYTE ba = RGBto16bit565(caTmp[0], caTmp[1], caTmp[2]);
			if (ba != 0)
			{
				int sa = 0;
			}
			pbtBuff1[(bmy-iy-1)*bmx + ix] = pbtAlpha[(bmy-iy-1)*bmx + ix] = ((float)(255-ba)/255.f)*32;
		}

	}

	SAFE_DELETE_ARY(pBmpData);
	SAFE_DELETE_ARY(pRgb);
	return 0;

}

