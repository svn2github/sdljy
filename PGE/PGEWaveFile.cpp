
// WaveFile.cpp: implementation of the CWaveFile class.
//
//////////////////////////////////////////////////////////////////////
#include "PGESTD.h"
#include "PgeWaveFile.h"
#include "ogginc/TgPlayOgg_use_vorbisfile.h"
#include <string.h>

#define WF_OFFSET_FORMATTAG			20
#define WF_OFFSET_CHANNELS			22
#define WF_OFFSET_SAMPLESPERSEC		24
#define WF_OFFSET_AVGBYTESPERSEC	28
#define WF_OFFSET_BLOCKALIGN		32
#define WF_OFFSET_BITSPERSAMPLE		34
#define WF_OFFSET_DATASIZE			40
#define WF_OFFSET_DATA				0x24
#define WF_HEADER_SIZE WF_OFFSET_DATA

HANDLE g_hldfSoundThId = NULL;	// 读取文件线程的句柄
DWORD g_dwldfSoundID = 0;		// 线程ID
#define MSG_PGE_SOUND_LOAD WM_USER+0xfff	// 读取下一段的消息

//////////////////////////////////////////////////////////////////////
// LDSOUND函数
// 此函数是读取文件线程的线程函数，实现了分段读取一个声音文件的功能
// 输入参数无意义
// 无返回
//////////////////////////////////////////////////////////////////////
DWORD WINAPI LDSOUND(LPVOID lpparm)	
{
	/*MSG msg; 
	while(1)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )		// 是否有消息
		{
			if(GetMessage( &msg, NULL, 0, 0 ) )		// 有消息便取得它
			{
				if (msg.message == MSG_PGE_SOUND_LOAD)	// 是否读取文件的消息
				{
					
					CPGEWaveFile* pWaveFile = (CPGEWaveFile*)msg.lParam;
					if (pWaveFile->m_fpWave == NULL)
					{
						BYTE* btBuf = (BYTE*)pWaveFile->GetWaveData(!pWaveFile->m_iUseMem);
						pWaveFile->DecodeOgg(btBuf, pWaveFile->GetLength(2), (*((int*)msg.wParam)));	
						goto OggOk;
					}
					else
					{	// 如果是Wave文件
						if (pWaveFile->GetLength(2) != PGE_WAVE_MAX_LOAD/2)	
							// 声音缓冲区的长度不对
						{
							char** pBuf = pWaveFile->GetWaveDataPtr(!pWaveFile->m_iUseMem);
							SAFE_DELETE((*pBuf));
							*pBuf = new char[PGE_WAVE_MAX_LOAD/2];
						}

						char* caBuf = pWaveFile->GetWaveData(!pWaveFile->m_iUseMem);	// 取得下一个声音缓冲
						DWORD dwSize = fread(caBuf, 1, pWaveFile->GetLength(2), 
							pWaveFile->m_fpWave);	// 从文件读取并填充到缓冲区
						if (dwSize < pWaveFile->GetLength(2))	// 到了文件尾
						{
							char* buf = new char[dwSize];		// 重新调整缓冲大小rtsp://61.145.118.250/encoder”

							memcpy(buf, caBuf, dwSize);
							char** ppCaBuf = pWaveFile->GetWaveDataPtr(!pWaveFile->m_iUseMem);
							*ppCaBuf = buf;
							SAFE_DELETE(caBuf);
							while(pWaveFile->m_iGetLFlag)	// 确认播放已经开始，因为要修改缓冲的大小，必须等待播放函数取得正确的缓冲大小后再修改
							{
								Sleep(10);
							}
							pWaveFile->m_nLength = dwSize;	// 修改缓冲大小
							(*((int*)msg.wParam))--;	// 播放的循环次数减一
							
							if ((*((int*)msg.wParam)))	// 如果到了文件尾以后还要再次播放
							{
								fseek(pWaveFile->m_fpWave, 0, WF_HEADER_SIZE);	// 文件指针定位到文件头
							}
							else
							{
								pWaveFile->bLoadOver = TRUE;		// 文件读取完毕
							}

						}
					}
OggOk:
					;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		{
			Sleep(30);	// 没有消息，休眠线程
		}
	}*/
}

//////////////////////////////////////////////////////////////////////
// CPGEWaveFile函数
// CPGEWaveFile构造函数
// 无参数
// 无返回
//////////////////////////////////////////////////////////////////////
CPGEWaveFile::CPGEWaveFile()
{
	m_pbtWaveData[0] = m_pbtWaveData[1] = NULL;
	m_fpWave = NULL;
	m_dwWaveSize = 0;
	m_iUseMem = PGE_SOUND_BUF_1;
	memset(&m_Format, 0, sizeof(m_Format));
	m_iGetLFlag = 0;
	memset(m_fileName, 0, sizeof(m_fileName));
	//if (!g_hldfSoundThId)	// 启动文件读取线程
	//{
	//	g_hldfSoundThId = CreateThread(NULL,0,LDSOUND,NULL,0,&g_dwldfSoundID);			
	//}
}

