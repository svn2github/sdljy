#ifndef PGEvKeyBoard_H
#define PGEvKeyBoard_H


#include "PGEstd.h"
#include "pgeime.h"
#include "PGEsurface.h"

#define PGE_SINGLE_KEYH 16  
#define PGE_SINGLE_KEYW 23

#define PGE_VKX_CT 10
#define PGE_VKY_CT 5

struct MouseDt;

struct VkImePyTp
{
	char caPy[8];
	int iKeyCt;
	int iRseCt;
	char* caRse;
	char caView[3];
	BOOL bCanInput;
};

enum PGE_VK_EVENTS
{
	PGE_VKEV_NONE = 0,
	PGE_VKEV_KEYDOWN = 0xf1,
		PGE_VKEV_KEYUP,
		PGE_VKEV_KEYPRESS
};
#define PGE_VKEV_MAX 255
#define PGE_VKEV_FUNTION(FunName) BOOL FunName (char PgeVk)

#define PGE_FUN_CALL DWORD

typedef struct tgaVkEvEnts
{
	PGE_FUN_CALL FunCall;
	PGE_VK_EVENTS Events;
}VkEvents;

class CPGEvKeyBoard :
	public CPGEBase 
{
public:
	CPGEvKeyBoard();
	~CPGEvKeyBoard();

	void DrawKBD(int x, int y, BOOL bTrasn);
	PGE_RESULT Init(char*caPicFile,  CPGEDevice* pDevice);
	PGE_RESULT SetIme( char* caImeFile);

	PGE_RESULT RegisterEvent(PGE_FUN_CALL FunCall, PGE_VK_EVENTS vkEvents);
	PGE_RESULT unRegisterEvent(PGE_FUN_CALL FunCall);
private:
	CPGEDevice* m_pDevice;
	CPGESurface* m_pVkSurface;
	CPGEIme* m_pIme;

	char m_caOutStr[100];
	BOOL m_bCn;

	MouseDt* m_pMouseDt;

	BYTE m_btTableW[320][2];
	BYTE m_btTableH[240][2];
	BYTE m_btChar[5][10];

	VkEvents m_vkEvents[PGE_VKEV_MAX];
	
	VkImePyTp m_ImePy;


};
#endif
