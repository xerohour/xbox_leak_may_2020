#include "iString.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <malloc.h>
#include <xtl.h>

iString::iString()
    {
    str = end = NULL;
    bufferSize = 0;
    aggressive = false;
    }

iString::iString(unsigned initialSize)
    {
    str = end = NULL;
    bufferSize = 0;
    aggressive = false;
    Grow(initialSize);
    }

iString::~iString()
    {
    if(allocated) free(str);
    }

void* iString::Grow(unsigned count /*=0*/)
    {
    if(bufferSize == 0)
        {
        bufferSize = count?count:16;
        str = (char*)malloc(bufferSize);
        if(!str) return NULL;
        *str = '\0';
        end = str + 1;
        allocated = true;
        return str;
        }

    // adjust buffer size
    if(aggressive || count != 0) bufferSize += count?count:16;
    else bufferSize = bufferSize << 1;

    // allocate, copy, and free
    char *temp;
    unsigned size = length();
    if(allocated)
        {
        // Note: this is MUCH faster than the standard: new, memcpy, delete
        temp = (char*)realloc(str, bufferSize);
        }
    else
        {
        temp = (char*)malloc(bufferSize);
        if(!temp) return NULL;
        memcpy(temp, str, size+1); // +1 for the NULL char
        allocated = true;
        }

    str = temp;
    end = str + size;
    return str;
    }

void iString::Attach(char *src)
    {
    if(allocated) free(str);
    allocated = false;
    str = src;
    bufferSize = ::strlen(str) + 1;
    end = str + bufferSize - 1;
    }

unsigned iString::Hash(void)
    {
    char *ptr = str;
    unsigned hash = 0;

    while(*ptr)
        {
        hash += (hash << 5) + *ptr++;
        }

    return hash;
    }

char* iString::sprintf(char *format, ...)
    {
    va_list args;
    va_start(args, format);

    int count;

    if(!str) Grow(); // cause _vsnprintf sucks

    while((count = _vsnprintf(str, bufferSize, format, args)) == -1)
        Grow();

    end = str + count;
    va_end(args);

    // deal with bug in _vsnprintf where terminating NULL char is omitted if string just fits
    if(*end != '\0')
        {
        Grow();
        *end = '\0';
        }

    return str;
    }

char* iString::scatf(char *format, ...)
    {
    va_list args;
    va_start(args, format);

    int count;

    if(!str) Grow(); // cause _vsnprintf sucks

    while((count = _vsnprintf(end, bufferSize-length(), format, args)) == -1)
        Grow();

    end += count;
    va_end(args);

    // deal with bug in _vsnprintf where terminating NULL char is omitted if string just fits
    if(*end != '\0')
        {
        Grow();
        *end = '\0';
        }

    return str;
    }

char* iString::strcpy(const char *src)
    {
    unsigned len = ::strlen(src);

    if(len+1 > bufferSize)
        {
        // no need to copy memory around when we will just write over it
        end = str;
        Grow(len+1);
        }

    memcpy(str, src, len+1);
    end = str + len;

    return str;
    }

char* iString::strcat(const char *src)
    {
    unsigned len = ::strlen(src);

    if(length()+len+1 > bufferSize) Grow(length()+len+1 - bufferSize);

    memcpy(end, src, len+1);
    end += len;

    return str;
    }

char* iString::strncpy(const char *src, unsigned len)
    {
    if(len+1 > bufferSize)
        {
        // no need to copy memory around when we will just write over it
        end = str;
        Grow(len+1);
        }

    memcpy(str, src, len+1);
    end = str + len;

    return str;
    }

char* iString::strncat(const char *src, unsigned len)
    {
    if(length()+len+1 > bufferSize) Grow(length()+len+1 - bufferSize);

    memcpy(end, src, len+1);
    end += len;

    return str;
    }

char* iString::strrchr(int ch)
    {
    if(!end) return NULL;

    char *string = end;

    while(--string != str && *string != (char)ch)
        {
        }

    if(*string == (char)ch)
        return (char *)string;

    return NULL;
    }

void iString::DebugPrint(void)
    {
    OutputDebugStringA(str);
    }

