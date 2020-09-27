/*
** vtxpgmconsts.h
**
** Vertex Program Constant Declarations.
**
** Copyright 2000 NVIDIA, Corporation.  All rights reserved.
** 
** THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
** NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
** IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*/

#ifndef __gl_vtxpgmconsts_h_
#define __gl_vtxpgmconsts_h_

#pragma warning (disable:4244)  /* Disable bogus VC++ 4.2 conversion warnings. */
#pragma warning (disable:4305)  /* VC++ 5.0 version of above warning. */

/*
** This file is shared between OpenGL and D3D so don't mess with it!
*/

/*
** Constants used for programs:
*/

#define __GL_NUMBER_OF_VERTEX_ATTRIBS       16
#define __GL_NUMBER_OF_PROGRAM_MATRICES     8
#define __GL_NUMBER_OF_TEMPORARY_REGISTERS  12
#define __GL_NUMBER_OF_RESULT_REGISTERS     16
#define __GL_NUMBER_OF_CONSTANT_REGISTERS   96
#define __GL_NUMBER_OF_PADDING_REGISTERS    128
#define __GL_MAX_NUMBER_OF_INSTRUCTIONS     128

#define __GL_ACTUAL_NUMBER_OF_INSTRUCTIONS_KELVIN   134

#define __GL_KELVIN_FIRST_USER_CONSTANT_REGISTER    96


/*
** Names for each of the vertex attributes
*/

#define __GL_ATTRIB_POSITION            0
#define __GL_ATTRIB_VERTEX_WEIGHT       1
#define __GL_ATTRIB_NORMAL              2
#define __GL_ATTRIB_COLOR               3
#define __GL_ATTRIB_SECONDARY_COLOR     4
#define __GL_ATTRIB_FOG_COORD           5
#define __GL_ATTRIB_UNUSED0             6
#define __GL_ATTRIB_UNUSED1             7
#define __GL_ATTRIB_TEXCOORD0           8
#define __GL_ATTRIB_TEXCOORD1           9
#define __GL_ATTRIB_TEXCOORD2           10
#define __GL_ATTRIB_TEXCOORD3           11
#define __GL_ATTRIB_TEXCOORD4           12
#define __GL_ATTRIB_TEXCOORD5           13
#define __GL_ATTRIB_TEXCOORD6           14
#define __GL_ATTRIB_TEXCOORD7           15

/*
** Names for the output components:
*/

#define __GL_ATTRIB_OUTPUT_HPOS         0
#define __GL_ATTRIB_OUTPUT_COL0         3
#define __GL_ATTRIB_OUTPUT_COL1         4
#define __GL_ATTRIB_OUTPUT_BFC0         7
#define __GL_ATTRIB_OUTPUT_BFC1         8
#define __GL_ATTRIB_OUTPUT_FOGC         5
#define __GL_ATTRIB_OUTPUT_PSIZ         6
#define __GL_ATTRIB_OUTPUT_TEX0         9
#define __GL_ATTRIB_OUTPUT_TEX1         10
#define __GL_ATTRIB_OUTPUT_TEX2         11
#define __GL_ATTRIB_OUTPUT_TEX3         12
#define __GL_ATTRIB_OUTPUT_TEX4         13
#define __GL_ATTRIB_OUTPUT_TEX5         14
#define __GL_ATTRIB_OUTPUT_TEX6         15
#define __GL_ATTRIB_OUTPUT_TEX7         15

/*
** Enable bits tested by compiled x886 code:
*/

#define __GL_PROGRAM_ENABLE_TEXTURE0    1
#define __GL_PROGRAM_ENABLE_TEXTURE1    2

#endif /* __gl_vtxpgmconsts_h_ */
