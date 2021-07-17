/*********************************************************************
 * $file	:	libavi.c
 * $desc	:	parsing AVI format file
 * $author	:	rockins
 * $date	:	Mon Nov 26 02:50:17 CST 2007
 * $copyright	:	all copyrights(c) reserved by rockins.
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libavi.h"

// 
// compare two indices' offset
//	returns:
// 		1 if elem1's offset large than elem2's
// 		0 if equal
// 		-1 if less
//
int avi_idx_cmp(const void * elem1, const void * elem2)
{
	AVIIDX1ENTRY * e1 = (AVIIDX1ENTRY *)elem1;
	AVIIDX1ENTRY * e2 = (AVIIDX1ENTRY *)elem2;
	DWORD a = e1->dwOffset;
	DWORD b = e2->dwOffset;
	return (a > b) - (b > a);
}

//
// initializae: return 0
//
int
avi_init(AVI * avi)
{
	// clear aviheader
	memset(&avi->aviheader, 0, sizeof(AVIHEADER));

	return (0);
}

//
// parse:
// parse the AVI file
// returns:
//	0 if success, otherwise following error code returned.
//	AVI_ERR_READ		if read error
//	AVI_ERR_NOT_RIFF	if not RIFF chunk
//	AVI_ERR_NOT_AVI	if not AVI file
//	AVI_ERR_MEM		if memory error
//	AVI_ERR_BROKEN	if AVI file is damaged
//
int
avi_parse(AVI * avi)
{
	AVISTREAMHEADER	avi_streamheader;
	AVISUPERINDEX * s;
	char data[256];
	int len;
	int	file_len;
	AVI_FOURCC_TYPE fourcc_type = FOURCC_UNDEF;
	AVI_TWOCC_TYPE twocc_type = TWOCC_UNDEF;
	
	// get file's total length
	file_len = lseek(avi->fd, 0, SEEK_END);
	
	// reset input stream to beginning
	lseek(avi->fd, 0, SEEK_SET);

	// validate that the file is a RIFF AVI format file
	if ( read(avi->fd, data, 12) != 12 )
		return (AVI_ERR_READ);
	if ( !(strncasecmp(data, "RIFF", 4) == 0 ) )
		return (AVI_ERR_NOT_RIFF);
    if ( !(strncasecmp(data+8, "AVI ", 4) == 0) )
		return (AVI_ERR_NOT_AVI);
	
	do {
		// clear data and read
		len = 0;
		memset(data, 0, 256);
		if ( read(avi->fd, data, 8) != 8 )
			return (AVI_ERR_READ);
		
		fourcc_type = FOURCC_UNDEF;
		if ( strncasecmp(data, "LIST", 4) == 0 )			// list type
			fourcc_type = FOURCC_LIST;
		else if ( strncasecmp(data, "avih", 4) == 0 )	// following are all chunks type
			fourcc_type = FOURCC_avih;
		else if ( strncasecmp(data, "strh", 4) == 0 )
			fourcc_type = FOURCC_strh;
		else if ( strncasecmp(data, "strf", 4) == 0 )
			fourcc_type = FOURCC_strf;
		else if ( strncasecmp(data, "strd", 4) == 0 )
			fourcc_type = FOURCC_strd;
		else if ( strncasecmp(data, "strn", 4) == 0 )
			fourcc_type = FOURCC_strn;
		else if ( strncasecmp(data, "idx1", 4) == 0 )
			fourcc_type = FOURCC_idx1;
		else if ( strncasecmp(data, "indx", 4) == 0 )
			fourcc_type = FOURCC_indx;
		else if ( strncasecmp(data, "JUNK", 4) == 0 )
			fourcc_type = FOURCC_JUNK;
		else if ( strncasecmp(data, "RIFF", 4) == 0 )	// for OpenDML extension
			fourcc_type = FOURCC_RIFF;

		// main parse place
		switch (fourcc_type) {
		case FOURCC_LIST:
			// reserved chunk size, used by movi handler branch
			len = *(int *)(data + 4);
			ALIGN_EVEN(len);			// align chunk size to even border
			
			if ( read(avi->fd, data, 4) != 4 )
				return (AVI_ERR_READ);
			
			if ( strncasecmp(data, "hdrl", 4) == 0 )
				avi->aviheader.have_hdrl = 1;
			if ( strncasecmp(data, "strl", 4) == 0 )
				avi->aviheader.have_strl = 1;
			if ( strncasecmp(data, "rec ", 4) == 0 )
				avi->aviheader.have_rec = 1;
			if ( strncasecmp(data, "movi", 4) == 0 ) {
				avi->aviheader.have_movi = 1;
				
				// get movi offset, aligned
				if(!avi->aviheader.movi_offset){
					avi->aviheader.movi_offset = lseek(avi->fd, 0, SEEK_CUR);
					ALIGN_EVEN(avi->aviheader.movi_offset);
				}
				// get movi length
				avi->aviheader.movi_length = len;
				
				// if there are indicies at file end, just skip movi block
				if ( avi->aviheader.have_idx1 )
					lseek(avi->fd, len - 4, SEEK_CUR);
			}
			if (strncasecmp(data, "INFO", 4) == 0) {
				lseek(avi->fd, -12, SEEK_CUR);
				if ( read(avi->fd, data, 8) != 8 )
					return (AVI_ERR_READ);
				len = *(int *)(data + 4);
				ALIGN_EVEN(len);			// align chunk size to even border
				lseek(avi->fd, len, SEEK_CUR);
			}
			break;
		case FOURCC_avih:
			if ( avi->aviheader.have_hdrl ) {
				// back trace 8 bytes
				lseek(avi->fd, -8, SEEK_CUR);
				
				len = sizeof(AVIMAINHEADER);
				ALIGN_EVEN(len);			// align chunk size to even border
				if ( read(avi->fd, 
					(char *)&avi->aviheader.mainheader, 
					len) != len )
					return (AVI_ERR_READ);
				
				if ( avi->aviheader.mainheader.dwFlags & AMHF_HASINDEX)
					avi->aviheader.have_idx1 = 1;
			}
			break;
		case FOURCC_strh:
			if ( avi->aviheader.have_strl ) {
				// back trace 8 bytes
				lseek(avi->fd, -8, SEEK_CUR);
				
				len = sizeof(AVISTREAMHEADER);
				ALIGN_EVEN(len);			// align chunk size to even border
				if ( read(avi->fd, 
					(char *)&avi_streamheader, len) != len )
					return (AVI_ERR_READ);
				
				if ( strncasecmp(avi_streamheader.fccType, "vids", 4) == 0 )
					avi->aviheader.vid_streamheader = avi_streamheader;
				else if ( strncasecmp(avi_streamheader.fccType, "auds", 4) == 0 )
					avi->aviheader.aud_streamheader = avi_streamheader;
			}
			break;
		case FOURCC_idx1:
			// read in index
			len = *(int *)(data + 4);
			ALIGN_EVEN(len);			// align chunk size to even border
			avi->aviheader.idx1_table.idx1_head = (AVIIDX1ENTRY *)malloc(len);
			if ( avi->aviheader.idx1_table.idx1_head == NULL )
				return (AVI_ERR_MEM);
			if ( read(avi->fd, (char *)avi->aviheader.idx1_table.idx1_head, 
									len) != len )
				return (AVI_ERR_READ);
			avi->aviheader.idx1_table.idx1_length = len;
			avi->aviheader.idx1_table.idx1_count = len / sizeof(AVIIDX1ENTRY);
			avi->aviheader.have_idx1 = 1;
			break;
		case FOURCC_indx:
			// super index
			len = *(int *)(data + 4);
			ALIGN_EVEN(len);			// align chunk size to even border
			if (len < 24)
				break;
			avi->aviheader.index_table.index_count++;
			avi->aviheader.index_table.index_head = realloc(
					avi->aviheader.index_table.index_head,
					avi->aviheader.index_table.index_count * sizeof(AVISUPERINDEX));
			if (avi->aviheader.index_table.index_head == NULL)
				return (AVI_ERR_MEM);
			s = avi->aviheader.index_table.index_head + avi->aviheader.index_table.index_count -1; 
			memcpy(s->fcc, "indx", 4);
			s->cb = len ;
			if (read(avi->fd, ((char *)s)+8, 24)!=24)
				return (AVI_ERR_READ);
			memset(s->dwReserved, 0, 3 * 4);		// stuff dwReserved[3] of AVISUPERINDEX
			s->aIndex = calloc(s->nEntriesInUse, sizeof(avisuperindex_entry));
			s->stdidx = calloc(s->nEntriesInUse, sizeof(avistdindex_chunk));
			if (s->aIndex == NULL)
				return (AVI_ERR_MEM);
			if (read(avi->fd, (char *)s->aIndex, 
				s->nEntriesInUse * sizeof(avisuperindex_entry))
				!= s->nEntriesInUse * sizeof(avisuperindex_entry))
				return (AVI_ERR_READ);
			avi->aviheader.have_indx = 1 ;
			break;
		case FOURCC_RIFF:
			// another RIFF List, for OpenDML
			if (read(avi->fd, data, 4) != 4)
				return (AVI_ERR_READ);
			if (strncmp(data, "AVIX", 4))
				return (AVI_ERR_NOT_RIFF);
			else{
				/* We got an extended AVI header, so we need to switch to
		 		 * OpenDML to get seeking to work, provided we got indx chunks
		 		 * in the header(have_indx == 1)*/
				if( avi->aviheader.have_indx)
					avi->aviheader.isodml = 1 ;
			}
			break;
		case FOURCC_strf:
		case FOURCC_strd:
		case FOURCC_strn:
		case FOURCC_JUNK:	
		default:
			// skipped
			len = *(int *)(data + 4);
			ALIGN_EVEN(len);			// align chunk size to even border
			lseek(avi->fd, len, SEEK_CUR);
			break;
		}
	} while ( lseek(avi->fd, 0, SEEK_CUR) < file_len );
	
	// if neither idx1 nor indx, then generate idx1
	if (!avi->aviheader.have_idx1 && !avi->aviheader.have_indx) {
		//rewind the stream position to the beginning of the file
		lseek(avi->fd, 0, SEEK_SET);
		
		//seek to the begining of movi chunk
		lseek(avi->fd, avi->aviheader.movi_offset, SEEK_CUR);
		
		AVIIDX1ENTRY * curidx;
		char id[4];
		while (read(avi->fd, id, 4) >=0 && lseek(avi->fd, 0, SEEK_CUR) < 
				avi->aviheader.movi_offset + avi->aviheader.movi_length - 4){
			if (read(avi->fd, data, 4) != 4)
				return (AVI_ERR_READ);
			len = *(int*)data;
			avi->aviheader.idx1_table.idx1_head = realloc(
					avi->aviheader.idx1_table.idx1_head,
					avi->aviheader.idx1_table.idx1_length + sizeof(AVIIDX1ENTRY));
			if ( avi->aviheader.idx1_table.idx1_head != NULL )
				curidx = avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count;
			else
				return (AVI_ERR_MEM);
			memcpy(&curidx->dwChunkId, id, 4);	// FOURCC
			curidx->dwFlags = AVIIF_KEYFRAME;
			curidx->dwFlags |= (lseek(avi->fd, 0, SEEK_CUR)) >> 16 & 0xffff0000U;
			curidx->dwOffset = (unsigned int)(lseek(avi->fd, 0, SEEK_CUR) - 
							avi->aviheader.movi_offset - 4);	// offset relative to movi
			curidx->dwSize = len;
			avi->aviheader.idx1_table.idx1_count++;
			avi->aviheader.idx1_table.idx1_length += sizeof(AVIIDX1ENTRY);
			ALIGN_EVEN(len);
			lseek(avi->fd, len, SEEK_CUR);
		}
		avi->aviheader.have_idx1 = 1 ;
	}
	
	//deal with super index
	if (avi->aviheader.isodml && avi->aviheader.have_indx) {
		AVISUPERINDEX  * cx;
    	AVIIDX1ENTRY   * idx;
		int i, j;
		
		if (avi->aviheader.idx1_table.idx1_head)
			free(avi->aviheader.idx1_table.idx1_head);
		avi->aviheader.idx1_table.idx1_count = 0;
		avi->aviheader.idx1_table.idx1_head = NULL ;
		
		// read the standard indices
		for (cx = &avi->aviheader.index_table.index_head[0], i = 0;
				i <  avi->aviheader.index_table.index_count; cx++, i++) {
			for (j = 0; j < cx->nEntriesInUse; j++){
				int ret1, ret2;
				
				memset(&cx->stdidx[j], 0, 32);
				// reset input stream to beginning
				lseek(avi->fd, 0, SEEK_SET);
				ret1 = lseek(avi->fd, (DWORDLONG)cx->aIndex[j].qwOffset, SEEK_CUR);
				if ((ret2 = read(avi->fd, (char *)&cx->stdidx[j], 32)) != 32)
					return (AVI_ERR_READ);
				if (ret1 < 0 || cx->stdidx[j].nEntriesInUse == 0) {
					// this is a broken file (probably incomplete)
					avi->aviheader.isodml = 0;
					avi->aviheader.idx1_table.idx1_count = 0 ;
					return (AVI_ERR_BROKEN);
				}
				avi->aviheader.idx1_table.idx1_count += cx->stdidx[j].nEntriesInUse;
				cx->stdidx[j].aIndex = malloc(cx->stdidx[j].nEntriesInUse * sizeof(avistdindex_entry));
				if (cx->stdidx[j].aIndex == NULL)
				 	return (AVI_ERR_MEM);
				if (read(avi->fd, (char *)cx->stdidx[j].aIndex, 
					cx->stdidx[j].nEntriesInUse * sizeof(avistdindex_entry)) != 
					cx->stdidx[j].nEntriesInUse * sizeof(avistdindex_entry))
				 	return (AVI_ERR_READ);
				cx->stdidx[j].dwReserved3 = 0;
			}
		}
		
		/*
     		* convert the index by translating all entries into AVIIDX1ENTRYs
    		* and sorting them by offset.  
     		*/
     	avi->aviheader.idx1_table.idx1_head = malloc(
				avi->aviheader.idx1_table.idx1_count * sizeof(AVIIDX1ENTRY));
		idx = avi->aviheader.idx1_table.idx1_head;
		if (idx == NULL)
			return (AVI_ERR_MEM);
		for (cx = avi->aviheader.index_table.index_head; 
			cx != &avi->aviheader.index_table.index_head[avi->aviheader.index_table.index_count]; cx++) {
			avistdindex_chunk * sic;
			for (sic = cx->stdidx; sic != &cx->stdidx[cx->nEntriesInUse]; sic++){
				avistdindex_entry * sie;
				for (sie=sic->aIndex; sie != &sic->aIndex[sic->nEntriesInUse]; sie++){
					DWORDLONG offset = sie->dwOffset + sic->qwBaseOffset;
					memcpy(idx->dwChunkId, sic->dwChunkId, 4);
					idx->dwOffset = offset;
					idx->dwFlags = (offset >> 32) << 16;
					idx->dwSize = sie->dwSize  & 0x7fffffff;
					idx->dwFlags |= (sie->dwSize & 0x80000000)?0x0:AVIIF_KEYFRAME;
					idx++;
				}
			}
		}
		
		// sorting in offset
		qsort(avi->aviheader.idx1_table.idx1_head, avi->aviheader.idx1_table.idx1_count,
			sizeof(AVIIDX1ENTRY), avi_idx_cmp);
	}
	
	return (0);
}

