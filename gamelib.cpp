#include "common.h"
#include "gamelib.h"
#include <time.h>
#include "SDL.h"
#include <map>
using namespace std;
//int ass = 0;

#define tostring(l, n) ((char*)lua_tostring((l),(n)))
#define C24toC16(color) RGBto16bit565((((color)&0xff0000)>>16), (((color)&0xff00)>>8), ((color)&0xff))

extern SDL_Surface* theMainSurf; // 这个你可以看成sdlpal中的gpScreen,而filp()就是向gpScreenReal上绘制
SDL_Surface* g_backupSuf; // 这个用来实现淡入淡出

#define MMAXX 480
#define MMAXY 480
#define SUBSX 64
#define SUBSY 64

typedef map<DWORD, SDL_Surface*> picmap;
#define PICMAP(j) (*((j)->piccache))


PALINPUTSTATE            g_InputState;
struct JyFile
{
	JyFile(FILE* _f = 0, DWORD* _d = 0)
		: fp(_f), index(_d), piccache(0){}
	FILE* fp;
	DWORD* index;
	picmap* piccache;
	int picount;
	string strfile;
};

struct ScreenGrid
{
	ScreenGrid(RECT& r, WPOINT& pt)
		: scrCoo(r), mapCoo(pt){}
	ScreenGrid(){}
	RECT scrCoo;
	WPOINT mapCoo;
};

struct JyData
{
	WORD pal[256];
	WORD EarthData[MMAXY][MMAXX]; // 主地图地面贴图编号数据
	WORD SurfaceData[MMAXY][MMAXX]; // 主地图地面表面贴图编号数据
	WORD BuildingData[MMAXY][MMAXX]; // 主地图建筑贴图编号数据
	WORD BuildxData[MMAXY][MMAXX]; // 主地图建筑占用坐标x数据
	WORD BuildyData[MMAXY][MMAXX]; // 主地图建筑占用坐标y数据

	WORD tmpBuildX[2000];
	WORD tmpBuildY[2000];
	WORD tmpBuild[2000];
	WORD numTmpBuild;

	JyFile filecache[10];
	RECT rclip;
	int curX, curY;
	int screenW, screenH;
	map<int, BYTE*> mapByte;
	wchar_t wcrBuf[2048];
	char mcrBuf[2048];
	BYTE loadBuf[1024*512];
	BYTE* Sdata;
	BYTE* Ddata;
	char* StrBuf[2048];
	map<DWORD, ScreenGrid> mapScreenGrid;

	BYTE *WMap0, *WMap1, *WMap2, *WMap3, *WMap4; 
};

JyData* g_pdata = 0;
//JyData g_data;
#define g_data (*g_pdata)

struct JyRleHead
{
	WORD w,h,x,y;
};

static int L_Debug(lua_State* L)//(str)
{
	char* str = tostring(L, 1);
	//lua_pop(L, 1);
	if (str)
		PutDebugString(str);
	return 0;
}

// 用到的按键表，你可以自己替换成相应的键值
JYKey mpkey[]=
{
	{0,0},
	{0x1B,0},
	{0x59,0},
	{0x4e,0},
	{0x20,0},
	{0x0d,0},
	{0x26,0},
	{0x28,0},
	{0x25,0},
	{0x27,0},
	{0x70,0},
	{193,0},
	{194,0}
};

static int L_GetKey(lua_State* L)//()
{
	//if (CPGEMouse::Instance()->lBtn)
	//{
	//	CPGEMouse::Instance()->lBtn = 0;
	//	lua_pushnumber(L, 0x12345);
	//	return 1;
	//}
	//else
	{
		for (int i=1; i<13; ++i)
		{
			if (g_InputState.dwKeyPress == mpkey[i].key) // 检侧按键状态是不是按下去了
			{
				lua_pushnumber(L, mpkey[i].key);
				return 1;
			}
		}
	}

	lua_pushnumber(L, -1);
	return 1;
}

static int L_GetTime(lua_State* L)//
{
	lua_pushnumber(L, time(0));
	return 1;
}

static int L_Delay(lua_State* L)//(t)
{	
	int tm = lua_tonumber(L, 1);
	//lua_pop(L, 1);
	if (tm)
		SDL_Delay(tm);
	return 0;
}

/*
lib.CharSet(str,flag)
返回把str转换后的字符串

flag=0   Big5 ---> GBK   
=1   GBK  ---> Big5
GBK → MultiByteToWideChar(CodePage=936) → WideCharToMultiByte(CodePage=950) → BIG5 
BIG5 → MultiByteToWideChar(CodePage=950) → WideCharToMultiByte(CodePage=936) → GBK 

*/

static int L_CharSet(lua_State* L)//(str,flag)
{
	char* str = tostring(L,1);
	int f = lua_tonumber(L,2);
	int n = strlen(str);
	if (f) // gbk2big5
	{
		n = MultiByteToWideChar(936, 0, str, n, g_data.wcrBuf, 2048); 
		n = WideCharToMultiByte(950, 0, g_data.wcrBuf, n, g_data.mcrBuf, 2048, 0, 0);
		g_data.mcrBuf[n]=0;

	}
	else // big52gb
	{
		n = MultiByteToWideChar(950, 0, str, n, g_data.wcrBuf, 2048);
		n = WideCharToMultiByte(936, 0, g_data.wcrBuf, n, g_data.mcrBuf, 2048, 0, 0);
		g_data.mcrBuf[n] = 0;
	}
	lua_pushstring(L, g_data.mcrBuf);
	return 1;
}

/*
lib.SetClip(x1,y1,x2,y2)
设置裁剪窗口。设置以后所有对表面的绘图操作都只影响(x1,y1)-(x2,y2)的矩形框内部。
如果x1,y1,x2,y2均为0，则裁剪窗口为整个表面。
*/
static int L_SetClip(lua_State* L)//(x1,y1,x2,y2)
{
	int x1 = lua_tonumber(L, 1);
	int y1 = lua_tonumber(L, 2);
	int x2 = lua_tonumber(L, 3);
	int y2 = lua_tonumber(L, 4);
	if (x1==0 && x2==0 && y1==0 && y2==0)
		x2 = g_data.screenW, y2=g_data.screenH;
	g_data.rclip = RECT(x1,y1,x2,y2);
	return 0;
}

static int L_FillColor(lua_State* L)//(x1,y1,x2,y2,color)
{
	int x1 = lua_tonumber(L, 1);
	int y1 = lua_tonumber(L, 2);
	int x2 = lua_tonumber(L, 3);
	int y2 = lua_tonumber(L, 4);
	int color = lua_tonumber(L, 5);
	if (x1==0 && x2==0 && y1==0 && y2==0)
		x2 = g_data.screenW, y2=g_data.screenH;

	if (x1<g_data.rclip.left) x1 = g_data.rclip.left;
	if (y1<g_data.rclip.top) y1 = g_data.rclip.top;
	if (x2>g_data.rclip.right) x2 = g_data.rclip.right;
	if (y2>g_data.rclip.bottom) y2 = g_data.rclip.bottom;

	theMainSurf->DrawRect(x1,y1,x2,y2, C24toC16(color), 1); // 我用的CPGESuface是我自己写的ppc上的游戏库，这个没办法用在s60上，还好用到的功能很少，可以用sdl的surface替代

	return 0;
}

static int L_Background(lua_State* L)//(x1,y1,x2,y2,Bright)
{
	int x1 = lua_tonumber(L, 1);
	int y1 = lua_tonumber(L, 2);
	int x2 = lua_tonumber(L, 3);
	int y2 = lua_tonumber(L, 4);
	int b = lua_tonumber(L, 5);
	//int tmp = 0;

	//WORD* p = 0;
	//theMainSurf->Lock((void**)&p);
	//int w = theMainSurf->GetSurfaceInfo()->wWiedth;
	//int x,y, ofset = y1*w;
	//
	//for (y=y1; y<=y2; ++y)
	//{
	//	for (x=x1; x<=x2; ++x)
	//	{
	//		tmp = *(p+ofset+x) * ((1+b)>>8);
	//		*(p+ofset+x) = tmp >> 16;
	//	}
	//	ofset+=w;
	//}
	return 0;
}

