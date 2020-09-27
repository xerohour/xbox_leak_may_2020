/*************************************************************************
*                                                                        *
* 	Function tables for event functions and sound call functions	  *
*                                                                        *
*************************************************************************/

#ifdef STARTUPANIMATION

#pragma data_seg("INIT_RW")
#pragma code_seg("INIT")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")

// Tell linker to put bootsound code and data into INIT section
#pragma comment(linker, "/merge:DSOUND=INIT")

#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")

#endif //STARTUPANIMATION

extern int	f_note(void);
extern int	f_rest(void);
extern int	f_jumpto(void);
extern int	f_loop(void);
extern int	f_endloop(void);
extern int	f_patch(void);
extern	int	f_pan(void);
extern	int	f_mux(void);
extern	int	f_demux(void);
extern	int	f_volume(void);
extern	int	f_xpose(void);
extern	int	f_xset(void);
extern	int	f_slur(void);
extern	int	f_ring(void);
extern	int	f_clockset(void);
extern	int	f_end(void);
extern	int	f_filterset(void);
extern	int	f_filterinc(void);


extern	int	f_mark(void);
extern	int	f_sound_call(void);
extern	int	f_srest(void);
extern	int	f_intvarset(void);
extern	int	f_intvarinc(void);
extern	int	f_user_1_var_evf(void);
extern	int	f_user_2_var_evf(void);
extern int f_sig(void);
extern	int	f_gliss(void);
extern	int	f_clockinc(void);
extern	int	f_paninc(void);
extern int f_musicclockinc(void);
extern	int	f_MidiNoteOn(void);
extern	int	f_MidiNoteOff(void);
extern	int	f_MidiProgram(void);
extern	int	f_MidiVolume(void);
extern	int	f_MidiTempo(void);
extern	int	f_MidiControl(void);
extern	int	f_MidiEOT(void);

extern	int	f_fxset(void);

extern void	call_silence(void);
extern void	call_music(void);
extern void	call_effect(void);
extern	void	call_volume(void);
extern	void	call_senddev(void);
extern	void	call_mark(void);
extern	void	call_ignore(void);
extern	void	call_user_function(void);
extern	void	call_timer(void);
extern	void	call_end_timer(void);
extern void	call_play_timer(void);
extern void call_kill_effect(void);

int	(* const event_fcns[])() = {
	f_rest,				//0
	f_note,				//1
	f_jumpto,			//2
	f_loop,				//3
	f_endloop,			//4
	f_patch,			//5
	f_pan,				//6
	f_mux,				//7
	f_demux,			//8
	f_volume,			//9
	f_xpose,			//10
	f_xset,				//11
	f_slur,				//12
	f_ring,				//13
	f_clockset,			//14
	f_end,				//15
	f_filterinc,		//16
	f_filterset,		//17
	f_gliss,			//18

	f_mark,
	f_sound_call,
	f_srest,
	f_intvarset,
	f_intvarinc,
	f_user_1_var_evf,
	f_user_2_var_evf,
	f_sig,

	f_clockinc,
	f_paninc,
	f_musicclockinc,
	f_MidiNoteOn,	/* 35 */
	f_MidiNoteOff,	/* 36 */
	f_MidiProgram,	/* 37 */
	f_MidiVolume,	/* 38 */
	f_MidiTempo,	/* 39 */
	f_MidiControl,	/* 40 */
	f_MidiEOT,		/* 41 */
	f_fxset,


};

/*
 *	NOTE:
 *	call music is used for BOTH forground sounds and background music
 *	This is becuase the code is essentially the same so it saves space
 */


void	(* const call_fcns[])() = {
	call_silence,		/* 0 sound call type 0 (silence)*/
	call_music,			/* 1 sound call type 1 (music)  */
	call_effect,		/* 2 sound call type 2 (effect) */
	call_volume, 		/* 3 sound call type 3 (volume) */
	call_ignore,  		/* 4 formerly call_oki	*/
	call_senddev,		/* 5 send data directly to yamaha chip */
	call_mark,			/* 6 leave a marker				*/
	call_ignore,		/* 7 ignore sound call		*/
	call_user_function,	/* 8 user defined function 	*/
	call_timer,			/* 9 make a music timer call	*/
	call_end_timer,		/* 10 stop timer, go back to prev back music */
	call_play_timer,		/* 11 sound table entry for timer */
	call_kill_effect	/* 12 kill all forground fx		*/
};
	