//
// go to first video block
// return 0 if found, else -1
//
int
avi_goto_first_video_block(AVI * avi)
{
	AVIIDX1ENTRY	* pIdx1Entry;
	FOURCC			four_cc;
	
	if ( avi->aviheader.have_idx1 || avi->aviheader.have_indx) {
		pIdx1Entry = avi->aviheader.idx1_table.idx1_head;
		memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
		
		while ( ! ( (four_cc[2] == 'd' && four_cc[3] == 'b') ||
					(four_cc[2] == 'd' && four_cc[3] == 'c') ) ) {
			if ( pIdx1Entry++ < avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count )
				memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
		}
		
		if ( pIdx1Entry >= avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count ) {
			avi->aviheader.idx1_table.current_vid_idx = NULL;
			return (-1);
		} else {
			avi->aviheader.idx1_table.current_vid_idx = pIdx1Entry;		// that's it
			return (0);
		}
	} else
		return (AVI_ERR_NOT_AVI);
	
	return (-1);
}

//
// go to first audio block
// return 0 if found, else -1
//
int
avi_goto_first_audio_block(AVI * avi)
{
	AVIIDX1ENTRY	* pIdx1Entry;
	FOURCC			four_cc;
	
	if ( avi->aviheader.have_idx1 || avi->aviheader.have_indx) {
		pIdx1Entry = avi->aviheader.idx1_table.idx1_head;
		memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
		
		while ( ! (four_cc[2] == 'w' && four_cc[3] == 'b') ) {
			if ( pIdx1Entry++ < avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count )
				memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
		}
		
		if ( pIdx1Entry >= avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count ) {
			avi->aviheader.idx1_table.current_aud_idx = NULL;
			return (-1);
		} else {
			avi->aviheader.idx1_table.current_aud_idx = pIdx1Entry;		// that's it
			return (0);
		}
	} else
		return (AVI_ERR_NOT_AVI);
	
	return (-1);
}