static int L_DrawRect(lua_State* L)//x1,y1,x2,y2,color)
{
	int x1 = lua_tonumber(L, 1);
	int y1 = lua_tonumber(L, 2);
	int x2 = lua_tonumber(L, 3);
	int y2 = lua_tonumber(L, 4);
	int color = lua_tonumber(L, 5);

	if (x1<g_data.rclip.left) x1 = g_data.rclip.left;
	if (y1<g_data.rclip.top) y1 = g_data.rclip.top;
	if (x2>g_data.rclip.right) x2 = g_data.rclip.right;
	if (y2>g_data.rclip.bottom) y2 = g_data.rclip.bottom;

	theMainSurf->DrawRect(x1,y1,x2,y2, C24toC16(color));

	return 0;
}

static int L_DrawStr(lua_State* L)//x,y,str,color,size,fontname,charset,OScharset)
{
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	char* str = tostring(L, 3);
	int color = lua_tonumber(L, 4);
	int fntsize = lua_tonumber(L, 5);
	char* fontname = tostring(L, 6);
	int charset = lua_tonumber(L, 7);
	int osset = lua_tonumber(L, 8);

	if (str)
		theMainSurf->DrawText(str, x, y, C24toC16(color));

	return 0;
}

static int L_ShowSurface(lua_State* L)//)
{
	// 这个是把theMainSuf上的内容翻转到屏幕上
	theApp->GetDevice()->Flip();
	//Sleep(10);
	return 0;
}

/*
t 为亮度没变化一次的间隔毫秒数。为了16/32位兼容，一共有32阶亮度变化。
flag: 0 从暗到亮，1 从亮到暗
*/

#define W2B(n, v) RGBto16bit565(GetRed(n)/v, GetGreen(n)/v, GetBlue(n)/v)
#define B2W(n, v) RGBto16bit565(GetRed(n)*v, GetGreen(n)*v, GetBlue(n)*v)

static int L_ShowSlow(lua_State* L)//t,flag)
{
	int t = lua_tonumber(L,1);
	int f = lua_tonumber(L,2);
	g_backupSuf->Blt(theMainSurf, 0, 0);

	WORD *psrc, *pdes;
	g_backupSuf->Lock((void**)&psrc);
	theMainSurf->Lock((void**)&pdes);
	DWORD sz = theMainSurf->GetDataSize();

	int r,g,b;
	for (int i=1; i<33; ++i)
	{
		if (f)
		{
			for (int j=0; j<sz; ++j)
				*(pdes+j) = W2B((*(psrc+j)), i);
		}
		else
		{
			for (int j=0; j<sz; ++j)
			{
				r = GetRed(*(psrc+j)) * i;
				g = GetGreen(*(psrc+j)) * i;
				b = GetBlue(*(psrc+j)) * i;

				*(pdes+j) = RGBto16bit565(r>>5,g>>5,b>>5);
			}
		}
		theApp->GetDevice()->Flip();
		Sleep(t>>1);
	}
	return 0;
}

static int L_PicInit(lua_State* L)//str)
{
	char* szfile = tostring(L, 1);
	//lua_pop(L, 1);

	if (szfile)
	{
		FILE* fp = fopen(GetResFile(szfile), "rb");
		BYTE rgb[3];
		if (fp)
		{
			for (int i=0; i<256; ++i)
			{
				fread(rgb, 1, 3, fp);
				g_data.pal[i] = RGBto16bit565(rgb[0]<<2, rgb[1]<<2, rgb[2]<<2);
			}

			fclose(fp);
		}
	}

	return 0;
}

static int L_PicLoadFile(lua_State* L)//filename,id)
{

	char* szfile = tostring(L,1);
	int id = lua_tonumber(L,2);
	JyFile* j = g_data.filecache+id;
	if(j->fp) 
	{
		fclose(j->fp);
		SAFE_DELETE_ARY(j->index);
		if (j->piccache)
		{
			for(picmap::iterator it = j->piccache->begin();
				it != j->piccache->end(); )
			{
				CPGESurface* p = it->second;
				if (p)
				{
					ass -= p->GetDataSize()*2;
				}
				SAFE_DELETE(p);
				it = j->piccache->erase(it);
			}
			j->piccache->clear();
			SAFE_DELETE(j->piccache);
		}

	}

	char buf[256];
	sprintf(buf, "%sResource\\%s.grp", GetAppPath(), szfile);
	j->fp = fopen(buf, "rb");

	sprintf(buf, "%sResource\\%s.idx", GetAppPath(), szfile);
	int l = GetFileLength(buf);

	j->picount = l/4;
	j->index = new DWORD[j->picount];

	j->strfile = buf;
	FILE* fp = fopen(buf, "rb");
	fread(j->index, 1, l, fp);
	fclose(fp);

	return 0;
}


void DecodeRle(CPGESurface** psuf, BYTE* prle, int len)
{
	JyRleHead* hd = (JyRleHead*)prle;
	BYTE* rledata = prle+8;
	len -= 8;

	(*psuf) = new CPGESurface;
	(*psuf)->CreateSurface(hd->w, hd->h, PGE_NOMARL_SURFACE, PGE_PURPLE);
	(*psuf)->GetSurfaceInfo()->ptCentPoint = WPOINT(hd->x, hd->y);

	ass += (*psuf)->GetDataSize()*2;
	WORD* p = 0;
	(*psuf)->Lock((void**)&p);

	BYTE* pn = 0;
	int i, row, col, tco, rco, l;
	for(col=0; col<hd->h; ++col)
	{	
		l = 0;
		row = *(rledata++);
		pn = rledata+row;
		while(row)
		{
			tco = *(rledata++);
			rco = *(rledata++);

			for(i=0; i<tco; ++i)
				*(p++) = PGE_PURPLE;

			for (i=0; i<rco; ++i)
				*(p++) = g_data.pal[*(rledata+i)];

			rledata+=rco;
			row -= (rco+2);
			l += (tco+rco);
		}

		l = hd->w - l;
		if (l)
		{
			for(i=0; i<l; ++i)
				*(p++) = PGE_PURPLE;
		}

		rledata = pn;
	}
}


/*
lib.PicLoadCache(id,picid,x,y,flag,value)
加载id所指示的贴图文件中编号为picid/2(为保持兼容，这里依然除2)的贴图到表面的(x,y)坐标。
*/

BYTE pngflag[] = {0x89,0x50,0x4e,0x47,0xd,0xa,0x1a,0xa};

void DrawResPic(int id, int picid, int x, int y, int flag, int value)
{
	JyFile* j = g_data.filecache+id;

	if (picid > j->picount) return;

	int s = picid > 0 ?  j->index[picid-1] : 0;
	int e = j->index[picid];

	if (!j->piccache)
	{
		j->piccache = new picmap;
	}

	CPGESurface*& psuf = PICMAP(j)[picid];
	if (!psuf)
	{
		fseek(j->fp, s, SEEK_SET);
		fread(g_data.loadBuf, 1, e-s, j->fp);
		if (memcmp(g_data.loadBuf, pngflag, 8) != 0)
		{
			DecodeRle(&psuf, g_data.loadBuf, e-s);
		}
		else
		{
			return;
		}

	}

	int sw = psuf->GetSurfaceInfo()->wWiedth;
	int sh = psuf->GetSurfaceInfo()->wHeigh;
	RECT rdes(x,y,x+sw, y+sh);
	RECT rsrc(0,0,sw,sh);

	if (!(flag & 1))
		rdes = rdes - psuf->GetSurfaceInfo()->ptCentPoint;

	if (rdes.right<0 || rdes.left>g_data.screenW || rdes.bottom<0 || rdes.top>g_data.screenH) return;

	if (rdes.left < 0) rsrc.left = -rdes.left,rdes.left=0;
	if (rdes.top < 0) rsrc.top = -rdes.top,rdes.top=0;
	if (rdes.bottom>g_data.screenH) rsrc.bottom -= (rdes.bottom-g_data.screenH), rdes.bottom=g_data.screenH;
	if (rdes.right>g_data.screenW) rsrc.right -= (rdes.right-g_data.screenW), rdes.right = g_data.screenW;

	psuf->AddEdge(PGE_RED);
	theMainSurf->Blt(psuf, &rsrc, &rdes, PGE_BLT_NORMAL | PGE_BLT_MASK);

}

