 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/
/******************* Common Debug Print Defines *****************************\
*                                                                           *
* Module: NVDEBUG.C                                                         *
*                                                                           *
\***************************************************************************/

#include <nvrm.h>

#if defined(DEBUG) || defined(__DJGPP__) || defined(_XBOX)

VOID nvDbg_Printf(
    int     module,
    int     debuglevel,
    const char *printf_format,
    ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(module, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID global_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_GLOBAL, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID arch_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_ARCH, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID os_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_OS, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID dac_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_DAC, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID fifo_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_FIFO, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID gr_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_GR, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID heap_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_HEAP, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID video_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_VIDEO, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID mp_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_MP, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID dacClass_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_DACCLASS, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID videoClass_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_VIDEOCLASS, debuglevel, printf_format, arglist);
    va_end(arglist);
}

VOID power_DbgPrintf(int debuglevel, const char* printf_format, ...)
{
    va_list arglist;

    va_start(arglist, printf_format);
    osDbgPrintf_va(DBG_MODULE_POWER, debuglevel, printf_format, arglist);
    va_end(arglist);
}


//======================================================================================
//
// nvDbgSprintf()
//
//======================================================================================
int nvDbgSprintf(U008 *dest, const U008 *fmt, va_list args)
{
    int ch, fieldwidth, precision, flags;
    int usedefaultprecision;
    S032 s32val;
    U032 u32val;

#if 0
    F064 f64val = 0.0;
#endif // 0 

    U008 *f, *d, *specptr;
    U008 *strpval;
    void *pval;

    if (dest == 0) // was nil
        return(0);  // If we don't have a destination, we didn't do any characters

    f = (U008 *)fmt;
    d = (U008 *)dest;

    while ((ch = *f++) != 0) {
        if (ch != '%') {
            *d++ = (U008)ch;
            continue;
        }
        specptr = f - 1;    // Save a pointer to the '%' specifier, in case of syntax errors
        ch = *f++;

        flags = DONTTERMINATE;  // Don't terminate substrings -- we'll null-terminate when we're all done
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
        fieldwidth = 0; // Default field width
        while (ch >= '0' && ch <= '9') {
            fieldwidth = fieldwidth * 10 + ch - '0';
            ch = *f++;
        }
        usedefaultprecision = TRUE; // In case a precision wasn't specified
        // Check for a precision specifier
        if (ch == '.') {    // We have a precision specifier, skip the '.'
            ch = *f++;
            precision = 0;  // Start with 0
            while (ch >= '0' && ch <= '9') {
                precision = precision * 10 + ch - '0';
                ch = *f++;
            }
            usedefaultprecision = FALSE;
        }

        // Perform the conversion operation
        switch (ch) {
        case 'c':   // Copy an ASCII character
            u32val = va_arg(args, int);
            *d++ = (U008) u32val;
            break;
        case 'u':   // Copy a formatted, unsigned decimal number
            flags |= UNSIGNED_F;
        case 'd':   // Copy a formatted, signed decimal number
            s32val = va_arg(args, int);
            // Format the number, increment the dest pointer by the characters copied
            d += nvDbg_int32todecfmtstr(s32val, d, fieldwidth, flags);
            break;
        case 'x':   // Copy a formatted, lower-case hexadecimal number
            flags |= LOWERCASE_F;
        case 'X':   // Copy a formatted, upper-case hexadecimal number
            u32val = va_arg(args, int);
            // Format the number, increment the dest pointer by the characters copied
            d += nvDbg_uint32tohexfmtstr(u32val, d, fieldwidth, flags);
            break;
        case 'p':   // Copy a formatted pointer value
            pval = va_arg(args, void *);
#if !defined(_M_IA64) && !defined(__ia64)
            // Fix me for 64 bit...

            d += nvDbg_uint32tohexfmtstr((U032)pval, d, fieldwidth, flags);
#endif
            break;
#if 0
        case 'f':   // Copy a formatted floating point number
            f64val = va_arg(args, F064);
            if (usedefaultprecision)
                precision = 6;
            // Format the number, increment the dest pointer by the characters copied
            d += nvDbg_float64todecfmtstr(f64val, d, fieldwidth, precision, flags);
            break;
#endif // 0
        case 's':   // Copy a formatted string
            strpval = va_arg(args, U008 *);
            d += nvDbg_strtofmtstr(strpval, d, fieldwidth, flags);
            break;
        case '%':   // Copy a formatted '%' sign
            d += nvDbg_strtofmtstr((U008 *)"%", d, fieldwidth, flags);
            break;
        case 0:     // Gracefully handle premature end-of-string
            f--;    // Back up, now f points to the null character again
        default:    // Unexpected conversion operator, so just echo to the destination
            while (specptr < f)
                *d++ = *specptr++;
            if (ch == 0)
                goto stringdone;
            break;
        }
    }

stringdone:
    *d = 0; // Null-terminate the string
    return((int)(d - (U008 *)dest));   // Return the number of characters we transferred
}


