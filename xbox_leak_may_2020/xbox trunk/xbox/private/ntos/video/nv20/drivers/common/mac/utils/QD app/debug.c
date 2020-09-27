/*
	debug.c

	Change history (most recent first):

	Date		By		Comments
	----		--		--------

	02-13-2000	SF		Reorganized the code, added more comments
	02-11-2000	SF		Added 'u', 's', 'f', field width, and precision support ('f' only)
	02-09-2000	SF		Added font-drawing tables and code for tough debugs
	12-08-1999	SF		Added 'c', 'X', and 'x' support, skip width specifier
	11-16-1999	SF		Basic macros and procedure names

*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <MacTypes.h>
#include <QuickDraw.h>
#include "debug.h"


#if DEBUG_BUILD		// Only generate code if we're doing a debug build

static char dbugstrbuf[512];	// Should be big enough for our usage


//=====================================================================================================
//
// dbugfmttostr()
//
//	This renders a printf-style argument list into a C string in a static buffer, and
//	returns a pointer to the beginning of the C string.  The arguments are the same as
//	what printf() uses.  See the description of dbugsprintf() for comments on the supported
//	print formatting options.
//
//=====================================================================================================

char *dbugfmttostr(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	dbugvsprintf(&dbugstrbuf[0], fmt, args);
	va_end(args);
	return(dbugstrbuf);		// Point to the Pascal string (length byte first)
}

//=====================================================================================================
//
// dbugfmttopstr()
//
//	This renders a printf-style argument list into a Pascal string in a static buffer, and
//	returns a pointer to the beginning of the Pascal string.  The arguments are the same as
//	what printf() uses.  See the description of dbugsprintf() for comments on the supported
//	print formatting options.
//
//=====================================================================================================

UInt8 *dbugfmttopstr(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	dbugstrbuf[0] = dbugvsprintf(&dbugstrbuf[1], fmt, args);	// Set the Pascal length byte
	va_end(args);
	return((UInt8 *)dbugstrbuf);	// Point to the Pascal string (length byte first)
}

//=====================================================================================================
//
// dbugsprintf()
//
//	It's equivalent to sprintf() -- it operates on C strings, recognizes many of the same
//	formatting options as sprintf, and returns the number of characters written to the
//	destination buffer.  It supports the following format types: c, u, d, x, X, f, and s,
//	with optional flags field width, and precision support ('f' only)
//	The 'precision' specifier is only implemented for floating point numbers, and '%f'
//	is the only floating point format implemented.  The field width specifier is fully
//	implemented for the options where it makes sense.  (See printf() documentation.)
//
//=====================================================================================================

int dbugsprintf(char *dest, const char *fmt, ...)
{
	int n;
	va_list args;

	va_start(args, fmt);
	n = dbugvsprintf(dest, fmt, args);
	va_end(args);
	return(n);		// Return the number of characters transferred
}

//=====================================================================================================
//
// dbugvsprintf()
//
//	This does the guts of dbugsprintf(), and takes a var-arg list instead of '...'
//
//=====================================================================================================

int dbugvsprintf(char *dest, const char *fmt, va_list args)
{
	int ch, fieldwidth, precision, flags;
	int usedefaultprecision;
	SInt32 s32val;
	UInt32 u32val;
	Float64 f64val;
	UInt8 *f, *d, *specptr;
	UInt8 *strpval;

	if (dest == nil)
		return(0);	// If we don't have a destination, we didn't do any characters

	f = (UInt8 *)fmt;
	d = (UInt8 *)dest;

	while ((ch = *f++) != 0) {
		if (ch != '%') {
			*d++ = ch;
			continue;
		}
		specptr = f - 1;	// Save a pointer to the '%' specifier, in case of syntax errors
		ch = *f++;

		flags = DONTTERMINATE;	// Don't terminate substrings -- we'll null-terminate when we're all done
		// Check for left-alignment
		if (ch == '-') {
			flags |= LEFTALIGN_F;
			ch = *f++;
		}
		// Check for using a plus sign for non-negative numbers
		if (ch == '+') {
			flags |= PLUSSIGN_F;
			ch = *f++;
		}
		// Check for using a space character (sign place-holder) for non-negative numbers
		if (ch == ' ') {
			flags |= SPACESIGN_F;
			ch = *f++;
		}
		// Check for leading zero fill
		if (ch == '0') {
			flags |= ZEROFILL_F;
			// Don't bump the character pointer in case '0' was the only digit
		}
		// Collect the field width specifier
		fieldwidth = 0;	// Default field width
		while (ch >= '0' && ch <= '9') {
			fieldwidth = fieldwidth * 10 + ch - '0';
			ch = *f++;
		}
		usedefaultprecision = true;	// In case a precision wasn't specified
		// Check for a precision specifier
		if (ch == '.') {	// We have a precision specifier, skip the '.'
			ch = *f++;
			precision = 0;	// Start with 0
			while (ch >= '0' && ch <= '9') {
				precision = precision * 10 + ch - '0';
				ch = *f++;
			}
			usedefaultprecision = false;
		}

		// Perform the conversion operation
		switch (ch) {
		case 'c':	// Copy an ASCII character
			u32val = va_arg(args, int);
			*d++ = u32val;
			break;
		case 'u':	// Copy a formatted, unsigned decimal number
			flags |= UNSIGNED_F;
		case 'd':	// Copy a formatted, signed decimal number
			s32val = va_arg(args, int);
			// Format the number, increment the dest pointer by the characters copied
			d += int32todecfmtstr(s32val, d, fieldwidth, flags);
			break;
		case 'x':	// Copy a formatted, lower-case hexadecimal number
			flags |= LOWERCASE_F;
		case 'X':	// Copy a formatted, upper-case hexadecimal number
			u32val = va_arg(args, int);
			// Format the number, increment the dest pointer by the characters copied
			d += uint32tohexfmtstr(u32val, d, fieldwidth, flags);
			break;
		case 'f':	// Copy a formatted floating point number
			f64val = va_arg(args, Float64);
			if (usedefaultprecision)
				precision = 6;
			// Format the number, increment the dest pointer by the characters copied
			d += float64todecfmtstr(f64val, d, fieldwidth, precision, flags);
			break;
		case 's':	// Copy a formatted string
			strpval = va_arg(args, UInt8 *);
			d += strtofmtstr(strpval, d, fieldwidth, flags);
			break;
		case '%':	// Copy a formatted '%' sign
			d += strtofmtstr((UInt8 *)"%", d, fieldwidth, flags);
			break;
		case 0:		// Gracefully handle premature end-of-string
			f--;	// Back up, now f points to the null character again
		default:	// Unexpected conversion operator, so just echo to the destination
			while (specptr < f)
				*d++ = *specptr++;
			if (ch == 0)
				goto stringdone;
			break;
		}
	}

stringdone:
	*d = 0;	// Null-terminate the string
	return(d - (UInt8 *)dest);	// Return the number of characters we transferred
}

enum {	// Padding option definitions
	PRESPACE_O = 1,
	PREZERO_O = 2,
	POSTSPACE_O = 4
};


#define NUMBUFSIZE	16	// Should be enough for 32-bit integers in decimal or hex

//=====================================================================================================
//
// int32todecfmtstr()
//
//	This takes a 32-bit integer value and converts it to a formatted decimal string,
//	using options (field width and flags) like those provided by sprintf().  The 32-bit
//	number is assumed to be signed unless the UNSIGNED_F flag is set.  Look at the code
//	for dbugsprintf() above to see which formatting options are implemented.
//
//=====================================================================================================

int int32todecfmtstr(SInt32 s32val, UInt8 *dest, int fieldwidth, int flags)
{
	int i, digitcount, destcount;
	int sign, signchar;
	int fillcount;
	int pad_options;
	UInt32 u32val, quotient, remainder;
	UInt8 *intdigp;
	UInt8 nbuf[NUMBUFSIZE];

	// Process the sign-related options
	if (flags & UNSIGNED_F) {	// Unsigned conversion
		sign = 0;	// No sign character
	} else {	// We're doing a signed conversion
		sign = 1;	// Assume we'll have a sign character
		if (s32val < 0) {
			signchar = '-';
			s32val = -s32val;	// Make the number positive now so we can 'digitize' it
		} else {	// s32val >= 0
			if (flags & PLUSSIGN_F)
				signchar = '+';
			else if (flags & SPACESIGN_F)
				signchar = ' ';
			else
				sign = 0;	// No sign character
		}
	}
	u32val = s32val;	// Do unsigned math from here on out

	// Convert the number into ASCII decimal digits in our local buffer, counting them
	intdigp = &nbuf[NUMBUFSIZE];	// Point past the last character in the buffer
	digitcount = 0;	// Nothing written to our local buffer yet
	do {
		quotient = u32val / 10;
		remainder = u32val - quotient * 10;
		*--intdigp = remainder + '0';	// Put the digit into the next lower buffer slot
		digitcount++;
		u32val = quotient;
	} while (u32val > 0);

	// Process the field-padding options
	pad_options = 0;	// Assume we won't be doing any padding
	fillcount = fieldwidth - (sign + digitcount);	// Account for the sign, if used
	if (fillcount > 0) {	// We need to do left or right padding
		if (flags & LEFTALIGN_F) {
			pad_options = POSTSPACE_O;
		} else {	// Right-aligned, fill with zeros or spaces
			if (flags & ZEROFILL_F)
				pad_options = PREZERO_O;
			else
				pad_options = PRESPACE_O;
		}
	}

	destcount = 0;	// Nothing written out to the destination yet

	// Copy any leading spaces
	if (pad_options & PRESPACE_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = ' ';
		destcount += fillcount;
	}
	// Copy the sign character, if any
	if (sign) {
		*dest++ = signchar;
		destcount++;
	}
	// Copy any leading zeros
	if (pad_options & PREZERO_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = '0';
		destcount += fillcount;
	}
	// Copy the decimal digits from our local buffer
	for (i = 0; i < digitcount; i++)
		*dest++ = *intdigp++;
	destcount += digitcount;

	// Copy any trailing spaces
	if (pad_options & POSTSPACE_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = ' ';
		destcount += fillcount;
	}
	if ((flags & DONTTERMINATE) == 0)	// Null-terminate the string unless requested not to
		*dest = 0;
	return(destcount);	// Return the character count, not including the null
}

//=====================================================================================================
//
// uint32tohexfmtstr()
//
//	This takes a 32-bit unsigned integer value and converts it to a formatted hexadecimal
//	string, using options (field width and flags) like those provided by sprintf().  Look
//	at the code for dbugsprintf() above to see which formatting options are implemented.
//
//=====================================================================================================

int uint32tohexfmtstr(UInt32 u32val,  UInt8 *dest, int fieldwidth, int flags)
{
	int i, digitcount, destcount;
	int c, hexadjust;
	int fillcount;
	UInt8 fillchar;
	int pad_options;
	UInt32 quotient;
	UInt8 *intdigp;
	UInt8 nbuf[NUMBUFSIZE];

	hexadjust = 'A' - '9' - 1;
	if (flags & LOWERCASE_F)
		hexadjust += 'a' - 'A';

	// Convert the number into ASCII hex digits in our local buffer, counting them
	intdigp = &nbuf[NUMBUFSIZE];	// Point past the last character in the buffer
	digitcount = 0;	// Nothing written to our local buffer yet
	do {
		quotient = u32val / 16;
		c = u32val % 16 + '0';
		if (c > '9')	/* A-F */
			c += hexadjust;
		*--intdigp = c;	// Put the digit into the next lower buffer slot
		digitcount++;
		u32val /= 16;
	} while (u32val > 0);

	// Process the field-padding options
	pad_options = 0;	// Assume we won't be doing any padding
	fillcount = fieldwidth - digitcount;	// No sign to worry about
	if (fillcount > 0) {	// We need to do left or right padding
		fillchar = ' ';		// Most common fill character is the space
		if (flags & LEFTALIGN_F) {
			pad_options = POSTSPACE_O;
		} else {	// Right-aligned, fill with zeros or spaces
			if (flags & ZEROFILL_F) {
				pad_options = PREZERO_O;
				fillchar = '0';
			} else {
				pad_options = PRESPACE_O;
			}
		}
	}

	destcount = 0;	// Nothing written out to the destination yet

	// Copy any leading zeros or spaces
	if (pad_options & (PREZERO_O | PRESPACE_O)) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = fillchar;
		destcount += fillcount;
	}
	// Copy the hex digits from our local buffer
	for (i = 0; i < digitcount; i++)
		*dest++ = *intdigp++;
	destcount += digitcount;

	// Copy any trailing spaces
	if (pad_options & POSTSPACE_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = fillchar;
		destcount += fillcount;
	}
	if ((flags & DONTTERMINATE) == 0)	// Null-terminate the string unless requested not to
		*dest = 0;
	return(destcount);	// Return the character count, not including the null
}

