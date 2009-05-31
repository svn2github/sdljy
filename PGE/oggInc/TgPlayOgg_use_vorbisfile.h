/*	
*	Files:	TgPlayOgg_use_vorbisfile.c - this C file is compiled into a DLL that can be
*			called from any .NET C# project.
*
*	Notes:	To create a standalone DLL, compile this file and statically linked with 
*			the following 3 Ogg Vorbis libraries availabe in the Ogg Vorbis SDK.
*
*           Debug version:
*              ogg_static_d.lib
*              vorbis_static_d.lib
*              vorbisfile_static_d.lib
*
*           Release version:
*              ogg_static.lib
*              vorbis_static.lib
*              vorbisfile_static.lib
*/


// Ogg Vorbis includes
#include "codec.h"
#include "vorbisfile.h"

//	Function: init_for_ogg_decode
//
//	Remarks: Initialization for decoding the given ogg file
//
//	Returns: 0 if no error; otherwise returns one of the ifod_err_values
//
//	Notes: If no error, the returned vf_out pointer is a pointer to malloc memory
//
//	IMPORTANT:	If no error, then when all done using the returned vf_out pointer for 
//				making whatever other calls into this DLL are wanted, then be sure to
//				call final_ogg_cleanup(vf_out)

//extern "C" __declspec(dllexport)
int init_for_ogg_decode(char *filename, void **vf_out);


//  Function: ogg_decode_at_most_one_vorbis_packet
//
//  Remarks: Writes PCM (Pulse Code Modulation) data into the given buffer beginning 
//			 at buf_out[0].
//
//  Returns: The number of bytes written into the buffer.
//
//	Notes: If it hits the end of the file then it returns 0.

//extern "C" __declspec(dllexport) 
int ogg_decode_at_most_one_vorbis_packet(void *vf_ptr, void *buf_out, int buf_byte_size,
                                         int ogg_sample_size,
                                         int *channels_cnt, int *sampling_rate,
                                         int *err_ov_hole_cnt, int *err_ov_ebadlink_cnt);


//	Function: final_ogg_cleanup
//
//	Remarks: Free vf_out's pointed-to memory and also close the Ogg Vorbis file 
//			 opened by init_for_ogg_decode()
//
//	Returns: A non-zero value if ov_clear() was called and failed.
//
//	Notes: OK to call with null vf_ptr parameter.

//extern "C" __declspec(dllexport) 
int final_ogg_cleanup(void *vf_ptr);

