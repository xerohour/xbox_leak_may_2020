/*
 	structures and equates for sound operation system
 */

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned long	ulong;


#ifndef NULL
#define	NULL			0
#endif

#define	LEVEL_SILENCE		0
#define	LEVEL_MUSIC		0
#define	LEVEL_EFFECT	1

#define	MUSIC_VOLUME		0
#define	EFFECT_VOLUME		1
#define	MUSIC_FADE			2
#define	RESTORE_MUSIC_VOLUME	3
#define	SET_RESTORE_VOLUME	4
#define	MUSIC_ATTEN		5

#define	MAX_DURATION		(0xffff-12)		/* max duration */

/* 
 * process (event) structure 
 */

struct	process {
	struct process *next;		/* pointer to next process in queue */
	struct process *prev;		/* pointer to previous process in queue*/
	union  {
			uchar	*c;
			ushort	*i;
			} mem_ptr;
	short	timer;			/* timer value for this process */
	short	prev_timer;		/* last value of timer for delta */
	ushort	function;		/* event type (function) */
	uchar	level;			/* sound level (type) NOT volume */
	uchar	hard_channel;		/* synthesis device channel */
	uchar	type;			/* for later use...*/
	ushort	data1;
	ushort	data2;
};


/*
 *	sound (call) structure
 */

struct	sound {
	const uchar	type;		/* sound type (backg, forg, vox, etc) */
	const uchar	priority;	/* priority of this sound */
	const ushort	track_map;	/* bit map for tracks this sound uses */
	const ushort	**tbl_ptr;	/* pointer to this sounds event pointers */
};	

/*
 * sound call receive fifo structure
 */
  
#define	FIFO_SIZE	16

struct	fifo {
	uchar	fifo_read;
	uchar	fifo_write;
	uchar	data[FIFO_SIZE];
};



/*************************************************************************
*                                                                        *
* 	Track information						  *
*      NOTE:								  *
*	Tracks are soft                                                   *
*************************************************************************/
#define	MAX_LOOP	4	/* # of nested loops allowed 	*/
#define	MAX_MUX		3   	/* # of nested mux's allowed 	*/

struct	track_info {
	ushort 	patch;			/* pointer to this tracks voice */
	signed char	pan;			/* left/right/center pan	*/
	uchar	volume;			/* patch's attenuation value	*/
	uchar	lfo_sens;		/* patch's lfo sensitivity value*/
	ushort	pitch;			/* current pitch 		*/
	ushort	loop_counter[MAX_LOOP];	/* # of times to loop		*/
	ushort	*loop_addr[MAX_LOOP];	/* address of loop back points  */
	uchar	loop_level;		/* deepness of loop		*/
	ushort	*mux_addr[MAX_MUX];	/* address of mux back points   */
	uchar	mux_level;		/* deepness of mux		*/
	short	transpose;		/* transposition of track	*/
	ushort	filtercutoff;	/* filter cutoff */
	ushort	filterres;		/* filter resonance */
};

/*************************************************************************
*                                                                        *
* 	Timer music structurs											  *
*                                                                        *
*************************************************************************/
#define	MUSIC_TIMER_PENDING	2		/* silent but "playing" */
#define	MUSIC_TIMER_ON		1		/* playing				*/
#define	MUSIC_TIMER_OFF		0		/* not playing			*/
#define	MAX_TIMERS			17

struct	timer_music {
	uchar	priority;		/* priority of timer					*/
	uchar	sound_call;		/* sound call to make for this timer	*/
	uchar	off_fx;			/* sound fx call to make for end of timr*/
};

/*************************************************************************
*                                                                        *
* 	Channel information						  *
* 	NOTE:								  *
* 	Channels are hard						  *
*                                                                        *
*************************************************************************/


struct	channel_info {
	uchar	type;		/* type of channel 			 */
	uchar	level;		/* current highest level playing on chan */
};



/*************************************************************************
*                                                                        *
* 	Add a watchdog check to code					  *
*                                                                        *
*************************************************************************/


#define	DOGIT	do_watchdog()






