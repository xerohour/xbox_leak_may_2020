/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	Overlay and Closed Captioning test

Author:

	Jason Gould (jgould) 2001

Environment:

	Xbox only

Revision History:

	2001		jgould
		Initial Version
	14-Nov-2001 robheit
		Cleaned up the code and made functional

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
//	Externs:
//------------------------------------------------------------------------------
extern "C" unsigned long HalReadSMBusValue(UCHAR, UCHAR, BOOLEAN, ULONG*);
extern "C" unsigned long HalWriteSMBusValue(UCHAR, UCHAR, BOOLEAN, ULONG);
extern "C" void DebugPrint(const char*, ...);

extern "C"
{
void    WINAPI D3DDevice_EnableCC(BOOL Enable);
void    WINAPI D3DDevice_SendCC(BOOL Field, BYTE cc1, BYTE cc2);
void    WINAPI D3DDevice_GetCCStatus(BOOL *pField1, BOOL *pField2);
};

//------------------------------------------------------------------------------
//	Cosed Captioning Codes:
//------------------------------------------------------------------------------

// Control Codes
#define	CC_RCL	0x14, 0x20	/* Resume caption loading */
#define	CC_BS	0x14, 0x21	/* Backspace */
#define	CC_AOF	0x14, 0x22	/* Reserved (formerly Alarm Off) */
#define	CC_AON	0x14, 0x23	/* Reserved (formerly Alarm On)  */
#define	CC_DER	0x14, 0x24	/* Delete to End of Row */
#define	CC_RU2	0x14, 0x25	/* Roll-Up Captions-2 Rows */
#define	CC_RU3	0x14, 0x26	/* Roll-Up Captions-3 Rows */
#define	CC_RU4	0x14, 0x27	/* Roll-Up Captions-4 Rows */
#define	CC_FON	0x14, 0x28	/* Flash On */
#define	CC_RDC	0x14, 0x29	/* Resume Direct Captioning */
#define	CC_TR	0x14, 0x2a	/* Text Restart */
#define	CC_RTD	0x14, 0x2b	/* Resume Text Display */
#define	CC_EDM	0x14, 0x2c	/* Erase Displayed Memory */
#define	CC_CR	0x14, 0x2d	/* Carriage Return */
#define	CC_ENM	0x14, 0x2e	/* Erase Non-Displayed Memory */
#define	CC_EOC	0x14, 0x2f	/* End fo Caption (Flip Memories) */
#define	CC_TO1	0x17, 0x21	/* Tab Offset 1 Column */
#define	CC_TO2	0x17, 0x22	/* Tab Offset 2 Columns */
#define	CC_TO3	0x17, 0x23	/* Tab Offset 3 Columns */

// Mid-Row Codes
#define CC_WHITE				0x11, 0x20	/* White */
#define CC_WHITE_UNDERLINE		0x11, 0x21	/* White Underline */
#define CC_GREEN				0x11, 0x22	/* Green */
#define CC_GREEN_UNDERLINE		0x11, 0x23	/* Green Underline */
#define CC_BLUE					0x11, 0x24	/* Blue */
#define CC_BLUE_UNDERLINE		0x11, 0x25	/* Blue Underline */
#define CC_CYAN					0x11, 0x26	/* Cyan */
#define CC_CYAN_UNDERLINE		0x11, 0x27	/* Cyan Underline */
#define CC_RED					0x11, 0x28	/* Red */
#define CC_RED_UNDERLINE		0x11, 0x29	/* Red Underline */
#define CC_YELLOW				0x11, 0x2a	/* Yellow */
#define CC_YELLOW_UNDERLINE		0x11, 0x2b	/* Yellow Underline */
#define CC_MAGENTA				0x11, 0x2c	/* Magenta */
#define CC_MAGENTA_UNDERLINE	0x11, 0x2d	/* Magenta Underline */
#define CC_ITALICS				0x11, 0x2e	/* Italics */
#define CC_ITALICS_UNDERLINE	0x11, 0x2f	/* Italics Underline */

// Special Characters
#define CC_REGISTERED			0x11, 0x30	/* Registered mark symbol */
#define CC_DEGREE				0x11, 0x31	/* Degree sign */
#define CC_HALF					0x11, 0x32	/* 1/2 */
#define CC_IQ					0x11, 0x33	/* Inverse Query */
#define CC_TRADEMARK			0x11, 0x34	/* Trademark symbol */
#define CC_CENTS				0x11, 0x35	/* Cents sign */
#define CC_POUNDS_STERLING		0x11, 0x36	/* Pounds Sterling sign */
#define CC_NOTE					0x11, 0x37	/* Musical note */
#define CC_a_GRAVE				0x11, 0x38	/* Lowercase a with grave accent */
#define CC_TRANSPARENT_SPACE	0x11, 0x39	/* Transparent space */
#define CC_e_GRAVE				0x11, 0x3a	/* Lowercase e with grave accent */
#define CC_a_CIRCUMFLEX			0x11, 0x3b	/* Lowercase a with circumflex */
#define CC_e_CIRCUMFLEX			0x11, 0x3c	/* Lowercase e with circumflex */
#define CC_i_CIRCUMFLEX			0x11, 0x3d	/* Lowercase i with circumflex */
#define CC_o_CIRCUMFLEX			0x11, 0x3e	/* Lowercase o with circumflex */
#define CC_u_CIRCUMFLEX			0x11, 0x3f	/* Lowercase u with circumflex */

