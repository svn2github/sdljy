// PGEDevice.cpp: implementation of the CPGEDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "PGEDevice.h"
#include "PGESurface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPGEDevice::CPGEDevice(HWND hWnd, PGE_SCREEN_DIRECTION ScrDirection)
{
	m_dwClassID = PGE_DEVICE;
	m_hWnd = hWnd;
	m_bEnable = FALSE;
	m_pMainSurface = NULL;
	m_iScreenDir = ScrDirection;
}

CPGEDevice::~CPGEDevice()
{
	if (m_pMainSurface != NULL)
	{
		SAFE_DELETE(m_pMainSurface);
	}
}

PGE_RESULT CPGEDevice::OpenDisplay()
{
	if (!m_bEnable)
	{
//		if(0 != GXOpenDisplay(m_hWnd, GX_FULLSCREEN))
//		{
//			m_DisplayProperties = GXGetDisplayProperties();
//			m_dwSize = m_DisplayProperties.cxWidth * m_DisplayProperties.cyHeight;
//			if (m_iScreenDir == PGE_SCREEN_H)
//			{
//				int iBuf = m_DisplayProperties.cxWidth;
//				m_DisplayProperties.cxWidth = m_DisplayProperties.cyHeight;
//				m_DisplayProperties.cyHeight = iBuf;
//			}
//			m_bEnable = TRUE;
//			return PGE_OK;
//		}
	}

	return PGE_FAIL;
	
}

PGE_RESULT CPGEDevice::CloseDisplay()
{
	//GXRETURN(GXCloseDisplay());
}

PGE_RESULT CPGEDevice::Suspend()
{
	//GXRETURN(GXSuspend());
}

PGE_RESULT CPGEDevice::Resume()
{
	//GXRETURN(GXResume());
}

PGEDisplayProperties* CPGEDevice::GetDisplayProperties()
{
	return &m_DisplayProperties;
}

WORD* CPGEDevice::BeginDraw()
{
	//return (WORD*) GXBeginDraw();
}

void CPGEDevice::EndDraw()
{
	//GXEndDraw();
}

HWND CPGEDevice::GetHwnd()
{
	return m_hWnd;
}

DWORD CPGEDevice::GetDeviceSize()
{
	return m_dwSize;
}

int CPGEDevice::GetScreenDir()
{
	return m_iScreenDir;
}

PGE_RESULT CPGEDevice::SetMainSurface(CPGESurface** pSurface)
{
	if ((*pSurface) == NULL)	// 如果主表面为空，创建它
	{
		(*pSurface) = new CPGESurface(this);
		(*pSurface)->CreateSurface(m_DisplayProperties.cyHeight, 
			m_DisplayProperties.cxWidth, PGE_MAIN_SURFACE);
		
			m_pMainSurface = (*pSurface);
			return PGE_OK;
	}
	if(m_pMainSurface != (*pSurface))
	{
		PGESurfaceInfo* pInfo = (*pSurface)->GetSurfaceInfo();
		if ((pInfo->wHeigh 
			== m_DisplayProperties.cyHeight) &&
			(pInfo->wWiedth
			== m_DisplayProperties.cxWidth))
		{
			pInfo->wSurfaceTp = PGE_MAIN_SURFACE;
			(*pSurface)->SetSurfaceInfo((*pInfo), this);
			
			m_pMainSurface = (*pSurface);
			return PGE_OK;
		}
		else
		{
			return PGE_FAIL;
		}

	}
	else
	{
		return PGE_OK;
	}


}

CPGESurface* CPGEDevice::GetMainSurface()
{
	return m_pMainSurface;
}

void CPGEDevice::Flip()
{
	if (m_iScreenDir == PGE_SCREEN_V)
	{		
		m_pMainSurface->Flip();
	}
	else
	{
		m_pMainSurface->FlipH();
	}
}
