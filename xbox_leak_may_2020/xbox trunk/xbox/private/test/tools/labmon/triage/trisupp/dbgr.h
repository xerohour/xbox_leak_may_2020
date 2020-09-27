#ifndef _DBGR_
	#define _DBGR_ 

HRESULT ConnectEngine(CHAR *server, CHAR *srvpipename);
HRESULT DisconnectEngine(VOID);
//HRESULT NewSendMyCommand(const CHAR *theCommand, CHAR **RetBuffer, ULONG Output = DEBUG_OUTCTL_ALL_CLIENTS, ULONG ulTimeout = 30000);

ULONG DbgGetProcType(VOID);
BOOL IsThereNTSDPropt(CHAR *InBuff);


#endif