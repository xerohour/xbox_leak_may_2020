#if defined(WIN32) || defined(WIN16)
#include <windows.h>
#endif // defined(WIN32) || defined(WIN16)

#include <dos.h>
#include "debug.h"
#include "defs.h"
#include "diskutil.h"
#include "mem.h"

#pragma pack(1)

// Used with GetExtendedDPB()
typedef struct tagGETEXTDPB
{
    WORD                    wSize;
    DPB                     dpb;
} GETEXTDPB, *PGETEXTDPB, far *LPGETEXTDPB;

// DISKIO structure used for int25, int26 calls
typedef struct tagDISKIO
{
    DWORD                   dwSecStart;
    WORD                    wSecCount;
    LPBYTE                  lpbBuffer;
} DISKIO, *PDISKIO, far *LPDISKIO;

#pragma pack()


// Globals
static WORD                 g_wDosVer = 0;


// Local prototypes
BOOL DISKUTILAPI SectorIo(BYTE, LPVOID, DWORD, WORD, BOOL);
BOOL DISKUTILAPI Fat16SectorIo(BYTE, LPDISKIO, BOOL);
BOOL DISKUTILAPI Fat32SectorIo(BYTE, LPDISKIO, BOOL);
BOOL DISKUTILAPI Fat16GetDPB(BYTE, LPDPB);
BOOL DISKUTILAPI Fat32GetDPB(BYTE, LPDPB);


BOOL DISKUTILAPI InitDiskUtil(void)
{
    g_wDosVer = GetDosVersion();
    
    return TRUE;
}


BOOL DISKUTILAPI FreeDiskUtil(void)
{
    g_wDosVer = 0;

    return TRUE;
}


BOOL DISKUTILAPI ReadSector(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, WORD wCount)
{
    DPF("Reading sectors %lu to %lu on drive %u", dwSector, dwSector + wCount, bDrive);

    return SectorIo(bDrive, lpvBuffer, dwSector, wCount, FALSE);
}


BOOL DISKUTILAPI WriteSector(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, WORD wCount)
{
    DPF("Writing sectors %lu to %lu on drive %u", dwSector, dwSector + wCount, bDrive);

    return SectorIo(bDrive, lpvBuffer, dwSector, wCount, TRUE);
}


BOOL DISKUTILAPI SectorIo(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, WORD wCount, BOOL fWrite)
{
    DISKIO                  diskio;
    BOOL                    fSuccess;
    BYTE                    bTry;
    
    diskio.dwSecStart = dwSector;
    diskio.wSecCount = wCount;
    diskio.lpbBuffer = (LPBYTE)lpvBuffer;

    bTry = 0;
    
    do
    {
        if(IS_FAT32_KERNEL(g_wDosVer))
        {
            fSuccess = Fat32SectorIo(bDrive, &diskio, fWrite);
        }
        else
        {
            fSuccess = Fat16SectorIo(bDrive, &diskio, fWrite);
        }

        if(!fSuccess)
        {
            bTry++;
        }
    }
    while(!fSuccess && bTry < ABSRETRIES);

    return fSuccess;
}


#pragma optimize("", off)


BOOL DISKUTILAPI ResetDrive(void)
{
    DPF("Flushing all disk buffers");

    _asm
    {
        mov     ah, 0Dh         ; Reset Drive
        int     21h
    }

    return TRUE;
}


BOOL DISKUTILAPI Fat16SectorIo(BYTE bDrive, LPDISKIO lpDiskIo, BOOL fWrite)
{
    BOOL                    fSuccess;

    _asm
    {
        mov     fSuccess, FALSE     ; assume failure
    
        push    ds
    
        mov     al, bDrive          ; zero-based drive number
        lds     bx, lpDiskIo        ; ds:bx points to a DISKIO structure
        mov     cx, 0FFFFh          ; indicate we're using a DISKIO structure
    
        cmp     fWrite, TRUE        ; are we reading or writing?
        je      dowrite
    
        int     25h
        jmp     iodone
    
    dowrite:

        int     26h

    iodone:

        jc      failure
    
        mov     fSuccess, TRUE      ; success
    
    failure:

        popf                        ; remove the flags left by int 25h/26h
        pop     ds
    }
    
    return fSuccess;            
}


