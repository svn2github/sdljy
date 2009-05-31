// PGESurface.h: interface for the CPGESurface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PGESurface_h
#define PGESurface_h

#include "pgestd.h"
#include "pgedevice.h"
#include "PGEFont.h"
#define PGE_MAX_SURFACE 255
#define PGE_RESNAME_MAX 30

#define RGBto16bit565(r, g, b) \
	(((WORD)r<<8)&0xf800)|(((WORD)g<<3)&0x07e0)|((WORD)b>>3) \

// PGE标准颜色
enum PGE_DEFCOLOR
{
	PGE_NONECOLOR = 0xfffff,
		PGE_BLACK = 0,
		PGE_WHITE = 0xffff,
		PGE_RED = 0xf800,
		PGE_GREEN = 0x7e0,
		PGE_BLUE = 0x1f,
		PGE_PURPLE = 0xf81f,
		PGE_YELLOW = 0xffe0,
		PGE_LIGHTBLUE = 0x7ff,
};

// PGE表面类型
enum PGE_SURFACE_TYPE
{
	PGE_NOMARL_SURFACE,
		PGE_MAIN_SURFACE,
		PGE_BACK_SURFACE,
		PGE_PGP_SURFACE
};

enum PGE_BLT_TYPE
{
	PGE_BLT_NORMAL = 0x1,
		PGE_BLT_ADD = 0x2,
		PGE_BLT_OR = 0x3,
		PGE_BLT_AND = 0x4,
		PGE_BLT_ALPHA = 0x5,

		PGE_BLT_MASK = 0x10,
		PGE_BLT_COLOR = 0x20,
		PGE_BLT_ROTSCL = 0x100,
		PGE_BLT_ANDALPHA = 0x1000,
		PGE_CLT_COPYALPHA = 0x2000,
	
};

enum PGE_FONT_DRAWTYPE
{
	PGE_FONT_USEMAIN = 0xff,
		PGE_FONT_USEFILE,
		PGE_FONT_USESURFACE,
	PGE_FONT_MUTIL = 0,
		PGE_FONT_SINGLELINE
};

typedef struct tgaSurfaceInfo
{
	WORD wWiedth, wHeigh, wSurfaceTp;
} PGESurfaceInfo;

typedef struct tgaSurfaceContainer
{
	char caName[PGE_RESNAME_MAX];
	DWORD pSurface;
	int iCiteCount;
} PGESurfaceContainer;

class CPGESurface : public CPGEBase   
{
public:
	CPGESurface(CPGEDevice* pDevice);
	virtual ~CPGESurface();

	// 创建一个表面
	PGE_RESULT CreateSurface(WORD wGrHeight, WORD wGrWide, 
		BYTE btSurfaceTp = PGE_NOMARL_SURFACE,  
		DWORD dwMaskColor = PGE_NONECOLOR);			

	// 锁定表面
	void Lock(void** pBuff);					

	// 表面解锁
	void unLock();							

	// 在表面上贴图
	void Blt(CPGESurface* pSrcSurface, 
		RECT* prSrc, RECT* prDis, WORD wFlag = PGE_BLT_NORMAL);						

	// 把表面的内容翻转到屏幕或者主表面
	void Flip();	// 竖向		
	void FlipH();	// 横向

	void Clean();

	PGE_RESULT SetSurfaceInfo(PGESurfaceInfo SurfaceInfo, CPGEBase* pPGEBase = NULL);
	PGESurfaceInfo* GetSurfaceInfo();
	DWORD GetMaskColor();

	void DrawText(char* pText, int x, int y, WORD wColor);
	PGE_RESULT CreateFont(void* pData = NULL, WORD iUseType = PGE_FONT_USEMAIN);
	CPGEFont* GetFont(CPGEBase* pPGEBase = NULL);

	BYTE* GetAlphaChanle();
	BYTE* GetStaticAlphaChanle(WORD wKey);
	void SetAlphaChanle(RECT* pRect, BYTE* pbtAlphaData);
	void RestoreAlphaChanle(RECT* pRect);
	PGE_RESULT EnableAlphaChanle();

	PGE_RESULT CreateAlphaWithMaskColor();

	void SetScale(float fScale);	// 放缩
	float GetScale();
	DWORD GetScaleOffset();

	void SetRot(int iRot);	// 旋转，单位： 角度
	int GetRot();

	void SetAlphaVal(BYTE btAlpha);
	BYTE GetAlphaVal();

	DWORD GetDataSize();
	
	void SetReplaceColor(WORD wColor);
	WORD GetReplaceColor();

	void SetPal(WORD* pwPal, int iPalPiexlCount);
	WORD* GetPal();

	void SetPgpSurface(WORD* pwPicData, BYTE* pbtAlpha);

	void SetPiexl(int x, int y, WORD wColor);
	void DrawLine(int x1, int y1, int x2, int y2, WORD wColor);
	void DrawRect(int x1, int y1, int x2, int y2, WORD wColor);

	BYTE m_btSurfaceBit;
	BYTE m_btUserData;

private:
	void Blt8(CPGESurface* pSrcSurface, RECT* prSrc, RECT* prDis, WORD wFlag);
	CPGEDevice* m_pDevice;	// PGE主设备

	PGESurfaceInfo m_SurfaceInfo;		// 表面类型
	BYTE* m_pbtAlphaChanle;	// Alpha通道
	BYTE* m_pbtAlphaChanleStatic;	// 静态Alpha通道
	WORD* m_pwMainData;		// 表面主缓冲
	DWORD m_dwDataSize;	// 表面大小
	
	CPGEFont* m_pFont;	// 字体
	BYTE m_btFontH;		// 字体高
	BYTE m_btFontW;		// 字体宽

	BOOL m_bAlphaChanle;	// 是否具有Alpha通道
	BOOL m_bLocked;			// 锁定标志
	DWORD m_dwScale;		// 缩放倍数
	int m_iRot;				// 旋转角度
	DWORD m_dwMaskColor;	// 遮罩颜色
	BYTE  m_btAlpha;		// alpha值，作用于整个表面，当使用时alpha通道不起作用

	WORD m_wRplaceColor;		// 用来叠加在表面上的颜色
	WORD* m_pwPal;
};


void CreateSurfaceFromFile(CPGESurface** pSurface, 
								 char* caFileName, CPGEDevice* 
								 pDevice, DWORD wMaskColor = PGE_NONECOLOR);	

PGE_RESULT CreateAlphaFromFile(CPGESurface* pSurface, 
								 char* caFileName);	



#endif
