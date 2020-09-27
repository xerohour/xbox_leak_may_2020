/*
	debug.h

	Change history (most recent first):

	Date		By		Comments
	----		--		--------

	02-13-2000	SF		Added new routines, tweaked some old routine names 
	02-09-2000	SF		Added definitions for the font-drawing code
	11-16-1999	SF		DEBUGGER & SYSDEBUGSTR abstracted

*/

#include <MacTypes.h>
#include <stdarg.h>

/* Examples of debugging macro usage:

	DBUG();
		Drops into MacsBug with a user break, and no message.
	DBUGSTR("This is a message\n");
		Drops into MacsBug and prints the literal C string.
	DBUGPRINTF(("Max count = %d, address = 0x%X\n", max, addr));
		Drops into MacsBug and prints the printf-style formatted C string,
		such as "Max count = 25, address = 0x259A0E".
		Note that the formatted string and numeric arguments have to be
		enclosed in an extra set of parentheses in order to hide the
		variable number of arguments from the macro preprocessor.
		Currently, the %c, %u, %d, %x, %X, %f, and %s data types are implemented.
	DBUGIF(x != 7);
		Drops into MacsBug with a user break (no message) if the
		condition is true.
	DBUGSTRIF(handle == nil, "We got a bad handle!\n");
		Drops into MacsBug if the condition is true and prints the literal C string.
	DBUGPRINTFIF(freenodes < 10, ("Node count = %d, header at 0x%x\n", freenodes, hdraddr));
		Drops into MacsBug if the condition is true and prints the printf-style
		formatted C string, such as "Node count = 7, header at 0x8B207C".
		Note that the formatted string and numeric arguments have to be
		enclosed in an extra set of parentheses in order to hide the
		variable number of arguments from the macro preprocessor.
		Currently, the %c, %u, %d, %x, %X, %f, and %s data types are implemented.
	DBUGASSERT(x >= 0);
		Drops into MacsBug if the asserted condition is false, printing a message
		that lists the failed assertion condition and the file name and line
		number of the ASSERT statement, such as:
		"ASSERT(x >= 0) failed: 'main.c' line 324"

	// These are the 'drawing directly to the screen' equivalents
	_RAWSTR("This is a message\n");
		Draws the literal C string directly to the screen.  It doesn't invoke MacsBug.
	_RAWPRINTF(("Max count = %d, address = 0x%X\n", max, addr));
		Draws the printf-style formatted C string directly to the screen,
		such as "Max count = 25, address = 0x259A0E".  It doesn't invoke MacsBug.
		Note that the formatted string and numeric arguments have to be
		enclosed in an extra set of parentheses in order to hide the
		variable number of arguments from the macro preprocessor.
		Currently, the %c, %u, %d, %x, %X, %f, and %s data types are implemented.
	_RAWSTRIF(handle == nil, "We got a bad handle (it was nil)!\n");
		If the condition is true, it draws the literal C string directly to the screen.
		It doesn't invoke MacsBug.
	_RAWPRINTFIF(freenodes < 10, ("Node count = %d, header at 0x%x\n", freenodes, hdraddr));
		If the condition is true, it draws the printf-style formatted C string directly
		to the screen, such as "Node count = 7, header at 0x8B207C".  It doesn't invoke
		MacsBug.  Note that the formatted string and numeric arguments have to be
		enclosed in an extra set of parentheses in order to hide the
		variable number of arguments from the macro preprocessor.
		Currently, the %c, %u, %d, %x, %X, %f, and %s data types are implemented.
	_RAWASSERT(x >= 0);
		If the asserted condition is false, it prints a message directly to the screen
		that lists the failed assertion condition and the file name and line
		number of the ASSERT statement, such as:
		"ASSERT(x >= 0) failed: 'main.c' line 324"  It doesn't invoke MacsBug.
*/


#if DEBUG_BUILD	// All the macros are enabled and generate code for debug builds

#define _RAWDRAW	0			// Set to 1 for raw screen printfs, 0 to suppress

#define DEBUGGER	Debugger	// Debugger for apps, SysDebug for drivers
#define DEBUGSTR	DebugStr	// DebugStr for regular apps, SysDebugStr for drivers

#define DBUG()						\
			DEBUGGER()
#define DBUGSTR(str)				\
			DEBUGSTR("\p" str)