enum {  // Padding option definitions
    PRESPACE_O = 1,
    PREZERO_O = 2,
    POSTSPACE_O = 4
};


#define NUMBUFSIZE  16  // Should be enough for 32-bit integers in decimal or hex

//======================================================================================
//
// nvDbg_int32todecfmtstr()
//
//  This takes a 32-bit integer value and converts it to a formatted decimal string,
//  using options (field width and flags) like those provided by sprintf().  The 32-bit
//  number is assumed to be signed unless the UNSIGNED_F flag is set.  Look at the code
//  for dbugsprintf() above to see which formatting options are implemented.
//
//======================================================================================
int nvDbg_int32todecfmtstr(S032 s32val, U008 *dest, int fieldwidth, int flags)
{
    int i, digitcount, destcount;
    int sign, signchar;
    int fillcount;
    int pad_options;
    U032 u32val, quotient, remainder;
    U008 *intdigp;
    U008 nbuf[NUMBUFSIZE];

    // Process the sign-related options
    if (flags & UNSIGNED_F) {   // Unsigned conversion
        sign = 0;   // No sign character
    } else {    // We're doing a signed conversion
        sign = 1;   // Assume we'll have a sign character
        if (s32val < 0) {
            signchar = '-';
            s32val = -s32val;   // Make the number positive now so we can 'digitize' it
        } else {    // s32val >= 0
            if (flags & PLUSSIGN_F)
                signchar = '+';
            else if (flags & SPACESIGN_F)
                signchar = ' ';
            else
                sign = 0;   // No sign character
        }
    }
    u32val = s32val;    // Do unsigned math from here on out

    // Convert the number into ASCII decimal digits in our local buffer, counting them
    intdigp = &nbuf[NUMBUFSIZE];    // Point past the last character in the buffer
    digitcount = 0; // Nothing written to our local buffer yet
    do {
        quotient = u32val / 10;
        remainder = u32val - quotient * 10;
        *--intdigp =  (U008) (remainder + '0'); // Put the digit into the next lower buffer slot
        digitcount++;
        u32val = quotient;
    } while (u32val > 0);

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    fillcount = fieldwidth - (sign + digitcount);   // Account for the sign, if used
    if (fillcount > 0) {    // We need to do left or right padding
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F)
                pad_options = PREZERO_O;
            else
                pad_options = PRESPACE_O;
        }
    }

    destcount = 0;  // Nothing written out to the destination yet

    // Copy any leading spaces
    if (pad_options & PRESPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    // Copy the sign character, if any
    if (sign) {
        *dest++ = (U008)signchar;
        destcount++;
    }
    // Copy any leading zeros
    if (pad_options & PREZERO_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = '0';
        destcount += fillcount;
    }
    // Copy the decimal digits from our local buffer
    for (i = 0; i < digitcount; i++)
        *dest++ = *intdigp++;
    destcount += digitcount;

    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *dest = 0;
    return(destcount);  // Return the character count, not including the null
}