//=====================================================================================================
//
// float64todecfmtstr()
//
//	This takes a 64-bit floating-point value and converts it to a formatted decimal
//	string, using options (field width, precision, and flags) like those provided by
//	sprintf().  Look at the code for dbugsprintf() above to see which formatting options
//	are implemented.
//
//=====================================================================================================

int float64todecfmtstr(Float64 f64val, UInt8 *dest, int fieldwidth,
								int precision, int flags)
{
	int i, firstcount, destcount;
	int sign, signchar, decpt;
	int fillcount;
	int pad_options;
	int reducecount, loopdigits, digitsleft;
	UInt32 u32val, quotient, remainder;
	Float64 f64mant9, f64mant9factor, fone, ften, fbillion, powerof10;
	UInt8 *digp;
	UInt8 nbuf[NUMBUFSIZE];	// This only needs to hold the first 9 digits of the integer part

	// Process the sign-related options
	sign = 1;	// Assume at first we'll have a sign character
	if (f64val < 0.0) {
		signchar = '-';
		f64val = -f64val;	// Make the number positive now so we can 'digitize' it
	} else {	// f64val >= 0.0
		if (flags & PLUSSIGN_F)
			signchar = '+';
		else if (flags & SPACESIGN_F)
			signchar = ' ';
		else
			sign = 0;	// No sign character
	}

	// Round the number to N decimal places.  We add 0.5 x 10^(-N), which is
	//	equivalent to adding 1 / (2*10^N).  We'll use this latter formula.
	fone = 1.0;		// Keep the compiler from always loading these constants from memory
	ften = 10.0;
	powerof10 = fone;	// 10 ^ 0
	for (i = 0; i < precision; i++)
		powerof10 *= ften;	// Build 10 ^ N
	f64val += fone / (2.0 * powerof10);
	// f64val now contains the properly rounded number

	f64mant9 = f64val;	// Start hunting for the mantissa's 9 uppermost decimal digits
	fbillion = 1e9;		// Keep it in a register
	f64mant9factor = fone;
	// Reduce the mantissa to less than 1 billion, so it will fit in a 32-bit integer
	for (reducecount = 0; f64mant9 >= fbillion; reducecount++) {
		f64mant9 /= fbillion;
		f64mant9factor *= fbillion;
	}

	// Process the highest 32-bits of the mantissa so we can count those digits first

	f64mant9 = f64val / f64mant9factor;	// Grab highest 9 integer decimal digits
	u32val = f64mant9;	// Drop any fraction
	f64mant9 = u32val;	// Now we have a float with only an integer part
	f64val -= f64mant9 * f64mant9factor;	// Subtract out the previous high digits
	f64mant9factor /= fbillion;		// Adjust our division factor

	// Convert the binary into ASCII decimal digits in our local buffer, counting them
	digp = &nbuf[NUMBUFSIZE];	// Point past the last char. of these 9 digits
	firstcount = 0;	// No digits of the first 32-bit integer part yet
	do {
		quotient = u32val / 10;
		remainder = u32val - quotient * 10;
		*--digp = remainder + '0';	// Put the digit into the next lower buffer slot
		firstcount++;
		u32val = quotient;
	} while (u32val > 0);

	// Figure out whether we'll have a decimal point
	decpt = (precision > 0);	// Don't use a decimal point if no fractional part

	// Process the field-padding options
	pad_options = 0;	// Assume we won't be doing any padding
	// We have the information we need to calculate how many output characters we'll have
	fillcount = fieldwidth - (sign + firstcount + (reducecount * 9) + decpt + precision);
	if (fillcount > 0) {	// We need to do left or right padding
		if (flags & LEFTALIGN_F) {
			pad_options = POSTSPACE_O;
		} else {	// Right-aligned, fill with zeros or spaces
			if (flags & ZEROFILL_F)
				pad_options = PREZERO_O;
			else
				pad_options = PRESPACE_O;
		}
	}

	destcount = 0;	// Nothing written out to the destination yet

	// Copy any leading spaces
	if (pad_options & PRESPACE_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = ' ';
		destcount += fillcount;
	}
	// Copy the sign character, if any
	if (sign) {
		*dest++ = signchar;
		destcount++;
	}
	// Copy any leading zeros
	if (pad_options & PREZERO_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = '0';
		destcount += fillcount;
	}
	// Copy the highest chunk of integer digits from the local buffer
	for (i = 0; i < firstcount; i++)
		*dest++ = *digp++;
	destcount += firstcount;

	// Now we need to convert the remaining integer digits, if any
	for (i = 0; i < reducecount; i++) {
		f64mant9 = f64val / f64mant9factor;	// Grab 9 more decimal digits
		u32val = f64mant9;		// Drop any fraction
		f64mant9 = u32val;		// Now we have a float with only an integer part
		f64val -= f64mant9 * f64mant9factor;	// Subtract out the previous high digits
		f64mant9factor /= fbillion;			// Adjust our division factor
		// Convert the integer part into ASCII decimal digits, directly to the destination
		dest += 9;				// Point past the last char. of this 9-digit chunk
		digp = dest;
		for (loopdigits = 0; loopdigits < 9; loopdigits++) {
			quotient = u32val / 10;
			remainder = u32val - quotient * 10;
			*--digp = remainder + '0';	// Put the digit into the next lower buffer slot
			u32val = quotient;
		}
		destcount += 9;
	}
	// f64val has only the fractional part now

	if (!decpt)
		goto checktrailing;	// Skip the laborious fraction-processing part

	// Copy the decimal point
	*dest++ = '.';
	destcount++;

	// Similar to how we handled the integer part processing, we'll process up to
	//	9 digits at a time, by multiplying the fraction by a power of 10,
	//	converting to an integer, and converting digits to the destination.

	digitsleft = precision;
	do {
		loopdigits = digitsleft;
		if (loopdigits > 9)
			loopdigits = 9;
		powerof10 = fone;	// 10 ^ 0
		for (i = 0; i < loopdigits; i++)
			powerof10 *= ften;	// Build 10 ^ N
		f64val *= powerof10;	// Push some fractional digits into the integer part
		u32val = f64val;	// Conversion truncates any remaining fraction
		f64val -= u32val;	// Remove the integer part, leave remaining fraction digits
		digp = dest + loopdigits;	// Point past the last char. of this chunk
		for (i = 0; i < loopdigits; i++) {
			quotient = u32val / 10;
			remainder = u32val - quotient * 10;
			*--digp = remainder + '0';	// Put the digit into the next lower buffer slot
			u32val = quotient;
		}
		dest += loopdigits;
		destcount += loopdigits;
		digitsleft -= loopdigits;
	} while (digitsleft > 0);

checktrailing:
	// Copy any trailing spaces
	if (pad_options & POSTSPACE_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*dest++ = ' ';
		destcount += fillcount;
	}
	if ((flags & DONTTERMINATE) == 0)	// Null-terminate the string unless requested not to
		*dest = 0;
	return(destcount);	// Return the character count, not including the null
}

