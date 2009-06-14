// ͷ�ļ� 
#ifndef _JYMAIN_H
#define _JYMAIN_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
//#include "smpeg.h"


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
 
#include "luafun.h"

#include "list.h" 

// ��������

//��ȫfreeָ��ĺ�

#define SafeFree(p) do {if(p) {free(p);p=NULL;}} while(0)

//ȫ�̱���


// jymain.c

// �����Ϣ���ļ�debug.txt��
int JY_Debug(const char * str);

//���� x�� xmin-xmax֮��
int limitX(int x, int xmin, int xmax);

//ȡ�ļ�����
int FileLength(const char *filename);



//CharSet.c

typedef struct UseFont_Type{      // ���嵱ǰʹ�õ�����ṹ
	int size;         //�ֺţ���λ����
	char *name;       //�����ļ���
    TTF_Font *font;   //�򿪵�����
}UseFont;

#define FONTNUM 10      //����ͬʱ�򿪵��������

//��ʼ������
int InitFont();

//�ͷ�����ṹ
int ExitFont();

// ���������ļ������ֺŴ�����
// size Ϊ�����ش�С���ֺ�
static TTF_Font *GetFont(const char *filename,int size);

// д�ַ���
// x,y ����
// str �ַ���
// color ��ɫ
// size �����С������Ϊ���塣 
// fontname ������
// charset �ַ��� 0 GBK 1 big5
// OScharset ����
int JY_DrawStr(int x, int y, const char *str,int color,int size,const char *fontname, 
			   int charset, int OScharset);

//�������ת���ļ�
int LoadMB(const char* mbfile);


// �����ַ���ת��
// flag = 0   Big5 --> GBK     
//      = 1   GBK  --> Big5    
//      = 2   Big5 --> Unicode
//      = 3   GBK  --> Unicode
// ע��Ҫ��֤dest���㹻�Ŀռ䣬һ�㽨��ȡsrc���ȵ�����+1����֤ȫӢ���ַ�Ҳ��ת��Ϊunicode
int  JY_CharSet(const char *src, char *dest, int flag);



//PicCache.c

// ����ʹ�õ����� 
struct CacheNode{    //��ͼcache�����ڵ�
	SDL_Surface *s;               // ����ͼ��Ӧ�ı���
	int xoff;                     // ��ͼƫ��
	int yoff;
	int id;                  //��ͼ���
    int fileid;              //��ͼ�ļ����
    struct list_head list;        // �����ṹ��linux.h�е�list.h�ж���
} ;


struct PicFileCache{   //��ͼ�ļ������ڵ�
	int num;                    // �ļ���ͼ����
    int *idx;                  // idx������
    int filelength;            //grp�ļ�����
    unsigned char *grp;                  // grp������
    struct CacheNode **pcache;  // �ļ������е���ͼ��Ӧ��cache�ڵ�ָ�룬Ϊ�����ʾû�С�
};

#define PIC_FILE_NUM 10   //�������ͼ�ļ�(idx/grp)����



int Init_Cache();

int JY_PicInit(char *PalletteFilename);

int JY_PicLoadFile(const char*filename, int id);

int JY_LoadPic(int fileid, int picid, int x,int y,int flag,int value);

static SDL_Surface *LoadPic(int fileid,int picid, int *xoffset,int *yoffset);

int JY_GetPicXY(int fileid, int picid, int *w,int *h);
 
static SDL_Surface* CreatePicSurface32(unsigned char *data, int w,int h,int datalong);

static int LoadPallette(char *filename); 
 

//mainmap.c

typedef struct Building_Type{   
	int x;
	int y;
	int num;
}BuildingType;



int JY_DrawMMap(int x, int y, int Mypic);

int JY_LoadMMap(const char* earthname, const char* surfacename, const char*buildingname,
				const char* buildxname, const char* buildyname, int x_max, int y_max);

int JY_UnloadMMap(void);

int JY_GetMMap(int x, int y , int flag);

int JY_SetMMap(short x, short y , int flag, short v);



int BuildingSort(short x, short y, short Mypic);

// ��������ͼ
int JY_DrawMMap(int x, int y, int Mypic);

int JY_LoadSMap(const char *Sfilename,const char*tmpfilename, int num,int x_max,int y_max,
				const char *Dfilename,int d_num1,int d_num2);

int JY_SaveSMap(const char *Sfilename,const char *Dfilename);

int JY_UnloadSMap();

int ReadS(int id);

int WriteS(int id);

int JY_GetS(int id,int x,int y,int level);



int JY_SetS(int id,int x,int y,int level,int v);

int JY_GetD(int Sceneid,int id,int i);

int JY_SetD(int Sceneid,int id,int i,int v);

int JY_DrawSMap(int sceneid,int x, int y,int xoff,int yoff, int Mypic);

int JY_LoadWarMap(const char *WarIDXfilename,const char *WarGRPfilename, int mapid,int num, int x_max,int y_max);

int JY_UnloadWarMap();

int JY_GetWarMap(int x,int y,int level);

int JY_SetWarMap(int x,int y,int level,int v);

int JY_CleanWarMap(int level,int v);

int JY_DrawWarMap(int flag, int x, int y, int v1,int v2);

int JY_DrawWarNum(int x, int y, int height,int color,int size,const char *fontname, 
			   int charset, int OScharset);

 


//sdlfun.c


int InitSDL(void);

int ExitSDL(void);

Uint32 ConvertColor(Uint32 color);

int InitGame(void);

int ExitGame(void);

int JY_LoadPicture(const char* str,int x,int y);

int JY_ShowSurface(); 

int JY_ShowSlow(int delaytime,int Flag);

int JY_Delay(int x);

int JY_GetTime();

int JY_PlayMIDI(const char *filename);

int StopMIDI();

int JY_PlayWAV(const char *filename);

int JY_GetKey();





int JY_SetClip(int x1,int y1,int x2,int y2);


int JY_DrawRect(int x1,int y1,int x2,int y2,int color);

void HLine32(int x1,int x2,int y,int color, unsigned char *vbuffer, int lpitch);
void VLine32(int x1,int x2,int y,int color, unsigned char *vbuffer, int lpitch);

int JY_FillColor(int x1,int y1,int x2,int y2,int color);



int BlitSurface(SDL_Surface* lpdds, int x, int y ,int flag,int value);

int JY_Background(int x1,int y1,int x2,int y2,int Bright);


int JY_PlayMPEG(const char* filename,int esckey);

int JY_FullScreen();

 
#endif