//======================================================================================
//
// nvDbg_uint32tohexfmtstr()
//
//  This takes a 32-bit unsigned integer value and converts it to a formatted hexadecimal
//  string, using options (field width and flags) like those provided by sprintf().  Look
//  at the code for dbugsprintf() above to see which formatting options are implemented.
//
//======================================================================================
int nvDbg_uint32tohexfmtstr(U032 u32val,  U008 *dest, int fieldwidth, int flags)
{
    int i, digitcount, destcount;
    int c, hexadjust;
    int fillcount;
    U008 fillchar;
    int pad_options;
    U032 quotient;
    U008 *intdigp;
    U008 nbuf[NUMBUFSIZE];

    hexadjust = 'A' - '9' - 1;
    if (flags & LOWERCASE_F)
        hexadjust += 'a' - 'A';

    // Convert the number into ASCII hex digits in our local buffer, counting them
    intdigp = &nbuf[NUMBUFSIZE];    // Point past the last character in the buffer
    digitcount = 0; // Nothing written to our local buffer yet
    do {
        quotient = u32val / 16;
        c = u32val % 16 + '0';
        if (c > '9')    /* A-F */
            c += hexadjust;
        *--intdigp = (U008)c; // Put the digit into the next lower buffer slot
        digitcount++;
        u32val /= 16;
    } while (u32val > 0);

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    fillcount = fieldwidth - digitcount;    // No sign to worry about
    if (fillcount > 0) {    // We need to do left or right padding
        fillchar = ' ';     // Most common fill character is the space
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F) {
                pad_options = PREZERO_O;
                fillchar = '0';
            } else {
                pad_options = PRESPACE_O;
            }
        }
    }

    destcount = 0;  // Nothing written out to the destination yet

    // Copy any leading zeros or spaces
    if (pad_options & (PREZERO_O | PRESPACE_O)) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = fillchar;
        destcount += fillcount;
    }
    // Copy the hex digits from our local buffer
    for (i = 0; i < digitcount; i++)
        *dest++ = *intdigp++;
    destcount += digitcount;

    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = fillchar;
        destcount += fillcount;
    }
    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *dest = 0;
    return(destcount);  // Return the character count, not including the null
}


#if 0