//=====================================================================================================
//
// strtofmtstr()
//
//	This takes a source C string and converts it to a formatted output C string,
//	using options (field width and flags) like those provided by sprintf().  Look at
//	the code for dbugsprintf() above to see which formatting options are implemented.
//
//=====================================================================================================

int strtofmtstr(UInt8 *src, UInt8 *dest, int fieldwidth, int flags)
{
	int i, srclen;
	int fillcount;
	UInt8 fillchar;
	int pad_options;
	UInt8 *s, *d;

	// For padding calculations, we need to know the source string length
	for (s = src, srclen = 0; *s != 0; s++)
		srclen++;
	// Process the field-padding options
	pad_options = 0;	// Assume we won't be doing any padding
	fillcount = fieldwidth - srclen;

	if (fillcount > 0) {	// We need to do left or right padding
		fillchar = ' ';		// Most common fill character is the space
		if (flags & LEFTALIGN_F) {
			pad_options = POSTSPACE_O;
		} else {	// Right-aligned, fill with zeros or spaces
			if (flags & ZEROFILL_F) {
				pad_options = PREZERO_O;
				fillchar = '0';
			} else {
				pad_options = PRESPACE_O;
			}
		}
	}

	s = src;
	d = dest;

	// Copy any leading zeros or spaces
	if (pad_options & (PREZERO_O | PRESPACE_O)) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*d++ = fillchar;
	}
	// Copy the characters from the source string
	for (i = 0; i < srclen; i++)
		*d++ = *s++;

	// Copy any trailing spaces
	if (pad_options & POSTSPACE_O) {
		for (i = 0; i < fillcount; i++)	// Copy the pad character(s)
			*d++ = fillchar;
	}

	if ((flags & DONTTERMINATE) == 0)	// Null-terminate the string unless requested not to
		*d = 0;
	return(d - dest);	// Return the character count, not including the null
}