//
// step to next video block
// 	returns: 1 if next video block exist, otherwise 0
//
int
avi_goto_next_video_block(AVI * avi)
{
	AVIIDX1ENTRY	* pIdx1Entry;
	FOURCC			four_cc;
	
	pIdx1Entry = avi->aviheader.idx1_table.current_vid_idx + 1;
	if (pIdx1Entry < avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count) {
		memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
		
		while ( ! ( (four_cc[2] == 'd' && four_cc[3] == 'b') ||
					(four_cc[2] == 'd' && four_cc[3] == 'c') ) ) {
			if (pIdx1Entry++ < avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count)
				memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
			else
				break;
		}
		
		if (pIdx1Entry >= avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count) {
			avi->aviheader.idx1_table.current_vid_idx = NULL;
		} else {
			avi->aviheader.idx1_table.current_vid_idx = pIdx1Entry;	// that's it, update it
		}
	} else {
		avi->aviheader.idx1_table.current_vid_idx = NULL;
	}
	
	return (avi->aviheader.idx1_table.current_vid_idx?1:0);
}

//
// step to next audio block
// 	returns: 1 if next audio block exist, otherwise 0
//
int
avi_goto_next_audio_block(AVI * avi)
{
	AVIIDX1ENTRY	* pIdx1Entry;
	FOURCC			four_cc;
	
	pIdx1Entry = avi->aviheader.idx1_table.current_aud_idx + 1;
	if (pIdx1Entry < avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count) {
		memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
		
		while ( ! (four_cc[2] == 'w' && four_cc[3] == 'b') ) {
			if (pIdx1Entry++ < avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count)
				memcpy(four_cc, &pIdx1Entry->dwChunkId, 4);
			else
				break;
		}
		
		if (pIdx1Entry >= avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count) {
			avi->aviheader.idx1_table.current_aud_idx = NULL;
		} else {
			avi->aviheader.idx1_table.current_aud_idx = pIdx1Entry;	// that's it, update it
		}
	} else {
		avi->aviheader.idx1_table.current_aud_idx = NULL;
	}
	
	return (avi->aviheader.idx1_table.current_aud_idx?1:0);
}

