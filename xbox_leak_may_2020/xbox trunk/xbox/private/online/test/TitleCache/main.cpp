#include "xonp.h"

#ifdef XONLINE_FEATURE_XBOX
extern class CXo * g_pXo;
#define GetXo() g_pXo
#endif

class CXoTest : public CXOnline
{
public:

    void Test();

};


//////////////////////////////////////////////////////////////////////////////

void __cdecl main()
{
    CXoTest XoTest;
    XoTest.Test();
}


void CXoTest::Test()
{
    DWORD                   i;

    HRESULT                 hr = S_OK;
    HANDLE                  hWorkEvent = NULL;
    DWORD                   dwReason = 0;
    BOOL                    bWait = TRUE;

	XONLINETASK_HANDLE      hTask = NULL;

	XNetStartupParams 	xnsp = { sizeof(XNetStartupParams), 
								XNET_STARTUP_BYPASS_SECURITY };

    hr = XOnlineStartup(NULL);
    if (FAILED(hr))
    {
        goto Error;
    }

    hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		
    //_asm int 3;

    DWORD dwError = XNetStartup(&xnsp);
    if (dwError != NO_ERROR)
    {
	    //DebugPrint("Failed XNetStartup with %d\n", dwError);
        return;
    }

	WSADATA				WsaData;
    if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR)
    {
        //DebugPrint("Error %d returned by WSAStartup\n", GetLastError());
        return;
    }

	if(FAILED(hr))
    {
        TraceSz1(Warning, "Could not allocate task handle...hr = %x \n" , hr);
        goto Error;
    }


	hr = GetXo()->TitleCacheInit(hWorkEvent , &hTask);
                          
    if(FAILED(hr))
    {
        TraceSz1(Warning, "TitleCacheInit FAILED hr = %x \n" , hr);
        goto Error;
    }

	for( int i = 1 ; i < 10 ; i ++)
    {
		DWORD dwTitleID = i;
		XONLINE_TITLEDATA titleData;
		BOOL bFound;

		bWait = TRUE;

		bFound = GetXo()->TitleCacheRetrieve(hTask, dwTitleID, &titleData);

		if( bFound )
		{
			TraceSz1(Warning, "TitleCacheRetrieve Title ID = %d\n" , dwTitleID);
			TraceSz1(Warning, "TitleCacheRetrieve Title Name = %s\n" , titleData.name);
		}
		else
		{
			while (bWait)
			{
				dwReason = WaitForSingleObject(hWorkEvent,0);
				if( WAIT_OBJECT_0 == dwReason )
				{
					hr = XOnlineTaskContinue(hTask);

					if(XONLINETASK_S_RUNNING != hr)
					{
						bWait = FALSE;

						if (FAILED(hr))
						{
							// Handle error condition
							TraceSz1(Warning, "TitleCacheRetrieve FAILED hr = %x\n" , hr);
							goto Error;
						}
						else
						{
							TraceSz1(Warning, "TitleCacheRetrieve Title ID = %d\n" , dwTitleID);
							TraceSz1(Warning, "TitleCacheRetrieve Title Name = %s\n" , titleData.name);
						}
					}
				}
        
			}
		}

    }



    
Error:
    if( NULL != hTask )
    {
        // Close the task handle
        XOnlineTaskClose((XONLINETASK_HANDLE)hTask);
        hTask = NULL;
    }

    TraceSz1(Warning, "Finished! hr = %x\n" , hr);


}
