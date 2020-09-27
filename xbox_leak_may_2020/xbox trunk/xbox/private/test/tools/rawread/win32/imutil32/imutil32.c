#define Not_VxD

#include <windows.h>
#include "..\debug.h"
#include "..\..\defs.h"
#include "..\..\diskutil.h"
#include "..\..\mem.h"

#define CARRY_FLAG_SET(_ptr) ((_ptr)->reg_Flags & 0x0001)

#define VWIN32_DIOC_DOS_IOCTL   1
#define VWIN32_DIOC_DOS_INT25   2
#define VWIN32_DIOC_DOS_INT26   3

typedef struct tagDIOC_REGISTERS
{
    DWORD   reg_EBX; 
    DWORD   reg_EDX; 
    DWORD   reg_ECX; 
    DWORD   reg_EAX; 
    DWORD   reg_EDI; 
    DWORD   reg_ESI; 
    DWORD   reg_Flags; 
} DIOC_REGISTERS, *LPDIOC_REGISTERS;

static WORD                 g_wDosVer = 0;
static HANDLE               g_hVWIN32 = NULL;


extern BOOL DISKUTILAPI InitDiskUtil16(void);
extern BOOL DISKUTILAPI FreeDiskUtil16(void);
extern BOOL DISKUTILAPI ResetDrive16(void);
extern BOOL DISKUTILAPI ReadSector16(BYTE, LPVOID, DWORD, WORD);
extern BOOL DISKUTILAPI WriteSector16(BYTE, LPVOID, DWORD, WORD);
extern WORD DISKUTILAPI GetDosVersion16(void);
extern BOOL DISKUTILAPI LockVolume16(BYTE);
extern BOOL DISKUTILAPI UnlockVolume16(BYTE);
extern BOOL DISKUTILAPI GetDriveMapping16(BYTE, LPBYTE, LPBYTE);
extern BOOL DISKUTILAPI GetDPB16(BYTE, LPDPB);


BOOL DeviceIoControlEx(HANDLE hDevice, DWORD dwIoControlCode, LPDIOC_REGISTERS lpReg, BOOL fCheckCarry)
{
    DWORD                   dwSize;
    BOOL                    fSuccess;

    fSuccess = DeviceIoControl(hDevice, dwIoControlCode, lpReg, sizeof(*lpReg), lpReg, sizeof(lpReg), &dwSize, NULL);

    if(!fSuccess)
    {
        return FALSE;
    }

    if(fCheckCarry && CARRY_FLAG_SET(lpReg))
    {
        SetLastError(0);
        return FALSE;
    }

    return TRUE;
}


void DISKUTILAPI InitRegisters(DIOC_REGISTERS *lpReg)
{
    MemSet(lpReg, 0, sizeof(DIOC_REGISTERS));
}


BOOL DISKUTILAPI InitDiskUtil32(void)
{
    if(!InitDiskUtil16())
    {
        return FALSE;
    }
    
    g_wDosVer = GetDosVersion32();
    g_hVWIN32 = CreateFile("\\\\.\\vwin32", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL);

    DPF("VWIN32.VXD device handle 0x%8.8lX", g_hVWIN32);
    
    if(!g_hVWIN32)
    {
        DPF("Unable to open VWIN32.VXD");
        return FALSE;
    }

    return TRUE;
}


BOOL DISKUTILAPI FreeDiskUtil32(void)
{
    CloseHandle(g_hVWIN32);

    g_wDosVer = 0;
    g_hVWIN32 = NULL;

    FreeDiskUtil16();

    return TRUE;
}


BOOL DISKUTILAPI ResetDrive32(void)
{
    return ResetDrive16();
}


BOOL DISKUTILAPI SectorIo32(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, WORD wCount, BOOL fWrite)
{
    DIOC_REGISTERS          reg;
    BOOL                    fSuccess;
    BYTE                    bTry;

    bTry = 0;

    do
    {
        if(bTry)
        {
            DPF("Retry %u", bTry);
        }

        InitRegisters(&reg);

        reg.reg_EAX = bDrive;
        reg.reg_EBX = (DWORD)lpvBuffer;
        reg.reg_ECX = wCount;
        reg.reg_EDX = dwSector;

        fSuccess = DeviceIoControlEx(g_hVWIN32, fWrite ? VWIN32_DIOC_DOS_INT26 : VWIN32_DIOC_DOS_INT25, &reg, TRUE);

        if(!fSuccess)
        {
            DPF("DeviceIoControl() returned error %lu", GetLastError());
        }
    }
    while(!fSuccess && ++bTry <= ABSRETRIES);

    return fSuccess;
}


BOOL DISKUTILAPI ReadSector32(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, WORD wCount)
{
    BOOL                    fSuccess;
    
    if(IS_FAT32_KERNEL(g_wDosVer))
    {
        fSuccess = ReadSector16(bDrive, lpvBuffer, dwSector, wCount);
    }
    else
    {
        fSuccess = SectorIo32(bDrive, lpvBuffer, dwSector, wCount, FALSE);
    }

    return fSuccess;
}


BOOL DISKUTILAPI WriteSector32(BYTE bDrive, LPVOID lpvBuffer, DWORD dwSector, WORD wCount)
{
    BOOL                    fSuccess;
    
    if(IS_FAT32_KERNEL(g_wDosVer))
    {
        fSuccess = WriteSector16(bDrive, lpvBuffer, dwSector, wCount);
    }
    else
    {
        fSuccess = SectorIo32(bDrive, lpvBuffer, dwSector, wCount, TRUE);
    }

    return fSuccess;
}


WORD DISKUTILAPI GetDosVersion32(void)
{
    return GetDosVersion16();
}


BOOL DISKUTILAPI LockVolume32(BYTE bDrive)
{
    DIOC_REGISTERS          reg;
    
    DPF("Attempting to obtain level 0 lock on drive %u", bDrive);

    if(!IS_DOS7_KERNEL(g_wDosVer))
    {
        DPF("OS does not support locking");
        return TRUE;
    }
    
    InitRegisters(&reg);

    reg.reg_EAX = 0x440D;
    reg.reg_EBX = bDrive + 1;
    reg.reg_ECX = 0x84A;
    
    if(!DeviceIoControlEx(g_hVWIN32, VWIN32_DIOC_DOS_IOCTL, &reg, TRUE))
    {
        DPF("DeviceIoControl() returned error %lu", GetLastError());
        return FALSE;
    }

    return TRUE;
}


BOOL DISKUTILAPI UnlockVolume32(BYTE bDrive)
{
    DIOC_REGISTERS          reg;
    
    DPF("Releasing level 0 lock on drive %u", bDrive);

    if(!IS_DOS7_KERNEL(g_wDosVer))
    {
        DPF("OS does not support locking");
        return TRUE;
    }
    
    InitRegisters(&reg);
    
    reg.reg_EAX = 0x440D;
    reg.reg_EBX = bDrive + 1;
    reg.reg_ECX = 0x86A;
    
    if(!DeviceIoControlEx(g_hVWIN32, VWIN32_DIOC_DOS_IOCTL, &reg, TRUE))
    {
        DPF("DeviceIoControl() returned error %lu", GetLastError());
        return FALSE;
    }

    return TRUE;
}


BOOL DISKUTILAPI GetDriveMapping32(BYTE bDrive, LPBYTE lpbHostDrive, LPBYTE lpbSequence)
{
    return GetDriveMapping16(bDrive, lpbHostDrive, lpbSequence);
}


BOOL DISKUTILAPI GetDPB32(BYTE bDrive, LPDPB lpdpb)
{
    return GetDPB16(bDrive, lpdpb);
}