//======================================================================================
//
// nvDbg_float64todecfmtstr()
//
//  This takes a 64-bit floating-point value and converts it to a formatted decimal
//  string, using options (field width, precision, and flags) like those provided by
//  sprintf().  Look at the code for dbugsprintf() above to see which formatting options
//  are implemented.
//
//======================================================================================
int nvDbg_float64todecfmtstr(F064 f64val, U008 *dest, int fieldwidth, int precision, int flags)
{
    int i, firstcount, destcount;
    int sign, signchar, decpt;
    int fillcount;
    int pad_options;
    int reducecount, loopdigits, digitsleft;
    U032 u32val, quotient, remainder;
    F064 f64mant9 = 0.0, f64mant9factor = 0.0, fone = 0.0, ften = 0.0, fbillion = 0.0, powerof10 = 0.0;
    U008 *digp;
    U008 nbuf[NUMBUFSIZE];  // This only needs to hold the first 9 digits of the integer part

    // Process the sign-related options
    sign = 1;   // Assume at first we'll have a sign character
    if (f64val < 0.0) {
        signchar = '-';
        f64val = -f64val;   // Make the number positive now so we can 'digitize' it
    } else {    // f64val >= 0.0
        if (flags & PLUSSIGN_F)
            signchar = '+';
        else if (flags & SPACESIGN_F)
            signchar = ' ';
        else
            sign = 0;   // No sign character
    }

    // Round the number to N decimal places.  We add 0.5 x 10^(-N), which is
    //  equivalent to adding 1 / (2*10^N).  We'll use this latter formula.
    fone = 1.0;     // Keep the compiler from always loading these constants from memory
    ften = 10.0;
    powerof10 = fone;   // 10 ^ 0
    for (i = 0; i < precision; i++)
        powerof10 *= ften;  // Build 10 ^ N
    f64val += fone / (2.0 * powerof10);
    // f64val now contains the properly rounded number

    f64mant9 = f64val;  // Start hunting for the mantissa's 9 uppermost decimal digits
    fbillion = 1e9;     // Keep it in a register
    f64mant9factor = fone;
    // Reduce the mantissa to less than 1 billion, so it will fit in a 32-bit integer
    for (reducecount = 0; f64mant9 >= fbillion; reducecount++) {
        f64mant9 /= fbillion;
        f64mant9factor *= fbillion;
    }

    // Process the highest 32-bits of the mantissa so we can count those digits first

    f64mant9 = f64val / f64mant9factor; // Grab highest 9 integer decimal digits
    u32val = (U032) f64mant9;   // Drop any fraction
    f64mant9 = u32val;  // Now we have a float with only an integer part
    f64val -= f64mant9 * f64mant9factor;    // Subtract out the previous high digits
    f64mant9factor /= fbillion;     // Adjust our division factor

    // Convert the binary into ASCII decimal digits in our local buffer, counting them
    digp = &nbuf[NUMBUFSIZE];   // Point past the last char. of these 9 digits
    firstcount = 0; // No digits of the first 32-bit integer part yet
    do {
        quotient = u32val / 10;
        remainder = u32val - quotient * 10;
        *--digp = (U008) (remainder + '0'); // Put the digit into the next lower buffer slot
        firstcount++;
        u32val = quotient;
    } while (u32val > 0);

    // Figure out whether we'll have a decimal point
    decpt = (precision > 0);    // Don't use a decimal point if no fractional part

    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    // We have the information we need to calculate how many output characters we'll have
    fillcount = fieldwidth - (sign + firstcount + (reducecount * 9) + decpt + precision);
    if (fillcount > 0) {    // We need to do left or right padding
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
            if (flags & ZEROFILL_F)
                pad_options = PREZERO_O;
            else
                pad_options = PRESPACE_O;
        }
    }

    destcount = 0;  // Nothing written out to the destination yet

    // Copy any leading spaces
    if (pad_options & PRESPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
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
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = '0';
        destcount += fillcount;
    }
    // Copy the highest chunk of integer digits from the local buffer
    for (i = 0; i < firstcount; i++)
        *dest++ = *digp++;
    destcount += firstcount;

    // Now we need to convert the remaining integer digits, if any
    for (i = 0; i < reducecount; i++) {
        f64mant9 = f64val / f64mant9factor; // Grab 9 more decimal digits
        u32val = (U032) f64mant9;       // Drop any fraction
        f64mant9 = u32val;      // Now we have a float with only an integer part
        f64val -= f64mant9 * f64mant9factor;    // Subtract out the previous high digits
        f64mant9factor /= fbillion;         // Adjust our division factor
        // Convert the integer part into ASCII decimal digits, directly to the destination
        dest += 9;              // Point past the last char. of this 9-digit chunk
        digp = dest;
        for (loopdigits = 0; loopdigits < 9; loopdigits++) {
            quotient = u32val / 10;
            remainder = u32val - quotient * 10;
            *--digp = (U008) (remainder + '0'); // Put the digit into the next lower buffer slot
            u32val = quotient;
        }
        destcount += 9;
    }
    // f64val has only the fractional part now

    if (!decpt)
        goto checktrailing; // Skip the laborious fraction-processing part

    // Copy the decimal point
    *dest++ = '.';
    destcount++;

    // Similar to how we handled the integer part processing, we'll process up to
    //  9 digits at a time, by multiplying the fraction by a power of 10,
    //  converting to an integer, and converting digits to the destination.

    digitsleft = precision;
    do {
        loopdigits = digitsleft;
        if (loopdigits > 9)
            loopdigits = 9;
        powerof10 = fone;   // 10 ^ 0
        for (i = 0; i < loopdigits; i++)
            powerof10 *= ften;  // Build 10 ^ N
        f64val *= powerof10;    // Push some fractional digits into the integer part
        u32val = (U032) f64val; // Conversion truncates any remaining fraction
        f64val -= u32val;   // Remove the integer part, leave remaining fraction digits
        digp = dest + loopdigits;   // Point past the last char. of this chunk
        for (i = 0; i < loopdigits; i++) {
            quotient = u32val / 10;
            remainder = u32val - quotient * 10;
            *--digp = (U008) (remainder + '0'); // Put the digit into the next lower buffer slot
            u32val = quotient;
        }
        dest += loopdigits;
        destcount += loopdigits;
        digitsleft -= loopdigits;
    } while (digitsleft > 0);

checktrailing:
    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *dest++ = ' ';
        destcount += fillcount;
    }
    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *dest = 0;
    return(destcount);  // Return the character count, not including the null
}

#endif // 0

