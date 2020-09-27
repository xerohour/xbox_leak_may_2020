#include "sos.h"
#include "error.h"
#include "protos.h"

#ifdef STARTUPANIMATION

#pragma data_seg("INIT_RW")
#pragma code_seg("INIT")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")


#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")

#endif //STARTUPANIMATION

struct	process	*  process_queue;		/* pointer to process queue 	*/
struct sound	*  current_call;		/* pointer to current sound call*/
struct process *  current_process;	/* pointer to current s. proc 	*/

struct	fifo fifo;			/* sound call fifo		*/

struct	track_info	*  ti;   	/* global pointer to current track info*/
 ushort		value_16_bit;	/* value to pass to asm routine */

short 		system_clock_music ;/* global music system timer     	*/
uchar		system_clock_mlow;  /* for 3 byte clocks		*/
short 		system_clock_fx; /* global fx system timer	    	*/
uchar		system_clock_fxlow; /* for 3-byte clocks	*/
uchar 		clock_cntr[2];	/* clock count-downer		*/
uchar 		clock_cntr_value[2];	/* clock count-downer reset val	*/
uchar		clock_cntr_save;	/* save value of clock count-downer */
ushort 		global_music_xpose;	/* master transposition for mus	*/
ushort		global_effect_xpose;	/* master transposition for f/x	*/
uchar		a_value;		/* byte for register a data 	*/
uchar		b_value;		/* byte for register b data 	*/
uchar		master_music_volume;	/* main attenuation for music	*/
uchar		master_effect_volume;	/* main attenuation for f/x	*/
uchar		music_atten;		/* music attenuation control */

uchar		marker;					/* tune transition marker	*/
uchar		current_level;		/* global level of current process */
uchar		current_channel;	/* global chan of current process */


ushort		dp_word1;			/* temp var */

uchar		gtifbtmp;			/* temp var for get_info_block */
uchar		last_music_call;	/* last music call made		*/
uchar		current_timer_priority;	/* priority of last timer call */
uchar		sos_only;			/* 1 to loop forever, 0 to return */
uchar		sound_call_table;	/* which soundtable are we on ?*/
ushort		global_fxpose;		/* global transpose value */
