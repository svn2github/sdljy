// PGEApplication.cpp: implementation of the CPGEApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "PGEApplication.h"
#include "SDL.h"
#define TITLE TEXT("PgeGame")

ObjectList world;
USE_LUA
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPGEApplication* g_pPgeApp;
CPGESound SoundPlayer;

int WndProc(
		HWND hWnd, 
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam)
{
	return g_pPgeApp->WindowProc(hWnd, message, wParam, lParam);
}

CPGEApplication::CPGEApplication()
{
	g_pPgeApp = this;
	m_hWnd = NULL;
	m_hInstance = NULL;
	memset(&m_MouseData, 0, sizeof(m_MouseData));
	m_dwFrameCount = 0;
	m_dwClassID = PGE_APP;
	m_iScrDir = PGE_SCREEN_V;
	m_bGameStart = FALSE;
}

CPGEApplication::~CPGEApplication()
{
	g_pPgeApp = NULL;
	SAFE_DELETE(m_pPgeDevice);
}

DWORD CPGEApplication::GetFrmCount()
{
	return m_dwFrameCount;
}


CPGEDevice* CPGEApplication::GetDevice()
{
	return m_pPgeDevice;
}
		
PGE_RESULT CPGEApplication::Create(HINSTANCE hInst, PGE_SCREEN_DIRECTION ScrDir)
{
	HWND                        hWnd;
	
#ifndef _WIN32_WCE
	#ifdef __WIN32
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX); 

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInst;
		wcex.hIcon			= LoadIcon(hInst, (LPCTSTR)IDI_W32BSFY);
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);;
		wcex.lpszMenuName	= NULL;//(LPCTSTR)IDI_W32BSFY;
		wcex.lpszClassName	= TEXT("PgeGames");
		wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
		RegisterClassEx(&wcex);


		DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
		DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
		DWORD dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
		DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
		DWORD dwWindowWidth   = WINDOW_WIDTH  + dwFrameWidth * 2;
		DWORD dwWindowHeight  = WINDOW_HEIGHT + dwFrameHeight * 2 + 
			dwMenuHeight + dwCaptionHeight;

		hWnd = CreateWindow(TEXT("PgeGames"), TEXT("PGEGame"), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, dwWindowWidth, dwWindowHeight, NULL, NULL, hInst, NULL);
	#endif
#else defined(_WIN32_WCE)

	WNDCLASS                    wc;

	m_hInstance = hInst;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = _T("PgeGames");
	wc.lpszMenuName = NULL;
	RegisterClass(&wc);

	hWnd = CreateWindowEx(WS_EX_TOPMOST,
		_T("PgeGames"),
		TITLE,
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hInst,
		NULL);
#endif
	if (!hWnd)
	{
		return PGE_FAIL;
	}

	m_hWnd = hWnd;
//	ShowWindow(hWnd, SW_SHOW);
//	UpdateWindow(hWnd);
//	SetFocus(hWnd);
	m_iScrDir = ScrDir;

	return InitDeviceObjects();
}


PGE_RESULT CPGEApplication::Run()
{
	MSG msg;
	timer.Init();
	while( m_bGameStart )
        {

			while (SDL_PollEvent(NULL));
			FrameMove(); 
			        
		}
	return 0;
}


PGE_RESULT CPGEApplication::InitDeviceObjects()
{
	m_pPgeDevice = new CPGEDevice(m_hWnd, PGE_SCREEN_DIRECTION(m_iScrDir));

	if (m_pPgeDevice->OpenDisplay() == PGE_FAIL)
		return PGE_FAIL;

	PGEDisplayProperties* p_displayprop = m_pPgeDevice->GetDisplayProperties();
	if (p_displayprop->cBPP != 16)
	{
		m_pPgeDevice->CloseDisplay();
		return PGE_FAIL;
	}

	m_bGameStart = TRUE;
	L_stat = lua_open(); 
	luaL_openlibs(L_stat);
	
	return Title();
}

