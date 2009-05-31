#ifndef PGEGameObj_h
#define PGEGameObj_h

#include "pgesurface.h"
#include <SDL.h>
//#include "mmsystem.h"

#define WORLD_CLASS extern ObjectList world;

class CPGETimer
{
public:
	DWORD dwDalyTimes;
	DWORD dwStaticTimes;
	DWORD dwAllTimes;

	CPGETimer(void)
	{
		dwDalyTimes = 0;
		dwStaticTimes = 0;
	}
	~CPGETimer(void){}

	void Init()
	{
		dwStaticTimes = SDL_GetTicks();
		dwAllTimes = 0;
	}

	void Tick()
	{	
		dwDalyTimes = SDL_GetTicks() - dwStaticTimes;
		dwStaticTimes += dwDalyTimes;
		dwAllTimes += dwDalyTimes;
	}

	DWORD GetAllTimes()
	{
		return dwAllTimes;
	}

	DWORD GetDalyTimes()
	{
		return dwDalyTimes;
	}

};

enum GAMEOBJ_TYPE
{
	OBJ_MAP = 0x6f,
	OBJ_PLAYER,
};

struct WPOINT
{
	short int wx;
	short int wy;
};

class CPGEGameObj
{
public:
	CPGEGameObj(BYTE btType){
		m_btType = btType;
	}
	~CPGEGameObj(void){}

	virtual void Tick(CPGETimer* timer){}
	virtual void Draw(CPGESurface* pMainSuf) = 0;
	WPOINT m_ptCur;
	BYTE m_btType;
};

#define HASH_SIZE 512
class ObjectList	// 身体的链表
{

public:
	ObjectList(){
		map = 0;
		player = 0;
		m_iStart = 0;
		clear();
	}
	~ObjectList(){}
	void push_back(CPGEGameObj* pBody)	// 在链表尾部添加节点
	{
		for (int i=0; i<HASH_SIZE; i++)
		{
			if (!m_pLstData[i])
			{

				m_pLstData[i] = pBody;
				m_iCount++;
				m_iEnd = m_iCount;
				break;
			}
		}
	}

	void reMove(CPGEGameObj* pobj)
	{
		for (int i=0; i<HASH_SIZE; i++)
		{
			if (m_pLstData[i] == pobj)
			{

				m_pLstData[i] = 0;
				m_iCount--;
				break;
			}
		}
	}

	CPGEGameObj* GetPos(int iPos)	// 取得指标指示节点的内容
	{
		return m_pLstData[iPos];
	}

	int begin()	// 取得头部节点的指标
	{
		return m_iStart;
	}

	int end()	// 取得尾部节点的指标
	{
		return m_iEnd;
	}

	void clear()	// 清除全部链表
	{
		memset(m_pLstData, 0, sizeof(m_pLstData));
		m_iStart = 0;
		m_iEnd = 0;
		m_iCount = 0;
	}
	
	void SetMap(CPGEGameObj* pMap)
	{
		map = pMap;
	}

	void sort()
	{
		DWORD dwBox[256]; 
		int j = 0; 
		DWORD pos = 0 ; 
		DWORD temp = 0; 
		for(int i=0; i<4; i++) 
		{ 
			memset(dwBox, 0, sizeof(DWORD)<<8); 
			memcpy(m_pLstDataBf, m_pLstData, HASH_SIZE * sizeof(void*)); 
			for(j=0; j<m_iCount; j++) 
			{ 
				++dwBox[(m_pLstDataBf[j]->m_ptCur.wy >> (i<<3)) & 0xFF]; 
			} 
			pos = 0; 
			for(j=0; j<256; j++) 
			{ 
				temp = dwBox[j]; 
				dwBox[j] = pos; 
				pos += temp; 
			} 
			for(j=0; j<m_iCount; j++) 
			{ 
				m_pLstData[dwBox[(m_pLstDataBf[j]->m_ptCur.wy >> (i<<3)) & 0xFF]++] = m_pLstDataBf[j]; 
			} 
		}	
	}

	CPGEGameObj* m_pLstData[HASH_SIZE];	
	CPGEGameObj* m_pLstDataBf[HASH_SIZE];
	CPGEGameObj* map;
	CPGEGameObj* player;
	int m_iStart;
	int m_iEnd;
	int m_iCount;

};

#endif
