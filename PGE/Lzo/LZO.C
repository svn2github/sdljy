// 代码是根据 Markus Franz Xaver Johannes Oberhumer 的 LZO 改写
// 而成, 所以算法上的问题请不要问我, 如果你对这个压缩算法有兴趣,
// 请拜访 LZO 的主页 
// http://wildsau.idv.uni-linz.ac.at/mfx/lzo.html
// LZO 属于 GNU 软件, 在此声明, 本文件中的代码使用权利不同于风魂游
// 戏程序库中的其他部分, 请遵守 GNU 协议使用. 
//#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#define BYTE unsigned char
#define byte unsigned char
int __stdcall compress(void *src, unsigned long src_len, void *dst);
int __stdcall decompress(void *src, unsigned long src_len,void *dst);

static unsigned long _do_compress (BYTE *in, unsigned long in_len, BYTE *out, unsigned long *out_len)
{
	static long wrkmem [16384L];
	register BYTE *ip;
	BYTE *op;
	BYTE *in_end = in + in_len;
	BYTE *ip_end = in + in_len - 13;
	BYTE *ii;
	BYTE **dict = (BYTE **)wrkmem;
	op = out;
	ip = in;
	ii = ip;
	ip += 4;
	for(;;)
	{
		register BYTE *m_pos;
		unsigned long m_off;
		unsigned long m_len;
		unsigned long dindex;
		dindex = ((0x21*(((((((unsigned long)(ip[3])<<6)^ip[2])<<5)^ip[1])<<5)^ip[0]))>>5) & 0x3fff;
		m_pos = dict [dindex];
		if(((unsigned long)m_pos < (unsigned long)in) ||
			(m_off = (unsigned long)((unsigned long)ip-(unsigned long)m_pos) ) <= 0 ||
			m_off > 0xbfff)
			goto literal;
		if(m_off <= 0x0800 || m_pos[3] == ip[3])
			goto try_match;
		dindex = (dindex & 0x7ff ) ^ 0x201f;
		m_pos = dict[dindex];
		if((unsigned long)(m_pos) < (unsigned long)(in) ||
			(m_off = (unsigned long)( (int)((unsigned long)ip-(unsigned long)m_pos))) <= 0 ||
			m_off > 0xbfff)
			goto literal;
		if (m_off <= 0x0800 || m_pos[3] == ip[3])
			goto try_match;
		goto literal;
try_match:
		if(*(unsigned long*)m_pos == *(unsigned long*)ip && m_pos[2]==ip[2])
			goto match;
literal:
		dict[dindex] = ip;
		++ip;
		if (ip >= ip_end)
			break;
		continue;
match:
		dict[dindex] = ip;
		if(ip - ii > 0)
		{
			register unsigned long t = ip - ii;

			if (t <= 3)
				op[-2] |= (BYTE)t;
			else if(t <= 18)
				*op++ = (BYTE)(t - 3);
			else
			{
				register unsigned long tt = t - 18;
				*op++ = 0;
				while(tt > 255)
				{
					tt -= 255;
					*op++ = 0;
				}
				*op++ = (BYTE)tt;
			}
			do *op++ = *ii++; while (--t > 0);
		}
		ip += 3;
		if(m_pos[3] != *ip++ || m_pos[4] != *ip++ || m_pos[5] != *ip++ ||
			m_pos[6] != *ip++ || m_pos[7] != *ip++ || m_pos[8] != *ip++ )
		{
			--ip;
			m_len = ip - ii;

			if(m_off <= 0x0800 )
			{
				--m_off;
				*op++ = (BYTE)(((m_len - 1) << 5) | ((m_off & 7) << 2));
				*op++ = (BYTE)(m_off >> 3);
			}
			else
				if (m_off <= 0x4000 )
				{
					-- m_off;
					*op++ = (BYTE)(32 | (m_len - 2));
					goto m3_m4_offset;
				}
				else
				{
					m_off -= 0x4000;
					*op++ = (BYTE)(16 | ((m_off & 0x4000) >> 11) | (m_len - 2));
					goto m3_m4_offset;
				}
		}
		else
		{
			{
				BYTE *end = in_end;
				BYTE *m = m_pos + 9;
				while (ip < end && *m == *ip)
					m++, ip++;
				m_len = (ip - ii);
			}

			if(m_off <= 0x4000)
			{
				--m_off;
				if (m_len <= 33)
					*op++ = (BYTE)(32 | (m_len - 2));
				else
				{
					m_len -= 33;
					*op++=32;
					goto m3_m4_len;
				}
			}
			else
			{
				m_off -= 0x4000;
				if(m_len <= 9)
					*op++ = (BYTE)(16|((m_off & 0x4000) >> 11) | (m_len - 2));
				else
				{
					m_len -= 9;
					*op++ = (BYTE)(16 | ((m_off & 0x4000) >> 11));
m3_m4_len:
					while (m_len > 255)
					{
						m_len -= 255;
						*op++ = 0;
					}
					*op++ = (BYTE)m_len;
				}
			}
m3_m4_offset:
			*op++ = (BYTE)((m_off & 63) << 2);
			*op++ = (BYTE)(m_off >> 6);
		}
		ii = ip;
		if (ip >= ip_end)
			break;
	}
	*out_len = op - out;
	return (unsigned long) (in_end - ii);
}
int __stdcall compress(void *in, unsigned long in_len,
					  void *out)
{
	BYTE *op = out;
	unsigned long t,out_len;
	if (in_len <= 13)
		t = in_len;
	else 
	{
		t = _do_compress (in,in_len,op,&out_len);
		op += out_len;
	}
	if (t > 0)
	{
		BYTE *ii = (BYTE*)in + in_len - t;
		if (op == (BYTE*)out && t <= 238)
			*op++ = (BYTE) ( 17 + t );
		else
			if (t <= 3)
				op[-2] |= (BYTE)t ;
			else
				if (t <= 18)
					*op++ = (BYTE)(t-3);
				else
				{
					unsigned long tt = t - 18;
					*op++ = 0;
					while (tt > 255) 
					{
						tt -= 255;
						*op++ = 0;
					}
					*op++ = (BYTE)tt;
				}
				do *op++ = *ii++; while (--t > 0);
	}
	*op++ = 17;
	*op++ = 0;
	*op++ = 0;
	return (op - (BYTE*)out);
}
int __stdcall decompress (void *in, unsigned long in_len,
						 void *out)
{
	register BYTE *op;
	register BYTE *ip;
	register unsigned long t;
	register BYTE *m_pos;
	unsigned short dwBuf;
	BYTE *ip_end = (BYTE*)in + in_len;
	op = out;
	ip = in;
	if(*ip > 17)
	{
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		do *op++ = *ip++; while (--t > 0);
		goto first_literal_run;
	}
	for(;;)
	{
		t = *ip++;
		if (t >= 16) goto match;
		if (t == 0)
		{
			while (*ip == 0)
			{
				t += 255;
				ip++;
			}
			t += 15 + *ip++;
		}
		//*(unsigned long*) op = *(unsigned long*) ip;
		//memcpy(op, ip, 4);
		op[0] = ip[0];
		op[1] = ip[1];
		op[2] = ip[2];
		op[3] = ip[3];
		op += 4; ip += 4;
		if (--t > 0)
		{
			if (t >= 4)
			{
				do
				{
					//*(unsigned long*) op = *(unsigned long*) ip;
					//memcpy(op, ip, 4);
					op[0] = ip[0];
					op[1] = ip[1];
					op[2] = ip[2];
					op[3] = ip[3];
					op += 4; ip += 4; t -= 4;
				} while (t >= 4);
				if (t > 0) do *op++ = *ip++; while (--t > 0);
			}
			else
				do *op++ = *ip++; while (--t > 0);
		}
first_literal_run:
		t = *ip++;
		if (t >= 16)
			goto match;
		m_pos = op - 0x0801;
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;
		*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos;
		goto match_done;
		for(;;)
		{
match:
			if (t >= 64)
			{
				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;
				goto copy_match;
			}
			else 
				if (t >= 32)
				{
					t &= 31;
					if (t == 0)
					{
						while (*ip == 0) 
						{
							t += 255;
							ip++;
						}
						t += 31 + *ip++;
					}
					m_pos = op - 1;
					//memcpy(&dwBuf, ip, 2);
					dwBuf = 0;
					dwBuf |= (ip[1] << 8);
					dwBuf |= (ip[0]);
					m_pos -= (dwBuf >> 2);
					ip += 2;
				}
				else
					if (t >= 16) 
					{
						m_pos = op;
						m_pos -= (t & 8) << 11;
						t &= 7;
						if (t == 0)
						{
							while (*ip == 0)
							{
								t += 255;
								ip++;
							}
							t += 7 + *ip++;
						}
						//memcpy(&dwBuf, ip, 2); 
						dwBuf = 0;
						dwBuf |= (ip[1] << 8);
						dwBuf |= (ip[0]);
						m_pos -= (dwBuf >> 2);
						ip += 2;
						if (m_pos == op)
							goto eof_found;
						m_pos -= 0x4000;
					}
					else 
					{
						m_pos = op - 1;
						m_pos -= t >> 2;
						m_pos -= *ip++ << 2;
						*op++ = *m_pos++; *op++ = *m_pos;
						goto match_done;
					}
					if (t >= 6 && (op - m_pos) >= 4) 
					{
						//* (unsigned long*) op = * (unsigned long*) m_pos;
						// memcpy(op, m_pos, 4);
						op[0] = m_pos[0];
						op[1] = m_pos[1];
						op[2] = m_pos[2];
						op[3] = m_pos[3];
						op += 4; m_pos += 4; t -= 2;
						do
						{
							//* (unsigned long*) op = * (unsigned long*)m_pos;
							// memcpy(op, m_pos, 4);
							op[0] = m_pos[0];
							op[1] = m_pos[1];
							op[2] = m_pos[2];
							op[3] = m_pos[3];
							op += 4; m_pos += 4; t -= 4;
						}while (t >= 4);
						if (t > 0)
							do *op++ = *m_pos++; while (--t > 0);
					}
					else
					{
copy_match:
						*op++ = *m_pos++; *op++ = *m_pos++;
						do *op++ = *m_pos++; while (--t > 0);
					}
match_done:
					t = ip[-2] & 3;
					if (t == 0)break;
match_next:
					do *op++ = *ip++; while (--t > 0);
					t = *ip++;
		}
	}
eof_found:
	if (ip != ip_end) return -1;
	return (op - (BYTE*)out);
}