//
// get current video block size:
// return block size in byte if success, else -1
//
int
avi_get_curr_video_block_size(AVI * avi)
{
	if ( avi->aviheader.have_idx1 || avi->aviheader.have_indx ) {
		if ( avi->aviheader.idx1_table.current_vid_idx == NULL ||
			 avi->aviheader.idx1_table.current_vid_idx >= 
			 avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count) {
			avi->aviheader.idx1_table.current_vid_idx == NULL;
			return (-1);
		} else
			return ( avi->aviheader.idx1_table.current_vid_idx->dwSize );
	}
	
	return (-1);
}

//
// get current audio block size
// return block size in byte if success, else -1
//
int
avi_get_curr_audio_block_size(AVI * avi)
{
	if ( avi->aviheader.have_idx1 || avi->aviheader.have_indx) {
		if ( avi->aviheader.idx1_table.current_aud_idx == NULL ||
			 avi->aviheader.idx1_table.current_aud_idx >= 
			 avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count) {
			avi->aviheader.idx1_table.current_aud_idx == NULL;
			return (-1);
		} else
			return ( avi->aviheader.idx1_table.current_aud_idx->dwSize );
	}
	
	return (-1);
}

//
// get current video block
// return actual size if success, else -1
//
int
avi_get_curr_video_block(AVI * avi, char * video_block, int len)
{
	FOURCC			four_cc;
	int				ret;
	
	if (avi->aviheader.have_idx1 || avi->aviheader.have_indx) {
		// validate
		if ( avi->aviheader.idx1_table.current_vid_idx == NULL ||
			 avi->aviheader.idx1_table.current_vid_idx->dwSize > len ||
			 avi->aviheader.idx1_table.current_vid_idx >= 
			 avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count )
			return (-1);
		
		// seek and read video block
		lseek(avi->fd, (1 - avi->aviheader.have_indx) * (avi->aviheader.movi_offset + 4) +
				avi->aviheader.idx1_table.current_vid_idx->dwOffset, SEEK_SET);
		ret = read(avi->fd, video_block, 
					avi->aviheader.idx1_table.current_vid_idx->dwSize);
					
		return (ret);
	}
	
	return (-1);
}

