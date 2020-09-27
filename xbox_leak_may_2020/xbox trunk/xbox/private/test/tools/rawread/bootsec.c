#include "bootsec.h"
#include "defs.h"
#include "diskutil.h"
#include "image.h"
#include "mem.h"


BOOL GetFileSystemType(BYTE bDrive, LPSTR lpszType)
{
    BGBOOTSECTOR            bs;
    BYTE                    abType[8];
    int                     nSize;
    
    // Read the boot sector
    if(!ReadSector(bDrive, &bs, 0, 1))
    {
        return FALSE;
    }
    
    MemCopy(abType, GetFSType(&bs), sizeof(abType));
    
    // Ignore any trailing spaces
    nSize = sizeof(abType);
    
    while(nSize && abType[nSize - 1] == ' ')
    {
        nSize--;
    }
    
    // Copy the string
    MemCopy(lpszType, abType, nSize);
    
    // Null-terminate the string
    *(lpszType + nSize) = 0;
    
    // Return success
    return TRUE;
}