// Standard Characters
#define CC_SPACE				0x20	/* Standard Space */
#define CC_EXCLAMATION			0x21	/* ! */
#define CC_QUOTE				0x22	/* " */
#define CC_POUND				0x23	/* # */
#define CC_DOLLAR				0x24	/* $ */
#define CC_PERCENT				0x25	/* % */
#define CC_AMPERSAND			0x26	/* & */
#define CC_APOSTROPHE			0x27	/* ' */
#define CC_OPEN_PARENTHESES		0x28	/* ( */
#define CC_CLOSE_PARENTHESES	0x29	/* ) */
#define CC_a_ACCENT				0x2a	/* Lowercase a with acute accent */
#define CC_PLUS					0x2b	/* + */
#define CC_COMMA				0x2c	/* , */
#define CC_MINUS				0x2d	/* - */
#define CC_PERIOD				0x2e	/* . */
#define CC_SLASH				0x2f	/* / */
#define CC_0					0x30	/* 0 */
#define CC_1					0x31	/* 1 */
#define CC_2					0x32	/* 2 */
#define CC_3					0x33	/* 3 */
#define CC_4					0x34	/* 4 */
#define CC_5					0x35	/* 5 */
#define CC_6					0x36	/* 6 */
#define CC_7					0x37	/* 7 */
#define CC_8					0x38	/* 8 */
#define CC_9					0x39	/* 9 */
#define CC_COLON				0x3a	/* : */
#define CC_SEMICOLON			0x3b	/* ; */
#define CC_LESS_THAN			0x3c	/* < */
#define CC_EQUAL				0x3d	/* = */
#define CC_GREATER_THAN			0x3e	/* > */
#define CC_QUESTION				0x3f	/* ? */
#define CC_AT					0x40	/* @ */
#define CC_A					0x41	/* A */
#define CC_B					0x42	/* B */
#define CC_C					0x43	/* C */
#define CC_D					0x44	/* D */
#define CC_E					0x45	/* E */
#define CC_F					0x46	/* F */
#define CC_G					0x47	/* G */
#define CC_H					0x48	/* H */
#define CC_I					0x49	/* I */
#define CC_J					0x4a	/* J */
#define CC_K					0x4b	/* K */
#define CC_L					0x4c	/* L */
#define CC_M					0x4d	/* M */
#define CC_N					0x4e	/* N */
#define CC_O					0x4f	/* O */
#define CC_P					0x50	/* P */
#define CC_Q					0x51	/* Q */
#define CC_R					0x52	/* R */
#define CC_S					0x53	/* S */
#define CC_T					0x54	/* T */
#define CC_U					0x55	/* U */
#define CC_V					0x56	/* V */
#define CC_W					0x57	/* W */
#define CC_X					0x58	/* X */
#define CC_Y					0x59	/* Y */
#define CC_Z					0x5a	/* Z */
#define CC_OPEN_BRACKET			0x5b	/* [ */
#define CC_e_ACCENT				0x5c	/* Lowercase e with acute accent */
#define CC_CLOSE_BRACKET		0x5d	/* ] */
#define CC_i_ACCENT				0x5e	/* Lowercase i with acute accent */
#define CC_o_ACCENT				0x5f	/* Lowercase o with acute accent */
#define CC_u_ACCENT				0x60	/* Lowercase u with acute accent */
#define CC_a					0x61	/* a */
#define CC_b					0x62	/* b */
#define CC_c					0x63	/* c */
#define CC_d					0x64	/* d */
#define CC_e					0x65	/* e */
#define CC_f					0x66	/* f */
#define CC_g					0x67	/* g */
#define CC_h					0x68	/* h */
#define CC_i					0x69	/* i */
#define CC_j					0x6a	/* j */
#define CC_k					0x6b	/* k */
#define CC_l					0x6c	/* l */
#define CC_m					0x6d	/* m */
#define CC_n					0x6e	/* n */
#define CC_o					0x6f	/* o */
#define CC_p					0x70	/* p */
#define CC_q					0x71	/* q */
#define CC_r					0x72	/* r */
#define CC_s					0x73	/* s */
#define CC_t					0x74	/* t */
#define CC_u					0x75	/* u */
#define CC_v					0x76	/* v */
#define CC_w					0x77	/* w */
#define CC_x					0x78	/* x */
#define CC_y					0x79	/* y */
#define CC_z					0x7a	/* z */
#define CC_c_CEDILLA			0x7b	/* Lowercase c with cedilla */
#define CC_DIVIDE				0x7c	/* Division sign */
#define CC_N_TILDE				0x7d	/* Uppercase N with tilde */
#define CC_n_TILDE				0x7e	/* Uppercase N with tilde */
#define CC_BLOCK				0x7f	/* Solid block */

