extern "C" 
{
#include <nvarm.h>
#include <aos.h>
#include <nv32.h>
#include <AHal.h>
#include <AudioHw.h>
}
#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalVoiceProc.h"
#include "CHalMCP1.h"

RM_STATUS InitAudioHal(U008 uRevID, HWINFO_COMMON_FIELDS *pDev)
{
	switch(uRevID)
	{
	case REV_MCP1:
		pDev->pHalContext = (VOID *) new CHalMCP1(uRevID, pDev);
		break;

	default:
		pDev->pHalContext = NULL;
		break;
	}

	return (pDev->pHalContext)?RM_OK:RM_ERROR;
}

VOID DestroyAudioHal(HWINFO_COMMON_FIELDS *pDev)
{
	delete ((CHalBase *)pDev->pHalContext);
}

RM_STATUS AllocDevice(HWINFO_COMMON_FIELDS *pDev)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->Allocate();
}

RM_STATUS FreeDevice(HWINFO_COMMON_FIELDS *pDev)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->Free();
}

RM_STATUS DeviceAddClient(HWINFO_COMMON_FIELDS *pDev, VOID *pParam, U008 *pClientRef)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->AddClient(pParam, pClientRef);
}

RM_STATUS DeviceRemoveClient(HWINFO_COMMON_FIELDS *pDev, U008 uClientRef)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->DeleteClient(uClientRef);
}

RM_STATUS AllocObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	APU_OBJECT	*pObjAllocParams = (APU_OBJECT *)pParam;
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->AllocateObject(pObjAllocParams->uObjectCode, (VOID *)pObjAllocParams->pParam);
}

RM_STATUS FreeObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	APU_OBJECT	*pObjFreeParams = (APU_OBJECT *)pParam;
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->FreeObject(pObjFreeParams->uObjectCode, (VOID *)pObjFreeParams->pParam);
}

RM_STATUS AllocContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->AllocateContextDma(pParam);
}

RM_STATUS FreeContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->FreeContextDma(pParam);
}

RM_STATUS InterruptService(HWINFO_COMMON_FIELDS *pDev)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->InterruptService();
}

// the following code is to make the driver compile in C++ 
void* _cdecl operator new(unsigned int n)
{
	void *p;
    aosAllocateMem((U032) n, ALIGN_DONT_CARE, &p, NULL);
    return p;
}

void _cdecl operator delete(void* p)
{
	if (p) 
		aosFreeMem(p);
}

extern "C" int _cdecl _purecall();

int _cdecl _purecall()
{
    // this better get printed!!
    aosDbgPrintString(DEBUGLEVEL_ERRORS + 5, "NVARM: _purecall - RUNTIME ERROR!  calling pure virtual function!!!\n");
	return 0;
}
