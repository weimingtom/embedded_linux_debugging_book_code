/*********************************************************************
 * $file	:	test_libavi.c
 * $desc	:	test libavi library, function as split video/audio 
 * 				data from interleaved audio-video data stream
 * $author	:	rockins
 * $date	:	Tue Nov 27 17:14:47 CST 2007
 * $copyright	:	all copyrights(c) reserved by rockins.
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "libavi.h"

#define	DUMP_VIDEO	1		// control dump video or audio

#define	BUFF_SZ		100 * 1024
int
main(int argc, char *argv[])
{
	AVI 	* avi;
	char	buff[BUFF_SZ];
	int		block_sz;
	int		block_len;
	int		ret;
	
	avi = avi_new("zsh_yxdcb.avi");
	
	avi->init(avi);
	avi->parse(avi);

#if DUMP_VIDEO
	ret = avi->goto_first_video_block(avi);
	assert(ret == 0);
	
	block_sz = avi->get_curr_video_block_size(avi);
	while (block_sz > 0) {
		assert(block_sz < BUFF_SZ);
		block_len = avi->get_curr_video_block(avi, buff, block_sz);
		assert(block_len == block_sz);
		
		write(STDOUT_FILENO, buff, block_len);
		
		avi->goto_next_video_block(avi);
		block_sz = avi->get_curr_video_block_size(avi);
	}
#else	// !DUMP_VIDEO, dump audio data
	ret = avi->goto_first_audio_block(avi);
	assert(ret == 0);
	
	block_sz = avi->get_curr_audio_block_size(avi);
	while (block_sz > 0) {
		assert(block_sz < BUFF_SZ);
		block_len = avi->get_curr_audio_block(avi, buff, block_sz);
		assert(block_len == block_sz);
		
		write(STDOUT_FILENO, buff, block_len);
		
		avi->goto_next_audio_block(avi);
		block_sz = avi->get_curr_audio_block_size(avi);
	}
#endif	//DUMP_VIDEO
	
	avi_del(avi);
	
	return (0);
}
