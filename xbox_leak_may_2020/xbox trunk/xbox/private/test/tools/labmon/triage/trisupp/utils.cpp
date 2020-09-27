#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Removes the lines in the lm output that have deferred in them.
// This isn't really necessary anymore because we have lm l but
// it doesnt hurt to leave it in here.
DWORD RemoveDeferred(CHAR **LineList)
    {
    DWORD i = 0, nextline = 0;
    
    for (i=0;i<MAXLINES;i++)
        {
        if (strstr(LineList[i], "(deferred)"))
            {
            LineList[i] = NULL;
            }
        }
    
    for (i=0;i<MAXLINES;i++)
        {
        if (LineList[i] != NULL)
            {
            LineList[nextline] = LineList[i];
            if (nextline != i)
                LineList[i] = NULL;
            nextline++;
            }
        }
    return nextline;
    }


// Takes a buffer and breaks it into lines.  Linelist is returned
// as a list of pointers into the buffer with each pointer being one line.
// This procedure has the side effect of changing the buffer.
DWORD GetLinesFromString(CHAR *theString, CHAR **LineList)
    {
    DWORD i=0;
    CHAR *p=theString;
    
    if (!LineList)
        return 0;
    
    if (p == NULL)
        {
        LineList[0] = NULL;
        return 0;
        }
    
    while ((p) && (*p)) 
        {
        LineList[i++] = p;
        
        p += strcspn(p, "\r\n");
        
        if (p && *p) 
            {
            *p = '\0';
            p++;
            if ((*p == '\r') || (*p == '\n')) 
                {
                p++;
                }
            }
        
        if (i == MAXLINES)
            {
            i = RemoveDeferred(LineList);
            if (i == MAXLINES)
                return i;
            }
        }
    
    return i;
    }

// Return a BOOL denoting whether the pointer is pointing at a Hex value
BOOL IsHex(CHAR *theStr)
    {
    DWORD x;
    return ((theStr) && 
        (*theStr) && 
        (strchr(theStr, ' ') >= theStr + 8) && 
        (0 != sscanf(theStr, "%x", &x)));
    }

// Case insensative strstr()
CHAR *stristr(const CHAR *s1, const CHAR *s2)
    {
    DWORD len1 = 0;
    DWORD len2 = 0;
    DWORD i = 0;
    CHAR *s1c = NULL;
    CHAR *s2c = NULL;
    CHAR *where = NULL;
    CHAR *retval = NULL;
    
    if ((s1 == NULL) || (s2 == NULL))
        return NULL;
    
    len1 = strlen(s1);
    len2 = strlen(s2);
    
    if (len2 > len1)
        return NULL;
    
    s1c = _strdup(s1);
    if (s1c == NULL)
        return NULL;
    
    s2c = _strdup(s2);
    if (s2c == NULL)
        return NULL;
    
    _strlwr(s1c);
    _strlwr(s2c);
    
    where = strstr(s1c, s2c);
    if (where != NULL)
        {
        retval = (CHAR *)(s1 + (where - s1c));
        }
    
    free(s1c);
    free(s2c);
    
    return retval;
    }



BOOL DynamicAppend(CHAR **ppBuffer, CHAR *AppendString)
    {
    CHAR *temp = NULL;
    
    if (!AppendString)
        return TRUE;
    
    if (!(*ppBuffer))
        {
        *ppBuffer = _strdup(AppendString);
        if (*ppBuffer)
            return TRUE;
        else
            return FALSE;
        }
    
    temp = (CHAR *) malloc((strlen(*ppBuffer) + strlen(AppendString) + 1) * sizeof(CHAR));
    if (!temp)
        return FALSE;
    
    strcpy(temp, *ppBuffer);
    strcat(temp, AppendString);
    
    free(*ppBuffer);
    
    *ppBuffer = temp;
    
    return TRUE;
    }

