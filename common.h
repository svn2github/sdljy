#include <stdlib>
#include <stdio.h>
typedef short int SWORD;
typedef unsigned long DWORD;
typedef unsigned int WORD;
typedef   unsigned   char   BYTE;
#define isNumber(c) ((c)>47 && (c)<58)
struct WPOINT
{
	WPOINT()
		:wx(0),wy(0)
	{
	}
	WPOINT(short x, short y)
		:wx(x),wy(y)
	{
	}

	WPOINT(char* c)
	{
		int n = 0;
		wx = atoi(c);
		while(*(++c))
		{
			if (!isNumber(*c))
			{
				++c;
				++n;
				if (n == 1) 
				{
					wy =  atoi(c);
					break;
				}
			}
		}
	}

	__inline bool operator == (const WPOINT& pt) const
	{
		return pt.wx == wx && pt.wy == wy;
	}

	__inline bool operator != (const WPOINT& pt) const
	{
		return pt.wx != wx || pt.wy != wy;
	}

	__inline WPOINT operator - (const WPOINT& pt)
	{
		return WPOINT(wx-pt.wx, wy-pt.wy);
	}

	__inline float slope()
	{
		return wy/(float)wx;
	}

	__inline bool operator >= (const int v) const
	{
		return wx >= v && wy >= v;
	}
	__inline bool operator < (const WPOINT& pt) const
	{
		return wx < pt.wx && wy < pt.wy;
	}
	__inline WPOINT operator / (const int v)
	{
		return WPOINT(wx/v, wy/v);
	}

	__inline WPOINT operator * (const int v)
	{
		return WPOINT(wx*v, wy*v);
	}

	__inline WPOINT operator >> (const int v)
	{
		return WPOINT(wx>>v, wy>>v);
	}

	__inline WPOINT operator << (const int v)
	{
		return WPOINT(wx<<v, wy<<v);
	}

	__inline WPOINT operator + (const int v)
	{
		return WPOINT(wx+v, wy+v);
	}


	__inline WPOINT operator + (const WPOINT& v)
	{
		return WPOINT(wx+v.wx, wy+v.wy);
	}

	__inline WPOINT operator - (const int v)
	{
		return WPOINT(wx-v, wy-v);
	}

	__inline void operator += (WPOINT& wp)
	{
		wx += wp.wx;
		wy += wp.wy;
	}

	__inline void DelimitMin(short int v)
	{
		if (wx<v) wx = v;
		if (wy<v) wy = v;
	}

	__inline void DelimitMax(short int v)
	{
		if (wx>v) wx = v;
		if (wy>v) wy = v;
	}

	__inline void DelimitMin(WPOINT& v)
	{
		if (wx<v.wx) wx = v.wx;
		if (wy<v.wy) wy = v.wy;
	}

	__inline void DelimitMax(WPOINT& v)
	{
		if (wx>v.wx) wx = v.wx;
		if (wy>v.wy) wy = v.wy;
	}

	short int wx;
	short int wy;
};

typedef struct tagRECT
{
	  long left;
	  long top;
	  long right;
	  long bottom;
} RECT;

