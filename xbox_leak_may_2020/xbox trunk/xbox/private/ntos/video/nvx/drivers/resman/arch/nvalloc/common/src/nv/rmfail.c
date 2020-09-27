/*
 * TODO
 *    what about multiboard stuff?
 *     win2k puts RM_FAILURE in per-device registery area
 *    win9x: need printf
 */

 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/*
 * Usage:
 *
 *    Add RM_FAILURE tests and triggers as desired
 *    #include <rmfail.h> in any file w/ RM_FAILURE
 * 
 *    Compile with -DDEBUG_RM_FAILURES
 *        Otherwise all RM_FAILURE macro's will vanish
 *
 *    use rmfail.pl in tools/scripts to set RM_FAILURES registry
 *
 *       rmfail.pl set '127 128:18 129:37% 113:50% 200-300:10%'
 *
 *    Optionally set RM_FAILURES_SEED to seed the random number generator.
 *    Otherwise it will be seeded w/ the current time.
 *
 *    Any RM_FAILURE's encountered before RM_FAILURE_ENABLE() will
 *    harmlessly do nothing.
 *    At startup, RmFailuresEnable() will parse the RM_FAILURES string
 *    and produce debug output indicating tests being probed.
 *
 */

#if !defined(__GNUC__) && !defined(MACOS)
#include <ntddk.h>       /* XXX just for DbgPrint; delete this if/when switch to another printf */
#endif
#include <nvrm.h>
#include <client.h>
#include <nvos.h>
#include <nvarch.h>

#include <rmfail.h>

#ifdef DEBUG_RM_FAILURES

/* our libc routines.  Since we can't get at libc from the driver we have
 * them incorporated in this file.  These are all from CYGNUS' newlib
 */
// we can't use ctype.h from within resman...
#undef isspace
#define isspace(c) (((c) == ' ') || ((c) == '\011') || ((c) == '\012'))
#undef isdigit
#define isdigit(c) (((c) >= '0') && ((c) <= '9'))
#undef isalpha
#define isalpha(c) ((((c) >= 'a') && ((c) <= 'z')) || (((c) >= 'A') && ((c) <= 'Z')))
#undef isupper
#define isupper(c) (((c) >= 'A') && ((c) <= 'Z'))

long nv_strtol(const char *nptr, char **endptr, int base);
#define RAND_MAX 0x7fffffff
void nv_srand(unsigned int seed);
int nv_rand(void);

static void rm_failure_clear(void)
{        
    (void) memset(rm_failure, 0, sizeof(rm_failure));
    (void) memset(rm_failure_counts, 0, sizeof(rm_failure_counts));
}

static void freakout(char *s)
{
    RM_FAILURE_PRINTF(("RM_FAILURE: init failed: %s\n", s));
    rm_failure_clear();
}

#endif // DEBUG_RM_FAILURES

int RmFailureTriggered(
    int n
)
{
    int rc = 0;
#ifdef DEBUG_RM_FAILURES
    int rint;
    int percent;
    int trigger_count;

    trigger_count = ++rm_failure_counts[n];

    if (rm_failure[n] & RM_FAILURES_MASK_PERCENT)
    {
        percent = rm_failure[n] & RM_FAILURES_MASK_COUNT;
        rint = 1 + (nv_rand() % 100);
        if (rint <= percent)
            rc = 1;
    }
    else
        rc = trigger_count >= rm_failure[n];

    if (rc)
    {
        RM_FAILURE_PRINTF(("RM_FAILURE: triggering %d\n", n));

        if (rm_failure[n] & RM_FAILURES_FLAG_BREAK_ON_TRIGGER)
        {
            osDbgBreakPoint();
        }
        
        // disable the trigger after it fires?
        if (rm_failure[n] & RM_FAILURES_FLAG_DONT_DISABLE_ON_TRIGGER)
        {
            // we aren't disabling, just set count to 0
            if ( ! (rm_failure[n] & RM_FAILURES_MASK_PERCENT))
                rm_failure_counts[n] = 0;
        }
        else
        {
            RM_FAILURE_TRIGGER_DISABLE(n);
        }
    }
#endif  // DEBUG_RM_FAILURES

    return rc;
}


