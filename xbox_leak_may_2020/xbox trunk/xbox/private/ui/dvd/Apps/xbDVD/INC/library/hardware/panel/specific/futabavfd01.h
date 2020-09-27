// FILE:			library\hardware\panel\specific\futabavfd01.h
// AUTHOR:		H.Horak
// COPYRIGHT:	(c) 1999 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:		13.08.1999
//
// PURPOSE:		Futaba VFD #1 segment position definitions
//
// HISTORY:

#ifndef FUTABA01VFD_H
#define FUTABA01VFD_H

// LAST_SEGMENT is used to determin VFD array size
#define LAST_SEGMENT 139
#define DISPLAY_SIZE 1+(LAST_SEGMENT>>3)	// Size of BYTE array

// VFD segment position in bitfield
#define SEG_DTS			3
#define SEG_ZOOM			4
#define SEG_5_1CH			5
#define SEG_PBC			6
#define SEG_LAST			7
#define SEG_INDEX			8
#define SEG_DIGITAL		40
#define SEG_DOLBY			41
#define SEG_ANGLE			64
#define SEG_REMAIN		65
#define SEG_TRACK			80
#define SEG_CHAPTER		88
#define SEG_B				89
#define SEG_A_TO			90
#define SEG_CHANNEL		104
#define SEG_TITLE			112
#define SEG_REPEAT		113
#define SEG_REPEAT_1		114	// belongs to repeat for repeat once
#define SEG_MICROPHONE	126
#define SEG_CUSTOM_0		127
#define SEG_CUSTOM_1		128
#define SEG_CUSTOM_2		129
#define SEG_PAUSE			130
#define SEG_PLAY			131
#define SEG_CD				132
#define SEG_VCD			133
#define SEG_DVD			134
#define SEG_SUPER			135	// belongs to VCD for SVCD
#define SEG_LOCKED		136
#define SEG_L				137
#define SEG_R				138
#define SEG_SLASH			139	// This is LAST_SEGMENT

#define SEG_PIE_OUTLINE	121
#define SEG_PIE_POS_0	122
#define SEG_PIE_POS_1	123
#define SEG_PIE_POS_2	124
#define SEG_PIE_POS_3	125

//  a
// b c
//  d
// e f
//  g

#define SEG_MINUTE_SEPERATOR	32
#define SEG_HOUR_SEPERATOR 	56

#define SEG7_0_a	9
#define SEG7_0_b	11
#define SEG7_0_c	10
#define SEG7_0_d	12
#define SEG7_0_e	14
#define SEG7_0_f	13
#define SEG7_0_g	15

#define SEG7_1_a	33
#define SEG7_1_b	35
#define SEG7_1_c	34
#define SEG7_1_d	36
#define SEG7_1_e	38
#define SEG7_1_f	37
#define SEG7_1_g	39

#define SEG7_2_a	25
#define SEG7_2_b	27
#define SEG7_2_c	26
#define SEG7_2_d	28
#define SEG7_2_e	30
#define SEG7_2_f	29
#define SEG7_2_g	31

#define SEG7_3_a	57
#define SEG7_3_b	59
#define SEG7_3_c	58
#define SEG7_3_d	60
#define SEG7_3_e	62
#define SEG7_3_f	61
#define SEG7_3_g	63

#define SEG7_4_a	49
#define SEG7_4_b	51
#define SEG7_4_c	50
#define SEG7_4_d	52
#define SEG7_4_e	54
#define SEG7_4_f	53
#define SEG7_4_g	55

#define SEG7_5_a	81
#define SEG7_5_b	83
#define SEG7_5_c	82
#define SEG7_5_d	84
#define SEG7_5_e	86
#define SEG7_5_f	85
#define SEG7_5_g	87

#define SEG7_6_a	73
#define SEG7_6_b	75
#define SEG7_6_c	74
#define SEG7_6_d	76
#define SEG7_6_e	78
#define SEG7_6_f	77
#define SEG7_6_g	79

#define SEG7_7_a	105
#define SEG7_7_b	107
#define SEG7_7_c	106
#define SEG7_7_d	108
#define SEG7_7_e	110
#define SEG7_7_f	109
#define SEG7_7_g	111

#define SEG7_8_a	97
#define SEG7_8_b	99
#define SEG7_8_c	98
#define SEG7_8_d	100
#define SEG7_8_e	102
#define SEG7_8_f	101
#define SEG7_8_g	103

#endif