//////////////////////////////////////////////////////////////////////
// ~CPGEWaveFile函数
// CPGEWaveFile析构函数
// 无参数
// 无返回
//////////////////////////////////////////////////////////////////////
CPGEWaveFile::~CPGEWaveFile()
{

	//PostThreadMessage(g_dwldfSoundID, WM_USER+0xffe, (WPARAM)0, (LPARAM)this);
	SAFE_DELETE(m_pbtWaveData[0]);
	SAFE_DELETE(m_pbtWaveData[1]);
	if (m_fpWave) 
	{
		fclose(m_fpWave);
		m_fpWave = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Open函数
// 此函数负责读取声音文件
// 参数 szFileName， 文件名
// 成功返回 PGE_OK,否则 PGE_FAIL
//////////////////////////////////////////////////////////////////////
PGE_RESULT CPGEWaveFile::Open(char* szFileName)
{
	SRESULT;
	if (m_pbtWaveData[PGE_SOUND_BUF_1] != NULL)
		SAFE_DELETE(m_pbtWaveData[PGE_SOUND_BUF_1]);	// 保正缓冲区不会泄露
	if (m_pbtWaveData[PGE_SOUND_BUF_2] != NULL)
		SAFE_DELETE(m_pbtWaveData[PGE_SOUND_BUF_2]);	// 保正缓冲区不会泄露

	strcpy(m_fileName, szFileName);
	char caExtName[4];
	caExtName[3] = 0;
	int iSize = strlen(szFileName) - 1;
	for (int i = 0; i < 3; i++)
	{
		char caTmp = szFileName[iSize - (2-i)];
		if (caTmp < 97) caTmp = caTmp + 32;
		caExtName[i] = caTmp;
	}

	if (0 == strcmp(caExtName, "ogg"))
	{

		if(init_for_ogg_decode(szFileName, &m_pVf)) RET;

		m_nLength = PGE_WAVE_MAX_LOAD / 2;
		m_pbtWaveData[PGE_SOUND_BUF_1] = new BYTE[m_nLength];
		m_pbtWaveData[PGE_SOUND_BUF_2] = new BYTE[m_nLength];
		bLoadOver = FALSE;
		int iLp = 1;
		if (PGEFAILED(DecodeOgg(m_pbtWaveData[PGE_SOUND_BUF_1], m_nLength, iLp))) RET;
		if (PGEFAILED(DecodeOgg(m_pbtWaveData[PGE_SOUND_BUF_2], m_nLength, iLp))) RET;

		RET;
	}


	BYTE aHeader[WF_HEADER_SIZE];	// 文件头

	// 打开文件
	m_fpWave = fopen(szFileName, "rb");
	if (!m_fpWave) {
		RET;
	}

	// 设置声音格式
	fseek(m_fpWave, 0, SEEK_SET);
	fread(aHeader, 1, WF_HEADER_SIZE, m_fpWave);
	m_Format.wFormatTag = *((WORD*) (aHeader + WF_OFFSET_FORMATTAG));
	m_Format.nChannels = *((WORD*) (aHeader + WF_OFFSET_CHANNELS));
	m_Format.nSamplesPerSec = *((DWORD*) (aHeader + WF_OFFSET_SAMPLESPERSEC));
	m_Format.nAvgBytesPerSec = *((DWORD*) (aHeader + WF_OFFSET_AVGBYTESPERSEC));
	m_Format.nBlockAlign = *((WORD*) (aHeader + WF_OFFSET_BLOCKALIGN));
	m_Format.wBitsPerSample = *((WORD*) (aHeader + WF_OFFSET_BITSPERSAMPLE));

	BOOL bData = FALSE;
	int iCount = 0;
	char caBuf[4];
	while(!bData)
	{
		fseek(m_fpWave, WF_HEADER_SIZE + iCount, SEEK_SET);
		fread(caBuf, 1, 4, m_fpWave);
		bData = (*((DWORD*)caBuf) == (*(DWORD*)"data"));
		iCount++;
	}


	// 计算文件长度
	//fseek(m_fpWave, 0, SEEK_END);
	//m_dwWaveSize = ftell(m_fpWave) - WF_HEADER_SIZE;// - 32;

	fread(&m_dwWaveSize, 1, 4, m_fpWave);

	m_nLength = m_dwWaveSize;
	bLoadOver = TRUE;
	if (m_dwWaveSize > PGE_WAVE_MAX_LOAD)	// 是否需要分段读取
	{

		m_nLength = PGE_WAVE_MAX_LOAD / 2;
		m_pbtWaveData[PGE_SOUND_BUF_1] = new BYTE[m_nLength];
		m_pbtWaveData[PGE_SOUND_BUF_2] = new BYTE[m_nLength];
		bLoadOver = FALSE;
	}
	
	if (!m_pbtWaveData[PGE_SOUND_BUF_1])  m_pbtWaveData[PGE_SOUND_BUF_1] = new BYTE[m_nLength];


	// 读取声音内容
	fread(m_pbtWaveData[PGE_SOUND_BUF_1], 1, m_nLength, m_fpWave);
	if (m_pbtWaveData[PGE_SOUND_BUF_2]) fread(m_pbtWaveData[PGE_SOUND_BUF_2], 1, m_nLength, m_fpWave);
	
	if (bLoadOver)		// 一次读取完毕s么？
	{
		fclose(m_fpWave);
		m_fpWave = NULL;
	}
	OKRESULT;
	RET;
}


DWORD CPGEWaveFile::GetLength(int iFlag)
{
	m_iGetLFlag = iFlag;
	return m_nLength;
}

WAVEFORMATEX* CPGEWaveFile::GetWaveFormat() 
{
	return (&m_Format);
}	

char* CPGEWaveFile::GetWaveData(int iBufId)	
{
	if (iBufId < 0)
	{
		return (char*)m_pbtWaveData[m_iUseMem];
	}
	else if (iBufId < 2)
	{
		return (char*)m_pbtWaveData[iBufId];
	}
	else
	{
		return NULL;
	}
}

char** CPGEWaveFile::GetWaveDataPtr(int iBufId)
{
	if (iBufId < 0)
	{
		return (char**)(&m_pbtWaveData[m_iUseMem]);
	}
	else if (iBufId < 2)
	{
		return (char**)(&m_pbtWaveData[iBufId]);
	}
	else
	{
		return NULL;
	}
}


PGE_RESULT CPGEWaveFile::ContinueLoad(BOOL &bLoop)
{
	/*SRESULT;
	if (bLoadOver) RET;
	m_iUseMem = !m_iUseMem;
	if (PostThreadMessage(g_dwldfSoundID, MSG_PGE_SOUND_LOAD, (WPARAM)&bLoop, (LPARAM)this))
	{
		OKRESULT;
	}

	RET;*/
}

PGE_RESULT CPGEWaveFile::ReLoad()
{
	bLoadOver = FALSE;
	fseek(m_fpWave, 0, WF_HEADER_SIZE);
	return PGE_OK;
}
//DWORD dwAllBt = 0;
__inline PGE_RESULT CPGEWaveFile::DecodeOgg(BYTE* pBuf, DWORD dwSize, int& iLoop)
{
	SRESULT;
	DWORD dwOutSize = 0;
	int ChannelsCount=0, SamplingRate=0, HoleCount=0, BadLinkCount=0;
	WORD wRealSize = 0;
	while(1)
	{
		
		wRealSize = ogg_decode_at_most_one_vorbis_packet(m_pVf, 
			m_caOggBuf, PGE_OGG_DECODE_SIZE, 16, 
			&ChannelsCount, &SamplingRate, &HoleCount, &BadLinkCount);
		if (wRealSize == 0)	// 到了文件尾
		{
			if (iLoop)
				iLoop--;
			if (iLoop)
			{
				final_ogg_cleanup(m_pVf);
				Open(m_fileName);

			}
			else
			{
				bLoadOver = TRUE;
			}
			break;
		}

		if (dwOutSize+wRealSize >= m_nLength) 
		{
			memcpy(pBuf+dwOutSize, m_caOggBuf, m_nLength-dwOutSize);
//			dwAllBt+=(pgeR(0)+inR(2)-m_nLength);
//			TRACE("忽略字节:%d   全部:%d\r\n", pgeR(0)+inR(2)-m_nLength, dwAllBt);
			
			//while(m_iGetLFlag) NULL;

			//m_nLength = pgeR(0)+inR(2);
			OKRESULT;
			break;
		}
		memcpy(pBuf+dwOutSize, m_caOggBuf, wRealSize);
		dwOutSize += wRealSize;

	}
	if (m_Format.wFormatTag == 0)
	{	
#ifdef _WIN32_WCE
		vorbis_info * pVb = ov_info(static_cast<OggVorbis_File*>(m_pVf), 0);
		ChannelsCount = pVb->channels;
		SamplingRate = pVb->rate;
#endif
		m_Format.wFormatTag = 1;
		m_Format.nChannels = ChannelsCount;
		m_Format.nSamplesPerSec = SamplingRate;
		m_Format.nAvgBytesPerSec = ChannelsCount*SamplingRate*16/8;
		m_Format.nBlockAlign = 16*ChannelsCount/8;
		m_Format.wBitsPerSample = 16;
	}
	
	RET;
}
