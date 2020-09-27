#ifndef __RESTYPES_H__
#define __RESTYPES_H__
// #define for resource types as a word
// same as in windows.h but as words as opposed to lpstrs
//

#define W_RT_NEWRESOURCE    0x2000
#define W_RT_ERROR          0x7fff
#define W_RT_CURSOR         1
#define W_RT_BITMAP         2
#define W_RT_ICON           3
#define W_RT_MENU           4
#define W_RT_DIALOG         5
#define W_RT_STRING         6
#define W_RT_FONTDIR        7
#define W_RT_FONT           8
#define W_RT_ACCELERATOR    9
#define W_RT_RCDATA         10

// folowing are not in windows.h

#define W_RT_ERROR_TABLE    11
#define W_RT_GROUP_CURSOR   12
/* The value 13 is unused */
#define W_RT_GROUP_ICON     14
#define W_RT_NAME_TABLE     15 /* Name Tables no longer exist (made for 3.1) */
#define W_RT_VERSION        16

// new, from winuser.h
#define W_RT_PLUGPLAY       19
#define W_RT_VXD            20
#define W_RT_ANICURSOR		21
#define W_RT_ANIICON		22
#define W_RT_HTML			23
// app framework stuff

#define W_RT_DLGINIT			240
#define W_RT_TOOLBAR			241
#define W_RT_GROUP_STRING		255

// WAVE file stuff
#define SZ_RT_WAVE "WAVE"

// flags in a resource type object

#define RTT_NONE    0x0000
#define RTT_GROUP   0x0001      // is a group
#define RTT_IGNORE  0x0002      // don't display these
#define RTT_NONEW   0x0004      // can't new these
#define RTT_ONLYONE 0x0008      // make only one
#define RTT_NORCOUT 0x0010      // don't call OutputToRc for this one
#define RTT_MFCONLY 0x0020		// only allowed in Mfc mode Compositions

#endif