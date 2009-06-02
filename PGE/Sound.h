#ifndef Sound_H
#define Sound_H
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
typedef unsigned int FOURCC;    /* a four character code */
 
typedef struct CHUNKHDR {
  FOURCC ckid;          /* chunk ID */
  unsigned int dwSize;  /* chunk size */
} CHUNKHDR;

typedef struct WAVEHDR {
    CHUNKHDR chkRiff;
    FOURCC fccWave;
    CHUNKHDR chkFmt;
    WORD wFormatTag;	   /* format type */
    WORD nChannels;	   /* number of channels (i.e. mono, stereo, etc.) */
    DWORD nSamplesPerSec;  /* sample rate */
    DWORD nAvgBytesPerSec; /* for buffer estimation */
    WORD nBlockAlign;	   /* block size of data */
  
    WORD wBitsPerSample;
    CHUNKHDR chkData;
} WAVEHDR;
typedef UINT HWAVEOUT;
typedef HWAVEOUT *LPHWAVEOUT; 
HWAVEOUT WaveOut;

#ifndef WAVE_FORMAT_PCM

/* general waveform format structure (information common to all formats) */
typedef struct waveformat_tag {
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
} WAVEFORMAT;
typedef WAVEFORMAT       *PWAVEFORMAT;
//typedef WAVEFORMAT NEAR *NPWAVEFORMAT;
//typedef WAVEFORMAT FAR  *LPWAVEFORMAT;

/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1
/* general extended waveform format structure 
   Use this for all NON PCM formats 
   (information common to all formats)
*/

#define WOM_OPEN		0x3BB
#define WOM_CLOSE		0x3BC
#define WOM_DONE		0x3BD
#define WAVE_FORMAT_QUERY	0x0001
#define WAVE_ALLOWSYNC		0x0002
#define WAVE_FORMAT_PCM		1
#define CALLBACK_FUNCTION	0x00030000l
#define WAVERR_BASE		32
#define WAVE_MAPPER		(UINT)-1
#define WHDR_DONE       0x00000001

#endif

typedef struct waveformat_extended_tag {
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;	       /* The count in bytes of the size of
				    extra information (after cbSize) */

} WAVEFORMATEX;
typedef WAVEFORMATEX       *PWAVEFORMATEX;
//typedef WAVEFORMATEX NEAR *NPWAVEFORMATEX;
//typedef WAVEFORMATEX FAR  *LPWAVEFORMATEX;

class CPGESoundFile
{
public:
	CPGESoundFile(void);
	~CPGESoundFile(void);

	//CRITICAL_SECTION m_waveCriticalSection;
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

#endif
