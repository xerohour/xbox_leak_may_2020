// NetDebug.cpp : Implementation of CNetDebug
#include "stdafx.h"
#include "XbDebug.h"
#include "NetDebug.h"
#include <comdef.h>
#include <iostream.h>



PDMN_SESSION DmSession = NULL;
BOOL fNotificationOpened = FALSE;
BOOL fConnectionOpened = FALSE;
INT EVENT_MESSAGE = 9999;

///////////////////////
// put linked list declaration here


/*
 * AnsiToUnicode converts the ANSI string pszA to a Unicode string
 * and returns the Unicode string through ppszW. Space for the
 * the converted string is allocated by AnsiToUnicode.
 */ 

HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW)
{

    ULONG cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return NOERROR;
    }

    // Determine number of wide characters to be allocated for the
    // Unicode string.
    cCharacters =  strlen(pszA)+1;

    // Use of the OLE allocator is required if the resultant Unicode
    // string will be passed to another COM component and if that
    // component will free it. Otherwise you can use your own allocator.
    *ppszW = (LPOLESTR) CoTaskMemAlloc(cCharacters*2);
    if (NULL == *ppszW)
        return E_OUTOFMEMORY;

    // Covert to Unicode.
    if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
                  *ppszW, cCharacters))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszW);
        *ppszW = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }

    return NOERROR;
}




/*
 * UnicodeToAnsi converts the Unicode string pszW to an ANSI string
 * and returns the ANSI string through ppszA. Space for the
 * the converted string is allocated by UnicodeToAnsi.
 */ 

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA)
{

    ULONG cbAnsi, cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (pszW == NULL)
    {
        *ppszA = NULL;
        return NOERROR;
    }

    cCharacters = wcslen(pszW)+1;
    // Determine number of bytes to be allocated for ANSI string. An
    // ANSI string can have at most 2 bytes per character (for Double
    // Byte Character Strings.)
    cbAnsi = cCharacters*2;

    // Use of the OLE allocator is not required because the resultant
    // ANSI  string will never be passed to another COM component. You
    // can use your own allocator.
    *ppszA = (LPSTR) CoTaskMemAlloc(cbAnsi);
    if (NULL == *ppszA)
        return E_OUTOFMEMORY;

    // Convert to ANSI.
    if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA,
                  cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszA);
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }
    return NOERROR;

} 





struct debugline
{
	BSTR data;
	debugline *next;

};




typedef debugline* debuglinePtr;

//declare the head of the list as global
debuglinePtr LIST_HEAD;





void tail_insert(debuglinePtr& head, BSTR the_data)
{
	//This function will add a debug line to the head of the linked list
	debuglinePtr temp_ptr;
	temp_ptr = head;
	
	while (temp_ptr->next != NULL)
	{
		temp_ptr=temp_ptr->next;
	}

	debuglinePtr newline;
	newline = new debugline;

	newline->data = the_data;
	newline->next = NULL;

	temp_ptr->next = newline;
	
}




void head_insert(debuglinePtr& head, BSTR the_data)
{
	//This function will add a debug line to the head of the linked list
	
	debuglinePtr temp_ptr;
	temp_ptr = new debugline;
	if (NULL == temp_ptr)
	{
		//couldn't create the new debugline struct
		exit(1);
	}

	temp_ptr->data = the_data;

	temp_ptr->next= head;
	head = temp_ptr;
}




BSTR output_lines(debuglinePtr& head)
{
	// this function get all the data in the linked list and output it as one BSTR
	_bstr_t Full_String = head->data;
	debuglinePtr temp_ptr;
	temp_ptr = head;
	while (head->next != NULL)
	{
		_bstr_t tmpbstr( temp_ptr->data, false );
		Full_String += tmpbstr;
		temp_ptr = temp_ptr->next;	
	}
	

	return Full_String;
}




BSTR output_lines(debuglinePtr& head, int begin, int end)
{
	// this function will erase all the data in the linked list and reset the head
	_bstr_t Full_String;
	if (begin <= end)
	{
		debuglinePtr temp_ptr = head;
		// Traversing the list
		for (int i = 1; (begin > i) && (temp_ptr != NULL); i++)
		{
			temp_ptr = temp_ptr->next;
		}
		
		// Start deleting
		while (i <= end && temp_ptr != NULL)
		{
			_bstr_t tmpbstr( temp_ptr->data, false );
			Full_String += tmpbstr;
			temp_ptr = temp_ptr->next;
			i++;
		}
	}
	else
	{
		// you can put error msg here if you wish.
	}
	return (LPWSTR)Full_String;
}




void clear(debuglinePtr& head)
{
	// this function will erase all the data in the linked list and reset the head
	debuglinePtr temp_ptr;
	while (head->next != NULL)
	{
		temp_ptr = head;
		head = head->next;
		delete temp_ptr;
	}
}





void clear(debuglinePtr& head, int begin, int end)
{
	// this function will erase all the data in the linked list and reset the head
	if (begin <= end)
	{
		debuglinePtr temp_ptr = head;
		debuglinePtr temp_end = NULL;
		// Traversing the list
		for (int i = 1; (begin > i) && (temp_ptr != NULL); i++)
		{
			temp_end = temp_ptr;
			temp_ptr = temp_ptr->next;
		}
		
		// Start deleting
		while (i <= end && temp_ptr != NULL)
		{
			temp_end->next = temp_ptr->next;
			delete temp_ptr;
			temp_ptr = temp_end->next;
			i++;
		}
	}
	else
	{
		// you can put error msg here if you wish.
	}

}