typedef struct tagWRECT
{
	tagWRECT(short l, short t, short r, short b)
	{
		left = l;
		right = r;
		top = t; 
		bottom = b;
	}

	tagWRECT(tagRECT& r)
	{
		left = r.left;
		right = r.right;
		top = r.top; 
		bottom = r.bottom;
	}

	tagWRECT(WPOINT& p)
	{
		left = p.wx;
		right = p.wx;
		top =  p.wy;
		bottom = p.wy;
	}

	tagWRECT(char* c)
	{
		int n = 0;
		left = atoi(c);
		while(*(++c))
		{
			if (!isNumber(*c))
			{
				++c;
				++n;
				if (n == 1) top =  atoi(c);
				else if (n == 2) right =  atoi(c);
				else if (n == 3) 
				{
					bottom =  atoi(c);
					break;
				}
			}
		}
	}

	tagWRECT(WPOINT& p1, WPOINT& p2)
	{
		left = p1.wx;
		right = p2.wx;
		top =  p1.wy;
		bottom = p2.wy;
	}

	tagWRECT(void)
	{
		left = 0;
		right = 0;
		top = 0; 
		bottom = 0;
	}

	__inline tagWRECT operator - (const WPOINT& wp)
	{
		return tagWRECT(left-wp.wx, top-wp.wy, right-wp.wx, bottom-wp.wy);
	}

	__inline tagWRECT operator + (const WPOINT& wp)
	{
		return tagWRECT(left+wp.wx, top+wp.wy, right+wp.wx, bottom+wp.wy);
	}

	__inline bool operator == (const tagWRECT& r) const
	{
		return r.bottom == bottom && r.left == left
			&& r.right == right && r.top == top;
	}
	__inline bool operator == (const tagRECT& r) const
	{
		return r.bottom == bottom && r.left == left
			&& r.right == right && r.top == top;
	}
	__inline bool operator == (const int v) const
	{
		return v == bottom && v == left
			&& v == right && v == top;
	}

	__inline bool operator != (const tagWRECT& r) const
	{
		return r.bottom != bottom || r.left != left
			|| r.right != right || r.top != top;
	}
	__inline bool operator != (const tagRECT& r) const
	{
		return r.bottom != bottom || r.left != left
			|| r.right != right || r.top != top;
	}
	__inline bool operator != (const int v) const
	{
		return v != bottom || v != left
			|| v != right || v != top;
	}

	__inline bool operator > (const int v) const
	{
		return bottom > v && left > v
			&& right > v && top > v;
	}

	__inline bool operator >= (const int v) const
	{
		return bottom >= v && left >= v
			&& right >= v && top >= v;
	}

	__inline tagWRECT operator & (const tagWRECT& r)
	{
		return tagWRECT(r.left > left ? r.left : left,
			r.top > top ? r.top : top,
			r.right > right ? right : r.right,
			r.bottom > bottom ? bottom : r.bottom);
	}

	__inline tagWRECT operator & (const tagRECT& r)
	{
		return tagWRECT(r.left > left ? r.left : left,
			r.top > top ? r.top : top,
			r.right > right ? right : r.right,
			r.bottom > bottom ? bottom : r.bottom);
	}

	__inline tagWRECT operator / (const int v)
	{
		return tagWRECT(left/v, top/v, right/v, bottom/v);
	}

	__inline void operator &= (const tagWRECT& r)
	{
		left = r.left > left ? r.left : left;
		top = r.top > top ? r.top : top;
		right =	r.right > right ? right : r.right;
		bottom = r.bottom > bottom ? bottom : r.bottom;
	}

	__inline void operator &= (const tagRECT& r)
	{
		left = r.left > left ? r.left : left;
		top = r.top > top ? r.top : top;
		right =	r.right > right ? right : r.right;
		bottom = r.bottom > bottom ? bottom : r.bottom;
	}

	__inline bool inThisRect (const tagWRECT& r) const
	{
		return r.left >= left && r.right < right && r.top >= top && r.bottom < bottom;
	}

	__inline bool inThisRect (const WPOINT& pt) const
	{
		return pt.wx >= left && pt.wx < right && pt.wy >= top && pt.wy < bottom;
	}

	__inline bool inRect (const tagWRECT& r) const
	{
		return r.left <= left && r.right > right && r.top <= top && r.bottom > bottom;
	}

	__inline int hit (const tagWRECT& r)
	{
		tagWRECT ra = (*this);
		tagWRECT rh = ra & r;
		if (rh.w()>0 && rh.h()>0) return 1; // 相交
		else if (rh.w()<0 || rh.h()<0) return 0; // 不相交
		else if (rh.w()==0 || rh.h()==0) return 2; // 相接
	}

	__inline void operator += (WPOINT& wp)
	{
		left += wp.wx;
		right += wp.wx;
		top += wp.wy;
		bottom += wp.wy;
	}

	void reject() // 反向
	{
		SWORD b;
		if (left > right) b=left,left=right,right=b;
		if (top > bottom) b=top,top=bottom,bottom=b;
	}

	tagWRECT& Inv() // 以left,top为原点
	{
		bottom -= top;
		right -= left;
		left = top = 0;
		return *this;
	}

	__inline short w() const { return right - left; }
	__inline short h() const { return bottom - top; }
	short left, right, top, bottom;
}wrect, *lpwrect;


// 自定义消息

typedef struct tagPALINPUTSTATE
{
   DWORD                  dwKeyPress;
} PALINPUTSTATE;

#define v wrect
void PutDebugString(char *msg, ...);

