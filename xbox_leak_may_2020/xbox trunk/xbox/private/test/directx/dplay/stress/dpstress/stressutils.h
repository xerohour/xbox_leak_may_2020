#ifndef __STRESSUTILS_H__
#define __STRESSUTILS_H__

namespace DPlayStressNamespace {

//==================================================================================
// Prototypes
//==================================================================================
BOOL   FindServerIP(HANDLE hLog, char *szServerName, IN_ADDR *pServerAddr);

// Random buffer allocate/verify functions for generating DPlay buffers
DWORD  GenerateRandomDataBuffer(BYTE **ppBuffer, DWORD dwMinSize, DWORD dwMaxSize);
BOOL   VerifyDataBuffer(LPVOID pBuffer, DWORD dwBufferSize);

// Generates a string representing an address
LPSTR GetAddressString(PDIRECTPLAY8ADDRESS pDP8Address);

// Work pump wrapper
BOOL PollPeerUntilEventSignalled(HANDLE hLog, PDIRECTPLAY8PEER pDP8Peer, DWORD dwMaxTime, HANDLE hEvent, LPDWORD pdwCallbackTime);

// Netsync communication functions
DWORD SendNetsyncHostKeepalive(HANDLE hNetsync);
DWORD SendNetsyncHostUpdate(HANDLE hNetsync, IN_ADDR *pLocalAddr);
BOOL IsNetsyncSessionOver(HANDLE hNetsync);
template< class T > void DoWorkForFixedInterval( T *pDPObject, DWORD dwInterval);


} // namespace DPlayStressNamespace

#endif