static int L_PicLoadCache(lua_State* L)//id,picid,x,y,flag,value)
{
	int id = lua_tonumber(L,1);
	int picid = lua_tonumber(L,2)/2;
	int x = (lua_tonumber(L,3));
	int y = (lua_tonumber(L,4));
	int flag = lua_tonumber(L,5);
	int value = lua_tonumber(L,6);

	if (id == 1)
	{
		int kkk = 0;
	}
	DrawResPic(id, picid, x, y, flag, value);

	return 0;
}

static int L_PlayMIDI(lua_State* L)//filename) 
{
	char* szfile = tostring(L,1);
	//SoundPlayer.play(GetResFile(szfile`));
	return 0;
}

static int L_PlayWAV(lua_State* L)//filename) 
{
	char* szfile = tostring(L,1);
	SoundPlayer.play(GetResFile(szfile),1);
	return 0;
}

/*
lib.LoadMMap(filename1,filename2,filename3,filename4,filename5,xmax,ymax)
加载主地图的5个结构文件*.002。
贴图文件依次为earth,surface,building,buildx,buildy
xmax,ymax为主地图宽，高。目前均为g_data.screenH
*/
static int L_LoadMMap(lua_State* L)//filename1,filename2,filename3,filename4,filename5,xmax,ymax)
{
	char* f1 = tostring(L,1);
	char* f2 = tostring(L,2);
	char* f3 = tostring(L,3);
	char* f4 = tostring(L,4);
	char* f5 = tostring(L,5);
	int x = lua_tonumber(L,6);
	int y = lua_tonumber(L,7);

	int size = MMAXY*MMAXX*2;
	FILE* fp = fopen(GetResFile(f1), "rb");
	fread(g_data.EarthData, 1, size, fp);
	fclose(fp);

	fp = fopen(GetResFile(f2), "rb");
	fread(g_data.SurfaceData, 1, size, fp);
	fclose(fp);

	fp = fopen(GetResFile(f3), "rb");
	fread(g_data.BuildingData, 1, size, fp);
	fclose(fp);

	fp = fopen(GetResFile(f4), "rb");
	fread(g_data.BuildxData, 1, size, fp);
	fclose(fp);

	fp = fopen(GetResFile(f5), "rb");
	fread(g_data.BuildyData, 1, size, fp);
	fclose(fp);

	return 0;
}
/*
flag=0 earth, 1 surface, 2 building, 3 buildx, 4 buildy
*/
static int L_GetMMap(lua_State* L)//x,y,flag)
{
	int x = lua_tonumber(L,1);
	int y = lua_tonumber(L,2);
	int flag = lua_tonumber(L,3);

	switch (flag)
	{
	case 0:
		lua_pushnumber(L, g_data.EarthData[y][x]);
		break;

	case 1:
		lua_pushnumber(L, g_data.SurfaceData[y][x]);
		break;

	case 2:
		lua_pushnumber(L, g_data.BuildingData[y][x]);
		break;

	case 3:
		lua_pushnumber(L, g_data.BuildxData[y][x]);
		break;

	case 4:
		lua_pushnumber(L, g_data.BuildyData[y][x]);
		break;

	default:
		lua_pushnumber(L, -1);
		break;
	}

	return 1;
}

void RangeValue(int& v, int xmin, int xmax)
{
	if (v<xmin) v=xmin;
	else if (v>xmax) v=xmax;
}

void SortBuild(int xx, int yy)
{

	int RangeX = 21, rangeY = 31,
		i =0, j =0, k =0, m =0, n =0,
		p =0, repeat =0, dy =0,
		bakx =0, baky =0, bak =0,
		tmpx =0, tmpy =0, tmp =0,
		Xmin =0, Xmax =0, ymin =0, ymax=0;

	Xmin = xx - RangeX - 6;
	RangeValue( Xmin, 1, MMAXX - 1);
	Xmax = xx + RangeX + 8;
	RangeValue( Xmax, 1, MMAXX - 1);
	ymin = yy - rangeY;
	RangeValue (ymin, 1, MMAXY - 1);
	ymax = yy + rangeY + 8;
	RangeValue( ymax, 1, MMAXY - 1);

	for (i = Xmin; i<= Xmax;++i)
	{
		dy = ymin;
		for(j = ymin ;j<= ymax;++j)
		{
			if( g_data.BuildxData[j][i] !=0  && g_data.BuildyData[j][i] !=0)
			{
				repeat = 0;
				for (k = 0 ; k<= p - 1; ++k)
				{
					if ((g_data.tmpBuildX[k] == g_data.BuildxData[j][i]) 
						&& (g_data.tmpBuildY[k] == g_data.BuildyData[j][i]))
					{
						repeat = 1;
						if (k == p - 1)
						{
							break;
						}
						for (m = j - 1; m>= dy;--m)
						{
							if( g_data.BuildxData[m][i] != 0 || g_data.BuildyData[m][i] != 0)
							{
								if( (g_data.BuildxData[m][i] != g_data.BuildxData[j][i]) 
									|| (g_data.BuildyData[m][i] != g_data.BuildyData[j][i]))
								{
									if ( (g_data.BuildxData[m][i] != g_data.tmpBuildX[k])
										|| (g_data.BuildyData[m][i] != g_data.tmpBuildY[k]) )
									{
										tmpx = g_data.tmpBuildX[p - 1];
										tmpy = g_data.tmpBuildY[p - 1];
										tmp = g_data.tmpBuild[p - 1];

										memcpy (g_data.tmpBuildX+k + 1, g_data.tmpBuildX+k, 
											(p - 2 - k + 1) << 2);

										memcpy ( g_data.tmpBuildY+k + 1, g_data.tmpBuildY+k, 
											(p - 2 - k + 1) << 2);

										memcpy ( g_data.tmpBuild+k + 1, g_data.tmpBuild+k, 
											(p - 2 - k + 1) << 2);

										g_data.tmpBuildX[k] = tmpx;
										g_data.tmpBuildY[k] = tmpy;
										g_data.tmpBuild[k] = tmp;
									}
								}
							}
						}
						dy = j + 1;
						break;
					}
				}
				if (repeat == 0)
				{
					g_data.tmpBuildX[p] = g_data.BuildxData[j][i];
					g_data.tmpBuildY[p] = g_data.BuildyData[j][i];
					g_data.tmpBuild[p] = 
						g_data.BuildingData[g_data.BuildyData[j][i]][g_data.BuildxData[j][i]];
					p = p + 1;
				}
			}
		}
	}
	g_data.numTmpBuild = p;

	int XSCALE = 18, YSCALE=9;
	for (i=0; i<p; ++i)
	{
		m = g_data.tmpBuildX[i] - xx;
		n = g_data.tmpBuildY[i] - yy;
		g_data.tmpBuildX[i] = XSCALE * (m - n) + g_data.screenW / 2;
		g_data.tmpBuildY[i] = YSCALE * (m + n) + g_data.screenH / 2;
	}

	for (i=0; i<p-1; ++i)
	{

		for (j=i+1; j<p; ++j)
		{
			if (g_data.tmpBuildY[i]>g_data.tmpBuildY[j])
			{
				tmp = g_data.tmpBuildY[i];
				g_data.tmpBuildY[i] = g_data.tmpBuildY[j];
				g_data.tmpBuildY[j] = tmp;

				tmp = g_data.tmpBuildX[i];
				g_data.tmpBuildX[i] = g_data.tmpBuildX[j];
				g_data.tmpBuildX[j] = tmp;

				tmp = g_data.tmpBuild[i];
				g_data.tmpBuild[i] = g_data.tmpBuild[j];
				g_data.tmpBuild[j] = tmp;
			}
		}
	}

}

