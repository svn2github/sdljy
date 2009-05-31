// WaveFile.h: ���ļ�������CPGEWaveFile��
// ����ʵ���˶�Wave�ļ��Ķ�ȡ
//
//////////////////////////////////////////////////////////////////////
#ifndef PGEWaveFile_H
#define PGEWaveFile_H

#include "pgestd.h"       
#define PGE_WAVE_MAX_LOAD 524288//1048576//1048576//8388608//33554432//65536////4194304// ʹ�õĻ������Ĵ�С
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

	
	PGE_RESULT Open(char* szFileName);		// ��Wave�ļ�

	DWORD GetLength(int iFlag = 0);			// ȡ�õ�ǰ���Ż������Ĵ�С����ʹ��Ĭ�ϲ���
	
	WAVEFORMATEX* GetWaveFormat();			// ���ػ�����������ʽ
	char* GetWaveData(int iBufId = -1);		// ȡ���������������ݣ���ʹ��Ĭ�ϲ���

	char** GetWaveDataPtr(int iBufId = -1);	// ȡ�������������ĵ�ַ����ʹ��Ĭ�ϲ���
	PGE_RESULT ContinueLoad(BOOL &bLoop);	// ��������һ�λ���֮�������ȡ֮�������

	PGE_RESULT ReLoad();		// �������һ�����ȳ������������ļ����ڲ�����ɺ���Ҫ�ٴβ��������ReLoad,ѭ������ʱ����
	BOOL bLoadOver;		// �Ƿ�ȫ������
	FILE* m_fpWave;		// �����ļ�
	int m_iUseMem;		// ��ǰʹ����һ������
	long m_nLength;		// ���Ż���ĳ���
	int m_iGetLFlag;	// 
	__inline PGE_RESULT DecodeOgg(BYTE* pBuf, DWORD dwSize, int& iLoop);		//�˺������ṩ���ã����߻������������

private:
	BYTE* m_pbtWaveData[2];		// ���Ż���
	WAVEFORMATEX m_Format;		// ������ʽ
	DWORD m_dwWaveSize;			// �ļ���С

	void* m_pVf;	// ogg�ļ�����
	char m_caOggBuf[PGE_OGG_DECODE_SIZE];	// ogg��ѹ����

	char m_fileName[200];
	

};

#endif
