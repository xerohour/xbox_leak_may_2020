/*----------------------------------------------------------------------------*/
/*
 * nvi2c.h
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

DWORD nvI2CInit ( ULONG ourRoot, ULONG ourDev, ULONG ourChannel, NvChannel* pNV, UINT subchan, UINT baseInst );
DWORD nvI2CUninit();
DWORD nvI2CWriteByte ( UCHAR chipAddr, UCHAR regAddr, UCHAR regData );
DWORD nvI2CReadByte ( UCHAR chipAddr, UCHAR regAddr, UCHAR* pData );
DWORD nvI2CWriteWord ( UCHAR chipAddr, UCHAR regAddr, USHORT regData );
DWORD nvI2CReadWord ( UCHAR chipAddr, UCHAR regAddr, USHORT* pData );
DWORD nvI2CWriteBuf ( UCHAR* pBuf, UINT bufSize );
DWORD nvI2CReadBuf ( UCHAR* pBuf, UINT bufSize, UCHAR* pData, UINT readBufSize );
DWORD nvI2CWrite(UCHAR chipAddr, DWORD subAddrLen, UCHAR* subAddr, DWORD dataLen, UCHAR* data);
DWORD nvI2CRead(UCHAR chipAddr, DWORD subAddrLen, UCHAR* subAddr, DWORD dataLen, UCHAR* data);

/* backdoors (hopefully temporary) */
DWORD nvI2CSetControlObj ( void* pObj );
VOID* nvI2CGetControlObj ();

#ifdef __cplusplus
}
#endif

