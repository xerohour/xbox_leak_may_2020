/*************************************************************************
*                                                                        *
*   This file contains the tables and definitions for                 *
*   the synthesis specific DEVice                                     *
*      The device tables are in devtab.c                                  *
**************************************************************************
*
*   The following variables and tables must be defined:
*
*   max_tracks:     1 byte, max # of synthesis channels
*   dev_init:       function called to initialize device
*   do_watchdog     function called to  kick watchdog timer
*   dtimer_int      function called to  disable timer interrupt
*   etimer_int      function called to  enable timer interrupt
*   send_dev_function:
*       function called to send a byte of data to the synthesis
*       device (sound call callable)
*       The data is in the global, "a_value" and the address is
*       in the global, "b_value."
**************************************************************************/

#define _base_

extern  struct DSPpatch *Patches[];
extern  unsigned short  Noise8192[];

#include    <dsound.h>
#include "sosdsp.h"
#include "sos.h"
#include "bootsnd.h"
#include    <xtl.h>
#include "dsptables.h"

#define MAX_BUFFERS 16

#ifdef STARTUPANIMATION
#pragma data_seg("INIT_RW")
#pragma code_seg("INIT")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")

#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")

#endif

LPDIRECTSOUND8          m_pDSound;                          // DirectSound object
LPDIRECTSOUNDBUFFER8    m_pDSBuffer[MAX_BUFFERS];           // DirectSoundBuffer
extern WCHAR    StringBuffer[];

#define MIN(a,b) (((signed) a) < ((signed) b) ? (a) : (b))

/*
 *  track_status has the necessary items to restore the state of
 *  a track after a track of a higher level on the same channel
 *  ends
 */

#define MAX_TRACKS  16
#define MAX_PROCESSES       30
#define MAX_LEVELS      2

uchar       channel_level[MAX_TRACKS];/* current level for each chan*/
struct track_info track_status[(MAX_LEVELS) * MAX_TRACKS];
struct process queue_list[MAX_PROCESSES]; /* pre-allocated process packets */
extern      struct sound    * _base_ current_call;      /* pointer to current sound call */
extern  uchar   sound_call_table;


const   unsigned short  max_processes = MAX_PROCESSES;

extern          uchar       a_value, b_value;
unsigned int    dsp_address;
unsigned int    dsp_data;   
extern  ushort      value_16_bit;
extern  uchar   current_channel;
extern  uchar       master_music_volume;    /* main attenuation for music   */
extern  uchar       master_effect_volume;   /* main attenuation for f/x */
extern  uchar       music_atten;            /* music attenuation */
extern  uchar   init_call;
extern  uchar   current_level;      /* global level of current process */
extern  struct  track_info  * _base_ ti;    /* track info pointer */
extern  uchar   pan_table[];                    /* panning table (8-bit) */
void    put_dsp(void);
extern  reset_dsp();
void    init_dsp(void);
void    put_fifo(unsigned char);
const   unsigned char   max_tracks = MAX_TRACKS;

extern void (* const call_fcns[])();

int volume_dsp(uchar, uchar, uchar *,uchar);
int silence_dsp(uchar);
int note_on_dsp();
int slur_dsp();
int note_off_dsp();
int patch_dsp(unsigned short);
int pan_dsp(uchar , signed char, uchar *);
int vp_filter();



int (* const filter_functions[])() = {
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter,
    vp_filter   
};


int (* const silence_functions[])() = {
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    silence_dsp,
    
};

/**************************************************************************
*                                                                        *
*   The note_on functions turn a note on.  They assume that           *
*   the global, "a_value" has the current channel # and that          *
*   the global "value_16_bit" has the 16 bit pitch (iiiiiiii.ffffffff *
*      and "b_value" has the integer part of the pitch                    *
*************************************************************************/
int (* const note_on_functions[])() = {
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
    note_on_dsp,
};



int (* const slur_functions[])() = {
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
    slur_dsp,
};


/*************************************************************************
*                                                                        *
*   The note_off functions turn a note off.  They assume that         *
*   the global, "a_value" has the current channel.                    *
*************************************************************************/
int (* const note_off_functions[])() = {
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
    note_off_dsp,
};
int (* const patch_functions[]) () = {
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,
    patch_dsp,

};


