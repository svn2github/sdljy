#include "pgevkeyboard.h"
#include "PGEApplication.h"
extern CPGEApplication* g_pPgeApp;


CPGEvKeyBoard::CPGEvKeyBoard()
{
	m_pDevice = NULL;
	m_pVkSurface = NULL;
	m_bCn = FALSE;
	m_pIme = NULL;
	memset(m_caOutStr, 0, sizeof(m_caOutStr));

	for(int i=0; i<320; i++)
	{
		m_btTableW[i][0] = (i / PGE_SINGLE_KEYW) * PGE_SINGLE_KEYW;
		m_btTableW[i][1] = (i / PGE_SINGLE_KEYW);
		if (i<240)
		{
			m_btTableH[i][0] = (i / PGE_SINGLE_KEYH) * PGE_SINGLE_KEYH;
			m_btTableH[i][1] = (i / PGE_SINGLE_KEYH);
		}
	}

	m_btChar[0][0] = 150;
	m_btChar[0][1] = 151;
	m_btChar[0][2] = 152;
	m_btChar[0][3] = 153;
	m_btChar[0][4] = 154;
	m_btChar[0][5] = 155;
	m_btChar[0][6] = 156;
	m_btChar[0][7] = 157;
	m_btChar[0][8] = 158;
	m_btChar[0][9] = 159;

	m_btChar[1][0] = 'q';
	m_btChar[1][1] = 'w';
	m_btChar[1][2] = 'e';
	m_btChar[1][3] = 'r';
	m_btChar[1][4] = 't';
	m_btChar[1][5] = 'y';
	m_btChar[1][6] = 'u';
	m_btChar[1][7] = 'i';
	m_btChar[1][8] = 'o';
	m_btChar[1][9] = 'p';

	m_btChar[2][0] = 'a';
	m_btChar[2][1] = 's';
	m_btChar[2][2] = 'd';
	m_btChar[2][3] = 'f';
	m_btChar[2][4] = 'g';
	m_btChar[2][5] = 'h';
	m_btChar[2][6] = 'j';
	m_btChar[2][7] = 'k';
	m_btChar[2][8] = 'l';
	m_btChar[2][9] = 13;

	m_btChar[3][0] = 'z';
	m_btChar[3][1] = 'x';
	m_btChar[3][2] = 'c';
	m_btChar[3][3] = 'v';
	m_btChar[3][4] = 'b';
	m_btChar[3][5] = 'n';
	m_btChar[3][6] = 'm';
	m_btChar[3][7] = 160;
	m_btChar[3][8] = 162;
	m_btChar[3][9] = 161;

	m_btChar[4][0] = 166;
	m_btChar[4][1] = 167;
	m_btChar[4][2] = 168;
	m_btChar[4][3] = 169;
	m_btChar[4][4] = 170;
	m_btChar[4][5] =171;
	m_btChar[4][6] = 0;
	m_btChar[4][7] = 163;
	m_btChar[4][8] = 164;
	m_btChar[4][9] = 165;

	memset(&m_ImePy, 0, sizeof(m_ImePy));
	memset(m_vkEvents, 0, sizeof(m_vkEvents));
}

CPGEvKeyBoard::~CPGEvKeyBoard()
{
	SAFE_DELETE(m_pVkSurface);
	SAFE_DELETE(m_pIme);
}