//------------------------------------------------------------------------------
//	Closed Captioning Row Styles
//------------------------------------------------------------------------------
enum CCStyle
{
	CCSTYLE_WHITE,
	CCSTYLE_WHITE_UNDERLINE,
	CCSTYLE_GREEN,
	CCSTYLE_GREEN_UNDERLINE,
	CCSTYLE_BLUE,
	CCSTYLE_BLUE_UNDERLINE,
	CCSTYLE_CYAN,
	CCSTYLE_CYAN_UNDERLINE,
	CCSTYLE_RED,
	CCSTYLE_RED_UNDERLINE,
	CCSTYLE_YELLOW,
	CCSTYLE_YELLOW_UNDERLINE,
	CCSTYLE_MAGENTA,
	CCSTYLE_MAGENTA_UNDERLINE,
	CCSTYLE_WHITE_ITALICS,
	CCSTYLE_WHITE_ITALICS_UNDERLINE,
	CCSTYLE_INDENT0,
	CCSTYLE_INDENT0_UNDERLINE,
	CCSTYLE_INDENT4,
	CCSTYLE_INDENT4_UNDERLINE,
	CCSTYLE_INDENT8,
	CCSTYLE_INDENT8_UNDERLINE,
	CCSTYLE_INDENT12,
	CCSTYLE_INDENT12_UNDERLINE,
	CCSTYLE_INDENT16,
	CCSTYLE_INDENT16_UNDERLINE,
	CCSTYLE_INDENT20,
	CCSTYLE_INDENT20_UNDERLINE,
	CCSTYLE_INDENT24,
	CCSTYLE_INDENT24_UNDERLINE,
	CCSTYLE_INDENT28,
	CCSTYLE_INDENT28_UNDERLINE,
	NUM_CCSTYLE
};

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define RAND_INT(x)			((rand() * (x)) / RAND_MAX)
#define RANDOM_VELOCITY()	(RAND_INT(2) + 1)
#define GetBValue(rgb)		((BYTE)((rgb) & 0xff))
#define GetGValue(rgb)		((BYTE)(((rgb) >> 8) & 0xff))
#define GetRValue(rgb)		((BYTE)(((rgb) >> 16) & 0xff))

//------------------------------------------------------------------------------
//	Structures and typedefs
//------------------------------------------------------------------------------
struct CLOSEDCAPTION 
{
	BOOL field;
	BYTE cc0;
	BYTE cc1;
};

struct TheVerts 
{ 
	float	x;
	float	y;
	float	z;
	float	w; 
	DWORD	color; 
	float	u;
	float	v; 
};

//------------------------------------------------------------------------------
//	Static Global Variables:
//------------------------------------------------------------------------------
CLOSEDCAPTION g_CC[] =
{
	1, CC_A, CC_B,
	1, CC_C, CC_D,
	1, CC_E, CC_F,
	1, CC_G, CC_H,
	1, CC_I, CC_J,
	1, CC_K, CC_L,
	1, CC_M, CC_N,
	1, CC_O, CC_P,
	1, CC_Q, CC_R,
	1, CC_S, CC_T,
	1, CC_U, CC_V,
	1, CC_W, CC_X,
	1, CC_Y, CC_Z,
	1, CC_CR,
	1, CC_a, CC_b,
	1, CC_c, CC_d,
	1, CC_e, CC_f,
	1, CC_g, CC_h,
	1, CC_i, CC_j,
	1, CC_k, CC_l,
	1, CC_m, CC_n,
	1, CC_o, CC_p,
	1, CC_q, CC_r,
	1, CC_s, CC_t,
	1, CC_u, CC_v,
	1, CC_w, CC_x,
	1, CC_y, CC_z,
	1, CC_CR,
	1, CC_0, CC_1,
	1, CC_2, CC_3,
	1, CC_4, CC_5,
	1, CC_6, CC_7,
	1, CC_8, CC_9,
	1, CC_CR,
	1, CC_EXCLAMATION, CC_QUOTE, 
	1, CC_POUND, CC_DOLLAR, 
	1, CC_PERCENT, CC_AMPERSAND, 
	1, CC_APOSTROPHE, CC_OPEN_PARENTHESES, 
	1, CC_CLOSE_PARENTHESES, CC_PLUS, 
	1, CC_COMMA, CC_MINUS, 
	1, CC_PERIOD, CC_SLASH, 
	1, CC_COLON, CC_SEMICOLON, 
	1, CC_LESS_THAN, CC_EQUAL, 
	1, CC_GREATER_THAN, CC_QUESTION, 
	1, CC_AT, CC_OPEN_BRACKET, 
	1, CC_CLOSE_BRACKET, CC_DIVIDE, 
	1, CC_CR,
	1, CC_REGISTERED, 
	1, CC_DEGREE,
	1, CC_HALF, 
	1, CC_IQ,
	1, CC_TRADEMARK, 
	1, CC_CENTS,
	1, CC_POUNDS_STERLING, 
	1, CC_NOTE,
	1, CC_a_GRAVE, 
	1, CC_TRANSPARENT_SPACE,
	1, CC_e_GRAVE, 
	1, CC_a_CIRCUMFLEX,
	1, CC_e_CIRCUMFLEX, 
	1, CC_i_CIRCUMFLEX,
	1, CC_o_CIRCUMFLEX, 
	1, CC_u_CIRCUMFLEX,
	1, CC_e_ACCENT, CC_i_ACCENT,
	1, CC_o_ACCENT, CC_u_ACCENT,
	1, CC_c_CEDILLA, CC_N_TILDE,
	1, CC_n_TILDE, CC_BLOCK
};
int					g_sx2;
int					g_sy1;
int					g_sy2;
int					g_dx1;
int					g_dx2;
int					g_dy1;
int					g_dy2;
int					g_swidth;
int					g_sheight;
int					g_sx1	= -5;
int					g_dsx1	= -2;
int					g_dsx2	= 5;
int					g_dsy1	= -3;
int					g_dsy2	= 2;
int					g_ddx1	= -4;
int					g_ddx2	= 1;
int					g_ddy1	= -3;
int					g_ddy2	= 4;
IDirect3DDevice8*	g_pDev	= NULL;
D3DTexture*			g_pTex0	= NULL;
D3DTexture*			g_pTex1	= NULL;
D3DTexture*			g_pTex2	= NULL;
D3DTexture*			g_pTex3	= NULL;
DWORD				g_dwFVF	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
int					g_delay = 1;	// Number of milliseconds to delay
									// between calls to GetCCStatus.
									// Setting this to 0 will eliminate the delay.