static int L_DrawSMap(lua_State* L)
{
	int xoff = lua_tonumber(L, 1);
	int yoff = lua_tonumber(L, 2);
	int bx = lua_tonumber(L, 3);
	int by = lua_tonumber(L, 4);
	//int tmpS = lua_tonumber(L,5);
	int subScene = lua_tonumber(L,5);
	//int Ddata = lua_tonumber(L, 7);
	int mypic = lua_tonumber(L,6);

	// PutDebugString("1:%d, 2:%d, 3:%d, 4:%d, 5:5d, 6:%d, 7:%d, 8:%d", xoff, yoff,bx,by,tmpS,subScene,Ddata,mypic);
	int XSCALE = 18, YSCALE=9;
	int rangeX=18+7;
	int rangeY=10+7;
	int DNum = 200;

	int i,j,i1,j1;
	int  x1,y1;
	int picNum;
	int xx,yy;
	int j_2;

	int dx = xoff + bx;
	int dy = yoff + by;
	int dd = SUBSX*SUBSY*2;

	BYTE* p = g_data.Sdata;
	BYTE* pd = g_data.Ddata;
	for (j=-rangeY; j<=2*rangeX+rangeY; ++j)
	{
		// PutDebugString("j:%d",j);
		j_2=j/2;
		// PutDebugString("j_2:%d",j_2);
		int i_add=-rangeX+j_2;
		// PutDebugString("i_add:%d",i_add);

		for (i=rangeX; i>= 0; --i)// -1 do
		{
			if ( 2*j_2==j )
			{
				i1=i_add+i;
				j1=-i+j_2;
			}
			else
			{
				i1=i_add+i;
				j1=-i+j_2+1;
			}

			// PutDebugString("j1:%d",j1);
			x1=XSCALE*(i1-j1)+g_data.screenW/2;
			y1=YSCALE*(i1+j1)+g_data.screenH/2;

			// PutDebugString("x1:%d",x1);
			// PutDebugString("dx:%d",dx);
			// PutDebugString("dy:%d",dy);
			xx=dx+i1;
			yy=dy+j1;

			if (xx==1 && yy==0)
			{
				int kkk = 0;
			}

			if (yy >=0 && xx>=0 && xx <SUBSX && yy<SUBSY)// then
			{
				// PutDebugString("xx:%d, yy:%d",xx,yy);
				int start=dd*(6*subScene)+(yy*SUBSX+xx)*2;
				// PutDebugString("start:%d", start);
				short d0=arByte2Int(p+start, 2);
				// PutDebugString("d0:%d",d0);
				short d1=arByte2Int(p+start+dd, 2);
				// PutDebugString("d1:%d", d1);
				short d2=arByte2Int(p+start+2*dd, 2);
				// PutDebugString("d2:%d",d2);
				short d3=arByte2Int(p+start+3*dd, 2 );
				// PutDebugString("d3:%d",d3);
				short d4=arByte2Int(p+start+4*dd, 2);
				// PutDebugString("d4:%d",d4);
				short d5=arByte2Int(p+start+5*dd, 2);
				// PutDebugString("d5:%d",d5);

				if (d0>0)
				{
					if ((x1-18)<g_data.screenW && (y1-9)<g_data.screenH && (x1+18)>0 && (y1+9)>0)
					{
						g_data.mapScreenGrid[((x1<<16)|(y1))] = ScreenGrid(RECT(x1, y1, x1+36, y1+18), WPOINT(xx, yy));
					}
					DrawResPic(0,d0/2,x1,y1,0,0);    // --地面
				}
				if (d1>0)
				{
					DrawResPic(0,d1/2,x1,y1-d4,0,0);    //--建筑
				}
				if (d2>0)
				{
					DrawResPic(0,d2/2,x1,y1-d5,0,0);    //--空中
				}
				if (d3>=0)
				{
					short picnum = arByte2Int(pd+(200*11*subScene+11*d3+7)*2, 2);
					if (picnum>0)
					{
						DrawResPic(0,picnum/2,x1,y1-d4,0,0);   //--事件
					}
				}
				if (i1==-xoff && j1==-yoff)             //--绘主角贴图
				{
					DrawResPic(0,mypic,x1,y1-d4,0,0);
				}
			}
		}
	}
	return 0;
}

static int L_DrawMMap(lua_State* L)//x,y,mypic)
{
	int xx = lua_tonumber(L,1);
	int yy = lua_tonumber(L,2);
	int mypic = lua_tonumber(L,3);

	static int ox,oy,opic;

	if (ox!=xx || oy != yy || opic != mypic)
	{
		SortBuild(xx, yy);
		int RangeX = 18 + 11;
		int rangeY = 10 + 11;

		int j=0, i=0, j1=0, i1=0, picnum=0, x1=0, y1=0;
		int XSCALE = 18, YSCALE=9;

		for (j = -rangeY ; j<=2 * RangeX + rangeY;++j)
		{
			for (i = RangeX; i >= 0; --i)
			{
				if( (j & 1) == 0)
				{
					i1 = -RangeX + i + (j >> 1);
					j1 = -i + (j >> 1);
				}
				else
				{
					i1 = -RangeX + i + (j >> 1);
					j1 = -i + (j >> 1) + 1;
				}

				x1 = XSCALE * (i1 - j1) + g_data.screenW / 2;
				y1 = YSCALE * (i1 + j1) + g_data.screenH / 2;

				if( yy + j1 >= 0 && xx + i1 >= 0 && yy + j1 < MMAXY && xx + i1 < MMAXX)
				{
					picnum = g_data.EarthData[ yy + j1][xx + i1] / 2;

					if (picnum>0 && (x1-18)<g_data.screenW && (y1-9)<g_data.screenH && (x1+18)>0 && (y1+9)>0)
					{
						g_data.mapScreenGrid[((x1<<16)|(y1))] = ScreenGrid(RECT(x1, y1, x1+36, y1+18), WPOINT(xx+i1, yy+j1));
						DrawResPic(0, picnum,x1, y1,0,0);
					}

					picnum = g_data.SurfaceData[yy + j1][xx + i1] / 2;

					if (picnum>0)
						DrawResPic(0, picnum,x1, y1,0,0);

				}
			}
		}

		int donce = 0;
		for (i = 0; i<=g_data.numTmpBuild - 1; ++i)
		{
			i1 = g_data.tmpBuildX[i];
			j1 = g_data.tmpBuildY[i];
			picnum = g_data.tmpBuild[i] / 2;

			if (j1>g_data.screenH/2+32 && !donce)
			{
				DrawResPic(0, mypic, g_data.screenW/2, g_data.screenH/2, 0, 0);
				donce = 1;
			}
			if (picnum>0)
				DrawResPic(0, picnum, i1, j1,0,0);
		}

		opic = mypic;
		ox = xx;
		oy = yy;
	}

	return 0;
}

static int L_LoadPicture(lua_State* L)
{
	char* szpic = tostring(L, 1);
	int x = lua_tonumber(L, 2);
	int y = lua_tonumber(L, 3);

	static string name;
	static CPGESurface* pic = 0;

	if(name != szpic)
	{

		if (!pic)
		{
			pic = new CPGESurface;
			pic->CreateSurface(320,200);
		}
		WORD* p = 0;
		pic->Lock((void**)&p);
		int sz = pic->GetDataSize();
		int fsz = GetFileLength(GetResFile(szpic));

		FILE* fp = fopen(GetResFile(szpic), "rb");
		fread(g_data.loadBuf, 1, fsz, fp);
		fclose(fp);
		for(int i=0; i<sz; ++i)
			*(p+i) = g_data.pal[*(g_data.loadBuf+i)];
	}
	RECT rdes(x, y, x+320, y+200);
	theMainSurf->Blt(pic, 0, &rdes);
	return 0;
}

static int L_SetBaseData(lua_State* L)
{
	int JYS = lua_tonumber(L, 1);
	int JYD = lua_tonumber(L, 2);
	g_data.Sdata = g_data.mapByte[JYS];
	g_data.Ddata = g_data.mapByte[JYD];
	return 0;

}

short GetD(int Sceneid, int id, int i)
{
	int start = (200*11*Sceneid+11*id+i)<<1;
	return arByte2Int(g_data.Ddata+start, 2);
}
void SetD(int Sceneid, int id, int i, short v)
{
	int start = (200*11*Sceneid+11*id+i)<<1;
	Int2arByte(g_data.Ddata+start, v, 2);
}

short GetS(int id, int x, int y, int level)
{
	int start = (SUBSX*SUBSY*(6*id+level)+y*SUBSX+x)<<1;
	return arByte2Int(g_data.Sdata+start, 2);
}

