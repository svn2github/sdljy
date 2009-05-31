// PGESound.h: interface for the CPGESound class.
//
//////////////////////////////////////////////////////////////////////
#ifndef PGESound_H
#define PGESound_H

#include "pgestd.h"
#include "PGEWaveFile.h"

#define PGE_SOUND_MAX 255
typedef UINT HWAVEOUT;
typedef struct wavehdr_tag {
      LPSTR lpData;
      DWORD dwBufferLength;
      DWORD dwBytesRecorded;
      DWORD dwUser;
      DWORD dwFlags;
      DWORD dwLoops;
      struct wavehdr_tag *lpNext;
      DWORD reserved;
  } WAVEHDR, *LPWAVEHDR; 
typedef struct tgaPgeSoundObj
{
	char caName[30];
	CPGEWaveFile* pWave;
	BYTE btFlag;
	BYTE btLoopTimes;
	HWAVEOUT hWaveOut;
	WAVEHDR whdr;
	DWORD dwVolume;
} PGESoundObj;

enum PGE_SOUND_FLAG
{
	PGE_SOUND_STOPED = 0,
		PGE_SOUND_PLAYING,
		PGE_SOUND_PAUSED

};

class CPGESound  : public CPGEBase
{
public:
	CPGESound();
	virtual ~CPGESound();

	PGE_RESULT LoadSound(char* caFileName, int iSoundIndex);
	PGE_RESULT Play(int iSoundIndex, BYTE btLoopTimes = 1, DWORD dwVolume = 0x7fff);
	void Play2(int iSoundIndex);
	PGE_RESULT Stop(int iSoundIndex);
	PGE_RESULT Pause(int iSoundIndex);
	PGE_RESULT Clear(int iSoundIndex);
	PGE_RESULT StopAll();
	PGE_RESULT PauseAll();
	PGE_RESULT ClearAll();
	PGE_RESULT VolumeCtrl(int iSoundIndex, DWORD dwVolume);
	
	BOOL isPlay(int iSoundIndex);
	PGESoundObj* m_pWaveObj[PGE_SOUND_MAX];
	
private:
	PGESoundObj* FindWave(int iSoundIndex);
	PGESoundObj* FindWave(char* caFileName);
	
	//static CALLBACK PlayedCall(DWORD dwParam);
	static void CALLBACK WaveProc( 
                                HWAVEOUT hwo, 
                                UINT uMsg, 
                                DWORD dwInstance, 
                                DWORD dwParam1, 
                                DWORD dwParam2 );

};

#endif