BOOL DISKUTILAPI Fat32SectorIo(BYTE bDrive, LPDISKIO lpDiskIo, BOOL fWrite)
{
    BOOL                        fSuccess;

    _asm
    {
        mov     fSuccess, FALSE     ; assume failure
    
        push    ds
        push    si
        
        mov     ax, 7305h           ; Extended Absolute Read/Write
        mov     cx, 0FFFFh          ; indicate we're using a DISKIO structure
        mov     dl, bDrive          ; 0-based drive number
        inc     dx                  ; 1-based drive number
        mov     bx, fWrite          ; are we reading or writing?
        mov     si, bx              ; indicate read/write
        lds     bx, lpDiskIo        ; ds:bx points to DISKIO structure
        int     21h
        jc      failure
    
        mov     fSuccess, TRUE      ; indicate success
    
    failure:

        pop     si
        pop     ds
    }

    return fSuccess;
}


WORD DISKUTILAPI GetDosVersion(void)
{
    WORD                    wVer;
    
    DPF("Getting DOS version");

    _asm
    {
        mov     ax, 3306h       ; Get MS-DOS Version
        int     21h
        
        mov     byte ptr [wVer], bh
        mov     byte ptr [wVer + 1], bl
    }
    
    DPF("DOS Version 0x%4.4X", wVer);

    return wVer;
}


BOOL DISKUTILAPI LockVolume(BYTE bDrive)
{
    BOOL                    fSuccess;
    
    DPF("Attempting to obtain level 0 lock on drive %u", bDrive);

    if(!IS_DOS7_KERNEL(g_wDosVer))
    {
        DPF("OS does not support locking");
        return TRUE;
    }
    
    _asm
    {
        mov     fSuccess, FALSE ; assume failure
        
        mov     ax, 440Dh       ; generic IOCTL
        mov     bl, bDrive      ; 0-based drive number
        inc     bl              ; 1-based drive number
        mov     bh, 0           ; lock level
        mov     cx, 084Ah       ; Lock Logical Volume
        mov     dx, 0           ; permissions
        int     21h
        jc      failure         ; CY set on error
        
        mov     fSuccess, TRUE  ; indicate success
        
    failure:
    
    }
    
    if(!fSuccess)
    {
        DPF("Lock failed");
    }
    
    return fSuccess;
}


BOOL DISKUTILAPI UnlockVolume(BYTE bDrive)
{
    BOOL                    fSuccess;
    
    DPF("Releasing level 0 lock on drive %u", bDrive);

    if(!IS_DOS7_KERNEL(g_wDosVer))
    {
        DPF("OS does not support locking");
        return TRUE;
    }
    
    _asm
    {
        mov     fSuccess, FALSE ; assume failure
        
        mov     ax, 440Dh       ; generic IOCTL
        mov     bl, bDrive      ; 0-based drive number
        inc     bl              ; 1-based drive number
        mov     bh, 0           ; lock level
        mov     cx, 086Ah       ; Unlock Logical Volume
        int     21h
        jc      failure         ; CY set on error
        
        mov     fSuccess, TRUE  ; indicate success
    
    failure:
    
    }
    
    if(!fSuccess)
    {
        DPF("Unlock failed");
    }

    return fSuccess;
}


BOOL DISKUTILAPI GetDriveMapping(BYTE bDrive, LPBYTE lpbHostDrive, LPBYTE lpbSequence)
{
    BOOL                    fSuccess;
    BOOL                    fCompressed;
    BYTE                    bHost;
    BYTE                    bSeq;
    
    DPF("Getting mapping for drive %u", bDrive);

    _asm
    {
        mov     fSuccess, FALSE     ; assume failure
        mov     fCompressed, FALSE  ; assume uncompressed
        
        mov     ax, 4A11h           ; DriveSpace multiplex
        mov     bx, 1               ; DSGetDriveMapping
        mov     dl, bDrive          ; 0-based drive number
        int     2Fh
        or      ax, ax              ; AX == 0 on success
        jnz     failure

        mov     fSuccess, TRUE
        
        test    bl, 80h             ; BL & 80h if drive is compressed
        jz      uncompressed
        
        mov     fCompressed, TRUE
        
        and     bl, 7Fh             ; mask off high bit
        mov     bHost, bl           ; host drive number stored in BL
        mov     bSeq, bh            ; CVF sequence number stored in BH
    
    uncompressed:
    failure:
    
    }
    
    if(!fSuccess)
    {
        DPF("Unable to get drive mapping");
        return FALSE;
    }
    
    if(!fCompressed)
    {
        DPF("Drive is not compressed");
        return FALSE;
    }
    
    GetDriveMapping(bHost, &bHost, &bSeq);
    
    if(lpbHostDrive)
    {
        *lpbHostDrive = bHost;
        DPF("Host drive:  %u", *lpbHostDrive);
    }
        
    if(lpbSequence)
    {
        *lpbSequence = bSeq;
        DPF("Sequence:  %u", *lpbSequence);
    }
    
    return TRUE;
}