void SetS(int id, int x, int y, int level, short v)
{
	int start = (SUBSX*SUBSY*(6*id+level)+y*SUBSX+x)<<1;
	Int2arByte(g_data.Sdata+start, v, 2);
}

static int L_GetS(lua_State* L)
{
	int id = lua_tonumber(L,1);
	int x = lua_tonumber(L,2);
	int y = lua_tonumber(L,3);
	int level = lua_tonumber(L,4);
	lua_pushnumber(L, GetS(id, x, y, level));
	return 1;
}

static int L_SetS(lua_State* L)
{
	int id = lua_tonumber(L,1);
	int x = lua_tonumber(L,2);
	int y = lua_tonumber(L,3);
	int level = lua_tonumber(L,4);
	short v = lua_tonumber(L,5);
	SetS(id, x, y, level,v);
	return 0;
}

static int L_GetD(lua_State* L)
{
	int Sceneid = lua_tonumber(L,1);
	int id = lua_tonumber(L, 2);
	int i = lua_tonumber(L, 3);

	lua_pushnumber(L, GetD(Sceneid, id, i));

	return 1;
}

static int L_SetD(lua_State* L)
{
	int Sceneid = lua_tonumber(L,1);
	int id = lua_tonumber(L, 2);
	int i = lua_tonumber(L, 3);
	short v = lua_tonumber(L, 4);

	SetD(Sceneid, id, i, v);
	return 0;
}

static int L_DtoSMap(lua_State* L)
{
	int SubScene = lua_tonumber(L, 1);
	int Mytick = lua_tonumber(L, 2);
	int DNum = 200;

	for (int i=0; i<DNum; ++i)// do
	{
		short x=GetD(SubScene,i,9);
		short y=GetD(SubScene,i,10);
		if (x>0 && y>0)// then
		{
			SetS(SubScene,x,y,3,i);

			short p1=GetD(SubScene,i,5);
			if (p1>=0)// then
			{
				short p2 = GetD(SubScene,i,6);
				short p3 = GetD(SubScene,i,7);
				short delay = GetD(SubScene,i,8);
				if (p3<=p1)// then     --动画已停止
				{	
					if ((Mytick % 100) > delay)// then
					{
						p3=p3+1;
					}
				}
				else
				{
					if ((Mytick & 3) ==0)// then      --4个节拍动画增加一次
					{
						p3=p3+1;
					}
				}
				if (p3>p2)// then
				{
					p3=p1;
				}
				SetD(SubScene,i,7,p3);
			}
		}
	}
	return 0;
}

#define RGB24(r,g,b) ((((r)<<16) |((g)<<8)) | b)
#define GETR24(c) (((c) & 0xff0000) >> 16)
#define GETG24(c) (((c) & 0xff00) >> 8)
#define GETB24(c) ((c) & 0xff)

static int L_RGB(lua_State* L) 
{
	int r = lua_tonumber(L, 1);
	int g = lua_tonumber(L, 2);
	int b = lua_tonumber(L, 3);
	lua_pushnumber(L, RGB24(r,g,b));
	return 1;
}


static int L_GetRGB(lua_State* L) 
{
	int color = lua_tonumber(L,1);
	lua_pushnumber(L, GETR24(color));
	lua_pushnumber(L, GETG24(color));
	lua_pushnumber(L, GETB24(color));

	return 3;
}

static int L_subRGB(lua_State* L)
{
	int color = lua_tonumber(L,1);
	int subv = lua_tonumber(L,2);
	lua_pushnumber(L, RGB24(GETR24(color)/subv,GETG24(color)/subv, GETB24(color)/subv));
	return 1;
}

void DrawBoard(WORD color, int x, int y, int w, int h, int brd=1)
{
	// 画边框
	if (x+w<0 || y+h<0 || x>g_data.screenW || y>g_data.screenH) return;
	if (x+w>g_data.screenW) w = g_data.screenW-x;
	if (y+h>g_data.screenH) h = g_data.screenH-y;
	if (brd)
	{
		theMainSurf->DrawRect(x,y,x+w,y+h,color);
		theMainSurf->DrawRect(x+1,y+1,x+w-1,y+h-1, 
			RGBto16bit565(GetRed(color)+90, GetGreen(color)+60, GetBlue(color)+80));
		theMainSurf->DrawRect(x+2,y+2,x+w-2,y+h-2, color);

		// 画背景
		theMainSurf->DrawRect(x+3,y+3, x+w-3, y+h-3, color, 12, 1);
	}
	else
	{
		theMainSurf->DrawRect(x,y, x+w, y+h, color, 12, 1);
	}

}

void DrawStrOnBoard(char* str, int strcolor, int boardcolor, 
					int x, int y, int w, int h, int brd=1)
{
	DrawBoard(boardcolor, x, y, w, h, brd);
	theMainSurf->DrawText(str, (brd?x+4:x+1), (brd?y+4:y+1), strcolor);
}

struct TalkDesc
{
	int headx, heady, talkx, talky, showhead;
};

TalkDesc talkCoo[] =
{
	{10, 10, 80, 10, 1},            //  --显示头像和对话的坐标
	{240, 160, 30, 160, 1}, 
	{10, 10, 80, 10, 0}, 
	{240, 160, 30, 160, 0}, 
	{190, 10, 10, 10, 1}, 
	{30, 160, 130, 160, 1}
};


void OnTalkWaitKey()
{
	static DWORD n = 0, c=0;
	n+=1;
	if (n > 20)
	{
		n=0;
		c = !c;

		theMainSurf->DrawText("▼", 300,75, (c ? PGE_YELLOW : PGE_WHITE));
		RECT r(300,75, 320, 100);
		theApp->GetDevice()->Flip(&r);
	}


}

int anykeypress = 0;
int WaitAnyKey()
{
	anykeypress = 0;
	while(1)
	{
		if (anykeypress)
		{
			int x = anykeypress;
			anykeypress = 0;
			return x;
		}
		else if (CPGEMouse::Instance()->lBtn)
		{
			CPGEMouse::Instance()->lBtn = 0;
			return  CPGEKey::Instance()->vkA;
		}
		Sleep(1);
	}
}

static int L_WaitKey(lua_State* L)//()
{
	if (!mpkey[0].bD)
	{
		int k = 0;
		for (int i=1; i<13; ++i)
		{
			if (mpkey[i].bD)
			{
				k = mpkey[i].key;
			}
			mpkey[i].bD = 0;
		}
		lua_pushnumber(L, k);
		return 1;
	}
	//if (CPGEMouse::Instance()->lBtn)
	//{
	//	CPGEMouse::Instance()->lBtn = 0;
	//	lua_pushnumber(L, k);
	//	return 1;
	//}
	lua_pushnumber(L, -1);
	return 1;
}

static int L_TalkEx(lua_State* L)
{
	anykeypress = 1;
	char* str = tostring(L,1);
	int headid = lua_tonumber(L,2);
	int flag = lua_tonumber(L,3);

	TalkDesc* xy = talkCoo;

	if (flag<0 || flag>5)// then
		flag=0;

	if (xy[flag].showhead==0)// then
		headid=-1;

	int hx = xy[flag].headx,
		hy = xy[flag].heady;

	int tx = xy[flag].talkx,
		ty = xy[flag].talky;

	int bx = min(hx, tx),
		by = min(hy, ty);


	int bsh = xy[flag].showhead;

	char* tbuf = (char*)g_data.StrBuf;
	strcpy(tbuf, str);

	DWORD buf[100];
	int n = 0; // 行数
	char** spl = spilwhit(tbuf, "*", buf, 100, n);

	for(int i=0; i<strlen(str); ++i)
	{
		if (tbuf[i] == '*') tbuf[i] = ' ';
	}

	int sline=0, eline=3>n ? n : 3;


	while(1)
	{
		lua_getglobal(L, "Cls");
		if (lua_isfunction(L, -1))
		{
			lua_call(L, 0, 0);
		}
		DrawBoard(PGE_BLUE, bx, by, 280, 70, 0);

		if (bsh)
			DrawResPic(1, headid, hx+2, hy+2, 1, 0);

		int k = 0;
		for (int i=sline; i<eline; ++i)
		{
			theMainSurf->DrawText(spl[i], tx+2, ty+(k++)*21+2, PGE_WHITE);
		}


		//theMainSurf->DrawText("▼", bx+300, by+75, PGE_YELLOW);

		theApp->GetDevice()->Flip();

		WaitAnyKey();
		Sleep(200);
		if (n>eline)
		{
			sline = eline;
			eline = eline+3>n ? n : eline+3;
		}
		else
		{
			break;
		}
	}

	return 0;
}