int (* const volume_functions[])() = {
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,
    volume_dsp,

};
int (* const pan_functions[])() = {
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,
    pan_dsp,

};


int (* const user_1_var_evf_functions[])() = {
    pan_dsp
};
int (* const user_2_var_evf_functions[])() = {
    pan_dsp
};

//
// write data from global var dsp_data to x memory space "dsp_address"
//

void put_dsp()
{

//  LPVOID pvXramBuffer;
//  pvXramBuffer = (LPVOID) (dsp_address);

//    memcpy(pvXramBuffer,pvData,dwDataSize);

//  *(PDWORD) (GPXMEM + dsp_address) = dsp_data;
    
}


WriteDSPDatablock
(
    DWORD                   dwDSPAddress, 
    LPCVOID                 pvData, 
    DWORD                   dwDataSize
)
{
    LPVOID pvXramBuffer;

        //
        // poke directly the XRAM offset
        //

    pvXramBuffer = (LPVOID) (GPXMEM + dwDSPAddress);
    memcpy(pvXramBuffer,pvData,dwDataSize);

 
    return 1;

}

ReadDSPDatablock
(
    DWORD                   dwDSPAddress, 
    LPVOID                  pvData, 
    DWORD                   dwDataSize
)
{
    LPVOID pvXramBuffer;
    //
    // copy fx data
    //
    pvXramBuffer = (LPVOID) (GPXMEM + dwDSPAddress);
    memcpy(pvData,pvXramBuffer,dwDataSize);

    return 1;
}

ReadDSPProgblock
(
    DWORD                   dwDSPAddress, 
    LPVOID                  pvData, 
    DWORD                   dwDataSize
)
{
    LPVOID pvXramBuffer;
    //
    // copy fx data
    //
    pvXramBuffer = (LPVOID) (GPPMEM + dwDSPAddress);
    memcpy(pvData,pvXramBuffer,dwDataSize);

    return 1;
}

// return dsp data
int get_dsp(unsigned int addr)
{
//  return( (*PDWORD) (GPXMEM + dsp_address) );
    return(1);
}

silence_dsp(unsigned char chan)
{

    return(1);
}   


note_on_dsp()
{
    DWORD   dwFreq;
    struct DSPpatch *addr;

#if DBG

//  dwFreq = pitch_table_dsp[value_16_bit>>8];
//  swprintf( StringBuffer, L"Current Pitch: 0x%x", dwFreq);

#endif

    dwFreq = (DWORD) ( ((value_16_bit >> 8) - 60) * (4096/12) );
    dwFreq += ((((DWORD) value_16_bit & 0xff) * 341) / 255);
//  IDirectSoundBuffer_StopEx(m_pDSBuffer[current_channel], 0, DSBSTOPEX_ENVELOPE);
    IDirectSoundBuffer_SetPitch(m_pDSBuffer[current_channel], dwFreq);

    addr = Patches[ti->patch];
    if (addr->LoopEnable) 
        IDirectSoundBuffer_Play(m_pDSBuffer[current_channel], 0,0,DSBPLAY_LOOPING);
    else
        IDirectSoundBuffer_Play(m_pDSBuffer[current_channel], 0,0,0);

    return(1);

}
slur_dsp()
{
    DWORD   dwFreq;

    dwFreq = (DWORD) ( ((value_16_bit >> 8) - 60) * (4096/12) );
    dwFreq += ((((DWORD) value_16_bit & 0xff) * 341) / 255);

#if DBG
//  swprintf( StringBuffer, L"Current Pitch Slur: 0x%x", dwFreq);
#endif

    IDirectSoundBuffer_SetPitch(m_pDSBuffer[current_channel], dwFreq);

    return(1);
}

note_off_dsp()
{
    IDirectSoundBuffer_StopEx(m_pDSBuffer[current_channel], 0, DSBSTOPEX_ENVELOPE);
    return(1);

}

vp_filter()
{
    DSFILTERDESC    fdesc;


    fdesc.dwMode = DSFILTER_MODE_DLS2;
    fdesc.dwQCoefficient = 0;
    fdesc.adwCoefficients[0] = ti->filtercutoff + 32768;
    fdesc.adwCoefficients[1] = ti->filterres;
    fdesc.adwCoefficients[2] = ti->filtercutoff + 32768;
    fdesc.adwCoefficients[3] = ti->filterres;
    IDirectSoundBuffer_SetFilter(m_pDSBuffer[current_channel], &fdesc);
    return(1);
}



