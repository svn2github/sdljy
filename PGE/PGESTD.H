#ifndef PGESTDH
#define PGESTDH

/*#include <windows.h>


#include "gx.h"


#include <mmsystem.h>*/

extern "C"
{
#include "LuaInc/lauxlib.h"
#include "LuaInc/lua.h"
#include "LuaInc/lualib.h"
}

#include "lzo/lzo.h"
//#define fopen PGEFileOpen

typedef struct tagRECT
 {
		 int left;
		 int top;
		 int right;
		 int bottom;
 }RECT;
 typedef RECT* LPRECT;
typedef struct tagPOINT 
 {
		 int x;
		 int y;
 }POINT;
 typedef POINT* LPPOINT;
#define PGE_RESULT int									// PGE返回类型
//typedef PGEDisplayProperties GXDisplayProperties		// PGE屏幕信息
//基本类型重定义
 typedef short  int8;
 typedef long int16;
 typedef long long int32;
 typedef __int64 int64;
 typedef unsigned short uint8;
 typedef unsigned int uint16,UINT;
 typedef unsigned long uint32;
 typedef unsigned __int64 uint64;
 typedef unsigned long DWORD;
 typedef unsigned short WORD;
 typedef unsigned char BYTE;
 //typedef long HWND;
 typedef int BOOL;
 typedef unsigned char* LPSTR,PSTR;
 typedef unsigned long HINSTANCE ;
 typedef unsigned long HWND ;
 typedef unsigned int WPARAM;
 typedef unsigned long LPARAM;
 
#define CALLBACK(foo)   WORD foo __attribute__((stdcall)) 
 #define PAS_PTR(x,arg)  typedef WORD (*x) arg __attribute__((stdcall)) 
 #define GUARD()         __asm__ __volatile__ ("pushfd; cli") 
 #define UNGUARD()       __asm__ __volatile__ ("popfd");
 typedef float FLOAT;
 typedef char CHAR;
 #define VOID         void
 #define WINAPI      __attribute__((stdcall))
// #define CALLBACK  __attribute__((stdcall));
 typedef struct { 
 UINT uMsg; 
 WPARAM wParam; 
 LPARAM lParam; 
 PSTR pStr; 
 POINT pt; 
 }MSG;

 #define CALLBACK
 //typedef void (*CALLBACK)(int);
struct PGEDisplayProperties 
{
    DWORD cxWidth;
    DWORD cyHeight;
    long cbxPitch;
    long cbyPitch;
    long cBPP;
    DWORD ffFormat;
}; 
#define SRESULT \
	int iRet = PGE_FAIL; \

#define OKRESULT \
	iRet = PGE_OK; \

#define RET \
	return iRet; \

#define PGEFAILED(RESULT) \
		(RESULT == PGE_FAIL ? TRUE : FALSE) \



// PGE翻译GX返回信息宏
#define GXRETURN(n) \
	if (n > 0) \
	{ \
		return PGE_OK; \
	} \
	else \
	{ \
		return PGE_FAIL; \
	} \

#define SAFE_DELETE(p) \
	if (p) \
	{ \
		delete (p); \
		(p) = NULL; \
	} \

#define SAFE_DELETE_ARY(p) \
	if (p) \
	{ \
	delete[] (p); \
	(p) = NULL; \
	} \

#define SAFE_RELEASE(p) \
	if (NULL != p) \
	{ \
		p->Release(); \
		p = NULL; \
	} \

#define COMP2VAL(v1, v2, sgn) \
	v1 sgn v2 ? v1 : v2; \

#define USE_LUA extern lua_State* L_stat;

#define OPEN_LUA \
	L_stat = lua_open(); \
	luaopen_base(L_stat); \
	luaopen_table(L_stat); \
	luaopen_io(L_stat); \
	luaopen_string(L_stat); \
	luaopen_math(L_stat); \

#define CLOSE_LUA lua_close(L_stat);

// 定义输出到lua的类的创建函数
#define DECLEAR_CREATE_CLASS(tClass)\
	static int new##tClass(lua_State *L) \


#define CREATE_CLASS(tClass)\
	static int new##tClass(lua_State *L) \
{ \
	tClass* pT = new tClass; \
	lua_pushnumber(L, (DWORD)pT); \
	return 1; \
} \