PGE_RESULT CPGEApplication::Title()
{
	CPGESurface* pMainSuf = NULL;
	m_pPgeDevice->SetMainSurface(&pMainSuf);
		return OnInitDeviceObjects();
#ifdef _WIN32_WCE

	CPGESurface* pTitleSuf = NULL;
	CreateSurfaceFromFile(&pTitleSuf, GetResFile("pic\\pge.pgp"), m_pPgeDevice);
	
	SoundPlayer.LoadSound(GetResFile("sound\\explosion2.wav"), 252);
	SoundPlayer.LoadSound(GetResFile("sound\\explosion3.wav"), 253);
	SoundPlayer.LoadSound(GetResFile("sound\\explosion4.wav"), 254);
	SoundPlayer.LoadSound(GetResFile("sound\\tileclick.wav") ,251);

	pMainSuf->Clean();
	WORD* pBuf;
	DWORD dx = 0;
	DWORD dwMx = pMainSuf->GetDataSize();
	int i = 0;
	for (i=0; i<255; i+=5)
	{
		pMainSuf->Lock((void**)&pBuf);
		WORD wColor = RGBto16bit565(i,i,i);
		for (dx=0; dx<dwMx; dx++)
			pBuf[dx] = wColor;
		pMainSuf->unLock();
		m_pPgeDevice->Flip();
	}
	
	PGESurfaceInfo* pMInfo = pMainSuf->GetSurfaceInfo();

	SoundPlayer.Play(252);
	RECT rDis, rSrc;
	rDis = FiRect((pMInfo->wWiedth-210)/2, (pMInfo->wHeigh-210)/2,(pMInfo->wWiedth-210)/2+70, (pMInfo->wHeigh-210)/2+110);
	rSrc = FiRect(0,0,70,110);
	pMainSuf->Blt(pTitleSuf, &rSrc, &rDis, PGE_BLT_NORMAL | PGE_BLT_MASK);
	m_pPgeDevice->Flip();
	
	DWORD dwTime = timeGetTime();
	while(timeGetTime()-dwTime < 550)
	{
		Sleep(50);
	}

	SoundPlayer.Play(253);
	rDis = FiRect((pMInfo->wWiedth-210)/2+70, (pMInfo->wHeigh-210)/2,
		(pMInfo->wWiedth-210)/2+70+70, (pMInfo->wHeigh-210)/2+110);
	rSrc = FiRect(70,0,140,110);
	pMainSuf->Blt(pTitleSuf, &rSrc, &rDis, PGE_BLT_NORMAL | PGE_BLT_MASK);
	m_pPgeDevice->Flip();


	dwTime = timeGetTime();
	while(timeGetTime()-dwTime < 550)
	{
		Sleep(50);
	}

	SoundPlayer.Play(254);
	rDis = FiRect((pMInfo->wWiedth-210)/2+140, (pMInfo->wHeigh-210)/2+5,
		(pMInfo->wWiedth-210)/2+70+140, (pMInfo->wHeigh-210)/2+110+5);
	rSrc = FiRect(140,0,140+70,110);
	pMainSuf->Blt(pTitleSuf, &rSrc, &rDis, PGE_BLT_NORMAL | PGE_BLT_MASK);
	m_pPgeDevice->Flip();


	dwTime = timeGetTime();
	while(timeGetTime()-dwTime < 800)
	{
		Sleep(50);
	}

	SoundPlayer.Play(251);

	rDis = FiRect((pMInfo->wWiedth-140)/2+30, (pMInfo->wHeigh-20)/2+20,
		(pMInfo->wWiedth-140)/2+150+30, (pMInfo->wHeigh-20)/2+20+20);
	rSrc = FiRect(0,110,150,130);

	for (i=0; i<32; i++)
	{
		pTitleSuf->SetAlphaVal(i);
		pMainSuf->Blt(pTitleSuf, &rSrc, &rDis, PGE_BLT_ALPHA);
		m_pPgeDevice->Flip();
	}
	dwTime = timeGetTime();

	while(timeGetTime()-dwTime < 1600)
	{
		Sleep(50);
	}

	OnTitle();

	SAFE_DELETE(pTitleSuf);
	SoundPlayer.StopAll();
	SoundPlayer.ClearAll();
#endif
	return OnInitDeviceObjects();
}

PGE_RESULT 	CPGEApplication::DeleteDeviceObjects()
{
	CLOSE_LUA;
	OnDeleteDeviceObjects();
	m_pPgeDevice->CloseDisplay();
	SAFE_DELETE(m_pPgeDevice);
	return PGE_OK;
}


PGE_RESULT CPGEApplication::FinalCleanup()
{
	return OnFinalCleanup();
}


void CPGEApplication::FrameMove()
{
	OnFrameMove();
	OnDraw();
	m_dwFrameCount++;
	timer.Tick();
}

PGE_RESULT CPGEApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	m_MouseData.lBtn = 0;
//    switch (message)
//    {
//	case WM_LBUTTONDOWN:
//		{
//		
//			m_MouseData.lBtn = TRUE;		
//
//#ifdef _WIN32_WCE
//			if(m_pPgeDevice->GetScreenDir() == PGE_SCREEN_V)
//			{
//				m_MouseData.ix = LOWORD(lParam);
//				m_MouseData.iy = HIWORD(lParam);
//			}
//			else
//			{
//				m_MouseData.ix = 320 - HIWORD(lParam);
//				m_MouseData.iy = LOWORD(lParam);
//			}
//#else
//#ifndef AUTOPDASCREEN
//			if(m_pPgeDevice->GetScreenDir() == PGE_SCREEN_V)
//			{
//				m_MouseData.ix = LOWORD(lParam);
//				m_MouseData.iy = HIWORD(lParam);
//			}
//			else
//			{
//				m_MouseData.ix = 320 - HIWORD(lParam);
//				m_MouseData.iy = LOWORD(lParam);
//			}
//#else
//			m_MouseData.ix = LOWORD(lParam);
//			m_MouseData.iy = HIWORD(lParam);
//#endif
//#endif
//		}
//		break;
//
//    case WM_DESTROY:
//		m_bGameStart = FALSE;
//		DeleteDeviceObjects();
//		FinalCleanup();
//        return 0L;
//		
//	case WM_KILLFOCUS:
//		m_pPgeDevice->Suspend();
//		break;
//
//	case WM_SETFOCUS:
//		m_pPgeDevice->Resume();
//		break;
//    }
//	OnWindowProc(hWnd, message, wParam, lParam);
//    return DefWindowProc(hWnd, message, wParam, lParam);
}

MouseDt* CPGEApplication::GetMouseData()
{
	return &m_MouseData;
}

CPGESurface* CPGEApplication::GetMainSurface()
{
	return m_pPgeDevice->GetMainSurface();
}