//======================================================================================
//
// nvDbg_strtofmtstr()
//
//  This takes a source C string and converts it to a formatted output C string,
//  using options (field width and flags) like those provided by sprintf().  Look at
//  the code for dbugsprintf() above to see which formatting options are implemented.
//
//======================================================================================
int nvDbg_strtofmtstr(U008 *src, U008 *dest, int fieldwidth, int flags)
{
    int i, srclen;
    int fillcount;
    U008 fillchar;
    int pad_options;
    U008 *s, *d;

    // For padding calculations, we need to know the source string length
    for (s = src, srclen = 0; *s != 0; s++)
        srclen++;
    // Process the field-padding options
    pad_options = 0;    // Assume we won't be doing any padding
    fillcount = fieldwidth - srclen;

    if (fillcount > 0) {    // We need to do left or right padding
        fillchar = ' ';     // Most common fill character is the space
        if (flags & LEFTALIGN_F) {
            pad_options = POSTSPACE_O;
        } else {    // Right-aligned, fill with zeros or spaces
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
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *d++ = fillchar;
    }
    // Copy the characters from the source string
    for (i = 0; i < srclen; i++)
        *d++ = *s++;

    // Copy any trailing spaces
    if (pad_options & POSTSPACE_O) {
        for (i = 0; i < fillcount; i++) // Copy the pad character(s)
            *d++ = fillchar;
    }

    if ((flags & DONTTERMINATE) == 0)   // Null-terminate the string unless requested not to
        *d = 0;
    return((int)(d - dest));   // Return the character count, not including the null
}

#endif // #if defined(DEBUG) || defined(__DJGPP__) || defined(_XBOX)


//********************************************************************************
//
//  NVRM_TRACE support
//    low-overhead runtime state capture
//    to enable, define USE_NVRM_TRACE (retail or debug builds)
//
//********************************************************************************

#ifdef USE_NVRM_TRACE

// the number of trace words in the ring buffer.
// a trace event can be 1 or more words
#define NVRM_TRACE_ENTRIES (8192)
// a typedef for the trace table.  Add a pad to simplify 
// bounds checking
typedef U032 NVRM_TRACE_t[NVRM_TRACE_ENTRIES + 16];

U032 NVRM_tracing = 0;    // enabled or not?
// a type'd ptr to the table.  This may make it easier for your debugger
//   to dump out the table (definitely helps on the mac)
NVRM_TRACE_t *NVRM_TRACE_GTRACE;
// actual table pointer
U032 *NVRM_TRACE_table;
// current index into the table
U032 NVRM_TRACE_idx;

#define NVRM_TRACE_BUMP(inc) { NVRM_TRACE_idx += (inc); \
                               if (NVRM_TRACE_idx >= NVRM_TRACE_ENTRIES) NVRM_TRACE_idx = 0; \
                               NVRM_TRACE_table[NVRM_TRACE_idx] = '****'; \
                             }

U032 NVRM_TRACE_INIT()
{
// allocate the table
// depending on when you call NVRM_TRACE_INIT, might not be able to
// use osAllocMem()
#ifdef MACOS9
    { extern void *PoolAllocateResident(ULONG, int);
      NVRM_TRACE_table = PoolAllocateResident(sizeof(NVRM_TRACE_t), 1/* clear */);
    }
#endif

    NVRM_TRACE_GTRACE = (void *) NVRM_TRACE_table;

#if defined(MACOS9) && defined(DEBUG) 
    // find the table
    *(U032 *) 0xf8 = (U032) NVRM_TRACE_table;
#endif

    if (NVRM_TRACE_table)
        NVRM_tracing = 1;
    return NVRM_tracing;
}

U032 NVRM_TRACE_DISABLE(void)
{
    U032 previous = NVRM_tracing;
    NVRM_tracing = 0;
    return previous;
}

void NVRM_TRACE_ENABLE(void)
{
    NVRM_tracing = 1;
}

void NVRM_TRACE(U032 value)
{
    if ( ! NVRM_tracing) return;
    if (NVRM_TRACE_table == (U032 *) 0)
        if ( ! NVRM_TRACE_INIT())
            return;

    NVRM_TRACE_table[NVRM_TRACE_idx] = value;
    NVRM_TRACE_BUMP(1);
}