TheVerts	g_originalVerts[] =
{
	{113.0f,   0.0f, 0.5f, 1.0f, 0xffff0000, 0.0f, 0.0f},
	{527.0f, 360.0f, 0.5f, 1.0f, 0xff0000ff, 1.0f, 1.0f},
	{113.0f, 360.0f, 0.5f, 1.0f, 0xff0000ff, 0.0f, 1.0f},
	{527.0f,   0.0f, 0.5f, 1.0f, 0xffff0000, 1.0f, 0.0f},
	{527.0f, 360.0f, 0.5f, 1.0f, 0xff0000ff, 1.0f, 1.0f},
	{113.0f,   0.0f, 0.5f, 1.0f, 0xffff0000, 0.0f, 0.0f},
};
TheVerts	g_verts[6];

//------------------------------------------------------------------------------
//	WriteSMC
//------------------------------------------------------------------------------
DWORD 
WriteSMC(
		 unsigned char addr, 
		 unsigned char value
		 )
{
	return HalWriteSMBusValue(0x20, addr, FALSE, value);
}

//------------------------------------------------------------------------------
//	ReadSMC
//------------------------------------------------------------------------------
DWORD 
ReadSMC(
		unsigned char addr
		)
{
	DWORD value = 0xCCCCCCCC;
	DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
	return value;
}

//------------------------------------------------------------------------------
//	Setp
//------------------------------------------------------------------------------
BYTE 
Setp(
	 BYTE b
	 ) 
/*++

Routine Description:

	set the parity bit...

Arguments:

	IN b -	Byte to set parity bit on

Return Value:

	Adjusted byte

--*/
{
	_asm 
	{
		xor eax, eax
		mov al, [b]
		test al, al
		jnp dontsetpb
		or al, 0x80
		dontsetpb:
	}
}

//------------------------------------------------------------------------------
//	SetCol
//------------------------------------------------------------------------------
void 
SetCol(
	   BYTE		col, 
	   BYTE*	a, 
	   BYTE*	b
	   ) 
/*++

Routine Description:

	My gueses on how this works have been unsuccessful...
	sending 0x94, 0x76 seems to set the column to about 12,
	and 0x94,74 seems to set it to about 8
	but it seems to also change the row. This may just be my TV though.

Arguments:

	None

Return Value:

	None

--*/
{
	*a = 0x14;
	if(col & 1) 
		*b = (col >> 2) | 0x50;
	else 
		*b = (col >> 2) | 0x70;
}

//------------------------------------------------------------------------------
//	Bounce
//------------------------------------------------------------------------------
void 
Bounce(
	   int	f, 
	   int*	a, 
	   int*	b, 
	   int	l, 
	   int*	pa, 
	   int*	pb
	   ) 
/*++

Routine Description:

	bounces one or two numbers off of eachother, and the min (f) and max (l).
	f is inclusive, l is exclusive, a and b are inclusive
	if b is not specified, a will bounce on the interval [f, l)
	spd is used to increment a and b.
	pa and pb are the current speeds of a and b (should be (+/- spd))

Arguments:

	None

Return Value:

	None

--*/
{
	if(!pa || !a) 
		_asm {int 3};
	if(pb && !b) 
		_asm {int 3};

	if(*a < f + abs(*pa))
		*pa =  abs(*pa);
	if(pb)
	{
		if(*a > *b - abs(*pa) - abs(*pb))
		{ 
			*pa = -abs(*pa); 
			*pb = abs(*pb); 
		}
	}
	if(pb)
	{
		if(*b >= l - abs(*pb))
			*pb = -abs(*pb);
	}
	if(!pb)
	{
		if(*a >= l - abs(*pa))
			*pa = -abs(*pa);
	}

	*a += *pa;
	if(pb)
		*b += *pb;
}

//------------------------------------------------------------------------------
//	SendCC
//------------------------------------------------------------------------------
void
SendCC(
	   IN BOOL	bField,
	   IN BYTE	cc1,
	   IN BYTE	cc2
	   )
/*++

Routine Description:

	Send the CC data

Arguments:

	IN bField -	Field: TRUE = field1, FALSE = field2

Return Value:

	None

--*/
{
	BOOL	bF1;
	BOOL	bF2;

	do
	{
		Sleep(g_delay);
		D3DDevice_GetCCStatus(&bF1, &bF2);
	}
	while((!bField && !bF2) || (bField && !bF1));
	D3DDevice_SendCC(bField, Setp(cc1), Setp(cc2));
}