#define DBUGPRINTF(parenargs)		\
			DEBUGSTR(dbugfmttopstr parenargs)
#define DBUGIF(cond)				\
			if (cond)				\
				DEBUGGER()
#define DBUGSTRIF(cond, str)		\
			if (cond)				\
				DEBUGSTR("\p" str)
#define DBUGPRINTFIF(cond, parenargs)	\
			if (cond)				\
				DEBUGSTR(dbugfmttopstr parenargs)

/* The following two macros are utilities to do argument-to-string conversion. */
#define _QUOTE(_x)					#_x
#define _MAKESTR(_x)				_QUOTE(_x)

/* The DBUGASSERT macro tells us what file and line the assertion failed at. */
#define DBUGASSERT(cond)			\
			if (!(cond))			\
				DEBUGSTR("\pASSERT(" #cond ") failed: '" __FILE__	\
							"' line " _MAKESTR(__LINE__))

#if _RAWDRAW

#define _RAWSTR(str)				\
			dbgdrawstr(str)
#define _RAWPRINTF(parenargs)		\
			dbgdrawstr(dbugfmttostr parenargs)
#define _RAWSTRIF(cond, str)		\
			if (cond)				\
				dbgdrawstr(str)
#define _RAWPRINTFIF(cond, parenargs)	\
			if (cond)				\
				dbgdrawstr(dbugfmttostr parenargs)
#define _RAWASSERT(cond)			\
			if (!(cond))			\
				dbgdrawstr("ASSERT(" #cond ") failed: '" __FILE__	\
							"' line " _MAKESTR(__LINE__) "\n")

#else

#define _RAWSTR(str)
#define _RAWPRINTF(parenargs)
#define _RAWSTRIF(cond, str)
#define _RAWPRINTFIF(cond, parenargs)
#define _RAWASSERT(cond)				

#endif

//=====================================================================================================
//
// Routines exported from debug.c (see that file for full descriptions)
//
//=====================================================================================================

extern char *dbugfmttostr(const char *fmt, ...);
extern UInt8 *dbugfmttopstr(const char *fmt, ...);
extern int dbugsprintf(char *dest, const char *fmt, ...);
extern int dbugvsprintf(char *dest, const char *fmt, va_list args);
extern void dbgdrawstr(char *str);

// The following are utility routines used by dbugsprintf() to format different types
//	of data values.  Most of the time you won't need to use them directly, but they're
//	listed here for completeness.  Consult the code in debug.c for sprintf() for
//	examples of how to use them.
extern int int32todecfmtstr(SInt32 s32val, UInt8 *dest, int fieldwidth, int flags);
extern int uint32tohexfmtstr(UInt32 u32val,  UInt8 *dest, int fieldwidth, int flags);
extern int float64todecfmtstr(Float64 f64val, UInt8 *dest, int fieldwidth,
								int precision, int flags);
extern int strtofmtstr(UInt8 *src, UInt8 *dest, int fieldwidth, int flags);

// Numeric & string conversion flags (used if you call the 'XtoYfmtstr' routines directly)
enum {
	DONTTERMINATE = 1,	// Don't null-terminate the string if this flag is set
	UNSIGNED_F = 2,		// Force an unsigned number conversion (other sign options are ignored)
	PLUSSIGN_F = 4,		// For signed numbers >= 0, force a '+' in the sign position
	SPACESIGN_F = 8,	// For signed numbers >= 0, force a space in the sign position
	LEFTALIGN_F = 16,	// Left-justify the result in the destination field (overrides zero fill)
	ZEROFILL_F = 32,	// Use leading zeros for padding to a field width
	LOWERCASE_F = 64	// Use lower case hex digits: a-f instead of A-F
};


#else	// All the macros are disabled and don't generate any code for release builds


#define DBUG()						
#define DBUGSTR(str)				
#define DBUGPRINTF(parenargs)		
#define DBUGIF(cond)				
#define DBUGSTRIF(cond, str)		
#define DBUGPRINTFIF(cond, parenargs)	
#define DBUGASSERT(cond)				
#define _RAWDRAW	0	// Don't do raw drawing if we're not doing a debug build
#define _RAWSTR(str)
#define _RAWPRINTF(parenargs)
#define _RAWSTRIF(cond, str)
#define _RAWPRINTFIF(cond, parenargs)
#define _RAWASSERT(cond)				

#endif