#define REGIST_CREATE(tName, tClass) \
	lua_register(L_stat, tName, new##tClass); \

// 定义输出到lua的类的删除函数
#define DECLEAR_DELETE_CLASS(tClass) \
	static int del##tClass(lua_State* L) \


#define DELETE_CLASS(tClass) \
	static int del##tClass(lua_State* L) \
{ \
	DWORD dwCl = (DWORD)lua_tonumber(L, 1); \
	if (dwCl) \
	delete ((tClass*)dwCl); \
	return 0; \
} \

#define REGIST_DELETE(tName, tClass) \
	lua_register(L_stat, tName, del##tClass); \

// 定义输出到lua的类的成员函数
#define DECLEAR_DEFINE_CLASS_FUNC(tClass, tFunc) \
	static int tClass##tFunc(lua_State* L) \


#define BEGIN_DEFINE_CLASS_FUNC(tClass, tFunc) \
	static int tClass##tFunc(lua_State* L) \
{ \

#define END_DEFINE_CLASS_FUNC \
}\


#define DECLEAR_REGIST_FUNC(tClass) \
	void regist##tClass() \


#define BEGIN_REGIST_FUNC(tClass) \
	void regist##tClass() \
	{ \

#define REGIST_FUNC(tName, tClass, tFunc) \
	lua_register(L_stat, (tName), tClass##tFunc); \

#define END_REGIST_FUNC \
	} \

#define DO_REGISTER_LUA_CLASS(tClass) \
	regist##tClass(); \



// 任意类型基数排序法函数宏
// pSrc 待排序序列
// SrcType 序列类型
// SortVal 按类型中的此成员值进行排序，可为空则用序列的值进行排序
// iCount 序列长度
// pBuf 排序用的缓存，需要与序列大小类型相同
// Sombl 类型分隔符号："." 和 "->" 可为空
#define RadixSort(pSrc, SrcType, SortVal, iCount, pBuf, Sombl) \
{ \
	SrcType OffsetTemp; \
	DWORD dwOffset = ((DWORD)&(OffsetTemp##Sombl##SortVal) - (DWORD)&OffsetTemp); \
	DWORD dwBox[256]; \
	DWORD j = 0; \
	DWORD pos = 0 ; \
	DWORD temp = 0; \
	for(DWORD i=0; i<4; i++) \
	{ \
		memset(dwBox, 0, 256 * sizeof(DWORD)); \
		memcpy(pBuf, pSrc, iCount * sizeof(SrcType)); \
		for(j=0; j<iCount; j++) \
		{ \
			++dwBox[((*((DWORD*)((DWORD)(&pBuf[j])+dwOffset))) >> (8 * i)) & 0xFF]; \
		} \
		pos = 0; \
		for(j=0; j<256; j++) \
		{ \
			temp = dwBox[j]; \
			dwBox[j] = pos; \
			pos += temp; \
		} \
		for(j=0; j<iCount; j++) \
		{ \
			pSrc[dwBox[((*((DWORD*)((DWORD)(&pBuf[j])+dwOffset))) >> (8 * i)) & 0xFF]++] = pBuf[j]; \
		} \
	} \
} \

// PGE标准错误 
enum PGE_ERROR_MSG
{
	PGE_OK = 0x0777,
		PGE_FAIL
};

enum PGE_SCREEN_DIRECTION
{
	PGE_SCREEN_H = 0,	// 横屏
		PGE_SCREEN_V	// 竖屏
};
enum PGE_CLASS_ID
{
	PGE_UNDEFINE = -1,
		PGE_DEVICE,
		PGE_SURFACE,
		PGE_MAP,
		PGE_FONT,
		PGE_INPUT,
		PGE_SOUND,
		PGE_MUSIC,
		PGE_NET,
		PGE_APP,
		PGE_VKBD,
		PGE_IME,
		PGE_LOG	
};

struct PGE_PAK_IMAGE_HEAD	// 图像文件包头
{
	DWORD	dwFlag;
	DWORD	dwImageCount;
	BYTE	btWidth;
	BYTE	btHeight;
};

struct FILE_OF_SIZE		// 图像文件包头后面的文件位置表
{
	DWORD dwStart;
	DWORD dwSize;
};

// PGE图像文件头
struct PGEPicBaseHead
{
	DWORD	dwHeadFlag;			// 标志 PGEP
	BYTE	btVersion;			// 版本号	
	BYTE	bBitCount;			// 图像位数
	BYTE	bCompess;			// 是否压缩
	DWORD	dwDataOffset;		// 图像数据偏移
	DWORD	dwDataSize;			// 图像数据大小
	DWORD	dwDeComSize;		// 解压后的大小
	WORD	wWidth;				// 图像宽
	WORD	wHeight;			// 图像高
	WORD	dwPalOffset;		// 调色板
	WORD	dwPalSize;			// 调色板大小
	WORD	wPalPiexlCount;		// 调色板颜色数
	DWORD	dwAlphaOffset;		// alpha通道
	DWORD	dwAlphaSize;		// alpha大小
	DWORD	dwAlphaDeComSize;	// alpha解压后的大小
	DWORD	dwResave;			// 保留
};

struct PGEPixe16
{
	WORD wPixeColor;
	BYTE btPixeCInfo;
};

class CPGEBase
{
public:
	CPGEBase();
	~CPGEBase();

	DWORD m_dwClassID;
	
};

int itoa(WORD* caOut, DWORD dwNum);
//char* TCharToChar(TCHAR* tcData, int iSize);
//TCHAR* ChartoTChar(char* caData);
// RECT操作函数
RECT FiRect(int iLeft, int iTop, int iRight, int iBottom);
RECT AdRect(RECT r1, RECT r2);
RECT SubRect(RECT r1, RECT r2);
RECT MuRect(RECT r1, RECT r2);
RECT MuRect(RECT r1, float fValue);	
BOOL PointInRect(POINT pt, RECT r);
BOOL PointInRect(int x, int y, RECT r);	
BOOL RectEqZero(RECT r);

#define RectEqRect(r1, r2) (r1.left == r2.left && r1.top == r2.top && r1.right == r2.right && r1.bottom == r2.bottom) 

RECT RectAndRect(RECT r1, RECT r2);
RECT FiRectWithStr(char* caStr);	
RECT TrunRect(RECT& r, const int iValue);
char* GetResFile(char* caFileName);
FILE* PGEFileOpen(char* fileName, char* tp);

#define FILLRECT(tRect, lLeft, lTop, lRight, lBottom) \
	tRect.left = lLeft; \
	tRect.top = lTop; \
	tRect.right = lRight; \
	tRect.bottom = lBottom;

#endif
