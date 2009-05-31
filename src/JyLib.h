#include "pgeutil.h"
#include "PGEAnime.h"
#include "LuaPge.h"


class CJyApp : public CPGEApplication
{
public:
	CJyApp(){};
	~CJyApp(){};

	virtual PGE_RESULT OnInitDeviceObjects();	// ��ʼ��
	virtual PGE_RESULT OnDeleteDeviceObjects(); // ����ʱ����
	virtual PGE_RESULT OnFinalCleanup();		// �˳�ʱ����
	virtual void OnFrameMove();				// ��ʼһ֡,��OnDraw֮ǰ
	virtual void OnDraw();					// ��һ֡
	virtual PGE_RESULT OnWindowProc(HWND hWnd, UINT message
		, WPARAM wParam, LPARAM lParam);	// ������Ϣ������

	CPGESurface*		m_pMainSuf;			// ������

};