#if _RAWDRAW

typedef struct {
	UInt8	ascent;
	UInt8	height;
	UInt16	rowbytes;
	UInt8	charwidth[256];
	UInt16	startoffset[256];
} fontdef, *fontdefptr;

static fontdef dbgfontdef = {   // Monaco 9
    // Ascent, height, rowbytes
     9, 11, 192,
    {   // Character width data
        0,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6
    },
    {   // Character starting offset data
           0,      0,      6,     12,     18,     24,     30,     36,
          42,     48,     54,     60,     66,     72,     72,     78,
          84,     90,     96,    102,    108,    114,    120,    126,
         132,    138,    144,    150,    156,    162,    168,    174,
         180,    186,    192,    198,    204,    210,    216,    222,
         228,    234,    240,    246,    252,    258,    264,    270,
         276,    282,    288,    294,    300,    306,    312,    318,
         324,    330,    336,    342,    348,    354,    360,    366,
         372,    378,    384,    390,    396,    402,    408,    414,
         420,    426,    432,    438,    444,    450,    456,    462,
         468,    474,    480,    486,    492,    498,    504,    510,
         516,    522,    528,    534,    540,    546,    552,    558,
         564,    570,    576,    582,    588,    594,    600,    606,
         612,    618,    624,    630,    636,    642,    648,    654,
         660,    666,    672,    678,    684,    690,    696,    702,
         708,    714,    720,    726,    732,    738,    744,    750,
         750,    756,    762,    768,    774,    780,    786,    792,
         798,    804,    810,    816,    822,    828,    834,    840,
         846,    852,    858,    864,    870,    876,    882,    888,
         894,    900,    906,    912,    918,    924,    930,    936,
         942,    948,    954,    960,    966,    972,    978,    984,
         990,    996,   1002,   1008,   1014,   1020,   1026,   1032,
        1038,   1044,   1050,   1056,   1062,   1068,   1074,   1080,
        1086,   1092,   1098,   1104,   1110,   1116,   1122,   1128,
        1134,   1140,   1146,   1152,   1158,   1164,   1170,   1176,
        1182,   1188,   1194,   1200,   1206,   1212,   1218,   1224,
        1230,   1236,   1242,   1248,   1254,   1260,   1266,   1272,
        1278,   1284,   1290,   1296,   1302,   1308,   1314,   1320,
        1326,   1332,   1338,   1344,   1350,   1356,   1362,   1368,
        1374,   1380,   1386,   1392,   1398,   1404,   1410,   1416,
        1422,   1428,   1434,   1440,   1446,   1452,   1458,   1464,
        1470,   1476,   1482,   1488,   1494,   1500,   1506,   1512
    }
};

