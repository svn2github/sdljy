// PGESound.cpp: implementation of the CPGESound class.
//
//////////////////////////////////////////////////////////////////////

#include "PGESound.h"
#include <string.h>
#define Sound(i) m_pWaveObj[i]
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPGESound* g_pPGESound;

CPGESound::CPGESound()
{
	m_dwClassID = PGE_SOUND;
	memset(m_pWaveObj, 0, sizeof(m_pWaveObj));
	g_pPGESound = this;
}

CPGESound::~CPGESound()
{
	g_pPGESound = NULL;
	ClearAll();

}

PGE_RESULT CPGESound::LoadSound(char* caFileName, int iSoundIndex)
{
	SRESULT;

	if (iSoundIndex > 254) RET; 
	PGESoundObj* pBuf = FindWave(caFileName);
	if (pBuf != NULL) 
	{
		Sound(iSoundIndex) = pBuf;
		OKRESULT;
		RET;
	}
	if (Sound(iSoundIndex) == NULL)
	{
		Sound(iSoundIndex) = new PGESoundObj();
		Sound(iSoundIndex)->hWaveOut = NULL;
		Sound(iSoundIndex)->btFlag = 0;
		int iSize = 
			(strlen(caFileName)-30) > 0 ? (strlen(caFileName)-30) : 0;	
		strcpy(Sound(iSoundIndex)->caName, caFileName+iSize);
		Sound(iSoundIndex)->pWave = new CPGEWaveFile();

	}
	if (Sound(iSoundIndex)->pWave->Open(caFileName)) OKRESULT;
	
	RET;
}

PGE_RESULT CPGESound::Play(int iSoundIndex, BYTE btLoopTimes, DWORD dwVolume)
{
//	SRESULT;
//	if (Sound(iSoundIndex) == NULL) RET;
//	if (Sound(iSoundIndex)->pWave == NULL) RET;
//	MMRESULT mmres;
//	if (Sound(iSoundIndex)->hWaveOut == NULL)
//	{
//		
//		UINT devId;
//		for (devId = 0; devId < waveOutGetNumDevs(); devId++) 
//		{
//			mmres = waveOutOpen(&(Sound(iSoundIndex)->hWaveOut), 
//				devId, Sound(iSoundIndex)->pWave->GetWaveFormat(), (DWORD)WaveProc,
//				iSoundIndex, CALLBACK_FUNCTION);
//			if (mmres == MMSYSERR_NOERROR) {
//				break;
//			}
//		}
//	}
//	
//	if (Sound(iSoundIndex)->btFlag == PGE_SOUND_PLAYING) RET;
//	if (Sound(iSoundIndex)->btFlag == PGE_SOUND_PAUSED)
//	{
//		if(MMSYSERR_NOERROR == waveOutRestart(Sound(iSoundIndex)->hWaveOut))
//		{
//			Sound(iSoundIndex)->btFlag = PGE_SOUND_PLAYING;
//			OKRESULT;
//		}	
//		RET;
//	}
//	Sound(iSoundIndex)->btLoopTimes = btLoopTimes;
//	DWORD dwBuf = (dwVolume << 16) | (dwVolume & 0xffff);
//	Sound(iSoundIndex)->dwVolume = dwBuf;
//	mmres = waveOutSetVolume(Sound(iSoundIndex)->hWaveOut, dwBuf);
//	if (mmres != MMSYSERR_NOERROR) 
//	{
//		RET;
//	}
//
//	ZeroMemory(&(Sound(iSoundIndex)->whdr), sizeof(WAVEHDR));
//	Sound(iSoundIndex)->whdr.dwBufferLength = Sound(iSoundIndex)->pWave->GetLength();
//	
//	Sound(iSoundIndex)->whdr.lpData = Sound(iSoundIndex)->pWave->GetWaveData();
//
//	mmres = waveOutPrepareHeader(Sound(iSoundIndex)->hWaveOut, 
//		&(Sound(iSoundIndex)->whdr), sizeof(WAVEHDR));	
//	if (mmres != MMSYSERR_NOERROR) 
//	{
//		RET;
//	}
//
//	mmres = waveOutWrite(Sound(iSoundIndex)->hWaveOut, 
//		&(Sound(iSoundIndex)->whdr), sizeof(WAVEHDR));	
//	if (mmres != MMSYSERR_NOERROR) 
//	{
//		RET;
//	}
//	Sound(iSoundIndex)->btFlag = PGE_SOUND_PLAYING;
//	OKRESULT
//	RET;
}


void CPGESound::Play2(int iSoundIndex)
{
//	Sound(iSoundIndex)->whdr.dwBufferLength = Sound(iSoundIndex)->pWave->GetLength();
//
//	Sound(iSoundIndex)->whdr.lpData = Sound(iSoundIndex)->pWave->GetWaveData();
//
//	waveOutWrite(Sound(iSoundIndex)->hWaveOut, 
//		&(Sound(iSoundIndex)->whdr), sizeof(WAVEHDR));	
//	Sound(iSoundIndex)->btFlag = PGE_SOUND_PLAYING;
}

