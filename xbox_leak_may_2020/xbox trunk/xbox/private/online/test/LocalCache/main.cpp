#include "xonp.h"
#include <stdlib.h>

#ifdef XONLINE_FEATURE_XBOX
extern class CXo * g_pXo;
#define GetXo() g_pXo
#endif

class CXoTest : public CXOnline
{
public:
    CXoTest(char * pszXbox = NULL) : CXOnline(pszXbox) {}
    void Test();
	void Test1();

};

const DWORD RECORD_SIZE = 10000;
typedef struct
{
    BYTE info[RECORD_SIZE];
}
TEST_RECORD;

typedef struct
{
    ULONGLONG qwUserID;
    BYTE     pInfo[33];
}TEST_INDEX_ENTRY1;

typedef struct
{
    ULONGLONG qwUserID;
    BYTE      pInfo[44];
}TEST_INDEX_ENTRY2;

typedef struct
{
    ULONGLONG qwUserID;
}LICENSING_INDEX_ENTRY;

typedef struct
{
    ULONGLONG qwUserID;
    DWORD     pInfo[2];
}BUDDYLIST_INDEX_ENTRY;

//////////////////////////////////////////////////////////////////////////////

int __cdecl main(int argc, char * argv[])
{
    char buffer[128];

//    sprintf(buffer, "%s@Nic/0", getenv("COMPUTERNAME") );
//    CXoTest * pXoTest = new CXoTest(buffer);
    
	CXoTest * pXoTest = new CXoTest;
    pXoTest->Test();
    
    delete pXoTest;
    return(0);
}

void CXoTest::Test()
{
    HANDLE                  hCache = NULL;
    XONLINETASK_HANDLE      hTask = NULL;
    DWORD                   i;

    TEST_INDEX_ENTRY2       indexData;
    TEST_RECORD             record;
    XONLC_CONTEXT           context;

    HRESULT                 hr;
    XONLC_TYPE              cacheType = XONLC_TYPE_TEST2;

    HANDLE                  hWorkEvent = NULL;
    DWORD                   dwReason = 0;
    BOOL                    bWait = TRUE;

    hr = XOnlineStartup(NULL);
    if (FAILED(hr))
    {
        goto Error;
    }

/*    hr = GetXo()->CacheFileDelete(XONLC_TYPE_TEST1);
    if(FAILED(hr))
    {
        goto Error;
    }
*/

    hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    //
    // Open the cache
    //
    hr = GetXo()->CacheOpen(cacheType,
                          NULL,
                          hWorkEvent,
                          &hCache,
                          &hTask 
                          );
    if(FAILED(hr))
    {
        TraceSz1(Warning, "Open FAILED on cache type %d\n" , cacheType);
        goto Error;
    }

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
                    TraceSz1(Warning, "Open FAILED on cache type %d\n" , cacheType);
                    goto Error;
                }
                else
                {
                    // hCache contains a valid cache handle
                    TraceSz1(Warning, "Opened cache type %d\n" , cacheType);
                }
            }
        }
        else
        {
            //Do something
        }
    }

    //
    // Update the cache
    //
    for( i = 1 ; i <= 15 ; i++ )
    {
        indexData.qwUserID =  i;
        memset(indexData.pInfo , i , sizeof(indexData.pInfo));
        memset(&record, i , sizeof(TEST_RECORD));

        if(0 == i % 5)
        {
            for( DWORD j = 0; j < RECORD_SIZE ; j++ )
            {
                record.info[j] = (BYTE)(j % 256);
            }       
        }

        hr = GetXo()->CacheUpdate ( hCache , hTask, (PBYTE)&indexData , (PBYTE)&record, FALSE);
        if(FAILED(hr))
        {
			TraceSz1(Warning, "Update FAILED on key = %x\n" , indexData.qwUserID);
            goto Error;
        }

        bWait = TRUE;

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
                        TraceSz1(Warning, "Update FAILED on key = %x\n" , indexData.qwUserID);
                        goto Error;
                    }
                    else
                    {
                        TraceSz1(Warning, "Updated key = %x\n" , indexData.qwUserID);
                    }
                }
            }
            else
            {
                //Do something
            }
        }
    }


	//
    // Flush the index
    //
	hr = GetXo()->CacheIndexFlush( hCache , hTask);
        if(FAILED(hr))
        {
			TraceSz1(Warning, "Flush FAILED hr = %x\n" , hr);
            goto Error;
        }

        bWait = TRUE;

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
                        	TraceSz1(Warning, "Flush FAILED hr = %x\n" , hr);
                        goto Error;
                    }
                    else
                    {
                        	TraceSz1(Warning, "Flush SUCCEEDED hr = %x\n" , hr);
                    }
                }
            }
            else
            {
                //Do something
            }
        }


/*
    //
    // Update the cache
    //
    indexData.qwUserID = 2;
    memset(indexData.pInfo , 50 , sizeof(indexData.pInfo));
    memset(&record, 171 , sizeof(TEST_RECORD));

    hr = GetXo()->CacheUpdate ( hCache , hTask, (PBYTE)&indexData , (PBYTE)&record);
    if(FAILED(hr))
    {
        DebugPrint("Update FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
        goto Error;
    }

    bWait = TRUE;

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
                    DebugPrint("Update FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
                    goto Error;
                }
                else
                {
                    DebugPrint("Updated key = %x\n" , indexData.qwUserID);
                }
            }
        }
        else
        {
            // Do something ...
        }
    }
*/

