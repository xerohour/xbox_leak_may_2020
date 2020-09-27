/*----------------------------------------------------------------------------*/
/*
 * nvgpio.h
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

DWORD nvGPIOInit ( ULONG ourRoot, ULONG ourDev, ULONG ourChannel, NvChannel* pNV, UINT subchan );
DWORD nvGPIOUnnit();
DWORD nvGPIOWrite ( DWORD regAddr, BYTE regData );
DWORD nvGPIORead ( DWORD regAddr, BYTE* pData );
DWORD nvVIPWrite ( DWORD regAddr, DWORD regData, DWORD dataSize );
DWORD nvVIPRead ( DWORD regAddr, DWORD* pBuf, DWORD dataSize );

/* backdoors (hopefully temporary) */
DWORD nvRegWrite ( DWORD regAddr, UINT regData );
DWORD nvRegRead ( DWORD regAddr, UINT* pData );
DWORD nvGPIOSetControlObj ( void* pObj );
VOID* nvGPIOGetControlObj();

#ifdef __cplusplus
}
#endif

