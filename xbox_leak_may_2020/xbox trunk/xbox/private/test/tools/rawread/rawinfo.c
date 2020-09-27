#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"
#include "defs.h"
#include "file.h"
#include "image.h"
#include "text.h"


int main(int argc, char **argv)
{
    FILEHANDLE                  hFile;
    LEADSECTOR                  ls;
    DWORD                       dwFirstData;
    IMGBITMAP                   bitmap;
    DWORD                       dwCluster;
    BOOL                        fSet;
    DWORD                       dwCount;
    int                         nRet;
    
    GoPropaganda();
    
    if((hFile = FileOpenForRead(argv[1])) == -1)
    {
        return ErrorMsg(ERROR_FILEOPEN);
    }
    
    if(nRet = ReadImageHeader(hFile, &ls, &dwFirstData))
    {
        return ErrorMsg(nRet);
    }
    
    printf("===============================================================================\n");
    printf("Image header\n");
    printf("===============================================================================\n");
    
    printf("dwClusterCount:     %lu\n", ls.dwClusterCount);
    printf("bSequence:          %u\n", ls.bSequence);
    printf("bFragCount:         %u\n", ls.bFragCount);
    printf("dwClustersPerFrag:  %lu\n", ls.dwClustersPerFrag);
    printf("szNextFrag:         %s\n", ls.szNextFrag);
    printf("Reserved1:          %u\n", ls.Reserved1);
    printf("wFlags:             %u %s%s\n", ls.wFlags, (ls.wFlags & OPTIONS_COMPRESS) ? "compress " : "", (ls.wFlags & OPTIONS_ALLOCATED) ? "allocated " : "");
    printf("Reserved2:          %lu\n", ls.Reserved2);
    printf("dwCompBitmapSize:   %lu\n", ls.dwCompBitmapSize);
    printf("dwAllocBitmapSize:  %lu\n", ls.dwAllocBitmapSize);
    printf("dwFirstCluster:     %lu\n", ls.dwFirstCluster);
    printf("dwLastCluster:      %lu\n", ls.dwLastCluster);
    printf("fCompleted:         %s\n", ls.fCompleted ? "TRUE" : "FALSE");
    printf("\n");
    printf("First data byte:    %lu\n", dwFirstData);
    printf("\n");
    
    if(ls.dwCompBitmapSize > 0)
    {
        if(nRet = InitBitmap(&bitmap, ls.dwCompBitmapSize, hFile, 2, TRUE))
        {
            return ErrorMsg(nRet);
        }
        
        printf("===============================================================================\n");
        printf("Compressed clusters\n");
        printf("===============================================================================\n");
        
        dwCount = 0;
        
        for(dwCluster = 0; dwCluster <= ls.dwLastCluster; dwCluster++)
        {
            if(nRet = GetBitmapBit(&bitmap, dwCluster, &fSet))
            {
                return ErrorMsg(nRet);
            }
            
            if(fSet)
            {
                printf("%lu\t", dwCluster + ls.dwFirstCluster + 2);
                dwCount++;
            }
        }
        
        if(nRet = DiscardBitmap(&bitmap))
        {
            return ErrorMsg(nRet);
        }
        
        printf("\n\n%lu compressed clusters found.\n\n", dwCount);
    }
        
    if(ls.dwCompBitmapSize > 0)
    {
        if(nRet = InitBitmap(&bitmap, ls.dwCompBitmapSize, hFile, 2 + ls.dwCompBitmapSize, TRUE))
        {
            return ErrorMsg(nRet);
        }
        
        printf("===============================================================================\n");
        printf("Unallocated clusters\n");
        printf("===============================================================================\n");
        
        dwCount = 0;
        
        for(dwCluster = 0; dwCluster <= ls.dwLastCluster; dwCluster++)
        {
            if(nRet = GetBitmapBit(&bitmap, dwCluster, &fSet))
            {
                return ErrorMsg(nRet);
            }
            
            if(fSet)
            {
                printf("%lu\t", dwCluster + ls.dwFirstCluster + 2);
                dwCount++;
            }
        }
        
        if(nRet = DiscardBitmap(&bitmap))
        {
            return ErrorMsg(nRet);
        }
        
        printf("\n\n%lu unallocated clusters found.\n\n", dwCount);
    }
        
    FileClose(hFile);
    
    return 0;
}