/*
ShowMenu
menuDesc，Desc： word,callback，isvisible|...isvisible...
showNmb view counts
*/
int ShowMenu(char* menuDesc, int showNmb, int x1, int y1, int x2, int y2
			 , int isBrd, int isEsc, WORD color, int selColor)
{
	anykeypress = 1;
	char* strbuf = (char*)g_data.StrBuf;
	strcpy(strbuf, menuDesc);

	int l = strlen(strbuf)-1;
	if (strbuf[l]=='|') strbuf[l]=0;

	DWORD fmatbuf[100];
	DWORD parbuf[10];
	int n = 0;
	char** spdesc = spilwhit(strbuf, "|", fmatbuf, 100, n);
	string asfunc[20];

	if (showNmb == 0) showNmb = n;
	int sline = 0;
	int eline = n > showNmb ? showNmb : n;

	int CurSel = 0;

	int tx = x1+3;
	int ty = y1+3;
	color = C24toC16(color);
	selColor = C24toC16(selColor);

	if (y2 == 0) y2 = n*20+y1;
	RECT* rbf = new RECT[eline];
	int i;

	if (x2<0 || y2<0 || x1>g_data.screenW || y1>g_data.screenH) return 0;
	if (x1<0) x1=0;
	if (y1<0) y1=0;
	if (x2>g_data.screenW) x2 = g_data.screenW;
	if (y2>g_data.screenH) y2 = g_data.screenH;

	while(1)
	{
		klua["Cls"]();

		DrawBoard(color, x1,y1,x2-x1,y2-y1);
		int yof = 0;
		for (i=sline; i<eline; ++i)
		{
			int parct = 0;
			char** par = spilwhit(spdesc[i], ",", parbuf, 10, parct);
			int nty = ty+(yof++)*20;
			if (i == CurSel)
			{
				DrawBoard(selColor, tx, nty, x2-x1-6, 20, 0);
			}
			theMainSurf->DrawText(par[0], tx, nty, PGE_YELLOW);
			asfunc[i] = par[1];
			rbf[i-sline] = RECT(tx, nty, tx+x2-x1-6, nty+20);
		}
		RECT r(x1,y1,x2,y2);
		theApp->GetDevice()->Flip(&r);

		anykeypress = 0;
		int brk = 0;
		CPGEMouse* pmous = CPGEMouse::Instance();
		while(1)
		{
			if (anykeypress == mpkey[4].key)
			{
				--CurSel;
				if (CurSel<0) CurSel = 0;
				brk = 1;
			}	
			else if (anykeypress == mpkey[5].key)
			{
				++CurSel;
				if (CurSel>eline) CurSel = eline;
				brk = 1;
			}
			else if (anykeypress == mpkey[3].key)
			{
				if (asfunc[CurSel] != "nil")
				{
					lua_getglobal(klua.m_l, asfunc[CurSel].c_str());
					if (lua_isfunction(klua.m_l, -1))
					{
						lua_call(klua.m_l, 0, 0);
					}
					//if ((int)klua[(char*)asfunc[CurSel].c_str()]() == 1)
					//{
					//	return -(CurSel+1);
					//}
				}
				else
				{
					return CurSel+1;
				}
				brk = 1;

			}
			else if (anykeypress != 0)
			{
				brk = 1;
			}

			if (pmous->lBtn)
			{
				for (i=0; i<n; ++i)
				{
					if (rbf[i].inThisRect(WPOINT(pmous->ix, pmous->iy)))
					{
						if (CurSel == i)
						{
							if (asfunc[CurSel] != "nil")
							{
								lua_getglobal(klua.m_l, asfunc[CurSel].c_str());
								if (lua_isfunction(klua.m_l, -1))
								{
									lua_call(klua.m_l, 0, 0);
								}
								//if ((int)klua[(char*)asfunc[CurSel].c_str()]() == 1)
								//{
								//	return -(i+1);
								//}
							}
							else
							{
								return i+1;
							}
							brk = 1;
						}
						else
						{
							CurSel = i;
							brk = 1;
							break;
						}
					}
				}
				pmous->lBtn = 0;
			}
			if (brk) break;
			Sleep(10);
		}

	}

	delete rbf;
	return 0;

}

static int L_ShowMenu(lua_State* L)
{
	char* menuDesc = tostring(L,1);
	int showNmb = lua_tonumber(L,2);
	int x1 = lua_tonumber(L,3);
	int y1 = lua_tonumber(L,4);
	int x2 = lua_tonumber(L,5);
	int y2 = lua_tonumber(L,6);
	int isBrd = lua_tonumber(L,7);
	int isEsc = lua_tonumber(L,8);
	WORD color = lua_tonumber(L,9);
	int selColor = lua_tonumber(L,10);

	int v = ShowMenu(menuDesc, showNmb, x1, y1, x2, y2
		, isBrd, isEsc, color, selColor);

	lua_pushnumber(L, v);
	return 1;

}