static UInt32 dbgfontdata[] = { // Monaco 9
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00001000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00014200,
    0x11A51400, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00C00000, 0x00000000,
    0x00000000, 0x00000000, 0x00060000, 0x00000000,
    0x00600000, 0x000041A6, 0x80000000, 0x00000000,
    0x05000000, 0x00000000, 0x00020411, 0x44082142,
    0x04200404, 0x21000000, 0x000C0002, 0x80880000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00020000, 0x81080000,
    0x00001000, 0x00000000, 0x00000000, 0x00000700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x000000C8, 0x0C100400, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000042, 0x08000500,
    0x22C00010, 0x82006880, 0x04408004, 0x40801A11,
    0x02006844, 0x08000300, 0x31201F30, 0x00000800,
    0x00000000, 0x00000000, 0x00080000, 0x00000000,
    0x00800000, 0x000022CB, 0x00000000, 0x00000000,
    0x00020000, 0x00000000, 0x00050A20, 0x02105001,
    0x08500208, 0x50800869, 0xE44C2002, 0x80500000,
    0x79E79E79, 0xE79E01E7, 0x9E79E79E, 0x79E79E79,
    0xE79E79E7, 0x9E79E008, 0x51471260, 0x82042000,
    0x00002708, 0x71C13E73, 0xE71C0000, 0x8041C89C,
    0xF1CF3EF9, 0xC8880A28, 0x2289CF1C, 0xF1CFA28A,
    0x28A2F884, 0x04280200, 0x80008018, 0x08082202,
    0x00000000, 0x00020000, 0x00000082, 0x0469C21C,
    0xFA272220, 0x4514B140, 0x08214508, 0x21452C20,
    0x8514B082, 0x14508480, 0x41003A4B, 0xCF3D1140,
    0x1E740204, 0x222008FB, 0xF00838C7, 0x00008100,
    0x00800000, 0x000071C7, 0x1E00000A, 0x78430001,
    0x48823800, 0x0C388000, 0x01071F73, 0xEF800080,
    0x1C702722, 0x022014B0, 0x03805005, 0x00200000,
    0x49249249, 0x24920124, 0x92492492, 0x49249249,
    0x24924924, 0x92492008, 0x53EAAA90, 0x8204A880,
    0x00002898, 0x8A232080, 0x28A23181, 0x002228A2,
    0x8A28A082, 0x28880A48, 0x36CA28A2, 0x8A22228A,
    0x25220884, 0x04440100, 0x80008020, 0x08000202,
    0x00000000, 0x00020000, 0x00000082, 0x04B22722,
    0x8328A200, 0x00000080, 0x00000000, 0x00000000,
    0x00000000, 0x0001C488, 0x40873A88, 0x20972001,
    0x28880208, 0x12200449, 0x20084928, 0x80040000,
    0x18808800, 0x00008A28, 0xA8000014, 0x28810820,
    0x08844400, 0x10488000, 0x02A8908A, 0x08082082,
    0x228848A2, 0x8A202200, 0x00002000, 0x00000000,
    0x49249249, 0x24920124, 0x92492492, 0x49249249,
    0x24924924, 0x92492008, 0x514A14A0, 0x04027080,
    0x00004988, 0x08253CF0, 0x28A23182, 0x3E102AA2,
    0x8A08A082, 0x08880A88, 0x2AAA28A2, 0x8A02228A,
    0x22221082, 0x0400001E, 0xF1C79C71, 0xEF082242,
    0x3CB1CF1E, 0xB1E7A28A, 0xA8A2F882, 0x040228A0,
    0x82A8A279, 0xE79E79E7, 0x1C71C708, 0x20822C71,
    0xC71C7228, 0xA288831C, 0xE14FBA93, 0x2697000F,
    0xA89B6F90, 0x09490621, 0x2F885928, 0x9C788100,
    0x11C7084A, 0x40008A28, 0xA870001E, 0x50C20052,
    0x2884F082, 0x3EE9C300, 0x0148908A, 0x08082082,
    0x2289B8A2, 0x8A220000, 0x00000000, 0x00000000,
    0x49249249, 0x24920124, 0x92492492, 0x49249249,
    0x24924924, 0x92492008, 0x03E70840, 0x0402ABE0,
    0x3E004A88, 0x10C90288, 0x47220004, 0x00084EBE,
    0xF208BCF2, 0x6F880B08, 0x229A2F22, 0xF1C22252,
    0x22142082, 0x04000022, 0x8A28A222, 0x28882282,
    0x2ACA28A2, 0xCA02228A, 0xA5221102, 0x0203E8A0,
    0xF268A28A, 0x28A28A28, 0xA28A2888, 0x2082328A,
    0x28A28A28, 0xA288802A, 0x412F9E92, 0xA8800002,
    0x3CAAA208, 0x13E90A11, 0x250828C8, 0xAA99013E,
    0x20889491, 0x2000FBE8, 0xACA9CFC0, 0x00003E8A,
    0x25084101, 0x12488300, 0x008F9EFB, 0xCF082082,
    0x228BE8A2, 0x8A220000, 0x00000000, 0x00000000,
    0x49249249, 0x24920124, 0x92492492, 0x49249249,
    0x24924924, 0x92492008, 0x014294A8, 0x04022080,
    0x00008C88, 0x202F8288, 0x889E0002, 0x3E108B22,
    0x8A08A082, 0x28888A88, 0x228A2822, 0x88222252,
    0xA2084081, 0x04000022, 0x8A08BE22, 0x28882382,
    0x2A8A28A2, 0x81C22252, 0xA2222082, 0x04022FA0,
    0x8228A28A, 0x28A28A28, 0x3EFBEF88, 0x2082228A,
    0x28A28A28, 0xA2888028, 0x40AF8A8B, 0x2880000F,
    0xA8CB6204, 0x20891221, 0x25080008, 0xAEAA0102,
    0xA0871491, 0x20008A28, 0xA8B80000, 0x00000052,
    0x2210F101, 0x12488000, 0x0128908A, 0x08082082,
    0x228BE8A2, 0x8A220000, 0x00000000, 0x00000000,
    0x49249249, 0x24920124, 0x92492492, 0x49249249,
    0x24924924, 0x92492000, 0x000AAA90, 0x02040086,
    0x00308888, 0x42212288, 0x88823181, 0x00200822,
    0x8A28A082, 0x28888A48, 0x228A2822, 0x8A222223,
    0x65088081, 0x04000026, 0x8A08A022, 0x28882242,
    0x2A8A28A2, 0x80222652, 0xA5224082, 0x040228A2,
    0x8228A29A, 0x69A69A68, 0x20820808, 0x2082228A,
    0x28A28A69, 0xA698802A, 0x48470A8A, 0xA6800004,
    0x28880000, 0x03E91249, 0x250879E5, 0x28CA2102,
    0x4088224A, 0x40008A28, 0xA8A00000, 0x00000822,
    0x22104482, 0x1248800C, 0x7AD8908A, 0x08082082,
    0x228BF8A2, 0x8A220000, 0x00000000, 0x00000000,
    0x79E79E79, 0xE79E01E7, 0x9E79E79E, 0x79E79E79,
    0xE79E79E7, 0x9E79E008, 0x00072468, 0x02040006,
    0x00310708, 0xF9C11C70, 0x871C3180, 0x804088A2,
    0xF1CF3E81, 0xC888722F, 0xA289C81C, 0x89C21C22,
    0x2888F880, 0x8403F01A, 0xF1E79E21, 0xE8882223,
    0x2A89CF1E, 0x83C19A21, 0x489EF882, 0x0402289C,
    0xFA271C69, 0xA69A69A7, 0x9E79E788, 0x20822271,
    0xC71C71A6, 0x9A68001C, 0xB0200BB0, 0x20800000,
    0x2F700F9E, 0x788E8CF9, 0x2508000D, 0x9EF1C100,
    0x40803E00, 0x0A808A27, 0x1E580000, 0x00000001,
    0xE2203800, 0x1249C00C, 0x29289F8B, 0xEF882082,
    0x1C71E71C, 0x71C20000, 0x00000080, 0x04000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00020000, 0x01080002,
    0x00010000, 0x00000000, 0x00000080, 0x00000700,
    0x00000000, 0x00000000, 0x00000002, 0x00000000,
    0x000000C0, 0x8C000000, 0x00000000, 0x20002000,
    0x00000802, 0x00000000, 0x00020042, 0x08000008,
    0x00000000, 0x00000002, 0x00000000, 0x00000000,
    0x00000000, 0x00000008, 0x01200083, 0xCF000000,
    0x00000000, 0x00880000, 0x00080000, 0x01000000,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x20200000, 0x00008004, 0x50000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000040, 0x08000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000004,
    0x00000000, 0x00000000, 0x00000100, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0xC000C000,
    0x00000802, 0x00000000, 0x001C0002, 0x00000010,
    0x00000000, 0x00000004, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00C00000, 0x00000000,
    0x00000000, 0x00080000, 0x00300000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0xC0000000, 0x00008008, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000080, 0x04000000
};

