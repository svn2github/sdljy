// PGEDevice.h: interface for the CPGEDevice class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _PGEDevice_H
#define _PGEDevice_H

#include "pgestd.h"

class CPGESurface;

class CPGEDevice : public CPGEBase 
{
public:
	// ����PGE�豸
	// hWnd�����ھ����ScrDirection���Ʒ���PGE_SCREEN_V ���� PGE_SCREEN_H����
	CPGEDevice(HWND hWnd, PGE_SCREEN_DIRECTION ScrDirection = PGE_SCREEN_V);
	~CPGEDevice();

	PGE_RESULT OpenDisplay();
	PGE_RESULT CloseDisplay();
	PGE_RESULT Suspend();
	PGE_RESULT Resume();
	PGEDisplayProperties* GetDisplayProperties();
	WORD* BeginDraw();
	void EndDraw();
	HWND GetHwnd();
	DWORD GetDeviceSize();

	PGE_RESULT SetMainSurface(CPGESurface** pSurface);
	CPGESurface* GetMainSurface();
	
	int GetScreenDir();
	void Flip();

private:
	HWND m_hWnd;
	BOOL m_bEnable;
	PGEDisplayProperties m_DisplayProperties;
	DWORD m_dwSize;
	CPGESurface* m_pMainSurface;
	int m_iScreenDir;

};

#endif
