#include "PGESTD.h"
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sdl.h>
HANDLE g_hldfSoundThId = NULL;	// 读取文件线程的句柄
DWORD g_dwldfSoundID = 0;		// 线程ID
SoundObj g_arSound[255];
DWORD WINAPI LDSOUND(LPVOID lpparm)
{
	/*int i=0;
	while(1)
	{
		for (i=0; i<255; i++)
		{
			if (g_arSound[i].iStat)
			{
				g_arSound[i].pSoundFile->Play();
				if (!g_arSound[i].pSoundFile->m_iStat)
				{
					g_arSound[i].iStat = 0;
					g_arSound[i].iLoopTimes--;
				}

				
			}
		}
	}*/
}

CSound::CSound(void)
{
	/*memset(g_arSound, 0, sizeof(g_arSound));
	if (!g_hldfSoundThId)	// 启动文件读取线程
	{
		g_hldfSoundThId = CreateThread(NULL,0,LDSOUND,NULL,0,&g_dwldfSoundID);			
	}*/
}

CSound::~CSound(void)
{
	for (int i=0; i<255; i++)
		Clear(i);
}

int CSound::LoadSound(char* caFile, int index)
{
	if (g_arSound[index].pSoundFile) return -1;
	
	g_arSound[index].pSoundFile = new CPGESoundFile();
	//g_arSound[index].pSoundFile->LoadSound(caFile);
	g_arSound[index].caFileName = new char[strlen(caFile)+1];
	strcpy(g_arSound[index].caFileName, caFile);
	return 0;


}

void CSound::PlaySound(int index, int iLoop)
{
	if (g_arSound[index].iStat == PLAYERPAUSE)
	{
		//g_arSound[index].pSoundFile->PausePlay();
		g_arSound[index].iStat = PLAYERPLAYING;
	}
	if (g_arSound[index].iStat 
		|| g_arSound[index].iLoopTimes) return;
//	if (!g_arSound[index].pSoundFile->m_iStat)
//	{
//		if (!g_arSound[index].pSoundFile->m_iType)
//			g_arSound[index].pSoundFile->LoadSound(g_arSound[index].caFileName);
//		else 
//			g_arSound[index].pSoundFile->RestartWave();
//	}
	g_arSound[index].iStat = PLAYERPLAYING;
	g_arSound[index].iLoopTimes = iLoop;
}

void CSound::Pause(int index)
{
	if (!g_arSound[index].iStat) return;
	g_arSound[index].iStat = PLAYERPAUSE;
	//g_arSound[index].pSoundFile->Pause();
}

void CSound::Stop(int index)
{
	g_arSound[index].iStat = 0;
	g_arSound[index].iLoopTimes = 0;
	//g_arSound[index].pSoundFile->Destory();
}

void CSound::Clear(int index)
{
	if (!g_arSound[index].pSoundFile) return;
	Stop(index);
	delete g_arSound[index].pSoundFile;
	delete g_arSound[index].caFileName;


}

int CSound::GetStat(int index)
{
	return g_arSound[index].iStat;
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//CPGESoundFile
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
#define WF_OFFSET_FORMATTAG			20
#define WF_OFFSET_CHANNELS			22
#define WF_OFFSET_SAMPLESPERSEC		24
#define WF_OFFSET_AVGBYTESPERSEC	28
#define WF_OFFSET_BLOCKALIGN		32
#define WF_OFFSET_BITSPERSAMPLE		34
#define WF_OFFSET_DATASIZE			40
#define WF_OFFSET_DATA				0x24
#define WF_HEADER_SIZE WF_OFFSET_DATA

extern "C"
{
	int init_for_ogg_decode(char *filename, void **vf_out);
	int ogg_decode_at_most_one_vorbis_packet(void *vf_ptr, void *buf_out, int buf_byte_size,
		int ogg_sample_size,
		int *channels_cnt, int *sampling_rate,
		int *err_ov_hole_cnt, int *err_ov_ebadlink_cnt);
	int final_ogg_cleanup(void *vf_ptr);
}
static void CALLBACK waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);

CPGESoundFile::CPGESoundFile(void)
{
	m_iStat = PGE_NONEFILE;
	m_pf = NULL;    /* file handle */
	m_waveBlocks = NULL;
	m_waveFreeBlockCount = 0;
	m_waveCurrentBlock = 0;
	m_iType = 0;
	m_iLoadSize = 0;
	m_hWaveOut = NULL; /* device handle */
	m_iWaveOffset = 0;
}

