
#ifndef _PROTOS_H_
#define _PROTOS_H_ 


init_track_status(uchar level, uchar channel);
remove_processes_by_level(uchar level);


void	do_sos_init_return();

void	sos_main(void);
void	sos_error(int);
int	create_process(ushort * p, uchar l, uchar c);
struct process * get_process_packet();
int	free_process_packet(struct process *pp);
struct track_info *get_track_info_block();
struct track_info *get_track_info_block2(uchar, uchar);
struct track_info *get_music_info_block();
struct track_info *get_music_info_block_i(uchar);
uchar	read_sound_call();
void	put_fifo(uchar);
void	nosound(uchar);
void	kill_process( struct process *pp);
uchar	get_fifo(void);
void do_volume_adjust(uchar volume,uchar level,uchar *patch_addr,uchar chan);
void	remove_processes_by_level_and_channel(uchar, uchar);
int	init_queuelist();
void	do_watchdog(void);


int	f_note(void);
int	f_rest(void);
int	f_jumpto(void);
int	f_loop(void);
int	f_endloop(void);
int	f_patch(void);
int	f_pan(void);
int	f_mux(void);
int	f_demux(void);
int	f_volume(void);
int	f_xpose(void);
int	f_xset(void);
int	f_slur(void);
int	f_ring(void);
int	f_clockset(void);
int	f_end(void);
int	f_filterset(void);
int	f_filterinc(void);


int	f_mark(void);
int	f_sound_call(void);
int	f_srest(void);
int	f_intvarset(void);
int	f_intvarinc(void);
int	f_user_1_var_evf(void);
int	f_user_2_var_evf(void);
int 	f_sig(void);
int	f_gliss(void);
int	f_clockinc(void);
int	f_paninc(void);
int 	f_musicclockinc(void);
int	f_MidiNoteOn(void);
int	f_MidiNoteOff(void);
int	f_MidiProgram(void);
int	f_MidiVolume(void);
int	f_MidiTempo(void);
int	f_MidiControl(void);
int	f_MidiEOT(void);

int	f_fxset(void);

void	call_silence(void);
void	call_music(void);
void	call_effect(void);
void	call_volume(void);
void	call_senddev(void);
void	call_mark(void);
void	call_ignore(void);
void	call_user_function(void);
void	call_timer(void);
void	call_end_timer(void);
void	call_play_timer(void);
void 	call_kill_effect(void);

#endif //protos_h_