///////////////////////////////////////////////////////////////////////////////
///  Sak's code

DWORD WINAPI NotificationProc( ULONG Notification, DWORD Parameter )
/*++

Routine Description:

    This is a callback routine registered by DmNotify.  It is called by XboxDbg
    for various types of notification event.

Arguments:

    Notification - Type of notification, DM_DEBUGSTR for example

    Parameter - Optional parameter based on notification type

Return Value:

    Always zero

--*/
{
	PDMN_DEBUGSTR p = NULL;
	p = (PDMN_DEBUGSTR)Parameter;

    //
    // Process only if notification type is DM_DEBUGSTR and there is
    // a valid string length
    //

    if ( Notification == DM_DEBUGSTR && p->Length ) {
        //Put into linked list here
		/*
		WCHAR* psz = new WCHAR[p->Length + 1];
		psz[p->Length] = 0;
		MultiByteToWideChar(CP_ACP, 0, p->String, p->Length, psz, p->Length);
		BSTR bstrMsg = SysAllocString(psz);
		cout<<*psz<<endl;
		delete[] psz;
		tail_insert(LIST_HEAD,bstrMsg);
		*/

		//Send Event Here
		SendMessage(HWND_BROADCAST,EVENT_MESSAGE,(LPARAM)"AUTO",(LPARAM)p->String);



    }

	
    return 0;
}






VOID TranslateError( HRESULT hr )

/*
Routine Description:

    This routine translates HRESULT returned from XboxDbg APIs into string and
    output to standard error

Auguments:

    hr - HRESULT returned from various XboxDbg APIs

Return Value:

    None
*/

{
    CHAR Buffer[512];

    DmTranslateErrorA( hr, Buffer, sizeof(Buffer) );
    cout<<"TRANSLATE ERROR - "<<Buffer<<endl;
}





BOOL WINAPI ConsoleCtrlHandler( DWORD CtrlType )
/*++

Routine Description:

    A HandlerRoutine function is an application-defined function used with
    the SetConsoleCtrlHandler function.  A console process uses this function
    to handle control signals received by the process

Arguments:

    CtrlType - Indicates the type of control signal received by the handler.

Return Value:

    Return FALSE to terminate the process

--*/
{
    switch ( CtrlType ) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:

        fprintf( stdout, "Terminating...\n" );

        //
        // Close notification and connection if needed
        //

        if ( fNotificationOpened ) {
            DmCloseNotificationSession( DmSession );
        }

        if ( fConnectionOpened ) {
            DmUseSharedConnection( FALSE );
        }

        return FALSE;
    }

    return TRUE;
}








/////////////////////////////////////////////////////////////////////////////
// CNetDebug


STDMETHODIMP CNetDebug::Connect(BSTR xboxName)
{
    HRESULT hr = S_OK;
	
	//
	//  Hook Ctrl+C handle so that we can do cleanup work
	//

	SetConsoleCtrlHandler( ConsoleCtrlHandler, TRUE );

    //
    // Open a shared debug connection with xbdm
    //

    hr = DmUseSharedConnection( TRUE );

    if ( FAILED(hr) )
	{
        TranslateError( hr );
    	return hr;
    }
	else
	{
        fConnectionOpened = TRUE;
    }

	LPSTR Ansi_XboxName = NULL;
	UnicodeToAnsi(xboxName,&Ansi_XboxName);
	
	cout<<"Connect to - "<<Ansi_XboxName<<endl;
	
	hr = DmSetXboxName(Ansi_XboxName);

	if ( FAILED(hr) )
	{
        TranslateError( hr );
    	DmUseSharedConnection( FALSE );
		return 1;
    }

	return hr;
}



STDMETHODIMP CNetDebug::Start()
{

	cout<<"START"<<endl;

	HRESULT hr = S_OK;


	LIST_HEAD = new debugline;
	LIST_HEAD->data = NULL;
	LIST_HEAD->next = NULL;


	hr = DmOpenNotificationSession( DM_PERSISTENT, &DmSession );

	
    if ( SUCCEEDED(hr) ) 
	{
        fNotificationOpened = TRUE;
        
		hr = DmNotify( DmSession, DM_DEBUGSTR, NotificationProc );
		if ( SUCCEEDED(hr) )
		{
			cout<<"+++++++++++++++++++++++++++++++++++Started Listening"<<endl;
		}
		else
		{
			TranslateError( hr );
		}
	}
	else
	{
		TranslateError( hr );
	}
	
	return hr;

}


STDMETHODIMP CNetDebug::Stop()
{
	cout<<"Stop"<<endl;
	return DmCloseNotificationSession( DmSession );
}

STDMETHODIMP CNetDebug::GetLine(unsigned int lineNum, short operation, BSTR *pVal)
{
	cout<<"GetLine"<<endl;


	return S_OK;
}

STDMETHODIMP CNetDebug::GetAllLines(BOOL clear, BSTR *pVal)
{
	cout<<"***********************************GetAllLines"<<endl;
	*pVal = output_lines(LIST_HEAD);
	return S_OK;
}

STDMETHODIMP CNetDebug::get_ParseTag(BSTR *pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CNetDebug::put_ParseTag(BSTR newVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CNetDebug::Disconnect()
{
	cout<<"Disconnect"<<endl;
	clear(LIST_HEAD);
	return DmUseSharedConnection( FALSE );

}

STDMETHODIMP CNetDebug::ClearLines()
{
	cout<<"ClearLines"<<endl;
	
	clear(LIST_HEAD);

	return S_OK;
}











