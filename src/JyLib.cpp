#include "Jylib.h"
#include "GameLib.h"
CJyApp* theApp = 0;
CPGESurface* theMainSurf = 0;
int GameLock = 0;

lua_State* lp = 0;
DWORD WINAPI JyGameThread(LPVOID lpparm)	
{
	lua_State* L = lua_open(); 
	lp = L;
	luaL_openlibs(L);
	RegJyLib(L);
	
	JyInit(L);
	
	return 0;
}

CPGESurface* tmp = 0;
PGE_RESULT CJyApp::OnInitDeviceObjects()
{
	theApp = this;
	m_pMainSuf = GetMainSurface();
	theMainSurf = m_pMainSuf;
	// 在这里添加初始化代码
 
	char buf[200];
	CreateSurfaceFromFile(&tmp, GetResFile("tts.bmp",buf));

	timer.SetFrmLim(30);
	DWORD tid;
	CreateThread(NULL, 0, JyGameThread, NULL, 0, &tid);	

	//RegJyLib(LuaWraper.m_l);
	//JyInit(LuaWraper.m_l);

	
	
	//
	return PGE_OK;
}

PGE_RESULT CJyApp::OnDeleteDeviceObjects()
{
	// 在这里进行退出清理

	return PGE_OK;
}

PGE_RESULT CJyApp::OnFinalCleanup()
{
	return PGE_OK;
}

void CJyApp::OnFrameMove()
{
	// 每帧渲染之前
	//LuaWraper["Game_Cycle"]();
}


int mmm=PGE_BLUE;
void CJyApp::OnDraw()
{
	// 渲染
	//m_pMainSuf->Clean();
	//m_pMainSuf->Blt(tmp, 0, 0);

	//DrawBoard(PGE_GREEN, 100, 100, 50, 100, 0);

	//DrawStrOnBoard("test1", PGE_LIGHTBLUE, PGE_BLUE, 200, 200, 80, 200);
	//
	//m_pMainSuf->DrawText(va("mmm:%d", mmm),0,0,PGE_GREEN);
	//GetDevice()->Flip();
	//Sleep(10);
}

extern JYKey mpkey[];
extern int anykeypress;
extern int ghh;
PGE_RESULT CJyApp::OnWindowProc(HWND hWnd, UINT message
	, WPARAM wParam, LPARAM lParam)
{
	// 消息处理
	if (message == WM_KEYDOWN)
	{
		if (wParam == 'P')
		{
			static int kkk = 0;
			if (++kkk == 4) kkk = 0;
			GetDevice()->SetScreenDir(kkk);

		}
		
		mpkey[0].bD = 1;
		for (int i=1; i<13; ++i)
		{
			if (wParam == (mpkey[i].key))
			{
				mpkey[i].bD = 1;
			}
		}
		mpkey[0].bD = 0;
	}
	else if (message == WM_KEYUP)
	{
		if (!anykeypress) anykeypress = wParam;
	}

	return PGE_OK;
}