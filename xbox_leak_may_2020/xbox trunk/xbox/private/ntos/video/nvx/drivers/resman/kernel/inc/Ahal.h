#if !defined _AHALH_
#define _AHALH_

#if defined __cplusplus
extern "C"
{
#endif

// can be max 255
#define MAX_CLIENTS		            8

typedef struct
{
    PISRCALLBACK	pFn;
    INTR_MASK		Mask;
    VOID			*pContext;
} MCP1_CLIENT_INFO, *PMCP1_CLIENT_INFO;


RM_STATUS InitAudioHal(U008 uRevID, HWINFO_COMMON_FIELDS *pDev);
VOID DestroyAudioHal(HWINFO_COMMON_FIELDS *pDev);

RM_STATUS AllocDevice(HWINFO_COMMON_FIELDS *pDev);
RM_STATUS FreeDevice(HWINFO_COMMON_FIELDS *pDev);

RM_STATUS DeviceAddClient(HWINFO_COMMON_FIELDS *pDev, VOID *pParam, U008 *pClientRef);
RM_STATUS DeviceRemoveClient(HWINFO_COMMON_FIELDS *pDev, U008 uClientRef);

RM_STATUS AllocContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);
RM_STATUS FreeContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);

RM_STATUS AllocObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);
RM_STATUS FreeObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);

RM_STATUS InterruptService(HWINFO_COMMON_FIELDS *pDev);

#if defined __cplusplus
};
#endif

#endif