CPGESoundFile::~CPGESoundFile(void)
{
	if (m_hWaveOut)
	{
		Destory();
	}
}

int ChannelsCount=0, SamplingRate=0, HoleCount=0, BadLinkCount=0;
void CPGESoundFile::LoadSound(char* caFileName)
{
	char caExtName[4];
	caExtName[3] = 0;
	int iSize = strlen(caFileName) - 1;
	for (int i = 0; i < 3; i++)
	{
		char caTmp = caFileName[iSize - (2-i)];
		if (caTmp < 97) caTmp = caTmp + 32;
		caExtName[i] = caTmp;
	}

	WAVEFORMATEX wfx;  /* look this up in your documentation */

	m_waveBlocks         = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
	m_waveFreeBlockCount = BLOCK_COUNT;
	m_waveCurrentBlock   = 0;

	//InitializeCriticalSection(&m_waveCriticalSection);

	if (0 == strcmp(caExtName, "ogg"))
	{
		init_for_ogg_decode(caFileName, (void**)(&m_pf));

		m_iLoadSize = ogg_decode_at_most_one_vorbis_packet(m_pf, 
			m_pLoadBuffer, sizeof(m_pLoadBuffer), 16, 
			&ChannelsCount, &SamplingRate, &HoleCount, &BadLinkCount);

		/*
		* set up the WAVEFORMATEX structure.
		*/
		wfx.nSamplesPerSec  = SamplingRate;  /* sample rate */
		wfx.wBitsPerSample  = 16;     /* sample size */
		wfx.nChannels       = ChannelsCount;      /* channels    */
		wfx.cbSize          = 0;      /* size of _extra_ info */
		wfx.wFormatTag      = WAVE_FORMAT_PCM;
		wfx.nBlockAlign     = 16*ChannelsCount/8;
		wfx.nAvgBytesPerSec = ChannelsCount*SamplingRate*16/8;
		m_iType = PGE_OGG;
	}
	else if (0 == strcmp(caExtName, "wav"))
	{
		BYTE aHeader[WF_HEADER_SIZE];	// 文件头

		// 打开文件
		m_pf = fopen(caFileName, "rb");
		if (!m_pf) {
			return;
		}

		// 设置声音格式
		fseek(m_pf, 0, SEEK_SET);
		fread(aHeader, 1, WF_HEADER_SIZE, m_pf);
		wfx.wFormatTag = *((WORD*) (aHeader + WF_OFFSET_FORMATTAG));
		wfx.nChannels = *((WORD*) (aHeader + WF_OFFSET_CHANNELS));
		wfx.nSamplesPerSec = *((DWORD*) (aHeader + WF_OFFSET_SAMPLESPERSEC));
		wfx.nAvgBytesPerSec = *((DWORD*) (aHeader + WF_OFFSET_AVGBYTESPERSEC));
		wfx.nBlockAlign = *((WORD*) (aHeader + WF_OFFSET_BLOCKALIGN));
		wfx.wBitsPerSample = *((WORD*) (aHeader + WF_OFFSET_BITSPERSAMPLE));

		BOOL bData = FALSE;
		int iCount = 0;
		char caBuf[4];
		while(!bData)
		{
			m_iWaveOffset = WF_HEADER_SIZE + iCount;
			fseek(m_pf, m_iWaveOffset, SEEK_SET);
			fread(caBuf, 1, 4, m_pf);
			bData = (*((DWORD*)caBuf) == (*(DWORD*)"data"));
			iCount++;
		}


		// 计算文件长度
		//fseek(m_fpWave, 0, SEEK_END);
		//m_dwWaveSize = ftell(m_fpWave) - WF_HEADER_SIZE;// - 32;
		DWORD dwSize;
		fread(&dwSize, 1, 4, m_pf);
		m_iWaveOffset += 4;
		m_iType = PGE_WAVE;
	}

//		if(waveOutOpen(
//			&m_hWaveOut, 
//			WAVE_MAPPER, 
//			&wfx, 
//			(DWORD_PTR)waveOutProc, 
//			(DWORD_PTR)this, 
//			CALLBACK_FUNCTION
//			) != MMSYSERR_NOERROR) {
//				//fprintf(stderr, "%s: unable to open wave mapper device\n", argv[0]);
//				//ExitProcess(1);
//			}


//	m_iStat = PGE_PREPARE;

}