//------------------------------------------------------------------------------
//	SetRow
//------------------------------------------------------------------------------
void 
SetRow(
	   IN BOOL		bField,
	   IN BYTE		channel,
	   IN BYTE		row, 
	   IN CCStyle	style
	   )
/*++

Routine Description:

	Sets a row style by making a SendCC call

Arguments:

	IN bField -		Field: TRUE == field1, FALSE == field2
	IN channel -	1 or 2
	IN row -		[1, 15]
	IN style -		Style. See CCStyle above

Return Value:

	None

--*/
{
	BYTE	a;
	BYTE	b;

	switch(row) 
	{
	case  1: 
		a = 0x11; 
		b = 0x40 + style; 
		break;
	case  2: 
		a = 0x11; 
		b = 0x60 + style; 
		break;
	case  3: 
		a = 0x12; 
		b = 0x40 + style; 
		break;
	case  4: 
		a = 0x12; 
		b = 0x60 + style; 
		break;
	case  5: 
		a = 0x15; 
		b = 0x40 + style; 
		break;
	case  6: 
		a = 0x15; 
		b = 0x60 + style; 
		break;
	case  7: 
		a = 0x16; 
		b = 0x40 + style; 
		break;
	case  8: 
		a = 0x16; 
		b = 0x60 + style; 
		break;
	case  9: 
		a = 0x17; 
		b = 0x40 + style; 
		break;
	case 10: 
		a = 0x17; 
		b = 0x60 + style; 
		break;
	case 11: 
		a = 0x10; 
		b = 0x40 + style; 
		break;
	case 12: 
		a = 0x13; 
		b = 0x40 + style; 
		break;
	case 13: 
		a = 0x13; 
		b = 0x60 + style; 
		break;
	case 14: 
		a = 0x14; 
		b = 0x40 + style; 
		break;
	case 15: 
		a = 0x14; 
		b = 0x60 + style; 
		break;
	}

	if(channel == 2) 
		a |= 8;

	SendCC(bField, a, b);
}

//------------------------------------------------------------------------------
//	AsciiToCC
//------------------------------------------------------------------------------
BYTE
AsciiToCC(
		  IN char	c
		  )
/*++

Routine Description:

	Converts an ascii character to a CC byte code. This method will only
	return a single byte code. SendCC needs two per call.

Arguments:

	IN c -	Character to convert

Return Value:

	CC byte code

--*/
{
	switch(c)
	{
	case ' ':
		return CC_SPACE;
	case '!':
		return CC_EXCLAMATION;
	case '"':
		return CC_QUOTE;
	case '#':
		return CC_POUND;
	case '$':
		return CC_DOLLAR;
	case '%':
		return CC_PERCENT;
	case '&':
		return CC_AMPERSAND;
	case '\'':
		return CC_APOSTROPHE;
	case '(':
		return CC_OPEN_PARENTHESES;
	case ')':
		return CC_CLOSE_PARENTHESES;
	case '+':
		return CC_PLUS;
	case ',':
		return CC_COMMA;
	case '-':
		return CC_MINUS;
	case '.':
		return CC_PERIOD;
	case '/':
		return CC_SLASH;
	case '0':
		return CC_0;
	case '1':
		return CC_1;
	case '2':
		return CC_2;
	case '3':
		return CC_3;
	case '4':
		return CC_4;
	case '5':
		return CC_5;
	case '6':
		return CC_6;
	case '7':
		return CC_7;
	case '8':
		return CC_8;
	case '9':
		return CC_9;
	case ':':
		return CC_COLON;
	case ';':
		return CC_SEMICOLON;
	case '<':
		return CC_LESS_THAN;
	case '=':
		return CC_EQUAL;
	case '>':
		return CC_GREATER_THAN;
	case '?':
		return CC_QUESTION;
	case '@':
		return CC_AT;
	case 'A':
		return CC_A;
	case 'B':
		return CC_B;
	case 'C':
		return CC_C;
	case 'D':
		return CC_D;
	case 'E':
		return CC_E;
	case 'F':
		return CC_F;
	case 'G':
		return CC_G;
	case 'H':
		return CC_H;
	case 'I':
		return CC_I;
	case 'J':
		return CC_J;
	case 'K':
		return CC_K;
	case 'L':
		return CC_L;
	case 'M':
		return CC_M;
	case 'N':
		return CC_N;
	case 'O':
		return CC_O;
	case 'P':
		return CC_P;
	case 'Q':
		return CC_Q;
	case 'R':
		return CC_R;
	case 'S':
		return CC_S;
	case 'T':
		return CC_T;
	case 'U':
		return CC_U;
	case 'V':
		return CC_V;
	case 'W':
		return CC_W;
	case 'X':
		return CC_X;
	case 'Y':
		return CC_Y;
	case 'Z':
		return CC_Z;
	case '[':
		return CC_OPEN_BRACKET;
	case ']':
		return CC_CLOSE_BRACKET;
	case 'a':
		return CC_a;
	case 'b':
		return CC_b;
	case 'c':
		return CC_c;
	case 'd':
		return CC_d;
	case 'e':
		return CC_e;
	case 'f':
		return CC_f;
	case 'g':
		return CC_g;
	case 'h':
		return CC_h;
	case 'i':
		return CC_i;
	case 'j':
		return CC_j;
	case 'k':
		return CC_k;
	case 'l':
		return CC_l;
	case 'm':
		return CC_m;
	case 'n':
		return CC_n;
	case 'o':
		return CC_o;
	case 'p':
		return CC_p;
	case 'q':
		return CC_q;
	case 'r':
		return CC_r;
	case 's':
		return CC_s;
	case 't':
		return CC_t;
	case 'u':
		return CC_u;
	case 'v':
		return CC_v;
	case 'w':
		return CC_w;
	case 'x':
		return CC_x;
	case 'y':
		return CC_y;
	case 'z':
		return CC_z;
	}
	return CC_BLOCK;
}