patch_dsp(unsigned short pat)
{
    struct DSPpatch *addr;

    addr = Patches[pat];

    IDirectSoundBuffer_SetEG(m_pDSBuffer[current_channel], addr->lpAmpEnvelope);
    IDirectSoundBuffer_SetEG(m_pDSBuffer[current_channel], addr->lpMultiEnvelope);
    IDirectSoundBuffer_SetBufferData(m_pDSBuffer[current_channel], addr->Start, addr->Length );
    IDirectSoundBuffer_SetLoopRegion( m_pDSBuffer[current_channel],0, addr->Length );
    IDirectSoundBuffer_SetCurrentPosition(m_pDSBuffer[current_channel], 0 );
    
    return(1);
}



pan_dsp(uchar chan, signed char pan, uchar *patch)
{
    uchar   mod;
    unsigned int    vol_mul;
    unsigned int    tmp;
    char            tmp_pan;

    vol_mul = 0x7fff;
    if (current_level >= 1) {           
/*      mod = master_effect_volume;*/
//      vol_mul = volume_table_dsp[master_music_volume];
    }
    else {
/*      mod = 0;*/
//      tmp = volume_table_dsp[music_atten];
//      vol_mul = volume_table_dsp[master_music_volume];

//      vol_mul = (long)((long)tmp * (long)vol_mul) >>15;
    
    }

    mod = 0;

    tmp_pan = ti->pan >> 3;

    tmp_pan += 16;                      /* set range 0 - 31 */
    tmp = 1;    
//  tmp = volume_table_dsp[MIN(127,ti->volume + mod)];
    tmp = (long)((long)tmp * (long)vol_mul) >>15;

/*  tmp >>= 7;*/
    dsp_data = (long) ((long)tmp * (long)pan_table[tmp_pan]) >> 7;
/*  dsp_data = (char)tmp * pan_table[tmp_pan];*/
//  a_value = VOLUME_L_ADDRESS(chan);
    put_dsp();

    dsp_data = (long) ((long)tmp * (long)pan_table[31 - tmp_pan]) >> 7;
/*  dsp_data = (char)tmp * pan_table[31 - tmp_pan];*/
//  a_value = VOLUME_R_ADDRESS(chan);
    put_dsp();

    return(1);

}


// initialize stuff for the dsp.
// write sine wave into high x memory.
// also create 16 dsound buffers that we'll use
// for our sounds.


static long holdrand = 1L;

void __cdecl srand(unsigned int seed)
{
    holdrand = (long)seed;
}