//
// get current audio block 
// return actual size in byte if success, else -1
//
int
avi_get_curr_audio_block(AVI * avi, char * audio_block, int len)
{
	FOURCC			four_cc;
	int				ret;
	
	if (avi->aviheader.have_idx1 || avi->aviheader.have_indx) {
		// validate
		if ( avi->aviheader.idx1_table.current_aud_idx == NULL ||
			 avi->aviheader.idx1_table.current_aud_idx->dwSize > len ||
			 avi->aviheader.idx1_table.current_aud_idx >= 
			 avi->aviheader.idx1_table.idx1_head + avi->aviheader.idx1_table.idx1_count )
			return (-1);
		
		// seek and read audio block
		lseek(avi->fd, (1 - avi->aviheader.have_indx) * (avi->aviheader.movi_offset + 4) +
				avi->aviheader.idx1_table.current_aud_idx->dwOffset, SEEK_SET);
		ret = read(avi->fd, audio_block, 
					avi->aviheader.idx1_table.current_aud_idx->dwSize);
		
		return (ret);
	}
	
	return (-1);
}


/************ constructor & deconstructor ******************/
//
// constructor:
//	param: file - input file's name
//	return: an AVI object if success, otherwise NULL
//
AVI * avi_new(char * file)
{
	AVI * avi;
	
	avi = (AVI *)malloc(sizeof(AVI));
	if (avi) {
		avi->file_name = file;
		avi->fd = open(file, O_RDONLY);
	}
	
	// install member functions
	avi->init = avi_init;
	avi->parse = avi_parse;
	avi->goto_first_video_block = avi_goto_first_video_block;
	avi->goto_first_audio_block = avi_goto_first_audio_block;
	avi->goto_next_video_block = avi_goto_next_video_block;
	avi->goto_next_audio_block = avi_goto_next_audio_block;
	avi->get_curr_video_block_size = avi_get_curr_video_block_size;
	avi->get_curr_audio_block_size = avi_get_curr_audio_block_size;
	avi->get_curr_video_block = avi_get_curr_video_block;
	avi->get_curr_audio_block = avi_get_curr_audio_block;
	
	return (avi?avi:NULL);
}

//
// deconstructor
//	param: an AVI object
//
void avi_del(AVI * avi)
{
	if (avi) {
		close(avi->fd);
		free(avi);
	}
}
