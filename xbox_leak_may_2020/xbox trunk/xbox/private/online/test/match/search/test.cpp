/***************************************************************************
 *
 *  Copyright (c) Microsoft Corporation.  All rights reserved
 *
 ****************************************************************************/

#include <xtl.h>
#include <xdbg.h>
#include <xonline.h>
#include <stdio.h>

typedef struct
{
	DWORD dwAttributeID;
	VOID *pvValue;
}
ATTRIBUTE_TEST, *PATTRIBUTE_TEST;

#define INT_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000001
#define INT_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000002
#define INT_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000003

#define STR_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING  | 0x00000001
#define STR_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING  | 0x00000002
#define STR_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING  | 0x00000003

#define BLB_ID1 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB    | 0x00000001
#define BLB_ID2 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB    | 0x00000002
#define BLB_ID3 X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB    | 0x00000003

#define INT1 111222
#define INT2 333444555
#define INT3 666

#define STRING1 "AAAAABBBBB"
#define STRING2 "CCCCCDDDDDEEEEE"
#define STRING3 "FFFFF"

#define BLOB1 "ZZZZZYYYYY"
#define BLOB2 "XXXXXWWWWWVVVVV"
#define BLOB3 "UUUUU"

void __cdecl main()
{
	HRESULT hr = S_OK;
	XONLINETASK_HANDLE hSearch;
	INT i = 0;
	ATTRIBUTE_TEST AttributeTest[4];
	LARGE_INTEGER puid;
	DWORD dwError = 0;
    WSADATA				WsaData;
    HANDLE hEvent = NULL;
    DWORD dwWorkFlags = 0;
	PXMATCH_SEARCHRESULT pSearchResult = NULL;

	puid.LowPart = 0;
	puid.HighPart = 0;

	AttributeTest[0].dwAttributeID = INT_ID1; AttributeTest[0].pvValue = (VOID *) INT1;
	AttributeTest[4].dwAttributeID = STR_ID1; AttributeTest[4].pvValue = (VOID *) STRING3;
	AttributeTest[2].dwAttributeID = BLB_ID1; AttributeTest[2].pvValue = (VOID *) BLOB1;
	AttributeTest[3].dwAttributeID = STR_ID2; AttributeTest[3].pvValue = (VOID *) STRING2;

	__asm int 3;

    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL)
        __asm int 3;

    dwError = XnetInitialize(NULL, TRUE);
    if (dwError != NO_ERROR)
        __asm int 3;

    if (WSAStartup(0x0101, &WsaData) == SOCKET_ERROR)
        __asm int 3;
    
	hr = XOnlineMatchSearchCreate( 0, 10, 5000, 130, hEvent, &hSearch );
	if (FAILED(hr))
		__asm int 3;
/*
	for ( i = 0; i < 4; i += 1 )
	{
		switch (AttributeTest[i].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSearchAppendInt(hSearch, (INT) AttributeTest[i].pvValue );
				if (FAILED(hr))
					__asm int 3;
                break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSearchAppendString( hSearch, (LPSTR) AttributeTest[i].pvValue );
				if (FAILED(hr))
					__asm int 3;
                break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSearchAppendBlob( hSearch, strlen((LPSTR)AttributeTest[i].pvValue), AttributeTest[i].pvValue );
				if (FAILED(hr))
					__asm int 3;
                break;
			}
		}
	}
*/
/*
	hr = XOnlineMatchSearchAppendInt( hSearch, 3 );
	if (FAILED(hr))
	    __asm int 3;
*/
	hr = XOnlineMatchSearchSend(hSearch);
	if (FAILED(hr))
	    __asm int 3;

	while (1)
	{
        hr = XOnlineTaskContinue(hSearch, 0, &dwWorkFlags);

        if (XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
        {
            // Close the handle

            break;
        }
	}

	hr = XMatchGetFirstSearchResult(hSearch, &pSearchResult);
	if (FAILED(hr))
	    __asm int 3;

	while (hr != XMATCH_S_NO_MORE_RESULTS)
	{
		hr = XMatchGetNextSearchResult(hSearch, &pSearchResult);
		if (FAILED(hr))
		    __asm int 3;
	}

        XOnlineTaskCloseHandle(hSearch);
	CloseHandle(hEvent);

    WSACleanup();
    XnetCleanup();
    
	__asm int 3;
}

