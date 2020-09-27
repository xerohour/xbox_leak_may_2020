/*
 *	Sound Operating System
 */



#include "sos.h"
#include "protos.h"
#include "ftables.h"			/* event/call function tables */

#include "externs.h"
#include <xtl.h>

#include <stdio.h>


unsigned char	sound_call;

//WCHAR	StringBuffer[256];

//extern	uchar	max_tracks;
extern	uchar	max_sound_call;
extern	ushort	default_clock_value;
extern	const ushort	max_processes;
extern	_base_	uchar	gtifbtmp;

#define	c_p	current_process

/* 
 * boolean T if a sound call is in buffer 
 */
#define	CALL_WAITING  (fifo.fifo_read != fifo.fifo_write)
/*
 * return the sound call in the buffer 
 */


/*************************************************************************
*                                                                        *
* 	Call main if we are going to do sos only						  *
* 	call do_sos_init_return followed by repeated calls to			  *
* 	sos_main if we are just part of another program					  *
*                                                                        *
*************************************************************************/

void	sos_main(void)
{
	struct	process	*tpp;
	do {
	    if (CALL_WAITING) {
			sound_call = get_fifo();
#if DBG
//			swprintf( StringBuffer, L"Current Sound: %d", sound_call);
#endif
			if (sound_call > max_sound_call)
		   	 	continue;
			current_call = &sound_calls[sound_call];
			call_fcns[current_call->type]();
	    }

		current_process = process_queue->next;
		while (current_process != (struct process *)NULL) {
		    tpp = current_process->next;
//			DINT;

				c_p->timer = c_p->timer +c_p->prev_timer - system_clock_music;
				current_process->prev_timer = system_clock_music;


//			EINT;
		    if (current_process->timer < 0) {
				current_channel = current_process->hard_channel;
				current_level = current_process->level;
/*				gtifbtmp = current_channel + current_level*max_tracks;*/
				if (current_level == 0)
					gtifbtmp = current_channel;
				else
					gtifbtmp = current_channel + max_tracks;
				ti = &(track_status[gtifbtmp]);
				if (event_fcns[current_process->function]() == 0) {
					current_process = process_queue->next;
					break;
				}
		    }
		    current_process = tpp;
		}
	} while (sos_only);
}



void	do_sos_init_return()
{

	sos_only = 0;
	fifo.fifo_read = 0;	
	fifo.fifo_write = 0;	
	dev_init();
	init_queuelist();
//	clock_cntr_value[0] = default_clock_value;
	system_clock_music = 1;
	sound_call_table = 0;
}

/*************************************************************************
*                                                                        *
* 	Put the value, value in the receive fifo			  *
*                                                                        *
*************************************************************************/

void put_fifo(uchar value)
{
	fifo.data[fifo.fifo_write++] = value;
	if (fifo.fifo_write >= FIFO_SIZE)
	    fifo.fifo_write = 0;
}

/*************************************************************************
*                                                                        *
* 	Get the next byte in the receive fifo				  *
*                                                                        *
*************************************************************************/
uchar	get_fifo(void)
{
	uchar	retval;

	retval = fifo.data[fifo.fifo_read++];
	if (fifo.fifo_read >= FIFO_SIZE)
	    fifo.fifo_read = 0;
	return(retval);
}
init_queuelist()
{
	uchar	i;
	queue_list[0].prev = (struct process *)NULL;
	queue_list[0].next = &(queue_list[1]);
	for (i = 1; i < max_processes - 1; i++) {
	    queue_list[i].next = &(queue_list[i+1]);	
	    queue_list[i].prev = &(queue_list[i-1]);
	}
	queue_list[max_processes-1].next = (struct process *)NULL;
	queue_list[max_processes-1].prev = &(queue_list[max_processes-2]);

	if ((process_queue = get_process_packet()) == NULL) {
//	    sos_error(SOS_NO_MEM);
	    return(0);
	}
	(*process_queue).next = (struct process *)NULL;
	process_queue->prev = (struct process *)NULL;
	for (i = 0; i < max_tracks; i++)
	    channel_level[i] = 0;
	return(0);
}


void	sos_error(int n)
{

}


/*************************************************************************
*                                                                        *
* 	Utilities for SOS						  *
*                                                                        *
*************************************************************************/



struct track_info *get_track_info_block()
{
	gtifbtmp = (current_level)*max_tracks + current_channel;	
	return(&(track_status[gtifbtmp]));
}
struct track_info *get_track_info_block2(uchar level, uchar chan)
{
	gtifbtmp = (level)*max_tracks + chan;	
	return(&(track_status[gtifbtmp]));
}

struct track_info *get_music_info_block()
{
#if LEVEL_MUSIC==0
	return(&(track_status[current_channel]));
#else
	gtifbtmp = (LEVEL_MUSIC)*max_tracks + current_channel;	
	return(&(track_status[gtifbtmp]));
#endif
}

	
struct track_info *get_music_info_block_i(uchar i)
{
#if LEVEL_MUSIC==0
	return(&(track_status[i]));
#else
	gtifbtmp = (LEVEL_MUSIC)*max_tracks + i;
	return(&(track_status[gtifbtmp]));
#endif
}
