#include "xtl.h"
#include "xvchat.h"

#include <assert.h>

int __cdecl main(int argc, char * argv[])
{
	HRESULT hr = S_OK;
	HANDLE  hVChatMgr = NULL; 

	XInitDevices(NULL, NULL);

	HANDLE hEvtMicInput = CreateEvent(NULL, FALSE, FALSE, NULL);

	VOICE_CHAT_CONFIG cfg;
	memset(&cfg, 0, sizeof(VOICE_CHAT_CONFIG));

	hr = XOnlineVChatStartup(&cfg, &hVChatMgr);
	
	if( FAILED( hr ) )
    {
		goto Error;
    }

	

Error:
	if( NULL != hVChatMgr )
    {
		XOnlineVChatCleanup(hVChatMgr);
		hVChatMgr = NULL;
    }

    return(0);

}