static int L_GetCurSorPos(lua_State* L)
{
	int xx = lua_tonumber(L,1);
	int yy = lua_tonumber(L,2);
	int x = CPGEMouse::Instance()->ix;
	int y = CPGEMouse::Instance()->iy;

	for (map<DWORD, ScreenGrid>::iterator it = g_data.mapScreenGrid.begin();
		it != g_data.mapScreenGrid.end(); ++it)
	{
		if (it->second.scrCoo.inThisRect(WPOINT(x,y)))
		{
			x = it->second.mapCoo.wx;
			y = it->second.mapCoo.wy;
			break;
		}
	}

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int L_WarLoadMap(lua_State* L)
{
	int mapid = lua_tonumber(L, 1);

	char* idxfile="data\\warfld.idx";
	char* grpfile="data\\warfld.grp";

	static int index = 0;

	int* p = 0;
	FILE* fp = 0;
	if (!index)
	{
		int idxnum = GetFileLength(GetResFile(idxfile));

		p = new int[idxnum/4];
		index = g_data.mapByte.size()+1;
		g_data.mapByte[index] = (BYTE*)p;

		fp = fopen(GetResFile(idxfile), "rb");
		if (fp)
		{
			fread(p, 1, idxnum, fp);
			fclose(fp);
		}

	}
	else
	{
		p = (int*)g_data.mapByte[index];
	}

	int i = g_data.mapByte.size();

	int ll=64*64*2;

	fp = fopen(GetResFile(grpfile), "rb");
	BYTE* pb = new BYTE[ll];
	int Map0 = ++i;
	g_data.mapByte[i] = pb;
	fseek(fp, p[mapid], SEEK_SET);
	fread(pb, 1, ll, fp);
	g_data.WMap0 = pb;

	int Map1=++i;
	pb = new BYTE[ll];
	g_data.mapByte[i] = pb;
	fseek(fp, p[mapid]+ll, SEEK_SET);
	fread(pb, 1, ll, fp);
	g_data.WMap1 = pb;


	fclose(fp);

	int Map2=++i;
	pb = new BYTE[ll];
	g_data.mapByte[i] = pb;
	g_data.WMap2 = pb;

	int Map3=++i;
	pb = new BYTE[ll];
	g_data.mapByte[i] = pb;
	g_data.WMap3 = pb;

	int Map4=++i;
	pb = new BYTE[ll];
	g_data.mapByte[i] = pb;
	g_data.WMap4 = pb;

	lua_pushnumber(L, Map0);
	lua_pushnumber(L, Map1);
	lua_pushnumber(L, Map2);
	lua_pushnumber(L, Map3);
	lua_pushnumber(L, Map4);
	return 5;
}

static int L_DrawWMap(lua_State* L)
{
	int rangeX=18+7;
	int rangeY=10+7;

	int i,j,i1,j1,
		x1,y1,picNum,
		xx,yy, j_2;

	//lib.FillColor(0,0,0,0,0);    --清屏

	int flag = lua_tonumber(L, 1);
	int movex = lua_tonumber(L, 2);
	int	movey = lua_tonumber(L, 3);
	int dx = lua_tonumber(L, 4);	//WAR.Person[WAR.CurID]["坐标X"];
	int dy = lua_tonumber(L, 5);		//WAR.Person[WAR.CurID]["坐标Y"];

	int XScale = 18, YScale = 9;
	for (j=-rangeY; j<=2*rangeX+rangeY; ++j)// do
	{
		j_2=j/2;//math.modf(j/2);
		for (i=rangeX; i>=0; --i)// do
		{
			if (2*j_2==j)// then
			{
				i1=-rangeX+i+ j_2;
				j1=-i+j_2;
			}
			else
			{
				i1=-rangeX+i+ j_2;
				j1=-i+j_2+1;
			}

			x1=XScale*(i1-j1)+g_data.screenW/2;
			y1=YScale*(i1+j1)+g_data.screenH/2;

			xx=dx+i1;
			yy=dy+j1;

			if (yy >=0 && xx>=0 && xx <64 && yy<64)// then
			{
				int offset=(yy*64+xx)*2;
				int num = *(short*)(g_data.WMap0+offset);
				//int num=Byte.get16(WAR.Map0,offset);
				if (num>0)// then
				{
					//lib.PicLoadCache(0,num,x1,y1);
					DrawResPic(0, num/2, x1, y1, 0, 0);
				}
			}
		}
	}

	if (flag==1)// then
	{
		for (j=-rangeY; j<= 2*rangeX+rangeY; ++j)// do
		{
			j_2=j/2;//math.modf(j/2);
			for (i=rangeX;i>= 0;--i)// do
			{
				if (2*j_2==j)// then
				{
					i1=-rangeX+i+ j_2;
					j1=-i+j_2;
				}
				else
				{
					i1=-rangeX+i+ j_2;
					j1=-i+j_2+1;
				}

				x1=XScale*(i1-j1)+g_data.screenW/2;
				y1=YScale*(i1+j1)+g_data.screenH/2;

				xx=dx+i1;
				yy=dy+j1;

				if (yy >=0 && xx>=0 && xx <64 && yy<64)// then
				{
					int offset=(yy*64+xx)*2;
					if (*(short*)(g_data.WMap3+offset)<128)// then
					{
						int flag;
						if ((int)klua["GetWarData"]("地图") == 0)//WAR.Data["地图"]==0 then     --雪地地图用黑色菱形
						{
							flag=2+4;
						}
						else    
						{	//--其他地图用白色菱形
							flag=2+8;
						}

						if (xx==movex && yy==movey) //then
						{
							//lib.PicLoadCache(0,0,x1,y1,flag,128);        //--使用战斗贴图0的菱形。注意不要改动此贴图内容
							DrawResPic(0, 0, x1, y1, flag, 128);
						}
						else
						{
							//lib.PicLoadCache(0,0,x1,y1,flag,64);
							DrawResPic(0, 0, x1, y1, flag, 64);
						}
					}
				}
			}
		}
	}

	for (j=-rangeY;j<= 2*rangeX+rangeY;++j)// do
	{
		j_2=i/2;//math.modf(j/2);
		for (i=rangeX;i>= 0;--i)// -1 do
		{
			if (2*j_2==j)// then
			{
				i1=-rangeX+i+ j_2;
				j1=-i+j_2;
			}
			else
			{
				i1=-rangeX+i+ j_2;
				j1=-i+j_2+1;
			}

			x1=XScale*(i1-j1)+g_data.screenW/2;
			y1=YScale*(i1+j1)+g_data.screenH/2;

			xx=dx+i1;
			yy=dy+j1;

			if (yy >=0 && xx>=0 && xx <64 && yy<64)// then
			{
				int offset=(yy*64+xx)*2;
				//int num=Byte.get16(WAR.Map1,offset);
				int num = *(short*)(g_data.WMap1+offset);
				if (num>0)// then
				{
					//lib.PicLoadCache(0,num,x1,y1);
					DrawResPic(0, num/2, x1, y1, 0, 0);
				}
				//num=Byte.get16(WAR.Map2,offset);
				//lib.Debug(string.format("offset:%d, num:%d",offset, num));
				num = *(short*)(g_data.WMap2+offset);
				if (num>=0) 
				{
					int picid = klua["GetWatPerson"](num, "贴图");

					if (flag==0 || flag==1 || flag==3)
					{
						if (1)//WAR.Person[num]["贴图类型"]==0 then
						{
							//lib.PicLoadCache(0,klua["GetWatPerson"](num, "贴图"),x1,y1);
							DrawResPic(0,picid/2, x1, y1, 0, 0);
						}
						else
						{
							//lib.PicLoadCache(4,klua["GetWatPerson"](num, "贴图"),x1,y1);
							DrawResPic(4, picid/2, x1, y1, 0, 0);
						}
					}
					else if( flag==2)// then
					{
						if  (*(short*)(g_data.WMap4+offset)>1)// then
						{
							//lib.PicLoadCache(0,klua["GetWatPerson"](num, "贴图"),x1,y1,4+2,256);
							DrawResPic(0, picid/2, x1, y1, 4+2,256);
						}
						else
						{
							//lib.PicLoadCache(0,klua["GetWatPerson"](num, "贴图"),x1,y1);
							DrawResPic(0, picid/2, x1, y1, 0, 0);
						}
					}
				}

				if (flag==3)// then
				{
					//int effect=Byte.get16(WAR.Map4,offset);
					int effect = *(short*)(g_data.WMap4+offset);
					if (effect>1)
					{
						int n=klua["GetWatPerson"](num, "点数");
						//DrawString(x1,y1-65-movex,string.format("%+d",n),WAR.EffectColor[effect],24);
					}
				}
			}
		}
	}


	//if (WAR.ShowHead==1)// then
	//		WarShowHead();
	return 0;
}

static int L_MsgBox(lua_State* L)
{
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	char* str = tostring(L, 3);
	int btn = lua_tonumber(L, 4);
	char* strb1 = tostring(L, 5);
	char* strb2 = tostring(L, 6);
	char* strb3 = tostring(L, 7);

	int w = 220;
	int h = 80;

	if (x == -1 && y == -1)
	{
		x = (g_data.screenW-w)/2;
		y = (g_data.screenH-h)/2;
	}

	DrawStrOnBoard(str, PGE_YELLOW, PGE_BLUE, x, y, w, h);
	int isel = 0;
	RECT r[4];
	while(1)
	{
		klua["Cls"]();
		if (btn == 1)
		{
			DrawStrOnBoard(strb1, PGE_WHITE, PGE_BLUE, x+w/2-20, y+40, 40, 18);
			r[1] = RECT(x+w/2-20, y+40, x+w/2-20+40, y+40+18);
		}
		else if (btn == 2)
		{
			DrawStrOnBoard(strb1, PGE_WHITE, isel==1 ? PGE_GREEN : PGE_BLUE, x+w/4-20, y+40, 40, 18);
			r[1] = RECT(x+w/4-20, y+40, x+w/4-20+40, y+40+18);
			DrawStrOnBoard(strb2, PGE_WHITE, isel==2 ? PGE_GREEN : PGE_BLUE, x+w/2+w/4-20, y+40, 40, 18);
			r[2] = RECT(x+w/2+w/4-20, y+40, x+w/2+w/4-20+40, y+40+18);
		}
		else if (btn == 3)
		{
			DrawStrOnBoard(strb1, PGE_YELLOW, isel==1 ? PGE_GREEN : PGE_BLUE, x+w/6-20, y+40, 40, 18);
			r[1] = RECT(x+w/6-20, y+40, x+w/6-20+40, y+40+18);
			DrawStrOnBoard(strb2, PGE_YELLOW, isel==2 ? PGE_GREEN : PGE_BLUE, x+w/2-20, y+40, 40, 18);
			r[2] = RECT(x+w/2-20, y+40, x+w/2-20+40, y+40+18);
			DrawStrOnBoard(strb3, PGE_YELLOW, isel==3 ? PGE_GREEN : PGE_BLUE, x+w*5/6-20, y+40, 40, 18);
			r[3] = RECT(x+w*5/6-20, y+40, x+w*5/6-20+40, y+40+18);
		}
		RECT rs(x, y, x+w, x+h);
		theApp->GetDevice()->Flip(&rs);

		while(1)
		{
			int brk = 0;
			CPGEMouse* pm = CPGEMouse::Instance();
			if (pm->lBtn)
			{
				pm->lBtn = 0;
				for (int i=1; i<4; ++i)
				{
					if (r[i].inThisRect(WPOINT(pm->ix, pm->iy)))
					{
						if (i == isel)
						{
							return isel;
						}
						else
						{
							isel = i;
							brk = 1;
							break;
						}
					}
				}
			}

			Sleep(10);
			if (brk) break;
		}

	}
	lua_pushnumber(L, isel);
	return 1;


}

static const struct luaL_reg jylib [] =
{
	{"Debug", L_Debug},
	{"GetKey", L_GetKey},
	{"GetTime", L_GetTime},
	{"Delay", L_Delay},
	{"CharSet", L_CharSet},
	{"SetClip", L_SetClip},
	{"FillColor", L_FillColor},
	{"Background", L_Background},
	{"DrawRect", L_DrawRect},
	{"DrawStr", L_DrawStr},
	{"ShowSurface", L_ShowSurface},
	{"ShowSlow", L_ShowSlow},
	{"PicInit", L_PicInit},
	{"PicLoadFile", L_PicLoadFile},
	{"PicLoadCache", L_PicLoadCache},
	{"PlayMIDI", L_PlayMIDI},
	{"PlayWAV", L_PlayWAV},
	{"LoadMMap", L_LoadMMap},
	{"GetMMap", L_GetMMap},
	{"DrawMMap", L_DrawMMap},
	{"LoadPicture", L_LoadPicture},
	{"DrawSMap", L_DrawSMap},
	{"SetBaseData", L_SetBaseData},
	{"DtoSMap", L_DtoSMap},
	{"SetD", L_SetD},
	{"GetD", L_GetD},
	{"SetS", L_SetS},
	{"GetS", L_GetS},
	{"RGB", L_RGB},
	{"GetRGB", L_GetRGB},
	{"subRGB", L_subRGB},
	{"TalkEx", L_TalkEx},
	{"WaitKey", L_WaitKey},
	{"ShowMenu", L_ShowMenu},
	{"GetCurSorPos", L_GetCurSorPos},
	{"WarLoadMap", L_WarLoadMap},
	{"MsgBox", L_MsgBox},
	{"DrawWMap", L_DrawWMap},
	{NULL, NULL}
};


static int L_create(lua_State* L)//size)
{
	static int id = 0;

	//	if (id>9) 
	//		return 0;
	int size = lua_tonumber(L, 1);
	if (size)
	{
		ass+=size;
		BYTE* p = new BYTE[size];
		g_data.mapByte[++id] = p;
		lua_pushnumber(L, id);
	}
	return 1;
}

static int L_loadfile(lua_State* L)//b,filename,start,length)
{
	int id = lua_tonumber(L,1);
	char* szfile = tostring(L,2);
	int s = lua_tonumber(L,3);
	int l = lua_tonumber(L,4);

	static string sn;
	if (sn != szfile)
	{
		sn = szfile;
	}
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		FILE* fp = fopen(GetResFile(szfile), "rb");
		fseek(fp, s, SEEK_SET);
		fread(p, 1, l, fp);
		fclose(fp);
	}
	return 0;
}

