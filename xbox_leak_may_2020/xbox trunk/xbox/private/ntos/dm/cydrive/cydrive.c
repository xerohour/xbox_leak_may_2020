/*
 *
 * cydrive.c
 *
 * Map C and Y drives for the debugger
 *
 */

#include "ntos.h"
#include "dm.h"

const OCHAR szADrive[] = OTEXT("\\??\\A:");
const OCHAR szCdPath[] = OTEXT("\\Device\\Cdrom0");
const OCHAR szFullPath[] = OTEXT("\\Device\\Harddisk0\\Partition1");

void DxtEntry(ULONG *pfUnload)
{
    OBJECT_STRING ostA, ostCD;
    DMDRIVE dr;

    dr.Path = ExAllocatePool(sizeof szFullPath);
    if(dr.Path) {
        dr.Drive = 'C';
        memcpy(dr.Path, szFullPath, sizeof szFullPath);
        DmTell(DMTELL_MAPDRIVE, &dr);
    }
    dr.Path = ExAllocatePool(sizeof szFullPath);
    if(dr.Path) {
        dr.Drive = 'Y';
        memcpy(dr.Path, szFullPath, sizeof szFullPath);
        dr.Path[sizeof szFullPath - 2] = '2';
        DmTell(DMTELL_MAPDRIVE, &dr);
    }

    RtlInitObjectString(&ostA, szADrive);
    RtlInitObjectString(&ostCD, szCdPath);
    IoCreateSymbolicLink(&ostA, &ostCD);
    *pfUnload = TRUE;
}