void CPGEvKeyBoard::DrawKBD(int x, int y, BOOL bTrasn)
{
	m_pMouseDt = g_pPgeApp->GetMouseData();

	RECT rSrc = FiRect(0,0,230,80), rDes = FiRect(x, y, x+230, y+80);
	if(bTrasn) 
	{
		m_pDevice->GetMainSurface()->Blt(m_pVkSurface, 
			&rSrc, &rDes, PGE_BLT_ALPHA);
	}
	else
	{
		m_pDevice->GetMainSurface()->Blt(m_pVkSurface, &rSrc, &rDes);
	}

	if (m_bCn)
	{	
		rSrc = FiRect(0, 80, 23, 96);
		rDes = FiRect(x+rSrc.left, y+rSrc.top-80, x+rSrc.left+23, y+rSrc.top+16-80);
		m_pDevice->GetMainSurface()->Blt(m_pVkSurface, &rSrc, &rDes);
	}
	WORD wKeyVal = 0;
	rDes = FiRect(x, y, x+230, y+80);
	if ((m_pMouseDt->lBtn == 1) && (PointInRect(m_pMouseDt->ix, m_pMouseDt->iy, rDes)))
	{
		rSrc = FiRect(m_btTableW[m_pMouseDt->ix - x][0],
			m_btTableH[m_pMouseDt->iy - y][0]+80,
			m_btTableW[m_pMouseDt->ix - x][0]+23, m_btTableH[m_pMouseDt->iy - y][0]+96);
		rDes = FiRect(x+rSrc.left, y+rSrc.top-80, x+rSrc.left+23, y+rSrc.top+16-80);
		m_pDevice->GetMainSurface()->Blt(m_pVkSurface, &rSrc, &rDes);
		
		if (m_ImePy.bCanInput == 0)	// 如果是一次Press
		{
			m_ImePy.bCanInput = m_pMouseDt->lBtn;

			if (m_btTableH[m_pMouseDt->iy - y][1] == 0 && 
						m_btTableW[m_pMouseDt->ix - x][1] == 0)
			{
				m_bCn = m_bCn ? 0 : 1;
			}
			
			else if ((m_btTableH[m_pMouseDt->iy - y][1] == 3 && 
						m_btTableW[m_pMouseDt->ix - x][1] > 6) ||
						
						(m_btTableH[m_pMouseDt->iy - y][1] >= 4)||
						
						(m_btTableH[m_pMouseDt->iy - y][1] < 1 && 
						m_btTableW[m_pMouseDt->ix - x][1] <=9))		
			{
				if (m_btTableH[m_pMouseDt->iy - y][1] == 0 && 
						m_btTableW[m_pMouseDt->ix - x][1] > 0)		// 字符选择键
				{	

					if (m_btTableH[m_pMouseDt->iy - y][1] == 0 && 
						m_btTableW[m_pMouseDt->ix - x][1] ==9)		// 橡皮建
					{

						if (m_ImePy.iKeyCt > 0)
						{
							
							m_ImePy.caPy[m_ImePy.iKeyCt-1] = 0;
							m_ImePy.iKeyCt--;
							m_ImePy.caRse = m_pIme->GetWords(m_ImePy.caPy, m_ImePy.iRseCt);
						}
						else if (strlen(m_caOutStr) > 0)
						{
							*(m_caOutStr+strlen(m_caOutStr)-2) = 0;
						}
						return;
					}

					if (m_ImePy.caRse != NULL)	// 有待选汉字
					{
						
						if (wKeyVal > m_ImePy.iRseCt) return;
						wKeyVal = 
							m_btChar[m_btTableH[m_pMouseDt->iy - y][1]][m_btTableW[m_pMouseDt->ix - x][1]] - 151;
						if (strlen(m_caOutStr) < 98) memcpy(m_caOutStr+strlen(m_caOutStr), m_ImePy.caRse+(wKeyVal<<1), 2);
						memset(m_ImePy.caPy, 0, sizeof(m_ImePy.caPy));
						m_ImePy.iKeyCt = 0;
					}
				}

				else if (m_btTableH[m_pMouseDt->iy - y][1] == 3 && 
						m_btTableW[m_pMouseDt->ix - x][1] ==7)		// 上翻页
				{
					if (m_ImePy.iRseCt == 0) return;
					if (strlen(m_ImePy.caRse) < (m_ImePy.iRseCt<<1))
								m_ImePy.caRse -= 18;
				}
				
				else if (m_btTableH[m_pMouseDt->iy - y][1] == 3 && 
						m_btTableW[m_pMouseDt->ix - x][1] ==9)		// 下翻页
				{
					if (m_ImePy.iRseCt == 0) return;
					if (strlen(m_ImePy.caRse) > 18)
							m_ImePy.caRse += 18;
				}
			}
			
			else if(m_btTableH[m_pMouseDt->iy - y][1] == 2 && 
						m_btTableW[m_pMouseDt->ix - x][1] == 9)		// Enter键
			{
				memset(m_caOutStr, 0, sizeof(m_caOutStr));
			}
			else	// 字符键a-z 0-9
			{
				if (m_bCn)
				{			
					m_ImePy.caPy[m_ImePy.iKeyCt] = 
						m_btChar[m_btTableH[m_pMouseDt->iy - y][1]][m_btTableW[m_pMouseDt->ix - x][1]];
					m_ImePy.iKeyCt++;
					if (m_ImePy.iKeyCt == 6) 
					{
						memset(m_ImePy.caPy, 0, sizeof(m_ImePy.caPy));
						m_ImePy.iKeyCt = 0;
					}
					m_ImePy.caRse = m_pIme->GetWords(m_ImePy.caPy, m_ImePy.iRseCt);
				}
				else
				{
					if (strlen(m_caOutStr) < 98) 
						*(m_caOutStr+strlen(m_caOutStr)) = 
							m_btChar[m_btTableH[m_pMouseDt->iy - y][1]][m_btTableW[m_pMouseDt->ix - x][1]];;
				}
			}
		}
	}
	m_ImePy.bCanInput = m_pMouseDt->lBtn;
	m_pDevice->GetMainSurface()->DrawText(m_ImePy.caPy, 0, 
				y-16, PGE_RED);
	rDes.left = x+4;
	for (int i=0; i<8; i++)
	{	
		rDes.left += 23;
		rDes.top = y+1;
		if (i < m_ImePy.iRseCt)
		{	
			DWORD dwSelChar = (DWORD)(m_ImePy.caRse+(i<<1));
			if (*((char*)dwSelChar) == 0) break;
			memcpy(m_ImePy.caView, (char*)dwSelChar, 2);
			m_pDevice->GetMainSurface()->DrawText(m_ImePy.caView, rDes.left, 
				rDes.top, PGE_WHITE);
		}
	}

	//inR(11, 输出字符串的长度) = 0; //strlen(m_caOutStr)-16;
//	inR(11) = inR(11) > 0 ? inR(11) : 0;
	m_pDevice->GetMainSurface()->DrawText(m_caOutStr, x+38, 
				y-16, PGE_WHITE);
}

