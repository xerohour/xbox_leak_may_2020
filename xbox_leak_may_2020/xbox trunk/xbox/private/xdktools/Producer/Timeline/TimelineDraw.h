#ifndef __TIMELINEDRAW_H__
#define __TIMELINEDRAW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TimelineDraw.h
// Macros and contants for drawing routines inside the Timeline

#define BORDER_VERT_DRAWWIDTH	2
#define BORDER_HORIZ_DRAWWIDTH	2
#define BORDER_VERT_SPACEWIDTH	1
//#define BORDER_HORIZ_SPACEWIDTH	1
#define BORDER_VERT_WIDTH		(BORDER_VERT_DRAWWIDTH + BORDER_VERT_SPACEWIDTH)
//#define BORDER_HORIZ_WIDTH		(BORDER_HORIZ_DRAWWIDTH + BORDER_HORIZ_SPACEWIDTH)
#define BORDER_HORIZ_WIDTH		BORDER_HORIZ_DRAWWIDTH
#define BORDER_COLOR			RGB(0,0,0)
#define GUTTER_WIDTH			8
#define MIN_STRIP_HEIGHT		20
#define MIN_FNBAR_WIDTH			(LEFT_DRAW_OFFSET + 1)
#define MAX_FNBAR_WIDTH			1000
#define MAX_SCROLL				31000
#define SCROLL_NPAGE			1000
#define LEFT_DRAW_OFFSET		(BORDER_VERT_WIDTH + GUTTER_WIDTH)
#define MAX_MEASURE_PIXELS		2000
#define MIN_MEASURE_PIXELS		10

#define	COLOR_GUTTER_ACTIVE			RGB( 255, 255, 0 )
#define COLOR_GUTTER_ACTIVESELECTED	RGB( 255, 128, 0 )
#define COLOR_GUTTER_SELECTED		RGB( 255, 0, 0 )
#define COLOR_GUTTER_NORMAL			RGB( 190, 190, 190 )
#define COLOR_LINE_GREY				RGB( 190, 190, 190 )
#define COLOR_TIME_CURSOR			RGB(0, 255, 255)

#define	FUNCTION_NAME_HEIGHT		15
#define COLOR_FUNCTION_NAME			RGB( 225, 225, 255 )
#define COLOR_GREY_FUNCTION_NAME	RGB( 192, 192, 192 )
#define COLOR_HATCH_FUNCTION_NAME	RGB( 128, 128, 128 )

#define SHADING_DARK_COLOR			192

#define STRIP_NAME_HEIGHT			15

// stuff for music line drawing
#define MEASURE_LINE_PENSTYLE	PS_SOLID
#define MEASURE_LINE_WIDTH		2
#define MEASURE_LINE_COLOR		RGB(0,0,0)
#define BEAT_LINE_PENSTYLE		PS_SOLID
#define BEAT_LINE_WIDTH			1
#define BEAT_LINE_COLOR			RGB(51,51,153)
#define SUBBEAT_LINE_PENSTYLE	PS_SOLID
#define SUBBEAT_LINE_WIDTH		1
#define SUBBEAT_LINE_COLOR		RGB(204,204,255)

#define CURSOR_WIDTH			1

#ifndef NOTE_TO_CLOCKS
#define NOTE_TO_CLOCKS(note, ppq)	( (ppq) * 4 / (note) )
#endif

// stuff to identify the current mouse mode
typedef enum tagMOUSEMODE
{
	MM_NORMAL,
	MM_RESIZE,			// cursor is over a resize area
	MM_ACTIVERESIZE,	// mouse button is clicked and we are actively resizing
	MM_GUTTER,			// cursor is over a gutter
	MM_ACTIVEGUTTER,	// the gutter was clicked
	MM_RESIZE_FN,		// cursor is over a function bar resize area
	MM_ACTIVERESIZE_FN, // mouse button is clicked and we are resizing the function bar
	MM_MINMAX,			// cursor is over minimize/maximize button
	MM_ACTIVEMINMAX,	// mouse button is clicked and we will minimize/maximize on button up
	MM_UNKNOWN,			// cursor is most likely over a strip, and in an unkown shape
} MOUSEMODE;

#endif // __TIMELINEDRAW_H__
