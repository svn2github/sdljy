// WaveFile.h: 此文件定义了CPGEWaveFile类
// 此类实现了对Wave文件的读取
//
//////////////////////////////////////////////////////////////////////
#ifndef PGEWaveFile_H
#define PGEWaveFile_H

#include "pgestd.h"       
#define PGE_WAVE_MAX_LOAD 524288//1048576//1048576//8388608//33554432//65536////4194304// 使用的缓冲区的大小
#define PGE_WAVE_BUF_SIZE 524288
#define PGE_OGG_DECODE_SIZE 4096

enum PGE_SOUND_BUF	
{
	PGE_SOUND_BUF_1 = 0,
		PGE_SOUND_BUF_2
};
typedef struct {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
  WORD  wBitsPerSample;
  WORD  cbSize;
} WAVEFORMATEX;

class CPGEWaveFile :
	public CPGEBase
{
public:
	CPGEWaveFile();
	virtual ~CPGEWaveFile();

	
	PGE_RESULT Open(char* szFileName);		// 打开Wave文件

	DWORD GetLength(int iFlag = 0);			// 取得当前播放缓冲区的大小，请使用默认参数
	
	WAVEFORMATEX* GetWaveFormat();			// 返回缓冲区声音格式
	char* GetWaveData(int iBufId = -1);		// 取得声音缓冲区数据，请使用默认参数

	char** GetWaveDataPtr(int iBufId = -1);	// 取得声音缓冲区的地址，请使用默认参数
	PGE_RESULT ContinueLoad(BOOL &bLoop);	// 当播放完一段缓冲之后继续读取之后的内容

	PGE_RESULT ReLoad();		// 如果播放一个长度超过缓冲区的文件，在播放完成后需要再次播放则必须ReLoad,循环播放时不用
	BOOL bLoadOver;		// 是否全部读完
	FILE* m_fpWave;		// 声音文件
	int m_iUseMem;		// 当前使用哪一个缓冲
	long m_nLength;		// 播放缓冲的长度
	int m_iGetLFlag;	// 
	__inline PGE_RESULT DecodeOgg(BYTE* pBuf, DWORD dwSize, int& iLoop);		//此函数不提供调用，否者会出现致命错误

private:
	BYTE* m_pbtWaveData[2];		// 播放缓冲
	WAVEFORMATEX m_Format;		// 声音格式
	DWORD m_dwWaveSize;			// 文件大小

	void* m_pVf;	// ogg文件缓存
	char m_caOggBuf[PGE_OGG_DECODE_SIZE];	// ogg解压缓存

	char m_fileName[200];
	

};

#endif
