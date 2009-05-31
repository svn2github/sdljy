// PGEApplication.h: interface for the CPGEApplication class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PGEApplication_h__
#define __PGEApplication_h__

// INCLUDES

#include "pgeutil.h"
#include "GameObject/PGEGameObj.h"

#define USE_SOUND extern CPGESound SoundPlayer;

struct MouseDt
{
	int ix,iy,iz;
	int lBtn, rBtn;
};

class CPGEApplication  : public CPGEBase
{
public:
	CPGEApplication();
	virtual ~CPGEApplication();

	PGE_RESULT Create(
			HINSTANCE hInst, 
			PGE_SCREEN_DIRECTION ScrDir = PGE_SCREEN_V);
	PGE_RESULT Run();

	virtual PGE_RESULT OnInitDeviceObjects() {return PGE_FAIL;}
	virtual PGE_RESULT OnDeleteDeviceObjects() {return PGE_FAIL;}
	virtual PGE_RESULT OnFinalCleanup() {return PGE_FAIL;}
	virtual void OnFrameMove(void) {}
	virtual void OnDraw(void) {}
	virtual PGE_RESULT OnWindowProc(
			HWND hWnd, 
			UINT message, 
			WPARAM wParam, 
			LPARAM lParam) {return PGE_FAIL;}
	virtual PGE_RESULT OnTitle(){return PGE_FAIL;}
	
	CPGESurface* GetMainSurface();
	MouseDt* GetMouseData();
	DWORD GetFrmCount();
	CPGEDevice* GetDevice();

	HWND m_hWnd;
	HINSTANCE m_hInstance;

	CPGETimer timer;
	MouseDt m_MouseData;
private:

	CPGEDevice* m_pPgeDevice;

	DWORD m_dwFrameCount;
	volatile BOOL m_bGameStart;
	int m_iScrDir;

	PGE_RESULT InitDeviceObjects();
	PGE_RESULT DeleteDeviceObjects();
	PGE_RESULT FinalCleanup();
	void FrameMove(void);
	PGE_RESULT Title();

public:
	PGE_RESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};
#endif