/*
    //
    // Lookup & Retrieve
    //
    memset(&context , 0 , sizeof(XONLC_CONTEXT));

    indexData.qwUserID = 2;
    memset(indexData.pInfo , 0 , sizeof(indexData.pInfo));
    memset(&record , 0 , sizeof(TEST_RECORD));

    context.pbIndexData = (PBYTE)(&indexData);
    
    hr = GetXo()->CacheLookup(hCache , &context);

    if( FAILED( hr ) )
    {
        DebugPrint("Lookup FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
        goto Error;
    }

    DebugPrint("Lookup returned bValidDataInCache = %d bCallRetrieveNext = %d\n" , context.bValidDataInCache , context.bCallRetrieveNext);

    if( context.bValidDataInCache )
    {
        if( context.bCallRetrieveNext )
        {
            context.pbRecordBuffer = (PBYTE)(&record);
            context.dwRecordBufferSize = sizeof(TEST_RECORD);

            hr = GetXo()->CacheRetrieve( hCache, hTask, &context);

            if(FAILED(hr))
            {
                DebugPrint("Retrieve FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
                goto Error;
            }

            bWait = TRUE;

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
                            DebugPrint("Retrieve FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
                            goto Error;
                        }
                        else
                        {
                            DebugPrint("Retrieved key = %x\n" , indexData.qwUserID);
                        }
                    }
                }
                else
                {
                    // Do something ...
                }
            }
        }
    }
*/

/*  //
    // Delete from cache
    //
    memset(&context , 0 , sizeof(XONLC_CONTEXT));
    indexData.qwUserID = 1050;
    //memset(indexData.pInfo , 10 , sizeof(indexData.pInfo));
    memset(&record , 0 , sizeof(TEST_RECORD));

    context.pbIndexData = (PBYTE)(&indexData);
    context.pbRecordBuffer = (PBYTE)(&record);
    context.dwRecordBufferSize = sizeof(TEST_RECORD);

    hr = GetXo()->CacheDelete(hCache, &context);

    if( FAILED( hr ) )
    {
        DebugPrint("Delete FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
        goto Error;
    }
*/

    for( i = 15 ; i > 0 ; i--)
    {
        //
        // Lookup & Retrieve
        //
        memset(&context , 0 , sizeof(XONLC_CONTEXT));

        indexData.qwUserID =  i;
        memset(indexData.pInfo , 0 , sizeof(indexData.pInfo));
        memset(&record, i , sizeof(TEST_RECORD));

        context.pbIndexData = (PBYTE)(&indexData);
        context.pbRecordBuffer = (PBYTE)(&record);
        context.dwRecordBufferSize = sizeof(TEST_RECORD);
        
        hr = GetXo()->CacheLookup(hCache , &context);

        if( FAILED( hr ) )
        {
            TraceSz2(Warning, "Lookup FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
            goto Error;
        }

        TraceSz3(Warning, "Lookup on key %x returned bValidDataInCache = %d bCallRetrieveNext = %d\n" ,
                 indexData.qwUserID , context.bValidDataInCache , context.bCallRetrieveNext);

        if( context.bValidDataInCache )
        {
            if( context.bCallRetrieveNext )
            {
                hr = GetXo()->CacheRetrieve( hCache, hTask, &context);

                if(FAILED(hr))
                {
                    TraceSz2(Warning, "Retrieve FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
                    goto Error;
                }

                bWait = TRUE;

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
                                TraceSz2(Warning, "Retrieve FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
                                goto Close;
                            }
                            else
                            {
                                TraceSz1(Warning, "Retrieved key = %x\n" , indexData.qwUserID);
                            }
                        }
                    }
                    else
                    {
                        // Do something ...
                    }
                }
            }
        }
    }


    for( i = 1 ; i <= 5 ; i++ )
    {   
        //
        // Delete from cache
        //
        memset(&context , 0 , sizeof(XONLC_CONTEXT));
        indexData.qwUserID = i;

        context.pbIndexData = (PBYTE)(&indexData);

        hr = GetXo()->CacheDelete(hCache, &context);

        if( FAILED( hr ) )
        {
            TraceSz2(Warning, "Delete FAILED on key = %x, hr = %x\n" , indexData.qwUserID , hr);
            goto Error;
        }
    }

Close:
    //
    // Close the cache
    //
    hr = GetXo()->CacheClose( hCache, hTask );
    if(FAILED(hr))
    {
        TraceSz1(Warning, "Close FAILED on cache type %d\n" , cacheType);
        goto Error;
    }

    bWait = TRUE;

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
                    TraceSz1(Warning, "Close FAILED on cache type %d\n" , cacheType);
                    goto Error;
                }
                else
                {
                    TraceSz1(Warning, "Closed cache type %d\n" , cacheType);
                }
            }
        }
        else
        {
            // Do something ...
        }
    }

    
Error:
    if( NULL != hTask )
    {
        // Close the task handle
        XOnlineTaskClose(hTask);
        hTask = NULL;
    } 

    XOnlineCleanup();

    TraceSz1(Warning, "Finished! hr = %x\n" , hr);
}

void CXoTest::Test1()
{

    HRESULT                 hr;

    hr = XOnlineStartup(NULL);
    if (FAILED(hr))
    {
        goto Error;
    }

    XOnlineCleanup();

Error:

    TraceSz1(Warning, "Finished! hr = %x\n" , hr);
}
