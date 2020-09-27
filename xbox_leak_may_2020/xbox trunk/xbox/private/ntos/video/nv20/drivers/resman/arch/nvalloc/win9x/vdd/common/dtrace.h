//
// low overhead tracing facility
//

// add "#define DEFINE_GLOBALS" in only 1 file!
#ifdef DEFINE_GLOBALS
#define	GLOBAL
#define	INIT(_x_)	= _x_
#else
#define	GLOBAL	extern
#define	INIT(_x_)
#endif

#define DTRACE	1

#ifdef DTRACE

#undef DTRACE
#define DTRACE(val)	do_dtrace((unsigned int) val, 0)
#define	DTRACE_CC(val)	do_dtrace((unsigned int) val, 1)
#define	DTRACE_RC(rc)	if ( (rc) != 0 ) { DTRACE_CC('EEEE'); DTRACE(rc); }
#define DTRACE_CYCLE_DELTA()  dtraceCycleDelta()
#define DTRACE_CYCLE_COUNT_AND_DELTA()  dtraceCycleCountAndDelta()
#define	DTRACING

#else // no trace support

#define DTRACE(val)
#define	DTRACE_CC(val)
#define	DTRACE_RC(rc)
#define DTRACE_CYCLE_DELTA()
#define DTRACE_CYCLE_COUNT_AND_DELTA()
#undef	DTRACING

#endif

#ifdef DTRACING

#define DTRACE_ENTRIES	1024	// must be power of two
GLOBAL unsigned int	dtrace_buf[DTRACE_ENTRIES+1];
GLOBAL int          dtrace_idx INIT(0);

static void
do_dtrace( unsigned int val, int char_const )
{

    /*
     * Some compilers "reverse" long character constants from their
     * readable forms.  Detect and fix that here.
     */

    if ( char_const && ('\0\0\0\1' != 1) )
	val = ((val & 0xff) << 24) +
	      (((val >> 8) & 0xff) << 16) +
	      (((val >> 16) & 0xff) << 8) +
	      ((val >> 24) & 0xff);

    /* XXX enter critical section */

    dtrace_buf[dtrace_idx++] = val;
    dtrace_idx &= (DTRACE_ENTRIES - 1);
    dtrace_buf[dtrace_idx] = '***\0';

    /* XXX exit critical section */

    return;

}

#endif /* DTRACING */

//
// cycle count stuff
//

GLOBAL unsigned int prevHi INIT(0);
GLOBAL unsigned int prevLo INIT(0);

static unsigned int getCycleCount( unsigned int* pHi, unsigned int* pLo )
{

    unsigned int        hi, lo;

    /* emit RDTSC instruction to get clock count */
    __asm
    {
        _emit   0x0f 
        _emit   0x31
        mov         hi, edx
        mov         lo, eax
    }

    *pHi = hi;
    *pLo = lo;

    return(lo);

}

static unsigned int getDeltaCycles( 
    unsigned int endHi, 
    unsigned int endLo, 
    unsigned int startHi, 
    unsigned int startLo)
{

    unsigned int resultHi, resultLo;

    if ( endLo > startLo ) {
        resultLo = endLo - startLo;
    }
    else {
        resultLo = 4294967295 - (startLo-endLo);
        endHi--;
    }

    // resultHi = endHi - endLo;
    
    return(resultLo);

}

static void dtraceCycleDelta()
{
    unsigned int    currentHi, currentLo, deltaLo;
    
    getCycleCount(&currentHi,&currentLo);
    deltaLo = getDeltaCycles(currentHi, currentLo, prevHi, prevLo);
    
    DTRACE(deltaLo);

    prevHi = currentHi;
    prevLo = currentLo;
    
}

static void dtraceCycleCountAndDelta()
{
    unsigned int    currentHi, currentLo, deltaLo;
    
    getCycleCount(&currentHi,&currentLo);
    deltaLo = getDeltaCycles(currentHi, currentLo, prevHi, prevLo);
    
    DTRACE(currentLo);
    DTRACE(deltaLo);

    prevHi = currentHi;
    prevLo = currentLo;
    
}