//------------------------------------------------------------------------------
//	InitD3D
//------------------------------------------------------------------------------
void 
InitD3D(void)
/*++

Routine Description:

	Initialize D3D

Arguments:

	None

Return Value:

	None

--*/
{
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow             = NULL;

    pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL,
					   D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pDev);
    pD3D->Release();
}

//------------------------------------------------------------------------------
//	InitVB
//------------------------------------------------------------------------------
void 
InitVB(void)
/*++

Routine Description:

	Setup the vertices for the texture

Arguments:

	None

Return Value:

	None

--*/
{
	for(int i=0; i<6; ++i)
	{
		g_verts[i].x		= g_originalVerts[i].x;
		g_verts[i].y		= g_originalVerts[i].y;
		g_verts[i].z		= g_originalVerts[i].z;
		g_verts[i].w		= g_originalVerts[i].w;
		g_verts[i].color	= g_originalVerts[i].color;
		g_verts[i].u		= g_originalVerts[i].u;
		g_verts[i].v		= g_originalVerts[i].v;
	}
    g_pDev->SetVertexShader(g_dwFVF);
}

//------------------------------------------------------------------------------
//	Paint
//------------------------------------------------------------------------------
void 
Paint(void)
/*++

Routine Description:

	Draw the scene

Arguments:

	None

Return Value:

	None

--*/
{
    static DWORD	Time = GetTickCount();
    DWORD			CurrentTime = GetTickCount();
    float			spin;
    DWORD			i;
	float			x;
	float			y;

	// 10 seconds per rotation
	spin = 2 * 3.14159f * (float)(CurrentTime - Time) / 10000.0f;

    for(i=0; i<6; ++i)
    {
        x = g_originalVerts[i].x - 320.0f;
        y = g_originalVerts[i].y - 240.0f;

        g_verts[i].x = x * (float)cos(spin) - y * (float)sin(spin);
        g_verts[i].y = x * (float)sin(spin) + y * (float)cos(spin);

        g_verts[i].x += 320.0f;
        g_verts[i].y += 240.0f;
    }

	g_pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
				  D3DCLEAR_STENCIL, 0x00000000, 1.0, 0);

    g_pDev->SetTextureStageState(0, D3DTSS_ADDRESSU,	D3DTADDRESS_CLAMP);
    g_pDev->SetTextureStageState(0, D3DTSS_ADDRESSV,	D3DTADDRESS_CLAMP);
    g_pDev->SetTextureStageState(0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
    g_pDev->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
    g_pDev->SetTextureStageState(0, D3DTSS_MINFILTER,	D3DTEXF_LINEAR);
    g_pDev->SetTextureStageState(0, D3DTSS_MAGFILTER,	D3DTEXF_LINEAR);
    g_pDev->SetTexture(0, g_pTex0);

    g_pDev->BeginScene();
	g_pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, g_verts, sizeof(TheVerts));
    g_pDev->EndScene();

    g_pDev->Present(NULL, NULL, NULL, NULL);
}

//------------------------------------------------------------------------------
//	ConvertToYUV
//------------------------------------------------------------------------------
void 
ConvertToYUV(
			 IN LPDIRECT3DSURFACE8	pFromSurface, 
			 OUT LPDIRECT3DSURFACE8	pToSurface
			 )
