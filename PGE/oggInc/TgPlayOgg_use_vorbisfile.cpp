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

#include "TgPlayOgg_use_vorbisfile.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
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
int init_for_ogg_decode(char *filename, void **vf_out)
{
    const int
        ifod_err_open_failed = 1,              // open file failed
        ifod_err_malloc_failed = 2,            // malloc() call failed; out of memory
        ifod_err_read_failed = 3,              // A read from media returned an error     
        ifod_err_not_vorbis_data = 4,          // Bitstream is not Vorbis data       
        ifod_err_vorbis_version_mismatch = 5,  // Vorbis version mismatch
        ifod_err_invalid_vorbis_header = 6,    // Invalid Vorbis bitstream header
        ifod_err_internal_fault = 7,           // Internal logic fault; indicates a bug or heap/stack corruption
        ifod_err_unspecified_error = 8;        // ov_open() returned an undocumented error
    
    int err_code, ov_ret;
    FILE *file_ptr;
	void *vf_ptr;
    
    // initialize the memory pointer from the caller
    *vf_out = NULL;

    // open the file for reading
    file_ptr = fopen(filename, "rb");  // NOTE: b for binary
    if (file_ptr == NULL)
        return ifod_err_open_failed;
    
    // get memory for holding an instance of struct OggVorbis_File
	vf_ptr = malloc(sizeof(OggVorbis_File));
	if (vf_ptr == NULL)
    {
        fclose(file_ptr);
		return ifod_err_malloc_failed;
    }

    // call
	ov_ret = ov_open(file_ptr, static_cast<OggVorbis_File *>(vf_ptr), NULL, 0);

	if (ov_ret < 0)
	{
		// there was an error
		if (ov_ret == OV_EREAD)
			err_code = ifod_err_read_failed;
		else if (ov_ret == OV_ENOTVORBIS)
			err_code = ifod_err_not_vorbis_data;
		else if (ov_ret == OV_EVERSION)
			err_code = ifod_err_vorbis_version_mismatch;
		else if (ov_ret == OV_EBADHEADER)
			err_code = ifod_err_invalid_vorbis_header;
		else if (ov_ret == OV_EFAULT)
			err_code = ifod_err_internal_fault;
		else 
			err_code = ifod_err_unspecified_error;
		
        // cleanup
        fclose(file_ptr);
        free(vf_ptr);

        // return the ifod_err_code
		return err_code;
    }

	// copy the memory pointer to the caller
    *vf_out = vf_ptr;
	
	return 0;  // success
}


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
                                         int *err_ov_hole_cnt, int *err_ov_ebadlink_cnt)
{
    int bytes_put_in_buf, bitstream, word_size, sgned;
    long ov_ret;
    vorbis_info *vi_ptr;

    assert(ogg_sample_size == 8 || ogg_sample_size == 16);
    
    if (ogg_sample_size == 8)
    {
        word_size = 1;
        // NOTE: We want unsigned data, since Microsoft's DirectSound 
        //       expects this when the sample size is 8 bits.
        sgned = 0;  
    }
    else 
    {
        word_size = 2;
        // NOTE: We want signed data, since Microsoft's DirectSound 
        //       expects this when the sample size is 16 bits.
        sgned = 1;
    }
    
    for(bytes_put_in_buf = 0;;)
    {
        // NOTE: parameter 4 is 0 for little endian
        //
        ov_ret = ov_read(static_cast<OggVorbis_File*>(vf_ptr), static_cast<char*>(buf_out), 
			buf_byte_size, 0, word_size, sgned, &bitstream);
        
        if (ov_ret == 0)  // at EOF
		{
            break;
		}
        else if (ov_ret < 0)  // an error, bad ogg data of some kind
        {
            // NOTE: other than recording the error, can't do anything about it
            // but skip over it, which is what the vorbis example code does.
            //
            // Possible error codes returned from ov_read() according to docs:
            //
            //      OV_HOLE 
            //         indicates there was an interruption in the data. 
            //         (one of: garbage between pages, loss of sync followed by
            //          recapture, or a corrupt page) 
            //
            //      OV_EBADLINK 
            //         indicates that an invalid stream section was supplied to
            //         libvorbisfile, or the requested link is corrupt.
            if (ov_ret == OV_HOLE)
                ++(*err_ov_hole_cnt);
            else if (ov_ret == OV_EBADLINK)
                ++(*err_ov_ebadlink_cnt);
        }
        else 
        {
            assert(ov_ret <= buf_byte_size);

            vi_ptr = ov_info(static_cast<OggVorbis_File*>(vf_ptr), bitstream);
            if (vi_ptr != NULL)
            {
                *channels_cnt = vi_ptr->channels;  // number of channels in the bitstream
                *sampling_rate = vi_ptr->rate;     // sampling rate of the bitstream
			}
            
            bytes_put_in_buf = ov_ret;
            break;
        }
    }
    
    return bytes_put_in_buf;
}


//	Function: final_ogg_cleanup
//
//	Remarks: Free vf_out's pointed-to memory and also close the Ogg Vorbis file 
//			 opened by init_for_ogg_decode()
//
//	Returns: A non-zero value if ov_clear() was called and failed.
//
//	Notes: OK to call with null vf_ptr parameter.

//extern "C" __declspec(dllexport) 
int final_ogg_cleanup(void *vf_ptr)
{
    int ret = 0;

    if (vf_ptr != NULL)
    {
        ret = ov_clear(static_cast<OggVorbis_File*>(vf_ptr));  // non-zero is failure!
        free(vf_ptr);
    }

    return ret;
}