static int L_savefile(lua_State* L)//b,filename,start,length)
{
	int id = lua_tonumber(L,1);
	char* szfile = tostring(L,2);
	int s = lua_tonumber(L,3);
	int l = lua_tonumber(L,4);

	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		FILE* fp = fopen(GetResFile(szfile), "wb");
		fseek(fp, s, SEEK_SET);
		fwrite(p, 1, l, fp);
		fclose(fp);
	}
	return 0;
}

static int L_get16(lua_State* L)//b,start)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		lua_pushnumber(L, (short)arByte2Int(p+s, 2));
	}
	else
	{
		lua_pushnumber(L, -1);
	}
	return 1;
}

static int L_set16(lua_State* L)//b,start,v)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	short v = (short)lua_tonumber(L, 3);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		Int2arByte(p+s, v, 2);
		//*(short*)(p+s) = v;
	}
	return 0;
}

static int L_getu16(lua_State* L)//b,start)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		lua_pushnumber(L, (WORD)arByte2Int(p+s, 2));
	}
	else
	{
		lua_pushnumber(L, 0);
	}
	return 1;
}

static int L_setu16(lua_State* L)//b,start,v)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	WORD v = (WORD)lua_tonumber(L, 3);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		Int2arByte(p+s, v, 2);
	}
	return 0;
}

static int L_get32(lua_State* L)//b,start)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		lua_pushnumber(L, (int)arByte2Int(p+s, 4));
	}
	else
	{
		lua_pushnumber(L, -1);
	}
	return 1;
}

static int L_set32(lua_State* L)//b,start,v)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	DWORD v = (int)lua_tonumber(L, 3);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		Int2arByte(p+s, v, 4);
		//*(int*)(p+s) = v;
	}
	return 0;
}

static int L_getstr(lua_State* L)//b,start,length)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	int l = lua_tonumber(L,3);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		lua_pushstring(L, (char*)p+s);
	}
	return 1;
}

static int L_setstr(lua_State* L)//b,start,length,str)
{
	int id = lua_tonumber(L, 1);
	int s = lua_tonumber(L, 2);
	int l = lua_tonumber(L,3);
	char* str = tostring(L, 4);
	BYTE* p = g_data.mapByte[id];
	if (p)
	{
		memcpy(p+s, str, l);
	}
	return 0;
}

static const struct luaL_reg ByteLib[] = 
{
	{"create", L_create},
	{"loadfile", L_loadfile},
	{"savefile", L_savefile},
	{"get16", L_get16},
	{"set16", L_set16},
	{"getu16", L_getu16},
	{"setu16", L_setu16},
	{"get32", L_get32},
	{"set32", L_set32},
	{"getstr", L_getstr},
	{"setstr", L_setstr},
	{NULL, NULL}
};

class CIoLib
{
public:
	CIoLib():fp(0){}
	~CIoLib(){}

	int seek(char* str)
	{
		if (strcmp(str, "end") == 0)
		{
			fseek(fp, 0, SEEK_END);
			return ftell(fp);
		}
	}

	void close()
	{
		fclose(fp);
		fp = 0;
	}

	FILE* fp;
	BeginDefLuaClass(CIoLib)
		DefMemberFunc(seek)
		DefMemberFunc(close)
		EndDef
};

static int L_iofsize(lua_State* L)//b,start,length,str)
{
	//static CIoLib cio;
	char* f = tostring(L,1);

	lua_pushnumber(L, GetFileLength(GetResFile(f)));

	//if (cio.fp) fclose(cio.fp);
	//cio.fp = fopen(GetResFile(f), w);

	//LuaPush(&cio);
	return 1;
}


static const struct luaL_reg IoLib[] = 
{
	{"fsize", L_iofsize},
	{NULL, NULL}
};

void JyInit()
{
	g_pdata = new JyData;

	g_backupSuf = new CPGESurface;
	g_data.screenH = theMainSurf->GetSurfaceInfo()->wHeigh;
	g_data.screenW = theMainSurf->GetSurfaceInfo()->wWiedth;

	g_backupSuf->CreateSurface(g_data.screenW, g_data.screenH);

	lua_State* L = lua_open(); 
	luaL_openlibs(L);
	RegJyLib(L);

	// 这里你自己替换成这些脚本的真是路径
	luaL_dofile(L, "old_talk.lua");
	luaL_dofile(L, "old_kdef.lua");
	luaL_dofile(L, "JYModify.lua");
	luaL_dofile(L, "JYConst.lua");
	luaL_dofile(L, "JYMain.lua");


	lua_getglobal(L, "JY_Main");
	if (lua_isfunction(L, -1))
	{
		lua_call(L, 0, 0);
	}
	
}

void RegJyLib(lua_State* L)
{
	//LuaWraper.RegisterClass<CIoLib>();
	luaL_openlib(L, "lib", jylib, 0);
	luaL_openlib(L, "Byte", ByteLib, 0);
	luaL_openlib(L, "io", IoLib, 0);
}