void RmFailuresEnable(void)
{
/*
    parse input of environment variables or registry or config file
        or all 3! into rm_failure[]

    comma or space separated list of

       t[-T][:n[%]] 

    where 't' is test number, 'n' is integer and '%' indicates
    'n' is a percent likelihood of failure
    T is used to specify a range

    Example:

        set RM_FAILURES=127b 128:18 129:37% 113:50% 200-300:10%
        
        test 127 will fail on first pass and enter debugger (checked build)
        test 128 will fail on 18th pass
        test 129 has a 37% chance of failing on every pass
        test 113 has a 50% chance of failing
        test range 200-300 to 10% failing each
*/

#ifdef DEBUG_RM_FAILURES
#define MY_MAX_STR 1024
    char s[MY_MAX_STR], sr[32];
    char *p;
    unsigned int seed;
    int len;
    RM_STATUS rm_status;
    extern char strDevNodeRM[];
    
    // init all to disabled
    rm_failure_clear();
    
    (void) memset(s, 0, sizeof(s));
    len = sizeof(s) - 1;	// for NULL term
    rm_status = osReadRegistryString(strDevNodeRM, "RM_FAILURES", s, &len);
    if (rm_status != RM_OK)
        goto done;

    RM_FAILURE_PRINTF(("RM_FAILURE: %s\n", s));
    
    // init random #
    len = 0; // XXX GetEnvironmentVariable("RM_FAILURES_SEED", sr, sizeof(sr));
    if (len > 0)
        seed = (unsigned int) nv_strtol(sr, (void *) 0, 0);
    else
    {
        // use current time in seconds as seed
        // not exactly defensible for Numerical Methods, but...
#ifdef UNIX
        U032 sec, usec;
        osGetCurrentTime(&sec, &usec);
#else
        U032 year, month, day, hour, min, sec, msec;
        osGetCurrentTime(&year, &month, &day, &hour, &min, &sec, &msec);
        if (year > 1970) year -= 1970;
        month += year * 12;
        day += month * 30;
        hour += day * 24;
        min += hour * 60;
        sec += min * 60;
#endif
        seed = (unsigned int) sec;
    }
    nv_srand(seed);

    RM_FAILURE_PRINTF(("RM_FAILURE: rand seed is %d\n", seed));

    // parse the string
    p = s;
    while (*p)
    {
        int t1, t2, n, percent;
        char *newp;
        int flags = 0;

        newp = p;
        t1 = nv_strtol(p, &newp, 0);
        if (newp == p)   // did not advance; garbage in str. bail out
            break;
        p = newp;

        n = 1;
        percent = 0;
        if ((t1 < 0) || (t1 >= RM_FAILURES_MAX))
            freakout("invalid test number");

        // test range specified?
        t2 = t1;
        if (*p == '-')
        {
            p++;
            newp = p;
            t2 = nv_strtol(p, &newp, 0);
            if (newp == p) // advance?
                break;
            p = newp;

            if ((t2 < 0) || (t2 >= RM_FAILURES_MAX) || (t1 > t2))
            {
                freakout("invalid test range");
                goto failed;
            }
        }

        // check for break and dont disable flags
        while (*p == 'b' || *p == 'd')
        {
            if (*p == 'b')
                flags |= RM_FAILURES_FLAG_BREAK_ON_TRIGGER;
            else if (*p == 'd')
                flags |= RM_FAILURES_FLAG_DONT_DISABLE_ON_TRIGGER;
            p++;
        }
        
        // how quick does the failure fire?
        if (*p == ':')
        {
            p++;
            n = nv_strtol(p, &p, 0);            
            if (n > RM_FAILURES_MAX_COUNT)
            {
                freakout("invalid count");
                goto failed;
            }
            if (*p == '%')
            {
                p++;
                percent = n;
                if ((percent > 100) || (percent < 1))
                {
                    freakout("invalid percent");
                    goto failed;
                }
            }
        }
        
        // record the failure instructions in rm_failure[]
        if (t2 != t1)
        {
            int t;

            for (t = t1; t <= t2; t++)
                if (percent)
                    rm_failure[t] = RM_FAILURES_MASK_PERCENT | percent | flags;
                else
                    rm_failure[t] = n | flags;

            RM_FAILURE_PRINTF(("RM_FAILURE: set range %d-%d to %d%s%s%s\n",
                               t1, t2,
                               percent ? percent : n,
                               percent ? "%" : "",
                               flags & RM_FAILURES_FLAG_BREAK_ON_TRIGGER ?
                                 " (break)" : "",
                               flags & RM_FAILURES_FLAG_DONT_DISABLE_ON_TRIGGER ?
                              "(dont disable)" : ""
                              ));
        }
        else
        {            
            if (percent)
                rm_failure[t1] = RM_FAILURES_MASK_PERCENT | percent | flags;
            else
                rm_failure[t1] = n | flags;

            RM_FAILURE_PRINTF(("RM_FAILURE: set %3d to %d%s%s%s\n",
                               t1,
                               percent ? percent : n,
                               percent ? "%" : "",
                               flags & RM_FAILURES_FLAG_BREAK_ON_TRIGGER ?
                                 " (break)" : "",
                               flags & RM_FAILURES_FLAG_DONT_DISABLE_ON_TRIGGER ?
                              "(dont disable)" : ""
                              ));
        }

        while (*p && (*p == ' '))
            p++;
        if (*p == ',') p++;
    }

 done:
    return;
 failed:
    return;
#endif
}

//
// Disable all RM_FAILURE triggers by zapping them
// To re-enable you must use RmFailuresEnable() which will
// re-read the registry RM_FAILURES value.
//
void RmFailuresDisable(void)
{
#ifdef DEBUG_RM_FAILURES
    RM_FAILURE_PRINTF(("RM_FAILURE: disabled\n"));
    rm_failure_clear();
#endif
}

#ifdef DEBUG_RM_FAILURES
/* Utility routines only below this point */

/*
 * Convert a string to a long integer.
 * From 
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 *
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define LONG_MAX  2147483647L
#define LONG_MIN	(- LONG_MAX - 1L)

long nv_strtol(
    const char *nptr,
    char **endptr,
    int base
)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
	} else if (neg)
		acc = -(long)acc;
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}
static unsigned int next = 1;

void nv_srand(unsigned int seed)
{
        next = seed;
}

int nv_rand(void)
{
        return ((next = next * 1103515245 + 12345) & RAND_MAX);
}


#endif  // DEBUG_RM_FAILURES