PGE_RESULT CPGEvKeyBoard::Init(char*caPicFile, CPGEDevice* pDevice)
{
	CreateSurfaceFromFile(&m_pVkSurface, caPicFile, pDevice);
	m_pVkSurface->SetAlphaVal(20);
	m_pDevice = pDevice;
	
	return PGE_OK;
}

PGE_RESULT CPGEvKeyBoard::SetIme(char* caImeFile)
{
	if (m_pIme == NULL)
		m_pIme = new CPGEIme();

	m_pIme->LoadIme(caImeFile);
	return PGE_OK;
}

PGE_RESULT CPGEvKeyBoard::RegisterEvent(PGE_FUN_CALL FunCall, PGE_VK_EVENTS vkEvents)
{
	SRESULT;
	for(int i=0; i<PGE_VKEV_MAX; i++)
	{
		if (!m_vkEvents[i].FunCall)
		{
			m_vkEvents[i].FunCall = FunCall;
			m_vkEvents[i].Events = vkEvents;
			OKRESULT;
			break;
		}
	}
	RET;
}

PGE_RESULT CPGEvKeyBoard::unRegisterEvent(PGE_FUN_CALL FunCall)
{
	SRESULT;
	for(int i=0; i<PGE_VKEV_MAX; i++)
	{
		if (m_vkEvents[i].FunCall == FunCall)
		{
			m_vkEvents[i].FunCall = 0;
			m_vkEvents[i].Events = PGE_VKEV_NONE;
			OKRESULT;
			break;
		}
	}
	RET;

}