void NVRM_TRACE1(U032 value)
{
    if ( ! NVRM_tracing) return;
    if (NVRM_TRACE_table == (U032 *) 0)
        if ( ! NVRM_TRACE_INIT())
            return;

    NVRM_TRACE_table[NVRM_TRACE_idx] = value;
    NVRM_TRACE_BUMP(1);
}

void NVRM_TRACE2(U032 value1, U032 value2)
{
    if ( ! NVRM_tracing) return;
    if (NVRM_TRACE_table == (U032 *) 0)
        if ( ! NVRM_TRACE_INIT())
            return;
    
    NVRM_TRACE_table[NVRM_TRACE_idx]   = value1;
    NVRM_TRACE_table[NVRM_TRACE_idx+1] = value2;
    NVRM_TRACE_BUMP(2);
}

void NVRM_TRACE3(U032 value1, U032 value2, U032 value3)
{
    if ( ! NVRM_tracing) return;
    if (NVRM_TRACE_table == (U032 *) 0)
        if ( ! NVRM_TRACE_INIT())
            return;
    
    NVRM_TRACE_table[NVRM_TRACE_idx]   = value1;
    NVRM_TRACE_table[NVRM_TRACE_idx+1] = value2;
    NVRM_TRACE_table[NVRM_TRACE_idx+2] = value3;
    NVRM_TRACE_BUMP(3);
}

void NVRM_TRACE4(U032 value1, U032 value2, U032 value3, U032 value4)
{
    if ( ! NVRM_tracing) return;
    if (NVRM_TRACE_table == (U032 *) 0)
        if ( ! NVRM_TRACE_INIT())
            return;
    
    NVRM_TRACE_table[NVRM_TRACE_idx]   = value1;
    NVRM_TRACE_table[NVRM_TRACE_idx+1] = value2;
    NVRM_TRACE_table[NVRM_TRACE_idx+2] = value3;
    NVRM_TRACE_table[NVRM_TRACE_idx+3] = value4;
    NVRM_TRACE_BUMP(4);
}

void NVRM_TRACE5(U032 value1, U032 value2, U032 value3, U032 value4, U032 value5)
{
    if ( ! NVRM_tracing) return;
    if (NVRM_TRACE_table == (U032 *) 0)
        if ( ! NVRM_TRACE_INIT())
            return;
    
    NVRM_TRACE_table[NVRM_TRACE_idx]   = value1;
    NVRM_TRACE_table[NVRM_TRACE_idx+1] = value2;
    NVRM_TRACE_table[NVRM_TRACE_idx+2] = value3;
    NVRM_TRACE_table[NVRM_TRACE_idx+3] = value4;
    NVRM_TRACE_table[NVRM_TRACE_idx+4] = value5;
    NVRM_TRACE_BUMP(5);
}

U032 NVRM_TRACE_REG_RD32(PHWINFO pDev, U032 offset)
{
    U032 value = ACTUAL_REG_RD32(offset);
    NVRM_TRACE3('RD32', offset, value);
    return value;
}

void NVRM_TRACE_REG_WR32(PHWINFO pDev, U032 offset, U032 value)
{
    ACTUAL_REG_WR32(offset, value);
    NVRM_TRACE3('WR32', offset, value);
}

#endif  // USE_NVRM_TRACE

#undef NVRM_TRACE_DUMP
void NVRM_TRACE_DUMP(void)
{
#ifdef USE_NVRM_TRACE    // make an empty one so dump can be called from oslog.c retail
    int i;
    static int dumping = 0;
    
    // No table?
    if ( ! NVRM_TRACE_table) return;
    
    // don't nest while dumping this
    if (dumping) return;
    dumping = 1;
    
    NVRM_TRACE_DISABLE();

    for (i=0; i <= NVRM_TRACE_ENTRIES; i += 8)
    {
        DBG_PRINTF((DEBUGLEVEL_ERRORS, "%x %x %x %x %x %x %x %x\n", 
                    NVRM_TRACE_table[i+0],
                    NVRM_TRACE_table[i+1],
                    NVRM_TRACE_table[i+2], 
                    NVRM_TRACE_table[i+3],
                    NVRM_TRACE_table[i+4],
                    NVRM_TRACE_table[i+5],
                    NVRM_TRACE_table[i+6],
                    NVRM_TRACE_table[i+7]));
    }
    dumping = 0;
#endif
}
