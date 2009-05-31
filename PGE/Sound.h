#pragma once
#include <mmsystem.h>
//#include <list>
#define BLOCK_SIZE  8192
#define BLOCK_COUNT 16


#define PGE_OGG 0
#define PGE_WAVE 1

enum PGE_SOUND_FILE_STAT
{
	PGE_NONEFILE = 0,
	PGE_PLAYING,
	PGE_PLAYEROVER,
	PGE_PREPARE,
	PGE_PLAYPAUSE,
};

class CPGESoundFile
{
public:
	CPGESoundFile(void);
	~CPGESoundFile(void);

	CRITICAL_SECTION m_waveCriticalSection;
	WAVEHDR*         m_waveBlocks;
	volatile int     m_waveFreeBlockCount;
	int              m_waveCurrentBlock;

	WAVEHDR* allocateBlocks(int size, int count);
	void freeBlocks(WAVEHDR* blockArray);
	void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);

	void LoadSound(char* caFileName);
	void Destory();
	FILE* m_pf;    /* file handle */
	char m_pLoadBuffer[1024]; /* intermediate buffer for reading */
	
	void Play();

	int m_iType;
	int m_iLoadSize;
	HWAVEOUT m_hWaveOut; /* device handle */

	int m_iStat;

	void Pause();
	void PausePlay();

	int m_iWaveOffset;
	void RestartWave();
};

//std::list<CPGESoundFile*> m_lstSoundFile;
enum SoundPlayerStat
{
	PLAYERSTOPED = 0,
	PLAYERPLAYING,
	PLAYERPAUSE,
};
struct SoundObj
{
	CPGESoundFile* pSoundFile;
	char* caFileName;
	int iStat;
	int iLoopTimes;
};

class CSound
{
public:
	CSound(void);
	~CSound(void);
	
	int LoadSound(char* caFile, int index);
	void PlaySound(int index, int iLoop = 1);
	void Pause(int index);
	void Stop(int index);
	void Clear(int index);
	int GetStat(int index);


};
