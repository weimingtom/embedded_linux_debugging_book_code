/***************************************************************************
 * 	core assembler (C) 2008 Rockins Chen
 *
 * Note!!! TODO list:
 *
 * 1.
 * avi_new() should just allocate an avi object, it shouldn't accept a file
 * name as parameter; [abort]
 *
 * 2.
 * event_new() and event_del() should change name to evt_new() and evt_del(); [done]
 *
 * 3.
 * add pingpong buffer management routine. [done]
 *
 * 4.
 * permit closing SDL window to exit normally. [done]
 *
 * 5.
 * strip redundant buffer allocation. [done]
 *
 * 6.
 * add cleanup support at exit. [done]
 *
 * Bug report:Rockins Chen<ybc2084@163.com>
 *
 * All Copyrights reserved.
 *
 * $Id: core.c 9 2008-02-17 08:09:01Z rockins $
 *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "libavi.h"
#include "common.h"
#include "decode.h"
#include "output.h"
#include "event.h"
#include "pingpong_buf_mgmt.h"

int main(int argc, char *argv[])
{
	AVI					*theAvi;
	event_t				*theDecToOutEvt;
	event_t				*theOutToDecEvt;
	decode_t			*theDec;
	output_t			*theOut;
	pingpong_buf_mgmt_t	*thePpbm;
	unsigned char		*thePing;
	unsigned char		*thePong;
	PtExitCodeType		*theDecThreadExitCode;
	PtExitCodeType		*theOutThreadExitCode;

	/*
	 * validate command line
	 */
	if (argc < 2) {
		fprintf(stderr, "usage: %s an_avi_file\n", argv[0]);
		return (-1);
	}

	/*
	 * create objects
	 */
	theAvi 			= avi_new(argv[1]);
	theDecToOutEvt 	= evt_new();
	theOutToDecEvt 	= evt_new();
	theDec 			= dec_new();
	theOut 			= out_new();
	thePpbm			= ppbm_new();

	/*
	 * allocate memory for outputing
	 */
	thePing = malloc(DFL_DEC_OUT_BUF_SZ);
	thePong = malloc(DFL_DEC_OUT_BUF_SZ);	
	assert(thePing);
	assert(thePong);
	memset(thePing, 0, DFL_DEC_OUT_BUF_SZ);
	memset(thePong, 0, DFL_DEC_OUT_BUF_SZ);

	/*
	 * init pingpong management routine
	 */
	thePpbm->init(thePpbm, thePing, thePong, DFL_DEC_OUT_BUF_SZ);

	/*
	 * init event objects
	 */
	theDecToOutEvt->init(theDecToOutEvt);
	theOutToDecEvt->init(theOutToDecEvt);

	/*
	 * init and startup outputing thread
	 */
	theOut->Init(theOut, theDecToOutEvt, theOutToDecEvt, thePpbm);
	theOut->MainLoop(theOut);

	/*
	 * init and startup decoding thread
	 */
	theDec->Init(theDec, theAvi, theDecToOutEvt, theOutToDecEvt, thePpbm);
	theDec->MainLoop(theDec);

	/*
	 * try to join threads
	 */
	(void) pthread_join(theDec->pt_id, (void **)&theDecThreadExitCode);
	(void) pthread_join(theOut->pt_id, (void **)&theOutThreadExitCode);
	
	if (*theDecThreadExitCode == PT_EXIT_NORMAL && 
		*theOutThreadExitCode == PT_EXIT_NORMAL)
		printf("ccplayer terminate normally\n");
	else if (*theDecThreadExitCode == PT_EXIT_ABNORMAL ||
			 *theOutThreadExitCode == PT_EXIT_ABNORMAL)
		printf("ccplayer terminate abnormally\n");

	/*
	 * de-construct objects
	 */
	ppbm_del(thePpbm);
	out_del(theOut);
	dec_del(theDec);
	evt_del(theOutToDecEvt);
	evt_del(theDecToOutEvt);
	avi_del(theAvi);

	return (0);
}