void CPGESoundFile::RestartWave()
{
	fseek(m_pf, m_iWaveOffset, SEEK_SET);
	m_iStat = PGE_PREPARE;

}

void CPGESoundFile::Play()
{
	if (m_iStat != PGE_PREPARE) return;
	if (!m_iType)
	{
		//writeAudio(m_hWaveOut, m_pLoadBuffer, m_iLoadSize); 

		m_iLoadSize = ogg_decode_at_most_one_vorbis_packet(m_pf, 
			m_pLoadBuffer, 1024, 16, 
			&ChannelsCount, &SamplingRate, &HoleCount, &BadLinkCount);

		if (!m_iLoadSize) {
			m_iStat = PGE_PLAYEROVER;
			Destory();
			return;
		}
	}
	else
	{
		m_iLoadSize = fread(m_pLoadBuffer, 1024, 1, m_pf);
		//writeAudio(m_hWaveOut, m_pLoadBuffer, m_iLoadSize);
		if (!m_iLoadSize)
		{
			m_iStat = PGE_PLAYEROVER;
			return;
		}
	}

}

void CPGESoundFile::Destory()
{

	while(m_waveFreeBlockCount < BLOCK_COUNT)
		SDL_Delay(10);

	for(int i = 0; i < m_waveFreeBlockCount; i++) 
//		if(m_waveBlocks[i].dwFlags & WHDR_PREPARED)
//			waveOutUnprepareHeader(m_hWaveOut, &m_waveBlocks[i], sizeof(WAVEHDR));

	if (m_iType == PGE_OGG)
		final_ogg_cleanup(m_pf);
//	DeleteCriticalSection(&m_waveCriticalSection);
	freeBlocks(m_waveBlocks);
//	waveOutClose(m_hWaveOut);
	m_hWaveOut = NULL;
	m_iStat = PGE_NONEFILE;
}

void CPGESoundFile::writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size)
{
	/*WAVEHDR* current;
	int remain;

	current = &m_waveBlocks[m_waveCurrentBlock];

	while(size > 0) {
	
		if(current->dwFlags & WHDR_PREPARED) 
			waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

		if(size < (int)(BLOCK_SIZE - current->dwUser)) {
			memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			break;
		}

		remain = BLOCK_SIZE - current->dwUser;
		memcpy(current->lpData + current->dwUser, data, remain);
		size -= remain;
		data += remain;
		current->dwBufferLength = BLOCK_SIZE;

		waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));

		EnterCriticalSection(&m_waveCriticalSection);
		m_waveFreeBlockCount--;
		LeaveCriticalSection(&m_waveCriticalSection);

		while(!m_waveFreeBlockCount)
			Sleep(10);

		m_waveCurrentBlock++;
		m_waveCurrentBlock %= BLOCK_COUNT;

		current = &m_waveBlocks[m_waveCurrentBlock];
		current->dwUser = 0;
	}*/
}

WAVEHDR* CPGESoundFile::allocateBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	buffer = new BYTE[totalBufferSize];
	memset(buffer, 0, totalBufferSize);

	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for(i = 0; i < count; i++) {
		//blocks[i].dwBufferLength = size;
		//blocks[i].lpData = (char*)buffer;
		buffer += size;
	}

	return blocks;
}

void CPGESoundFile::Pause()
{
	//waveOutPause(m_hWaveOut);
	m_iStat = PGE_PLAYPAUSE;
}

void CPGESoundFile::PausePlay()
{
	//waveOutRestart(m_hWaveOut);
	m_iStat = PGE_PREPARE;
}

void CPGESoundFile::freeBlocks(WAVEHDR* blockArray)
{
	delete [] (BYTE*)blockArray;
}

static void CALLBACK waveOutProc(
								 HWAVEOUT hWaveOut, 
								 UINT uMsg, 
								 DWORD dwInstance,  
								 DWORD dwParam1,    
								 DWORD dwParam2     
								 )
{
	CPGESoundFile* pCtrl = (CPGESoundFile*)dwInstance;

	if(uMsg != WOM_DONE)
		return;

	//EnterCriticalSection(&pCtrl->m_waveCriticalSection);
	(pCtrl->m_waveFreeBlockCount)++;
	//LeaveCriticalSection(&pCtrl->m_waveCriticalSection);
}