BOOL DISKUTILAPI GetDPB(BYTE bDrive, LPDPB lpDpb)
{
    BOOL                    fSuccess;
    
    if(IS_FAT32_KERNEL(g_wDosVer))
    {
        fSuccess = Fat32GetDPB(bDrive, lpDpb);
    }
    else
    {
        fSuccess = Fat16GetDPB(bDrive, lpDpb);
    }

    if(fSuccess && lpDpb->wOldFATSize)
    {
        lpDpb->wExtFlags = 0;
        lpDpb->dwFirstSector = lpDpb->wOldFirstSector;
        lpDpb->dwMaxCluster = lpDpb->wOldMaxCluster;
        lpDpb->dwFATSize = lpDpb->wOldFATSize;
        lpDpb->dwRootCluster = 0;
        lpDpb->dwNextFree = lpDpb->wOldNextFree;
    }

    return fSuccess;
}


BOOL DISKUTILAPI Fat16GetDPB(BYTE bDrive, LPDPB lpDpb)
{
    LPDPB               lpDpbTemp;
    
    _asm
    {
        mov     word ptr [lpDpbTemp], 0 ; assume failure
        mov     word ptr [lpDpbTemp + 2], 0
        
        push    ds
        push    si
        
        mov     ax, 3200h       ; Get DPB
        mov     dl, bDrive      ; 0-based drive number
        inc     dl              ; 1-based drive number
        
        int     21h
        
        cmp     al, 0FFh        ; AL == FFh on error
        je      failure
        
        mov     word ptr [lpDpbTemp], bx   
        mov     word ptr [lpDpbTemp + 2], ds   ; pointer to DPB stored in ds:bx
    
    failure:
    
        pop     si
        pop     ds
    }
    
    if(!lpDpbTemp)
    {
        DPF("Unable to get DPB");
        return FALSE;
    }
    
    MemSet(lpDpb, 0, sizeof(*lpDpb));
    MemCopy(lpDpb, lpDpbTemp, sizeof(*lpDpbTemp));

    return TRUE;
}


BOOL DISKUTILAPI Fat32GetDPB(BYTE bDrive, LPDPB lpDpb)
{
    GETEXTDPB           gd;
    LPGETEXTDPB         lpgd;
    WORD                wSize;
    BOOL                fSuccess;
    
    lpgd = &gd;
    wSize = sizeof(gd);
    
    _asm
    {
        mov     fSuccess, FALSE ; assume failure
        
        push    di
        
        mov     ax, 7302h       ; Get Extended DPB
        mov     cx, wSize       ; buffer size
        mov     dl, bDrive      ; 0-based drive number
        inc     dl              ; 1-based drive number
        mov     di, word ptr [lpgd]
        mov     bx, word ptr [lpgd + 2]
        mov     es, bx          ; es:di points to GETEXTDPB structure
        int     21h
        jc      failure         ; CY indicates error
        
        mov     fSuccess, TRUE  ; indicate success
    
    failure:
    
        pop     di
    }
    
    if(!fSuccess)
    {
        DPF("Unable to get DPB");
        return FALSE;
    }
    
    MemSet(lpDpb, 0, sizeof(*lpDpb));
    MemCopy(lpDpb, &(gd.dpb), gd.wSize);
    
    return TRUE;
}


#pragma optimize("", on)