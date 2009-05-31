extern "C"
{
	int __stdcall compress(void *src, unsigned src_len, void *dst);
	int __stdcall decompress(void *src, unsigned src_len,void *dst);
}
