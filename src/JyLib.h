#include "pgeutil.h"
#include "PGEAnime.h"
#include "LuaPge.h"


class CJyApp : public CPGEApplication
{
public:
	CJyApp(){};
	~CJyApp(){};

	virtual PGE_RESULT OnInitDeviceObjects();	// 初始化
	virtual PGE_RESULT OnDeleteDeviceObjects(); // 结束时清理
	virtual PGE_RESULT OnFinalCleanup();		// 退出时清理
	virtual void OnFrameMove();				// 开始一帧,在OnDraw之前
	virtual void OnDraw();					// 画一帧
	virtual PGE_RESULT OnWindowProc(HWND hWnd, UINT message
		, WPARAM wParam, LPARAM lParam);	// 窗口消息处理函数

	CPGESurface*		m_pMainSuf;			// 主表面

};
