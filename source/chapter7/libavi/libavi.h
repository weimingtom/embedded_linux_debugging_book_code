/*********************************************************************
 * $file	:	libavi.h
 * $desc	:	parsing AVI format file
 * $author	:	rockins
 * $date	:	Mon Nov 26 02:46:53 CST 2007
 * $copyright	:	all copyrights(c) reserved by rockins.
 **********************************************************************/
#ifndef	_LIB_AVI_H_
#define	_LIB_AVI_H_

#ifdef __cplusplus
	extern "C" {
#endif

/********************************** error code *************************/
#define	AVI_ERR_READ		0x00000001	// read error
#define	AVI_ERR_MEM			0x00000002	// memory access error
#define	AVI_ERR_NOT_RIFF	0x00000003	// not RIFF
#define	AVI_ERR_NOT_AVI		0x00000004	// not AVI file
#define AVI_ERR_BROKEN		0x00000005	// broken AVI file

#define AVIIF_KEYFRAME      0x00000010L	// indicate a keyfram

// chunk size must be aligned even
#define	ALIGN_EVEN(x)	x = (((x) & 0x01) ? ((x)+1) : (x))

/***************************** typedef ***********************/
typedef	unsigned char 		FOURCC[4];
typedef	unsigned short		WORD;
typedef	unsigned int		DWORD;
typedef unsigned long long 	DWORDLONG;

// FOURCC
typedef enum {
	FOURCC_UNDEF = 0,
	FOURCC_RIFF,
	FOURCC_LIST,
	FOURCC_JUNK,
	FOURCC_AVI,
	FOURCC_AVIX,
	FOURCC_WAVE,
	FOURCC_INFO,
	FOURCC_avih,
	FOURCC_hdrl,
	FOURCC_movi,
	FOURCC_idx1,
	FOURCC_strl,
	FOURCC_strh,
	FOURCC_strf,
	FOURCC_strd,
	FOURCC_strn,
	FOURCC_indx,
	FOURCC_rec,
	FOURCC_auds,
	FOURCC_vids,
	FOURCC_txts,
	FOURCC_mids,
	
	// following fourcc are used to identify subchunks of INFO chunk, 
	// they are ignored in this library.
	FOURCC_IARL,
	FOURCC_IART,
	FOURCC_ICMS,
	FOURCC_ICMT,
	FOURCC_ICOP,
	FOURCC_ICRD,
	FOURCC_ICRP,
	FOURCC_IDIM,
	FOURCC_IDPI,
	FOURCC_IENG,
	FOURCC_IGNR,
	FOURCC_IKEY,
	FOURCC_ILGT,
	FOURCC_IMED,
	FOURCC_INAM,
	FOURCC_IPLT,
	FOURCC_IPRD,
	FOURCC_ISBJ,
	FOURCC_ISFT,
	FOURCC_ISHP,
	FOURCC_ISRC,
	FOURCC_ISRF,
	FOURCC_ITCH,
	FOURCC_ISMP,
	FOURCC_IDIT,
}AVI_FOURCC_TYPE;

// TWOCC
typedef enum {
	TWOCC_UNDEF = 0,
	TWOCC_wb,
	TWOCC_db,
	TWOCC_dc,
	TWOCC_pc,
}AVI_TWOCC_TYPE;


// flags for dwFlags field in AVIMAINHEADER
// prefix AMHF denoting Avi Main Header Flag
#define	AMHF_HASINDEX		0x00000010
#define	AMHF_MUSTUSEINDEX	0x00000020
#define	AMHF_ISINTERLEAVED	0x00000100
#define	AMHF_TRUSTCKTYPE	0x00000800  /* Use CKType to find key frames? */
#define	AMHF_WASCAPTUREFILE	0x00010000
#define	AMHF_COPYRIGHTED	0x00020000
typedef struct _avimainheader {
    FOURCC 		fcc;   					// 必须为‘avih’
    DWORD  		cb;    					// 本数据结构的大小，不包括最初的8个字节（fcc和cb两个域）
    DWORD  		dwMicroSecPerFrame;   	// 视频帧间隔时间（以毫秒为单位）
    DWORD  		dwMaxBytesPerSec;     	// 这个AVI文件的最大数据率
    DWORD		dwPaddingGranularity; 	// 数据填充的粒度
    DWORD  		dwFlags;         		// AVI文件的全局标记，比如是否含有索引块等
    DWORD  		dwTotalFrames;   		// 总帧数
    DWORD  		dwInitialFrames; 		// 为交互格式指定初始帧数（非交互格式应该指定为0）
    DWORD  		dwStreams;       		// 本文件包含的流的个数
    DWORD  		dwSuggestedBufferSize; 	// 建议读取本文件的缓存大小（应能容纳最大的块）
    DWORD  		dwWidth;         		// 视频图像的宽（以像素为单位）
    DWORD  		dwHeight;        		// 视频图像的高（以像素为单位）
    DWORD  		dwReserved[4];   		// 保留
}AVIMAINHEADER;

typedef struct _avistreamheader {
	FOURCC 		fcc;  					// 必须为‘strh’
	DWORD  		cb;   					// 本数据结构的大小，不包括最初的8个字节（fcc和cb两个域）
	FOURCC 		fccType;    			// 流的类型：‘auds’（音频流）、‘vids’（视频流）、‘mids’（MIDI流）、‘txts’（文字流）
	FOURCC 		fccHandler; 			// 指定流的处理者，对于音视频来说就是解码器
	DWORD  		dwFlags;    			// 标记：是否允许这个流输出？调色板是否变化？
	WORD   		wPriority;  			// 流的优先级（当有多个相同类型的流时优先级最高的为默认流）
	WORD   		wLanguage;
	DWORD  		dwInitialFrames; 		// 为交互格式指定初始帧数
	DWORD  		dwScale;   				// 这个流使用的时间尺度
	DWORD  		dwRate;
	DWORD  		dwStart;   				// 流的开始时间
	DWORD  		dwLength;  				// 流的长度（单位与dwScale和dwRate的定义有关）
	DWORD  		dwSuggestedBufferSize; 	// 读取这个流数据建议使用的缓存大小
	DWORD  		dwQuality;    			// 流数据的质量指标（0 ~ 10,000）
	DWORD  		dwSampleSize; 			// Sample的大小
	struct {
		short int left;
		short int top;
		short int right;
		short int bottom;
	}rcFrame;  							// 指定这个流（视频流或文字流）在视频主窗口中的显示位置
										// 视频主窗口由AVIMAINHEADER结构中的dwWidth和dwHeight决定
}AVISTREAMHEADER;

// flags for dwFlags field of AVIIDX1ENTRY
// prefix AIEF denoting Avi Index Entry Flag
#define	AIEF_LIST			0x00000001L		// indexed chunk is a list
#define	AIEF_KEYFRAME		0x00000010L		// indexed chunk is a key frame
#define	AIEF_NOTIME			0x00000100L		// indexed chunk frame do not consume any time
#define	AIEF_COMPUSE		0x0FFF0000L		// these bits are used by compressor
#define	AIEF_FIXKEYFRAME	0x00001000L		// XXX: borrowed from VLC, avoid using
typedef struct _aviidx1_entry {
    FOURCC  	dwChunkId;   			// 表征本数据块的四字符码,eg, 01wb, 00dc, et al
    DWORD   	dwFlags;     			// 说明本数据块是不是关键帧、是不是‘rec ’列表等信息
    DWORD   	dwOffset;    			// 本数据块在文件中的偏移量，可能相对于文件开头，也可能相对于movi块
    DWORD   	dwSize;      			// 本数据块的大小
}AVIIDX1ENTRY;

typedef struct _avisuperindex_entry {
    DWORDLONG 	qwOffset;           	// absolute file offset
    DWORD 		dwSize;             	// size of index chunk at this offset
    DWORD 		dwDuration;         	// time span in stream ticks
} avisuperindex_entry;

typedef struct _avistdindex_entry {
    DWORD 		dwOffset;           	// qwBaseOffset + this is absolute file offset
    DWORD 		dwSize;             	// bit 31 is set if this is NOT a keyframe
} avistdindex_entry;

typedef struct _avistdindex_chunk {
    FOURCC 		fcc;       				// ix##
    DWORD 		dwSize;            		// size of this chunk
    WORD 		wLongsPerEntry;     	// must be sizeof(aIndex[0])/sizeof(DWORD)
    char  		bIndexSubType;      	// must be 0
    char  		bIndexType;         	// must be AVI_INDEX_OF_CHUNKS
    DWORD 		nEntriesInUse;     		// first unused entry
    FOURCC 		dwChunkId; 				// '##dc' or '##db' or '##wb' etc..
    DWORDLONG 	qwBaseOffset;       	// all dwOffsets in aIndex array are relative to this
    DWORD 		dwReserved3;       		// must be 0
    avistdindex_entry *aIndex;   		// the actual frames
} avistdindex_chunk;

typedef struct _avisuperindex {
   FOURCC   	fcc;               		// 'indx'
   DWORD 		cb;                		// size of this structure
   WORD     	wLongsPerEntry;    		// ==4
   char     	bIndexSubType;     		// ==0 (frame index) or AVI_INDEX_SUB_2FIELD 
   char    		bIndexType;        		// ==AVI_INDEX_OF_INDEXES
   DWORD    	nEntriesInUse;     		// offset of next unused entry in aIndex
   DWORD    	dwChunkId;         		// chunk ID of chunks being indexed, (i.e. RGB8)
   DWORD    	dwReserved[3];     		// must be 0
   avisuperindex_entry *aIndex;			// index to std indices
   avistdindex_chunk *stdidx;       	// the actual std indices
} AVISUPERINDEX;

/*********************************customarized defines*************************************/
typedef struct _aviidx1_table {
	AVIIDX1ENTRY	* idx1_head;
	AVIIDX1ENTRY	* current_vid_idx;	// for traversing
	AVIIDX1ENTRY	* current_aud_idx;
	int				idx1_count;
	int				idx1_length;
}AVIIDX1TABLE;

typedef struct _aviindex_table{
	AVISUPERINDEX    *index_head;
	int 			index_count;
}AVIINDXTABLE;

typedef struct _aviheader {

	// structure for maintaining and accessing
	AVIMAINHEADER	mainheader;
	AVISTREAMHEADER	vid_streamheader;
	AVISTREAMHEADER	aud_streamheader;
	AVIIDX1TABLE	idx1_table;
	AVIINDXTABLE   	index_table;
	int				movi_offset;
	int				movi_length;
	
	// some indicator
	int 			have_hdrl;
	int 			have_strl;
	int 			have_movi;
	int 			have_idx1;
	int 			have_rec;
	int 			have_indx;
	int 			isodml;		// is OpenDML extension
}AVIHEADER;

/**************************** AVI object ***************************/
typedef struct _avi {
	char			* file_name;	// avi file's name
	int				fd;				// avi file's descriptor
	
	AVIHEADER		aviheader;
	
	int	(*init)(struct _avi * avi);
	int (*parse)(struct _avi * avi);
	
	int (*goto_first_video_block)(struct _avi * avi);
	int (*goto_first_audio_block)(struct _avi * avi);
	
	int (*goto_next_video_block)(struct _avi * avi);
	int	(*goto_next_audio_block)(struct _avi * avi);
	
	int (*get_curr_video_block_size)(struct _avi * avi);
	int (*get_curr_audio_block_size)(struct _avi * avi);
	
	int (*get_curr_video_block)(struct _avi * avi, char * video_block, int len);
	int (*get_curr_audio_block)(struct _avi * avi, char * audio_block, int len);
}AVI;


/***************************** real interface ***************************/
static int	avi_init(AVI * avi);
static int	avi_parse(AVI * avi);
static int	avi_goto_first_video_block(AVI * avi);
static int	avi_goto_first_audio_block(AVI * avi);
static int	avi_goto_next_video_block(AVI * avi);
static int	avi_goto_next_audio_block(AVI * avi);
static int	avi_get_curr_video_block_size(AVI * avi);
static int	avi_get_curr_audio_block_size(AVI * avi);
static int	avi_get_curr_video_block(AVI * avi, char * video_block, int len);
static int	avi_get_curr_audio_block(AVI * avi, char * audio_block, int len);

/************ constructor & deconstructor ******************/
AVI * avi_new(char * file);
void avi_del(AVI * avi);

#ifdef __cplusplus
	};
#endif	// __cplusplus

#endif	//_LIB_AVI_H_
