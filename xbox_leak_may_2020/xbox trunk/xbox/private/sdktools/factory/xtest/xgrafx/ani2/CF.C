/*************************************************************************
*                                                                        *
* 	Here live the sound call processing functions			  *
*                                                                        *
*************************************************************************/

#include "sos.h"
#include "protos.h"
#include "externs.h"
#include <dsound.h>

extern	unsigned char	sound_call;	
extern struct timer_music music_timers[];
extern	unsigned char	n_timers;


#ifdef	BIGOS
unsigned char	restore_volume;

#endif

/*************************************************************************
*                                                                        *
* 	New Music Call:							  *
*	Note that if no tracks are specified, this is a silence call	  *
*									  *
* 	1.  Remove all current process for this level			  *
* 	2.  For each track in the new sound, create a new process	  *
* 	    for it.							  *
*                                                                        *
*************************************************************************/


	
void call_silence()
{
	struct process *tpp;
	struct process *pp;
	uchar	i;

	pp = process_queue->next;
	while (pp != NULL) {
	    tpp = pp->next;
	    kill_process(pp);
	    pp = tpp;
	}
	current_timer_priority = 0;
	for (i = 0; i < max_tracks; i++) {
	    channel_level[i] = 0;
		init_track_status(LEVEL_MUSIC,i);
		init_track_status(LEVEL_EFFECT,i);
	    nosound(i);
	}

	user_silence_function();
}


void call_music()
{
	ushort	const **track_ptr;		/* pointer to addr's of tracks */
	uchar	i;
	ushort	mask;
	ushort	track_map;

	marker = 0;	
	last_music_call = sound_call;

	remove_processes_by_level(LEVEL_MUSIC);
	track_map = current_call->track_map;
	track_ptr = current_call->tbl_ptr;
	for (i = 0,mask = 1; i < max_tracks; i++,mask <<= 1) {
	    if (track_map & mask) {	/* got a track to pl*/
			init_track_status(LEVEL_MUSIC,i);
			create_process((ushort *)*(track_ptr++),LEVEL_MUSIC,i);	
			if (channel_level[i] < LEVEL_EFFECT) {
				nosound(i);
			    channel_level[i] = LEVEL_MUSIC;
			}
	    }
	}

}

void call_play_timer()
{

}

void call_timer()
{

}

void call_end_timer()
{


}
void call_ignore()
{

}

extern	flash_yellow();
void call_effect()
{
	ushort	const **track_ptr;		/* pointer to addr's of tracks */
	uchar	i;
	ushort	mask;
	ushort	track_map;

	track_ptr = current_call->tbl_ptr;
	track_map = current_call->track_map;
	for (i = 0,mask = 1; i < max_tracks; i++,mask <<= 1) {
	    if (track_map & mask) {	/* got a track to pl*/
			remove_processes_by_level_and_channel(LEVEL_EFFECT,i);
			channel_level[i] = LEVEL_EFFECT;
			init_track_status(LEVEL_EFFECT,i);
			create_process((ushort *)*(track_ptr++),LEVEL_EFFECT,i);
	    }
	}
}

/*************************************************************************
*                                                                        *
* 	kill fx on all tracks											  *
*                                                                        *
*************************************************************************/

void call_kill_effect()
{

}



/*************************************************************************
*                                                                        *
* 	Send a byte to the yamaha										  *
* 	WARNING:  This routine blocks until it recieves both the		  *
* 	address AND data												  *
*                                                                        *
*************************************************************************/
#define	CALL_WAITING  (fifo.fifo_read != fifo.fifo_write)

void call_senddev()
{
#ifdef	BIGOS

	while (!CALL_WAITING)
		;
	b_value = get_fifo();
	while (!CALL_WAITING)
		;
	a_value = get_fifo();
	send_dev_function();
#endif
}
/*************************************************************************
*                                                                        *
* 	VOLUME SOUND CALL						  *
* 	byte 1:("type")	       	3 -> volume call			  *
* 	byte 2:("priority")	1 -> music volume; 2 -> fx volume	  *
* 	byte 3,4:("track_map")	level 					  *
* 	byte 5,6:		not used (MUST BE THERE)		  *
*                                                                        *
*************************************************************************/

void call_volume()
{

}


/************************************************************************
*                                                                      *
* 	MARK SOUND CALL
* 	byte 1:("type")	       	6 -> mark call							*
* 	byte 2:("priority")	marker #
* 	byte 3,4:("track_map")	NOT USED				  				*
* 	byte 5,6:		not used (MUST BE THERE)		  				*
*                                                                      *
*************************************************************************/

void call_mark()
{
	marker = current_call->priority;
}
		
init_track_status(uchar level, uchar channel)
{
	struct	track_info	*ti;
	ti = get_track_info_block2(level,channel);
/*	ti = get_track_info_block();*/
	
	ti->patch = 0;
	ti->loop_level = 0;
	ti->mux_level = 0;
	ti->transpose = 0;
	ti->filtercutoff = 0;
	ti->volume = 127;  /* */
	ti->pan = 0;
}


/*
 *	stop sound on channel chan
 */

void nosound(uchar chan)
{
	if (chan < max_tracks)
		silence_functions[chan](chan);
}

/*
 *	Remove all processes of level, 'level' from process queue
 */


remove_processes_by_level(uchar level)
{
	struct process *tpp;
	struct process *pp;

	pp = process_queue->next;

	while (pp != NULL) {
	    tpp = pp->next;
	    if (pp->level == level)
	        kill_process(pp);
	    pp = tpp;
	}
}
void remove_processes_by_level_and_channel(uchar level, uchar channel)
{
	struct process *tpp;
	struct process *pp;

	pp = process_queue->next;

	while (pp != NULL) {
	    tpp = pp->next;
	    if (pp->level == level) {
			if (pp->hard_channel == channel) {
		    	kill_process(pp);
			}
	    }
	    pp = tpp;
	}
}