int __cdecl rand(void)
{
    return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

dev_init()
{
    
    DSBUFFERDESC dsbdesc;
    WAVEFORMATEX wfFirst;
//    DWORD       dwMixBinMask = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FXSEND_0;
    DSMIXBINS    dsmb;
    DSMIXBINVOLUMEPAIR dsvp[3];
    long        lVolumes[3];
    int j;
    double  dtmp;
    double  FMc = 4.0;
    double  FMm = 2.0;


    int i;

    srand(1003);
    for (i = 0; i < 8192; i++) {
        Noise8192[i] = (unsigned short) rand();
    }

    for (i = 0; i < 128; i++) {
        Sin128[i] = (unsigned short)(32767*sin(2.0*3.14159*(double)i/128.0));
    }
    j = 0;
    for (i = 0; i < 32768; i++) {
        if (i < 16384)
            j++;
        else
            j--;
        dtmp = (double)j/16384.0  * sin(FMm * 2.0*3.14159*(double)i/128.0);
        FM32768[i] = (unsigned short)(32767*sin(dtmp + FMc * 2.0*3.14159*(double)i/128.0));
    }

    for (i = 0; i < 128; i++) {         // create sawtooth wave
        Saw128[i] = (unsigned short) (65536 * ((float)(i-64) /128.0));
    }

    for (i = 0; i < 0x5540; i++) {      // size of glock sound..make 16-bit
        ThunEl16[i] = (ThunEl16Data[i]) << 8;
    }
    for (i = 0,j=0x5540; i < 0x5540; i++,j--) {     // size of glock sound..make 16-bit
        ReverseThunEl16[i] = ThunEl16[j];
    }

    

    for (i = 0; i < 3768; i++) {        // size of glock sound..make 16-bit
        Glock[i] = (GlockData[i]^0x80) << 8;
    }
        
    for (i = 0; i < 6719; i++) {        // size of glock sound..make 16-bit
        Bubble[i] = (BubbleData[i]^0x80) << 8;
    }


    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return (0);


    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    wfFirst.wFormatTag = WAVE_FORMAT_PCM;
    wfFirst.nChannels = 1;
    wfFirst.nSamplesPerSec = 48000;
    wfFirst.wBitsPerSample = 16;
    wfFirst.nBlockAlign = wfFirst.nChannels * wfFirst.wBitsPerSample/8;
    wfFirst.nAvgBytesPerSec = wfFirst.nSamplesPerSec * wfFirst.nBlockAlign;

    dsbdesc.dwFlags = 0;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = &wfFirst;
//    dsbdesc.dwMixBinMask = dwMixBinMask;
    dsbdesc.lpMixBins = &dsmb;

    dsmb.lpMixBinVolumePairs = dsvp;

    for (i = 0; i < MAX_BUFFERS; i++) {


        if (i%2) {
//        lVolumes[0] = -600;
//        lVolumes[1] = 0;
//        lVolumes[2] = -2800;
//        dwMixBinMask = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT;  
//            dsbdesc.dwMixBinMask = dwMixBinMask;        
        dsmb.dwMixBinCount = 2;
        dsvp[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
        dsvp[0].lVolume = -600;
        dsvp[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
        dsvp[1].lVolume = 0;
        }
        else {
//        lVolumes[0] = 0;
//        lVolumes[1] = -600;
//        lVolumes[2] = -2800;
//        dwMixBinMask = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT;  
//            dsbdesc.dwMixBinMask = dwMixBinMask;    
        dsmb.dwMixBinCount = 2;
        dsvp[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
        dsvp[0].lVolume = 0;
        dsvp[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
        dsvp[1].lVolume = -600;
        }
        if ((i == 3) || (i == 5)) {
//        lVolumes[0] = 0;
//        lVolumes[1] = -100;
//        lVolumes[2] = 00;
//        dwMixBinMask = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FXSEND_0;
//            dsbdesc.dwMixBinMask = dwMixBinMask;
        dsmb.dwMixBinCount = 3;
        dsvp[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
        dsvp[0].lVolume = 0;
        dsvp[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
        dsvp[1].lVolume = -100;
        dsvp[2].dwMixBin = DSMIXBIN_FXSEND_0;
        dsvp[2].lVolume = 0;
        }

            if( FAILED( DirectSoundCreateBuffer( &dsbdesc, &m_pDSBuffer[i]) ) )
                return E_FAIL;

//            IDirectSoundBuffer_SetMixBinVolumes(m_pDSBuffer[i], dwMixBinMask, lVolumes);
            IDirectSoundBuffer_SetMixBinVolumes(m_pDSBuffer[i], &dsmb);
    }

//  ReadDSPDatablock(0xa00*4, databack, sizeof(databack) );

     put_fifo(0x1);
     return(1);
}
//
// free dsound buffers and dsound object we created
//
dev_cleanup()
{
    int i;

    for (i = 0; i < MAX_BUFFERS; i++) {
        IDirectSoundBuffer_Release(m_pDSBuffer[i]);
    }
    IDirectSound_Release(m_pDSound);

}


do_watchdog()
{
}
dtimer_int()
{
}
etimer_int()
{
}
send_dev_function()
{
}
/*************************************************************************
*                                                                        *
*   Adjust the volume of that patch, "patch_addr" on channel,     *
*   "chan" by the amount volume + whatever the global volume is   *
*   for "level"                           *
*                                                                        *
*************************************************************************/

volume_dsp(
uchar op_level, 
uchar sound_level,
uchar *patch_addr,
uchar   chan)
{

    IDirectSoundBuffer_SetVolume(m_pDSBuffer[current_channel], (-1*op_level*30) + 200);
    return(1);
}

call_user_function()
{

}

extern  f_end();


user_silence_function()
{
#if DBG
//  swprintf( StringBuffer, L"Current Sound: %S", "SilenceFunction");
#endif
}

uchar pan_table[] = {
    1,2,3,4,5,6,7,8
};


