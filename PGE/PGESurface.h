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

// PGE��׼��ɫ
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

// PGE��������
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

	// ����һ������
	PGE_RESULT CreateSurface(WORD wGrHeight, WORD wGrWide, 
		BYTE btSurfaceTp = PGE_NOMARL_SURFACE,  
		DWORD dwMaskColor = PGE_NONECOLOR);			

	// ��������
	void Lock(void** pBuff);					

	// �������
	void unLock();							

	// �ڱ�������ͼ
	void Blt(CPGESurface* pSrcSurface, 
		RECT* prSrc, RECT* prDis, WORD wFlag = PGE_BLT_NORMAL);						

	// �ѱ�������ݷ�ת����Ļ����������
	void Flip();	// ����		
	void FlipH();	// ����

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

	void SetScale(float fScale);	// ����
	float GetScale();
	DWORD GetScaleOffset();

	void SetRot(int iRot);	// ��ת����λ�� �Ƕ�
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
	CPGEDevice* m_pDevice;	// PGE���豸

	PGESurfaceInfo m_SurfaceInfo;		// ��������
	BYTE* m_pbtAlphaChanle;	// Alphaͨ��
	BYTE* m_pbtAlphaChanleStatic;	// ��̬Alphaͨ��
	WORD* m_pwMainData;		// ����������
	DWORD m_dwDataSize;	// �����С
	
	CPGEFont* m_pFont;	// ����
	BYTE m_btFontH;		// �����
	BYTE m_btFontW;		// �����

	BOOL m_bAlphaChanle;	// �Ƿ����Alphaͨ��
	BOOL m_bLocked;			// ������־
	DWORD m_dwScale;		// ���ű���
	int m_iRot;				// ��ת�Ƕ�
	DWORD m_dwMaskColor;	// ������ɫ
	BYTE  m_btAlpha;		// alphaֵ���������������棬��ʹ��ʱalphaͨ����������

	WORD m_wRplaceColor;		// ���������ڱ����ϵ���ɫ
	WORD* m_pwPal;
};


void CreateSurfaceFromFile(CPGESurface** pSurface, 
								 char* caFileName, CPGEDevice* 
								 pDevice, DWORD wMaskColor = PGE_NONECOLOR);	

PGE_RESULT CreateAlphaFromFile(CPGESurface* pSurface, 
								 char* caFileName);	



#endif
