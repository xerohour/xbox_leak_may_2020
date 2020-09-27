#include "dmusprod.h"

#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#define DSMIXBIN_FRONT_LEFT         0x00000001
#define DSMIXBIN_FRONT_RIGHT        0x00000002
#define DSMIXBIN_FRONT_CENTER       0x00000004
#define DSMIXBIN_LOW_FREQUENCY      0x00000008
#define DSMIXBIN_BACK_LEFT          0x00000010
#define DSMIXBIN_BACK_RIGHT         0x00000020

#define DSMIXBIN_SPEAKER_MASK       0x0000003F

#define DSMIXBIN_XTLK_FRONT_LEFT    0x00000040
#define DSMIXBIN_XTLK_FRONT_RIGHT   0x00000080
#define DSMIXBIN_XTLK_BACK_LEFT     0x00000100
#define DSMIXBIN_XTLK_BACK_RIGHT    0x00000200
#define DSMIXBIN_XTLK_MASK          0x000003C0

#define DSMIXBIN_I3DL2              0x00000400

#define DSMIXBIN_FXSEND_0           0x00000800
#define DSMIXBIN_FXSEND_1           0x00001000
#define DSMIXBIN_FXSEND_2           0x00002000
#define DSMIXBIN_FXSEND_3           0x00004000
#define DSMIXBIN_FXSEND_4           0x00008000
#define DSMIXBIN_FXSEND_5           0x00010000
#define DSMIXBIN_FXSEND_6           0x00020000
#define DSMIXBIN_FXSEND_7           0x00040000
#define DSMIXBIN_FXSEND_8           0x00080000
#define DSMIXBIN_FXSEND_9           0x00100000
#define DSMIXBIN_FXSEND_10          0x00200000
#define DSMIXBIN_FXSEND_11          0x00400000
#define DSMIXBIN_FXSEND_12          0x00800000
#define DSMIXBIN_FXSEND_13          0x01000000
#define DSMIXBIN_FXSEND_14          0x02000000
#define DSMIXBIN_FXSEND_15          0x04000000
#define DSMIXBIN_FXSEND_16          0x08000000
#define DSMIXBIN_FXSEND_17          0x10000000
#define DSMIXBIN_FXSEND_18          0x20000000
#define DSMIXBIN_FXSEND_19          0x40000000

/* A standard music set up with stereo outs and no reverb or chorus send. */
#define XBOX_APATH_SHARED_STEREO             0xFFFF0001
/* A standard music set up with stereo outs and reverb & chorus sends. */
#define XBOX_APATH_SHARED_STEREOPLUSREVERB   0xFFFF0002
/* An audio path with one dynamic bus from the synth feeding to a dynamic mono buffer. */
#define XBOX_APATH_DYNAMIC_MONO              0xFFFF0003   
/* An audio path with one dynamic bus from the synth feeding to a dynamic 3d buffer.*/
#define XBOX_APATH_DYNAMIC_3D                0xFFFF0004
/* Sends to quad mixbins on channels 1 through 4. */
#define XBOX_APATH_MIXBIN_QUAD               (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT)
/* Sends to quad mixbins on channels 1 through 4 and environmental reverb on 5. */
#define XBOX_APATH_MIXBIN_QUAD_ENV           (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT | DSMIXBIN_I3DL2)
/* Sends to quad mixbins on channels 1 through 4 and music reverb and chorus on 5, 6. */
#define XBOX_APATH_MIXBIN_QUAD_MUSIC         (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT | DSMIXBIN_FXSEND_0 | DSMIXBIN_FXSEND_1)
/* Sends to 5.1 mixbins on channels 1 through 6. */
#define XBOX_APATH_MIXBIN_5DOT1              (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FRONT_CENTER | DSMIXBIN_LOW_FREQUENCY | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT)
/* Sends to 5.1 mixbins on channels 1 through 6 and environmental reverb on 7. */
#define XBOX_APATH_MIXBIN_5DOT1_ENV          (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FRONT_CENTER | DSMIXBIN_LOW_FREQUENCY | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT | DSMIXBIN_I3DL2)
/* Sends to 5.1 mixbins on channels 1 through 6 and music reverb and chorus on 7, 8. */
#define XBOX_APATH_MIXBIN_5DOT1_MUSIC        (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FRONT_CENTER | DSMIXBIN_LOW_FREQUENCY | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT | DSMIXBIN_FXSEND_0 | DSMIXBIN_FXSEND_1)
/* 1,2 -> Stereo, 3 -> environmental reverb, 4 through 8 -> effects. */
#define XBOX_APATH_MIXBIN_STEREO_EFFECTS     (DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_I3DL2 | DSMIXBIN_FXSEND_0 | DSMIXBIN_FXSEND_1 | DSMIXBIN_FXSEND_2 | DSMIXBIN_FXSEND_3 | DSMIXBIN_FXSEND_4)


// {139F9905-BEE2-4a6a-A30E-B3ED94E7CC98}
DEFINE_GUID(CLSID_XboxAddinComponent, 0x139f9905, 0xbee2, 0x4a6a, 0xa3, 0xe, 0xb3, 0xed, 0x94, 0xe7, 0xcc, 0x98);

// {AEE78E4D-8818-4020-AA5D-997756AF53FB}
DEFINE_GUID(CLSID_XboxSynth, 
0xaee78e4d, 0x8818, 0x4020, 0xaa, 0x5d, 0x99, 0x77, 0x56, 0xaf, 0x53, 0xfb);

// {E51F2AA9-F7B6-4397-9F70-783FC3642EF8}
DEFINE_GUID(GUID_Xbox_PROP_PCSynth, 
0xe51f2aa9, 0xf7b6, 0x4397, 0x9f, 0x70, 0x78, 0x3f, 0xc3, 0x64, 0x2e, 0xf8);

// {207348EF-09E7-425c-BBBA-94C92FEF09B7}
DEFINE_GUID(GUID_Xbox_PROP_XboxSynth, 
0x207348ef, 0x9e7, 0x425c, 0xbb, 0xba, 0x94, 0xc9, 0x2f, 0xef, 0x9, 0xb7);


DEFINE_GUID(GUID_Xbox_PROP_XboxAudioPath, 
0xa70f376a, 0x49f1, 0x4339, 0xbb, 0x93, 0xb2, 0x9b, 0x67, 0xee, 0xf5, 0x88);





#endif CONDUCTOR_H