//=====================================================================================================
//
// dbgdrawstr()
//
//	Note: This routine assumes the maximum font character width is 32 pixels
//
//=====================================================================================================

void dbgdrawstr(char *str)	// Takes a C string
{
	static UInt32 gLineCount = 0;	// Counts how many lines we've printed
	static GDHandle gDrawingGDevH;
	static SInt32 gDrawX;	// Screen coords of top-left pixel of dest char box
	static SInt32 gDrawY;
	static SInt32 gStartX;	// Starting H coord. on the screen after horizontal wrap
	static SInt32 gStartY;	// Starting V coord. on the screen after vertical wrap
	static UInt32 gLastPageRight = 0;	// Max. right edge of the previous 'page'
	static UInt32 gCurPageRight = 0;	// Max. right edge of the current 'page'
	static UInt8 gDidEndOfLine = false;
	static UInt8 sbuf[512];
	Ptr destbase;
	UInt32 destrowbytes, destpixelshift;
	UInt32 clipheight, clipwidth;
	SInt32 destX, destY;
	SInt32 startX, startY;
	UInt32 i, j, ch;
	SInt32 clearwidth;
	UInt32 charheight, charwidth;
	UInt32 bitoffset, longindex, fontrowbytes;
	UInt32 shift, long0, long1;
	Ptr deststringrowptr, destpixrowptr, destpixptr;
	UInt32 *fontlongrowptr;
	UInt8 *bufp, *destp;
	GDHandle gdh;
	PixMapPtr pmp;
#define STARTX		0	// If set to a negative number, it insets from the right edge
#define STARTY		0	// If set to a negative number, it insets from the bottom edge
#define FGCOLOR8	0xFF
#define BGCOLOR8	0x00
#define FGCOLOR16	0x0000
#define BGCOLOR16	0x7FFF
#define FGCOLOR32	0x00000000
#define BGCOLOR32	0x00FFFFFF

	if (gLineCount == 0) {	// First-time initialization
		// We want to avoid the MacsBug screen above all, since we might be
		//	trying to debug graphics drivers that MacsBug may interact with.
		gdh = GetDeviceList();	// First device in list is the MacsBug device
		for (gdh = GetNextDevice(gdh); gdh != nil; gdh = GetNextDevice(gdh)) {
			// Secondarily, we want to avoid the menubar screen.
			if (gdh == GetMainDevice())	// Avoid the menu bar device if possible
				continue;
			goto gotgdev;	// Grab the first non-MacsBug, non-menubar screen
		}
		// Otherwise we'll just take the menubar device
		gdh = GetMainDevice();
gotgdev:
		gDrawingGDevH = gdh;
		gDidEndOfLine = true;	// Kick out a line number this time
	}

	pmp = *(*gDrawingGDevH)->gdPMap;
	destbase = pmp->baseAddr;
	destrowbytes = pmp->rowBytes & 0x7FFF;	// Mask the PixMap bit
	destpixelshift = pmp->pixelSize / 16;	// Map 8,16,32 onto 0,1,2
	clipheight = pmp->bounds.bottom - pmp->bounds.top;
	clipwidth = pmp->bounds.right - pmp->bounds.left;
	if (gLineCount == 0) {	// First-time initialization
		startY = STARTY;
		if (startY < 0)
			startY += clipheight;
		gStartY = startY;
		gDrawY = startY;

		startX = STARTX;
		if (startX < 0)
			startX += clipwidth;
		gStartX = startX;
		gDrawX = startX;
	}
	startY = gStartY;
	startX = gStartX;

	charheight = dbgfontdef.height;
	fontrowbytes = dbgfontdef.rowbytes;
	destY = gDrawY;
	destX = gDrawX;

	bufp = (UInt8 *)str;	// Get the start of the string

	if (gDidEndOfLine) {
		destp = sbuf;
		*destp++ = ' ';	// Put in a leading space for aesthetics
		// Convert the current line number into a string, as a prefix
		destp += int32todecfmtstr(gLineCount, destp, 4, DONTTERMINATE);
		*destp++ = ':';
		*destp++ = ' ';
		// Copy the rest of the source string into our buffer
		while ((*destp++ = *bufp++) != 0)
			;
		*destp = 0;	// Null-terminate the resulting string
		bufp = sbuf;
		gDidEndOfLine = false;
		gLineCount++;
	}

	for (ch = *bufp++; ch != 0; ch = *bufp++) {
		charwidth = dbgfontdef.charwidth[ch];
		deststringrowptr = destbase + destY * destrowbytes + (startX << destpixelshift);
		if (ch == '\n') {	// Check to see if we need to erase to a known width
			clearwidth = gLastPageRight - destX;
			if (clearwidth > 0) {	// We need to erase the rest of the line
				destpixrowptr = deststringrowptr + ((destX - startX) << destpixelshift);
				switch (destpixelshift) {
				case 0:		// 8 bits
					for (i = 0; i < charheight; i++) {
						destpixptr = destpixrowptr;
						for (j = 0; j < clearwidth; j++) {
							*(UInt8 *)destpixptr = BGCOLOR8;
							destpixptr += sizeof(UInt8);
						}
						destpixrowptr += destrowbytes;
					}
					break;
				case 1:		// 16 bits
					for (i = 0; i < charheight; i++) {
						destpixptr = destpixrowptr;
						for (j = 0; j < clearwidth; j++) {
							*(UInt16 *)destpixptr = BGCOLOR16;
							destpixptr += sizeof(UInt16);
						}
						destpixrowptr += destrowbytes;
					}
					break;
				default:	// 32 bits
					for (i = 0; i < charheight; i++) {
						destpixptr = destpixrowptr;
						for (j = 0; j < clearwidth; j++) {
							*(UInt32 *)destpixptr = BGCOLOR32;
							destpixptr += sizeof(UInt32);
						}
						destpixrowptr += destrowbytes;
					}
					break;
				}
			}
			gDidEndOfLine = true;	// Kick out a line number next time
		}
		// Wrap to beginning of next line if we get a newline or hit the right edge
		if (ch == '\n' || destX + charwidth > clipwidth) {
			// First save the width of the line just finished
			if (destX > gCurPageRight)
				gCurPageRight = destX;
			destX = startX;
			destY += charheight;
		}
		if (destY + charheight > clipheight) {	// Wrap vertically
			destY = startY;
			gLastPageRight = gCurPageRight;
			gCurPageRight = 0;
		}

		if (ch == '\n')	// Don't draw anything for newline
			continue;

		deststringrowptr = destbase + destY * destrowbytes + (destX << destpixelshift);
		destpixrowptr = deststringrowptr;
		bitoffset = dbgfontdef.startoffset[ch];	// Get the bit offset to the character
		longindex = bitoffset / 32;
		fontlongrowptr = &dbgfontdata[longindex];

		switch (destpixelshift) {
		case 0:		// 8 bits
			for (i = 0; i < charheight; i++) {
				// Extract and align the character's scan line data (max 32 bits)
				long0 = *fontlongrowptr;
				shift = bitoffset - longindex * 32;
				if (shift > 0) {	// Need to extract a 32-bit unaligned bitfield
					long1 = *(fontlongrowptr+1);
					long0 <<= shift;
					long1 >>= 32 - shift;
					long0 |= long1;
				}
				destpixptr = destpixrowptr;
				for (j = 0; j < charwidth; j++) {
					if (long0 & 0x80000000)	// Use foreground color
						*(UInt8 *)destpixptr = FGCOLOR8;
					else	// Use background color
						*(UInt8 *)destpixptr = BGCOLOR8;
					long0 <<= 1;
					destpixptr += sizeof(UInt8);
				}
				fontlongrowptr = (UInt32 *)((Ptr)fontlongrowptr + fontrowbytes);
				destpixrowptr += destrowbytes;
			}
			break;
		case 1:		// 16 bits
			for (i = 0; i < charheight; i++) {
				// Extract and align the character's scan line data (max 32 bits)
				long0 = *fontlongrowptr;
				shift = bitoffset - longindex * 32;
				if (shift > 0) {	// Need to extract a 32-bit unaligned bitfield
					long1 = *(fontlongrowptr+1);
					long0 <<= shift;
					long1 >>= 32 - shift;
					long0 |= long1;
				}
				destpixptr = destpixrowptr;
				for (j = 0; j < charwidth; j++) {
					if (long0 & 0x80000000)	// Use foreground color
						*(UInt16 *)destpixptr = FGCOLOR16;
					else	// Use background color
						*(UInt16 *)destpixptr = BGCOLOR16;
					long0 <<= 1;
					destpixptr += sizeof(UInt16);
				}
				fontlongrowptr = (UInt32 *)((Ptr)fontlongrowptr + fontrowbytes);
				destpixrowptr += destrowbytes;
			}
			break;
		default:	// 32 bits
			for (i = 0; i < charheight; i++) {
				// Extract and align the character's scan line data (max 32 bits)
				long0 = *fontlongrowptr;
				shift = bitoffset - longindex * 32;
				if (shift > 0) {	// Need to extract a 32-bit unaligned bitfield
					long1 = *(fontlongrowptr+1);
					long0 <<= shift;
					long1 >>= 32 - shift;
					long0 |= long1;
				}
				destpixptr = destpixrowptr;
				for (j = 0; j < charwidth; j++) {
					if (long0 & 0x80000000)	// Use foreground color
						*(UInt32 *)destpixptr = FGCOLOR32;
					else	// Use background color
						*(UInt32 *)destpixptr = BGCOLOR32;
					long0 <<= 1;
					destpixptr += sizeof(UInt32);
				}
				fontlongrowptr = (UInt32 *)((Ptr)fontlongrowptr + fontrowbytes);
				destpixrowptr += destrowbytes;
			}
			break;
		}

		destX += charwidth;
	}

	// Update the global state
	gDrawY = destY;
	gDrawX = destX;
}

#endif	// #if _RAWDRAW
#endif	// #if DEBUG_BUILD