#define NULLRET \
	SRESULT; \
	if (Sound(iSoundIndex) == NULL) RET; \
	if (Sound(iSoundIndex)->pWave == NULL) RET; \
	if (Sound(iSoundIndex)->hWaveOut == NULL) RET; \


PGE_RESULT CPGESound::Stop(int iSoundIndex)
{
//	NULLRET;
//	Sound(iSoundIndex)->btLoopTimes = 0;
//	Sound(iSoundIndex)->btFlag = PGE_SOUND_STOPED;
//	MMRESULT mmres = waveOutReset(Sound(iSoundIndex)->hWaveOut);
//	if (mmres != MMSYSERR_NOERROR) return PGE_FAIL;
//	
//
//	return PGE_OK;
}

PGE_RESULT CPGESound::Pause(int iSoundIndex)
{
//	NULLRET;
//	MMRESULT mmres = waveOutPause(Sound(iSoundIndex)->hWaveOut);
//	if (mmres != MMSYSERR_NOERROR) return PGE_FAIL;
//	
//	Sound(iSoundIndex)->btFlag = PGE_SOUND_PAUSED;
//	return PGE_OK;
}

PGE_RESULT CPGESound::Clear(int iSoundIndex)
{
//	SRESULT;
//	if (Sound(iSoundIndex) == NULL) RET;
//	if (Sound(iSoundIndex)->pWave == NULL) RET;
//
//	MMRESULT mmres;
//
//	mmres = waveOutUnprepareHeader(Sound(iSoundIndex)->hWaveOut, 
//		&(Sound(iSoundIndex)->whdr), sizeof(WAVEHDR));	
//
//
//	mmres = waveOutClose(Sound(iSoundIndex)->hWaveOut);
//
//	SAFE_DELETE(Sound(iSoundIndex)->pWave);
//
//	SAFE_DELETE(Sound(iSoundIndex));
//
//
//	return PGE_OK;
}

PGE_RESULT CPGESound::StopAll()
{
	for (int i=0; i<PGE_SOUND_MAX; i++)
	{
		Stop(i);
	}
	return PGE_OK;
}

PGE_RESULT CPGESound::PauseAll()
{
	for (int i=0; i<PGE_SOUND_MAX; i++)
	{
		Pause(i);
	}
	return PGE_OK;
}

PGE_RESULT CPGESound::ClearAll()
{
	for (int i=0; i<PGE_SOUND_MAX; i++)
	{
		Clear(i);
	}
	return PGE_OK;
}

BOOL CPGESound::isPlay(int iSoundIndex)
{
	if (Sound(iSoundIndex)->btFlag == PGE_SOUND_PLAYING)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

PGE_RESULT CPGESound::VolumeCtrl(int iSoundIndex, DWORD dwVolume)
{
//	NULLRET;
//	if (waveOutSetVolume(Sound(iSoundIndex)->hWaveOut, dwVolume) != MMSYSERR_NOERROR) 
//	{
//		RET;
//	}
//	Sound(iSoundIndex)->dwVolume = dwVolume;
//	return PGE_OK;
}

PGESoundObj* CPGESound::FindWave(int iSoundIndex)
{
	return m_pWaveObj[iSoundIndex];
}

PGESoundObj* CPGESound::FindWave(char* caFileName)
{
	int iLen = (strlen(caFileName)-30) > 0 ? (strlen(caFileName)-30) : 0;	
	for(int i=0; i<PGE_SOUND_MAX; i++)
	{
		if (Sound(i) == NULL) break;
		if (strcmp(Sound(i)->caName, caFileName+iLen) != 0)
		{
			return Sound(i);
		}
	}
	return NULL;
}

void CALLBACK CPGESound::WaveProc( 
                                HWAVEOUT hwo, 
                                UINT uMsg, 
                                DWORD dwInstance, 
                                DWORD dwParam1, 
                                DWORD dwParam2 )
{
//	if (WOM_DONE == uMsg)
//	{
//		if (g_pPGESound == NULL) return;
//		if (g_pPGESound->m_pWaveObj[dwInstance]->btFlag == PGE_SOUND_STOPED)
//		{
//			return;
//		}
//		g_pPGESound->m_pWaveObj[dwInstance]->btFlag = PGE_SOUND_STOPED;
//		int iLoop = g_pPGESound->m_pWaveObj[dwInstance]->btLoopTimes;
//
//		if (!g_pPGESound->m_pWaveObj[dwInstance]->pWave->bLoadOver)
//		{
//			g_pPGESound->m_pWaveObj[dwInstance]->pWave->ContinueLoad(iLoop);
//		}
//		else
//		{
//			if (iLoop<128) iLoop--;
//		}
//
//		if (iLoop)
//		{
//			g_pPGESound->Play(dwInstance, iLoop, g_pPGESound->m_pWaveObj[dwInstance]->dwVolume);
//		}	
//	}
}
