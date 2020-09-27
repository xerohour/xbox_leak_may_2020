/*
 *
 * isoemu.cpp
 *
 * Emulate from ISO files
 *
 */

#include "precomp.h"

class CISO : public CEmulationFilesystem
{
public:
    virtual void MountUnmount(BOOL);
    virtual void GetPhysicalGeometry(PHYSICAL_GEOMETRY *, BOOL);
    virtual int NumberOfPlaceholders(int Layer);
    virtual void GetPlaceholderInformation(int Layer,
        PLACEHOLDER *Placeholders);
    virtual ULONG GetSectorData(ULONG lsn, ULONG cblk, PUCHAR rgbData);

    HANDLE m_h;
    DWORD m_cblk;
};

CISO g_iso;
HANDLE g_hevtUnmount;

void CISO::MountUnmount(BOOL fMount)
{
    if(!fMount)
        SetEvent(g_hevtUnmount);
}

void CISO::GetPhysicalGeometry(PHYSICAL_GEOMETRY *pgeom, BOOL)
{
    memset(pgeom, 0, sizeof *pgeom);
    pgeom->Layer0StartPSN = 0x30000;
    pgeom->Layer0SectorCount = m_cblk;
}

int CISO::NumberOfPlaceholders(int iLayer)
{
    return 0;
}

void CISO::GetPlaceholderInformation(int iLayer, PLACEHOLDER *Placeholders)
{
}

ULONG CISO::GetSectorData(ULONG lsn, ULONG cblk, PUCHAR rgbData)
{
    LARGE_INTEGER li;
    DWORD cb;

    if(lsn < m_cblk && (lsn + cblk) <= m_cblk) {
        li.QuadPart = lsn * 2048;
        if(!SetFilePointerEx(m_h, li, NULL, FILE_BEGIN))
            return EMU_ERR_UNEXPECTED_ERROR;
        if(!ReadFile(m_h, rgbData, 2048 * cblk, &cb, NULL) || cb != 2048 * cblk)
            return EMU_ERR_UNEXPECTED_ERROR;
    } else
        return EMU_ERR_INVALID_LSN;

    return EMU_ERR_SUCCESS;
}

int __cdecl main(int argc, char **argv)
{
    LARGE_INTEGER li;
    HANDLE h;
    DWORD dw;

    if(argc < 2) {
        fprintf(stderr, "usage: isoemu iso.iso\n");
        return 1;
    }

    h = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        0, NULL);
    if(INVALID_HANDLE_VALUE == h || !GetFileSizeEx(h, &li)) {
        fprintf(stderr, "error opening %s\n", argv[1]);
        return 1;
    }

    g_hevtUnmount = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_iso.m_h = h;
    g_iso.m_cblk = (ULONG)(li.QuadPart / 2048);
    StartEmulating();
    EmulationOpenTray(TRUE);
    EmulationCloseTray(&g_iso, TRUE);
    WaitForSingleObject(g_hevtUnmount, INFINITE);
    EmulationCloseTray(NULL, TRUE);
    StopEmulating();
    return 0;
}
