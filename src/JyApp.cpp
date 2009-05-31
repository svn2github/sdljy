#include "Jylib.h"


int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{ 

	CJyApp theapp;
#ifdef _WIN32_WCE
	if (PGEFAILED(theapp.Create(hInstance, PGE_SCREEN_90))) return 0;
#else
	if (PGEFAILED(theapp.Create(hInstance, PGE_SCREEN_0))) return 0;
#endif
	
	theapp.Run();
	return 0;
}