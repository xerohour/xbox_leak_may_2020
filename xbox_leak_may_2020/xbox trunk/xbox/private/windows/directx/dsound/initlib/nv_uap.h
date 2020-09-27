/*  FILE: nv_uap.h
 *  CREATED: Mon Nov 20 21:05:43 2000
 *
 *  Copyright (c) 2000, by NVIDIA CORP
 *  All Rights Reserved.
 */

 /*** DO NOT EDIT, THIS FILE GENERATED FROM ../../../manuals/usr_audio_proc.ref ***/


#ifndef __NV_UAP_H__
#define __NV_UAP_H__


/***********************************************/
/* Macros and constants                        */
/***********************************************/

#ifndef REF_VAL
#define DRF_SHIFT(drf)    ((0?drf) % 32)
#define DRF_MASK(drf)     (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define REF_VAL(drf,v)    (((v)>>DRF_SHIFT(drf))&DRF_MASK(drf))
#define REF_NUM(drf,n)    (((n)&DRF_MASK(drf))<<DRF_SHIFT(drf))
#endif /* !REF_VAL */

/***********************************************/
/*    #defines from the manuals                */
/***********************************************/

#define NV1BA0_PIO_FREE                                    0x00000010 /* R--4R */
#define NV1BA0_PIO_FREE_COUNT                              11:2 /* R-X4F */
#define NV1BA0_PIO_FREE_COUNT_FIFO_FULL                    0x00 /* R---V */
#define NV1BA0_PIO_INFO                                    0x00000014 /* R--4R */
#define NV1BA0_PIO_INFO_PIO                                0:0 /* R-X4F */
#define NV1BA0_PIO_INFO_PIO_EMPTY_AND_IDLE                 0x00 /* R---V */
#define NV1BA0_PIO_INFO_PIO_BUSY                           0x01 /* R---V */
#define NV1BA0_PIO_INFO_DMA                                1:1 /* R-X4F */
#define NV1BA0_PIO_INFO_DMA_EMPTY_AND_IDLE                 0x00 /* R---V */
#define NV1BA0_PIO_INFO_DMA_BUSY                           0x01 /* R---V */
#define NV1BA0_CHANNEL_DMA_PUT                             0x00000040 /* -W-4R */
#define NV1BA0_CHANNEL_DMA_PUT_OFFSET                      11:2 /* -WXUF */
#define NV1BA0_CHANNEL_DMA_GET                             0x00000044 /* R--4R */
#define NV1BA0_CHANNEL_DMA_GET_OFFSET                      11:2 /* R-X4F */
#define NV1BA0_PIO_NOP                                     0x00000100 /* -W-4R */
#define NV1BA0_PIO_NOP_PARAMETER                           31:0 /* -W-VF */
#define NV1BA0_PIO_NOTIFICATION_STATUS_DONE_SUCCESS        0x00000001 /* -W--V */
#define NV1BA0_PIO_SYNCHRONIZE                             0x00000104 /* -W-4R */
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER                   31:0 /* -W-VF */
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_NO_OPERATION      0x00000000 /* -W--V */
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE     0x00000001 /* -W--V */
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY 0x00000002 /* -W--V */
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY_AWAKEN 0x00000003 /* -W--V */
#define NV1BA0_PIO_SET_TIME                                0x00000108 /* -W-4R */
#define NV1BA0_PIO_SET_TIME_VALUE                          31:0 /* -W-UF */
#define NV1BA0_PIO_SET_MODE                                0x0000010C /* -W-4R */
#define NV1BA0_PIO_SET_MODE_PARAMETER                      31:0 /* -W-VF */
#define NV1BA0_PIO_SET_MODE_PARAMETER_OFF                  0x00000000 /* -W--V */
#define NV1BA0_PIO_SET_MODE_PARAMETER_RUN                  0x00000001 /* -W--V */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE                    0x00000120 /* -W-4R */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE             15:0 /* -W-VF */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE_NULL        0xFFFF /* -W--V */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST               17:16 /* -W-VF */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_INHERIT       0x00 /* -W--V */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_2D_TOP        0x01 /* -W--V */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_3D_TOP        0x02 /* -W--V */
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_MP_TOP        0x03 /* -W--V */
#define NV1BA0_PIO_VOICE_ON                                0x00000124 /* -W-4R */
#define NV1BA0_PIO_VOICE_ON_HANDLE                         15:0 /* -W-VF */
#define NV1BA0_PIO_VOICE_ON_ENVF                           27:24 /* -W-VF */
#define NV1BA0_PIO_VOICE_ON_ENVF_DISABLE                   0x00 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVF_DELAY                     0x01 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVF_ATTACK                    0x02 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVF_HOLD                      0x03 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVA                           31:28 /* -W-VF */
#define NV1BA0_PIO_VOICE_ON_ENVA_DISABLE                   0x00 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVA_DELAY                     0x01 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVA_ATTACK                    0x02 /* -W--V */
#define NV1BA0_PIO_VOICE_ON_ENVA_HOLD                      0x03 /* -W--V */
#define NV1BA0_PIO_VOICE_OFF                               0x00000128 /* -W-4R */
#define NV1BA0_PIO_VOICE_OFF_HANDLE                        15:0 /* -W-VF */
#define NV1BA0_PIO_VOICE_RELEASE                           0x0000012C /* -W-4R */
#define NV1BA0_PIO_VOICE_RELEASE_HANDLE                    15:0 /* -W-VF */
#define NV1BA0_PIO_GET_VOICE_POSITION                      0x00000130 /* -W-4R */
#define NV1BA0_PIO_GET_VOICE_POSITION_HANDLE               15:0 /* -W-VF */
#define NV1BA0_PIO_VOICE_PAUSE                             0x00000140 /* -W-4R */
#define NV1BA0_PIO_VOICE_PAUSE_HANDLE                      15:0 /* -W-VF */
#define NV1BA0_PIO_VOICE_PAUSE_ACTION                      18:18 /* -W-VF */
#define NV1BA0_PIO_VOICE_PAUSE_ACTION_RESUME               0x00 /* -W--V */
#define NV1BA0_PIO_VOICE_PAUSE_ACTION_STOP                 0x01 /* -W--V */
#define NV1BA0_PIO_SET_CONTEXT_DMA_NOTIFY                  0x00000180 /* -W-4R */
#define NV1BA0_PIO_SET_CONTEXT_DMA_NOTIFY_HANDLE           31:0 /* -W-VF */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM(i)                  (0x00000200+(i)*4) /* -W-4A */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM__SIZE_1             32 /* */ 
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT              2:0 /* -W-VF */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_ZEROBITS     0x00 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_ONEBIT       0x01 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_TWOBITS      0x02 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_THREEBITS    0x03 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_FOURBITS     0x04 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_FIVEBITS     0x05 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_SIXBITS      0x06 /* -W--V */
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_SEVENBITS    0x07 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM                       0x00000280 /* -W-4R */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT                2:0 /* -W-VF */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_ZEROBITS       0x00 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_ONEBIT         0x01 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_TWOBITS        0x02 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_THREEBITS      0x03 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_FOURBITS       0x04 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_FIVEBITS       0x05 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_SIXBITS        0x06 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_SEVENBITS      0x07 /* -W--V */
#define NV1BA0_PIO_SET_HRTF_SUBMIX(i)                      (0x00000290+(i)*4) /* -W-4A */
#define NV1BA0_PIO_SET_HRTF_SUBMIX__SIZE_1                 4 /* */ 
#define NV1BA0_PIO_SET_HRTF_SUBMIX_BIN                     4:0 /* -W-UF */
#define NV1BA0_PIO_SET_HRTF_SUBMIXES                       0x000002C0 /* -W-4R */
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN0                  4:0 /* -W-UF */
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN1                  12:8 /* -W-UF */
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN2                  20:16 /* -W-UF */
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN3                  28:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOLUME_TRACKING                     0x000002A0 /* -W-4R */
#define NV1BA0_PIO_SET_VOLUME_TRACKING_PARAMETER           11:0 /* -W-UF */
#define NV1BA0_PIO_SET_PITCH_TRACKING                      0x000002A4 /* -W-4R */
#define NV1BA0_PIO_SET_PITCH_TRACKING_PARAMETER            11:0 /* -W-UF */
#define NV1BA0_PIO_SET_HRTF_TRACKING                       0x000002A8 /* -W-4R */
#define NV1BA0_PIO_SET_HRTF_TRACKING_PARAMETER             11:0 /* -W-UF */
#define NV1BA0_PIO_SET_ITD_TRACKING                        0x000002AC /* -W-4R */
#define NV1BA0_PIO_SET_ITD_TRACKING_PARAMETER              11:0 /* -W-UF */
#define NV1BA0_PIO_SET_FILTER_TRACKING                     0x000002B0 /* -W-4R */
#define NV1BA0_PIO_SET_FILTER_TRACKING_PARAMETER           11:0 /* -W-UF */
#define NV1BA0_PIO_SET_CURRENT_VOICE                       0x000002F8 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_VOICE_HANDLE                15:0 /* -W-VF */
#define NV1BA0_PIO_VOICE_LOCK                              0x000002FC /* -W-4R */
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER                    7:0 /* -W-VF */
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF                0x00000000 /* -W--V */
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON                 0x00000001 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN                      0x00000300 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V0BIN                4:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V1BIN                9:5 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V2BIN                14:10 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V3BIN                20:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V4BIN                25:21 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V5BIN                30:26 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT                       0x00000304 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6BIN                 4:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7BIN                 9:5 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE               10:10 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE_POSITIVE      0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE_NEGATIVE      0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE               11:11 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE_POSITIVE      0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE_NEGATIVE      0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN                  12:12 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN_X1               0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN_X2               0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM              15:13 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT0    0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT1    0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT2    0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT3    0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT0    0x04 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT1    0x05 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT2    0x06 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT3    0x07 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS0   0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS1   0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS2   0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS3   0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS4   0x04 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS5   0x05 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS6   0x06 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS7   0x07 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLES_PER_BLOCK     20:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_BIN__ALIAS__ 20:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS             21:21 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_OFF         0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_ON          0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE          22:22 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE_OFF      0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE_ON       0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST               23:23 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_OFF           0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_ON            0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE             24:24 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_BUFFER      0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_STREAM      0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP                  25:25 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_OFF              0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_ON               0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX             26:26 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX_FALSE       0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX_TRUE        0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO                27:27 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_DISABLE        0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_ENABLE         0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE           29:28 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_U8        0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S16       0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S24       0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S32       0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE        31:30 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B8     0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B16    0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM  0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B32    0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0                      0x00000308 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_ATTACKRATE        11:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_DELAYTIME         23:12 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EF_PITCHSCALE        31:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA                      0x0000030C /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_DECAYRATE         11:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_HOLDTIME          23:12 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_SUSTAINLEVEL      31:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1                      0x00000310 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_ATTACKRATE        11:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_DELAYTIME         23:12 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_FCSCALE           31:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF                      0x00000314 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_DECAYRATE         11:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_HOLDTIME          23:12 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_SUSTAINLEVEL      31:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC                      0x00000318 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_EF_RELEASERATE       11:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE       14:14 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE_NORMAL 0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE_DELAY 0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE       15:15 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE_NORMAL 0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE_DELAY 0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE                17:16 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_BYPASS         0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_DLS2    0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_P_EQ    0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_BYPASS  0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_DLS2      0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_P_EQ      0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_DLS2_P_EQ 0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_DLS2_I3DL2 0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_P_EQ_I3DL2 0x02 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_I3DL2   0x03 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ                  20:18 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT     23:23 /* -W-VF */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_DISABLE 0x00 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_ENABLE 0x01 /* -W--V */
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_V_GSCNT              31:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_HRTF                      0x0000031C /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_HRTF_HANDLE               15:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY                    0x00000350 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADLY            14:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADR             15:15 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDLY            30:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDR             31:31 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_SSL_A                         0x00000320 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_SSL_A_COUNT                   7:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_SSL_A_BASE                    23:8 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_SSL_B                         0x0000035C /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_SSL_B_COUNT                   7:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_SSL_B_BASE                    23:8 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE                  0x000003A0 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE_OFFSET           23:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO                   0x000003A4 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO_OFFSET            23:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC                   0x000003D4 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC_FRACTION          15:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_BUF_CBO                       0x000003D8 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_BUF_CBO_OFFSET                23:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO                   0x000003DC /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO_OFFSET            23:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA                      0x00000360 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME6_B3_0         3:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME0              15:4 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME7_B3_0         19:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME1              31:20 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB                      0x00000364 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME6_B7_4         3:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME2              15:4 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME7_B7_4         19:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME3              31:20 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC                      0x00000368 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME6_B11_8        3:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME4              15:4 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME7_B11_8        19:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME5              31:20 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_ENV                       0x0000036C /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE        11:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOADLT               21:12 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOFDLT               31:22 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_MOD                       0x00000370 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAAM                7:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFM                15:8 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFC                23:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOFFM                31:24 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_FCA                       0x00000374 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0                   15:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1                   31:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_FCB                       0x00000378 /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_FCB_FC2                   15:0 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_FCB_FC3                   31:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_PITCH                     0x0000037c /* -W-4R */
#define NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP                31:16 /* -W-UF */
#define NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP_SMAX           0x00001fff /* -W--V */
#define NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY                  0x00000160 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY_HANDLE           15:0 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR(i)                             (0x00000400+(i)*4) /* -W-4A */
#define NV1BA0_PIO_SET_HRIR__SIZE_1                        15 /* */ 
#define NV1BA0_PIO_SET_HRIR_LEFT0                          7:0 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR_RIGHT0                         15:8 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR_LEFT1                          23:16 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR_RIGHT1                         31:24 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR_X                              0x0000043C /* -W-4R */
#define NV1BA0_PIO_SET_HRIR_X_LEFT30                       7:0 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR_X_RIGHT30                      15:8 /* -W-VF */
#define NV1BA0_PIO_SET_HRIR_X_ITD                          31:16 /* -W-SF */
#define NV1BA0_PIO_SET_HRIR_X_ITD_SMIN                     0x0000ac01 /* RW--V */
#define NV1BA0_PIO_SET_HRIR_X_ITD_SMAX                     0x000053ff /* RW--V */
#define NV1BA0_PIO_SET_CURRENT_SSL_CONTEXT_DMA             0x0000018C /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_SSL_CONTEXT_DMA_HANDLE      31:0 /* -W-VF */
#define NV1BA0_PIO_SET_CURRENT_SSL                         0x00000190 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_SSL_BASE_PAGE               21:6 /* -W-VF */
#define NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET(i)               (0x00000600+(i)*8) /* -W-4A */
#define NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET__SIZE_1          64 /* */ 
#define NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_PARAMETER        31:0 /* -W-VF */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH(i)               (0x00000604+(i)*8) /* -W-4A */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH__SIZE_1          64 /* */ 
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_PARAMETER        15:0 /* -W-UF */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE   17:16 /* -W-VF */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B8 0x00 /* -W--V */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B16 0x01 /* -W--V */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_ADPCM 0x02 /* -W--V */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B32 0x03 /* -W--V */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_SAMPLES_PER_BLOCK 22:18 /* -W-UF */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO           23:23 /* -W-VF */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_NOTOK     0x00 /* -W--V */
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_OK        0x01 /* -W--V */
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_CONTEXT_DMA       0x00000800 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_CONTEXT_DMA_HANDLE 31:0 /* -W-VF */
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE                   0x00000804 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_HANDLE            31:0 /* -W-VF */
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET            0x00000808 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET_PARAMETER  31:12 /* -W-VF */
#define NV1BA0_PIO_SET_OUTBUF_BA(i)                        (0x00001000+(i)*8) /* -W-4A */
#define NV1BA0_PIO_SET_OUTBUF_BA__SIZE_1                   4 /* */ 
#define NV1BA0_PIO_SET_OUTBUF_BA_ADDRESS                   22:8 /* -W-UF */
#define NV1BA0_PIO_SET_OUTBUF_LEN(i)                       (0x00001004+(i)*8) /* -W-4A */
#define NV1BA0_PIO_SET_OUTBUF_LEN__SIZE_1                  4 /* */ 
#define NV1BA0_PIO_SET_OUTBUF_LEN_VALUE                    22:8 /* -W-UF */
#define NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE                  0x00001800 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_HANDLE           31:0 /* -W-VF */
#define NV1BA0_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA      0x00001804 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA_HANDLE 31:0 /* -W-VF */
#define NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET           0x00001808 /* -W-4R */
#define NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET_PARAMETER 31:12 /* -W-VF */
#define NV1BA0_DMA_NOP                                     0x00000100 /* -W-4R */
#define NV1BA0_DMA_NOP_PARAMETER                           31:0 /* -W-VF */

#endif /* __NV_UAP_H__ */

