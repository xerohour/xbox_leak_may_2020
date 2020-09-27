/*----------------------------------------------------------------------------*/
/*
 * miscR0.c
 *
 *      misc ring0 functions
 *	
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

#define WANTVXDWRAPS 
#include <basedef.h>
#include <vmm.h>
#include <vxdldr.h>
//#include <debug.h>
#include <vxdwraps.h>
#include <vwin32.h>

#include "nvtypes.h"
#include "nvrmr0api.h"
#include "nvrmwin.h"
#include "nvos.h"

/*----------------------------------------------------------------------------*/
/*
 * pragma's to put the code and data into locked memory (XXX not sure if needed)
 */

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

/*----------------------------------------------------------------------------*/
/*
 * globals
 */

typedef DWORD (* LPDWFN)();
    
/*----------------------------------------------------------------------------*/
/*
 * getApiProcR0 - get the API Proc entry point for a given Vxd from Ring0
 */

LPDWFN getApiProcR0(PCHAR nameVxd)
{

    DWORD	    rc;
    PVMMDDB	    pddb;
    PDEVICEINFO	pdevinfo = NULL;
    
    rc = VXDLDR_LoadDevice( &pddb, &pdevinfo, nameVxd, TRUE); /* XXX what does TRUE do? */
                                                              /* XXX what's the return value */
    if ( (pdevinfo == NULL) || (pddb == NULL) ) {
        return ( NULL );
    }
    else {
        return ( (LPDWFN) pddb->DDB_PM_API_Proc );
    }

}