/*++

Routine Description:

	Converts a surface from an ARGB to a YUV surface.
	The dimensions of both surfaces must be the same.

Arguments:

	IN pFromSurface -	Source surface
	OUT pToSurface -	Destination surface

Return Value:

	None

--*/
{
    D3DSURFACE_DESC	descFrom;
    D3DSURFACE_DESC	descTo;
    D3DLOCKED_RECT	lockFrom;
    D3DLOCKED_RECT	lockTo;
    PVOID			pBitsFrom;
    PVOID			pBitsCopy;
    PBYTE			pTo;
    BYTE			R;
    BYTE			G;
    BYTE			B;
    BYTE			Y0;
    BYTE			Y1;
    BYTE			U;
    BYTE			V;
    DWORD*			pFrom;
    DWORD			y;
	DWORD			x;

    pFromSurface->GetDesc(&descFrom);

    pBitsFrom = LocalAlloc(0, descFrom.Width * descFrom.Height * 4);
    pBitsCopy = LocalAlloc(0, descFrom.Width * descFrom.Height * 4);

    pFromSurface->LockRect(&lockFrom, 0, D3DLOCK_READONLY);
    memcpy(pBitsCopy, lockFrom.pBits, descFrom.Width * descFrom.Height * 4);
    pFromSurface->UnlockRect();

    XGUnswizzleRect(pBitsCopy, descFrom.Width, descFrom.Height,
                    NULL, pBitsFrom, 0, NULL, 4);
    LocalFree(pBitsCopy);

    pToSurface->GetDesc(&descTo);
    pToSurface->LockRect(&lockTo, NULL, 0);

	pTo		= (PBYTE)lockTo.pBits;
    pFrom	= (DWORD*)pBitsFrom;
    for(y=0; y<descTo.Height; ++y)
    {
        for(x=0; x<descFrom.Width; x+=2)
        {
            R	= GetRValue(*pFrom);
            G	= GetGValue(*pFrom);
            B	= GetBValue(*pFrom);
            Y0	= (BYTE)(0.29*R + 0.59*G + 0.14*B);
            U	= (BYTE)(128.0 - 0.14*R - 0.29*G + 0.43*B);
            ++pFrom;

            R	= GetRValue(*pFrom);
            G	= GetGValue(*pFrom);
            B	= GetBValue(*pFrom);
            Y1	= (BYTE)(0.29*R + 0.57*G + 0.14*B);
            V	= (BYTE)(128.0 + 0.36*R - 0.29*G - 0.07*B);
			++pFrom;

            *(pTo++)	= Y0;
            *(pTo++)	= U;
            *(pTo++)	= Y1;
            *(pTo++)	= V;
        }
    }

	LocalFree(pBitsFrom);
    pToSurface->UnlockRect();
}

//------------------------------------------------------------------------------
//	LoadImage
//------------------------------------------------------------------------------
D3DTexture*
LoadImage(
		  IN char*	psz,
		  IN BOOL	convert
		  )
/*++

Routine Description:

	Loads a texture from a file into a surface

Arguments:

	IN psz -		Filename
	IN convert -	TRUE to convert to YUV, FALSE otherwise

Return Value:

	D3DTexture* -	The texture

--*/
{
    D3DTexture*			pTex;
    IDirect3DSurface8*	pSurT;
	IDirect3DSurface8*	pSur;

	if(convert)
	{
		D3DDevice_CreateTexture(256, 256, 1, 0, D3DFMT_YUY2, 0, &pTex);
		D3DTexture_GetSurfaceLevel(pTex, 0, &pSur);
		g_pDev->CreateImageSurface(256, 256, D3DFMT_A8R8G8B8, &pSurT);
		D3DXLoadSurfaceFromFile(pSurT, NULL, NULL, psz, NULL, 
								D3DX_FILTER_LINEAR, 0, NULL);
		ConvertToYUV(pSurT, pSur);
		pSurT->Release();
	}
	else
	{
	    D3DDevice_CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, 0, &pTex);
		D3DTexture_GetSurfaceLevel(pTex, 0, &pSur);
	    D3DXLoadSurfaceFromFile(pSur, NULL, NULL, psz, NULL, 
								D3DX_FILTER_LINEAR, 0, NULL);
	}
	pSur->Release();
    return pTex;
}

//------------------------------------------------------------------------------
//	InitOverlay
//------------------------------------------------------------------------------
void 
InitOverlay(void)
/*++

Routine Description:

	Loads all the images and creates the associated textures

Arguments:

	None

Return Value:

	None

--*/
{
    g_pTex0 = LoadImage("d:\\media\\mosquit1.bmp", FALSE);
    g_pTex1 = LoadImage("d:\\media\\mosquit1.bmp", TRUE);
    g_pTex2 = LoadImage("d:\\media\\mosquit2.bmp", TRUE);
	g_pTex3 = LoadImage("d:\\media\\mosquit3.bmp", TRUE);
}

//------------------------------------------------------------------------------
//	InitPosition
//------------------------------------------------------------------------------
void 
InitPosition(
			 IN int sWidth, 
			 IN int sHeight, 
			 IN int dWidth, 
			 IN int dHeight
			 ) 
/*++

Routine Description:

	Initializes the position of the moving box

Arguments:

	IN sWidth -		Source Width
	IN sHeight -	Source Height
	IN dWidth -		Destination width
	IN dHeight -	Destination height

Return Value:

	None

--*/
{
	g_sx1		= RAND_INT(sWidth - 2);
	g_sx2		= g_sx1 + (RAND_INT(sWidth - g_sx1));
	g_sy1		= RAND_INT(sHeight - 2);
	g_sy2		= g_sy1 + (RAND_INT(sHeight - g_sy1));
	g_dx1		= RAND_INT(dWidth - 2);
	g_dx2		= g_dx1 + (RAND_INT(dWidth - g_dx1));
	g_dy1		= RAND_INT(dHeight - 2);
	g_dy2		= g_dy1 + (RAND_INT(dHeight - g_dy1));
	g_swidth	= sWidth;
	g_sheight	= sHeight;
}

//------------------------------------------------------------------------------
//	UpdatePosition
//------------------------------------------------------------------------------
void 
UpdatePosition(void)
/*++

Routine Description:

	Updates the bouncing box's position

Arguments:

	None

Return Value:

	None

--*/
{
	Bounce(0, &g_sx1, &g_sx2, g_swidth,		&g_dsx1, &g_dsx2);
    Bounce(0, &g_sy1, &g_sy2, g_sheight,	&g_dsy1, &g_dsy2);
    Bounce(0, &g_dx1, &g_dx2, 640,			&g_ddx1, &g_ddx2);
    Bounce(0, &g_dy1, &g_dy2, 480,			&g_ddy1, &g_ddy2);
}

