#include "stdafx.h"
#include "utils.h"

/*++

Routine Description:



Arguments:



Return Value:

    pointer to newly allocated and filled buffer
    NULL on error


Notes:

    the last char will always be '\0' (so if minSize == maxSize == 1) then the 
     string will only contain the null char.

    you always have to delete[] the return value (except on error)

--*/
char* NewRandomBuffer(size_t minSize, size_t maxSize, unsigned char fill, size_t &outLen, unsigned int &seed)
    {
    char *buffer = NULL;
    outLen = 0;

    if(minSize > maxSize) maxSize = minSize;
    if(minSize == 0 || maxSize == 0) return NULL;

    // initialize random number generator
    if(!seed)
        {
        LARGE_INTEGER liPerf;
        QueryPerformanceCounter(&liPerf);
        seed = liPerf.LowPart & 0xFFFF;
        srand(seed);
        }

    // get size of buffer & allocate
    if(maxSize == minSize) outLen = maxSize;
    else outLen = (rand() % (maxSize-minSize)) + minSize;

    buffer = new char[outLen];
    if(!buffer)
        {
        outLen = 0;
        return NULL;
        }

    if(fill == BUFFER_FILL_NULL)
        {
        memset(buffer, '\0', outLen);
        return buffer;
        }

    // fill in the buffer
    int randval;
    for(size_t i=0; i<outLen-1; i++)
        {
        randval = rand();
        switch(fill)
            {
            case BUFFER_FILL_ALPHA:
                buffer[i] = (char)((randval % 26) + 'a');
                break;
            case BUFFER_FILL_NUM:
                buffer[i] = (char)((randval % 10) + '0');
                break;
            case BUFFER_FILL_PUNCT:
                buffer[i] = (char)((randval % 16) + ' ');
                break;
            case BUFFER_FILL_ASCII:
                buffer[i] = (char)randval;
                break;

            default: // BUFFER_FILL_ALPHA
                buffer[i] = (char)((randval % 26) + 'a');
                break;
            }
        }

    // set the terminating char
    buffer[outLen - 1] = '\0';

    return buffer;
    }

/*++

Routine Description:

    Trace
    diagnostic debug output

Arguments:

    LPCTSTR lpszFormat  - pointer to vsprintf format string
    ...                 - list of args to vsprintf

Return Value:

    none

Notes:

    don't exceed 512 bytes of data in the output
    may want to dynamically allocate szBuffer if output exceeds the current setting


--*/
void Trace(LPCTSTR lpszFormat, ...)
    {
    va_list args;
    va_start(args, lpszFormat);

    int nBuf;
    char szBuffer[512];

    nBuf = vsprintf(szBuffer, lpszFormat, args);
    OutputDebugString(szBuffer);

    va_end(args);
    }
