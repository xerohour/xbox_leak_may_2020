		
/*************************************************************************
*                                                                        *
* 	External Variables						  *
*                                                                        *
*************************************************************************/
#define _base_

#ifdef STARTUPANIMATION

#pragma data_seg("INIT_RW")
#pragma code_seg("INIT")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")


#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")

#endif //STARTUPANIMATION

extern		struct process	* _base_ process_queue;		/* pointer to process queue */
extern		struct sound	* _base_ current_call;		/* pointer to current sound call */
extern		struct sound	sound_calls[];
extern		struct sound	sound_calls2[];				/* alternate sound call table */
extern		struct process 	* _base_ current_process;	/* pointer to current s. proc */
extern		struct fifo	fifo;

extern		_base_ uchar hcount;			/* counter for oki nmi		*/
extern		_base_ uchar voice_state;		/* current nmi voice state	*/
extern		uchar * _base_ voice_data_pointer;	/* pointer to cur. voice data	*/
extern		uchar * _base_ voice_data_end;	/* pointer to end of v data	*/

extern		_base_ uchar 	a_value;		/* byte for register a data */
extern		_base_ uchar 	b_value;		/* byte for register b data */
extern		struct track_info * _base_ ti;/* global pointer to current track info*/
extern		_base_ ushort 	value_16_bit;		/* tmp value for passing */


extern _base_ short 		system_clock_music ;/* global music system timer     	*/
extern _base_ uchar		system_clock_mlow;  /* for 3 byte clocks		*/
extern _base_ short 		system_clock_fx; /* global fx system timer	    	*/
extern _base_ uchar		system_clock_fxlow; /* for 3-byte clocks	*/

extern		_base_ uchar 	clock_cntr[2];		/* timer count-downer	*/
extern		_base_ uchar 	clock_cntr_value[2];	/* timer count-downer	*/
extern		_base_ uchar	clock_cntr_save;	/* save val of clock cntr */
extern 	_base_ uchar 	master_music_volume;	/* main attenuation for music	*/
extern 	_base_ uchar 	master_effect_volume;	/* main attenuation for f/x	*/
extern		_base_ uchar	music_atten;		/* music attenuateion */
extern 	_base_ uchar 	master_music_amd;	/* main amp mod depth for music	*/
extern 	_base_ uchar 	master_effect_amd;	/* main amp mod depth for effect*/
extern 	_base_ uchar 	master_music_pmd;	/* main phase mod depth/music	*/
extern 	_base_ uchar 	master_effect_pmd;	/* main phase mod depth/effect	*/
extern		_base_ uchar 	marker;
extern		_base_ uchar	current_level;		/* global level of current process */
extern		_base_ uchar	current_channel;	/* global chan of current process */

extern	_base_	ushort		int_vars[12];		/* variables 2-byte */
extern	_base_ uchar		byte_vars[12];		/* variables 1-byte */

extern	_base_	uchar		dp_byte1;			/* temp var */
extern	_base_	uchar		dp_byte2;			/* temp var */
extern	_base_	uchar		dp_byte3;			/* temp var */
extern	_base_	uchar		dp_byte4;			/* temp var */
extern	_base_	uchar		dp_byte5;			/* temp var */
extern	_base_	ushort		dp_word1;			/* temp var */
extern	_base_	ushort		dp_word2;			/* temp var */
extern	_base_	ushort		dp_word3;			/* temp var */
extern	_base_	ushort		dp_word4;			/* temp var */
extern	_base_	ushort		dp_word5;			/* temp var */
extern	_base_ uchar		last_music_call;	/* last music call made		*/
extern _base_ uchar 	current_timer_priority;	/* priority of last timer call */
extern	uchar		music_timer_status[]; 
extern _base_ uchar	sos_only;
extern _base_ uchar		sound_call_table;	/* which soundtable are we on ?*/
extern	ushort		global_fxpose;		/* global transpose value */

extern	uchar		channel_level[];	/* current level for each chan*/
extern	struct track_info track_status[];
extern	struct process 	queue_list[]; 	/* pre-allocated process packets */


extern	const uchar	max_tracks;				/* from dev.c		*/
extern	void	do_watchdog(void);			/* from dev.c		*/
extern			dtimer_int();			/* from dev.c		*/
extern			etimer_int();			/* from dev.c		*/
extern			send_dev_function();	/* from dev.c		*/
extern	int	(*silence_functions[])();	/* from dev.c		*/
extern	int	(*note_on_functions[])();	/* from dev.c 		*/
extern	int	(*slur_functions[])();		/* from dev.c 		*/
extern	int	(*note_off_functions[])();	/* from dev.c		*/
extern	int	(*patch_functions[])();		/* from dev.c		*/
extern	int	(*volume_functions[])();   	/* from dev.c		*/
extern	int	(*pan_functions[])();		/* from dev.c		*/
extern	int	(*filter_functions[])();	/* from dev.c		*/
extern	int	dev_init();					/* from dev.c		*/
extern  int dev_cleanup();              /* from dev.c       */

extern	int	(*user_1_var_evf_functions[])();		/* from dev.c		*/
extern	int	(*user_2_var_evf_functions[])();		/* from dev.c		*/
extern	int   user_silence_function();				/* from dev.c		*/
extern int signal_function();			/* from dev.c */