//------------------------------------------------------------------------------
//	UpdateOverlay
//------------------------------------------------------------------------------
void 
UpdateOverlay(
			  IN D3DTexture *pTex
			  )
/*++

Routine Description:

	Updates the overlay

Arguments:

	IN pTex -	Texture to use for the overlay

Return Value:

	None

--*/
{
    BOOL			status;
    D3DSURFACE_DESC	desc;
    D3DSurface*		pSur;

    D3DTexture_GetSurfaceLevel(pTex, 0, &pSur);

    pSur->GetDesc(&desc);

    // Initial value
	if(g_sx1 <= -3)
        InitPosition(desc.Width, desc.Height, 640,480);
    
	RECT rcSrc = {0, 0, 256, 256};//{g_sx1,g_sy1,g_sx2,g_sy2};
	RECT rcDst = {g_dx1,g_dy1,g_dx2,g_dy2};

    while(!g_pDev->GetOverlayUpdateStatus());

    g_pDev->UpdateOverlay(pSur, &rcSrc, &rcDst, TRUE, 0xff000000);

    pSur->Release();
}

//------------------------------------------------------------------------------
//	Pass1
//------------------------------------------------------------------------------
BOOL
Pass1(void)
/*++

Routine Description:

	Scrolling text

Arguments:

	None

Return Value:

	TRUE if the pass is complete, FALSE if not

--*/
{
	static BOOL		bNewPass	= TRUE;
	static BOOL		bCr			= FALSE;
	static DWORD	i			= 0;

	if(bNewPass)
	{
		// Clear any previous data
		SendCC(1, CC_ENM);
		SendCC(1, CC_EDM);

		// Start the pass
		SendCC(1, CC_TR);

		// Setup for 4 rows of scrolling text
		SendCC(1, CC_RU4);

		bNewPass	= FALSE;
		bCr			= FALSE;
		i			= 0;
	}

	else
	{
		// Still working?
		if(i < (sizeof(g_CC) / sizeof(g_CC[0])))
		{
			SendCC(g_CC[i].field, g_CC[i].cc0, g_CC[i].cc1);
			++i;
		}

		// Wait a while
		else if(i < (2 * (sizeof(g_CC) / sizeof(g_CC[0]))))
			++i;

		// Otherwise done
		else
		{
			// End the closed captioning
			SendCC(1, CC_EOC);
			bNewPass = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	Pass2
//------------------------------------------------------------------------------
BOOL
Pass2(void)
/*++

Routine Description:

	Randomly placed and formatted text

Arguments:

	None

Return Value:

	TRUE if the pass is complete, FALSE if not

--*/
{
	static BOOL	bNewPass	= TRUE;
	static BOOL	bCr			= FALSE;
	static BYTE	i			= 1;

	if(bNewPass)
	{
		// Clear any previous data
		SendCC(1, CC_ENM);
		SendCC(1, CC_EDM);

		// Start the pass
		SendCC(1, CC_TR);

		// Setup for 4 Direct Captioning
		SendCC(1, CC_RDC);

		bNewPass	= FALSE;
		bCr			= FALSE;
		i			= 1;
	}

	else
	{
		// Still working?
		if(i <= 15)
		{
			// Random style
			SetRow(1, 1, i, (CCStyle)(rand() % NUM_CCSTYLE));

			// Random chars
			SendCC(1, (rand() % (CC_BLOCK - CC_SPACE)) + CC_SPACE,
				   (rand() % (CC_BLOCK - CC_SPACE)) + CC_SPACE);
			++i;
		}

		// Otherwise done
		else
		{
			// End the closed captioning
			SendCC(1, CC_EOC);
			bNewPass = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
void __cdecl 
main(void)
{
    DWORD			LastTickShow	= 0;
	DWORD			LastTickMove	= 0;
	DWORD			i				= sizeof(g_CC);
    D3DTexture*		pTex			= NULL;
    DWORD			CurrentTick;
	int				temp;
	int				pass1			= 0;
	int				pass2			= 0;
	int				frame			= 0;

    _asm 
	{
		rdtsc
		mov [CurrentTick], eax
    }
    srand((CurrentTick));

    InitD3D();
    InitVB();
    InitOverlay();

    g_pDev->EnableOverlay(TRUE);
    D3DDevice_EnableCC(TRUE);

    while(TRUE)
    {
        Paint();

        CurrentTick = GetTickCount();

        if (CurrentTick - LastTickMove > 80)
        {
            UpdatePosition();
            LastTickMove = CurrentTick;
        }

        if (pTex == g_pTex1)
            pTex = g_pTex2;
        else if(pTex == g_pTex2)
            pTex = g_pTex3;
        else
            pTex = g_pTex1;

		UpdateOverlay(pTex);

		if(pass1 < 5)
		{
			if(Pass1())
				++pass1;
		}

		else if(pass2 < 10)
		{
			if(frame == 30) 
			{
				frame = 0;
				if(Pass2())
					++pass2;
			}
			else
				++frame;
		}

		else
		{
			pass1 = 0;
			pass2 = 0;
		}
	}
}

