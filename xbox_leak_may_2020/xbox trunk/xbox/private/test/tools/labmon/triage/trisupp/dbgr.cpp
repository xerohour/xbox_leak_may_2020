#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "failure.h"
#include "dbgr.h"

//IMAGE_FILE_MACHINE_I386              0x014c
//IMAGE_FILE_MACHINE_IA64              0x0200
//IMAGE_FILE_MACHINE_ALPHA             0x0184
//IMAGE_FILE_MACHINE_ALPHA64           0x0284
/*
class DefOutputCallbacks :
    public IDebugOutputCallbacks
{
public:
    // IUnknown.
    STDMETHOD(QueryInterface)(
        THIS_
        IN REFIID InterfaceId,
        OUT PVOID* Interface
        );
    STDMETHOD_(ULONG, AddRef)(
        THIS
        );
    STDMETHOD_(ULONG, Release)(
        THIS
        );
};
class ConOutputCallbacks : public DefOutputCallbacks
{
public:
    // IDebugOutputCallbacks.
    STDMETHOD(Output)(
        THIS_
        IN ULONG Mask,
        IN PCSTR Text
        );
};

// My interface Pointers
IDebugClient* g_DbgClient = NULL;
IDebugControl* g_DbgControl = NULL;

// Output callback class object
ConOutputCallbacks* g_ConOutputCb = NULL;
*/
/*
//Buffer Transfer variables
BOOL gbGotFirst = FALSE;
CHAR *gFirstBuffer = NULL;
CHAR *gTransferBuffer = NULL;

//Global State Variables
DWORD gPromptType = PROMPT_UNK;

ULONG DbgGetProcType(VOID)
{
	ULONG Type = IMAGE_FILE_MACHINE_UNKNOWN;
	
	if (g_DbgControl)
		g_DbgControl->GetActualProcessorType(&Type);

	return Type;
}


//----------------------------------------------------------------------------
//
// Default output callbacks implementation, provides IUnknown.
//
//----------------------------------------------------------------------------

STDMETHODIMP 
DefOutputCallbacks::QueryInterface(
    THIS_
    IN REFIID InterfaceId,
    OUT PVOID* Interface)
{
    *Interface = NULL;

    if (IsEqualIID(InterfaceId, IID_IUnknown) ||
        IsEqualIID(InterfaceId, IID_IDebugOutputCallbacks))
    {
        *Interface = (IDebugOutputCallbacks *)this;
        AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG) 
DefOutputCallbacks::AddRef(THIS)
{
    return 1;
}

STDMETHODIMP_(ULONG)
DefOutputCallbacks::Release(THIS)
{
    return 0;
}

//----------------------------------------------------------------------------
//
// Console output callbacks.
//
//----------------------------------------------------------------------------

STDMETHODIMP
ConOutputCallbacks::Output(
    THIS_
    IN ULONG Mask,
    IN PCSTR Text)
{
	HRESULT hRes = S_OK;
	if (!gbGotFirst)
	{
		CHAR *sdstring = NULL;

		if (!gFirstBuffer)
			gFirstBuffer = _strdup(Text);
		else
		{
			DWORD dwLength = strlen(Text) + strlen(gFirstBuffer) + 1;
			CHAR *temp = (CHAR *)malloc(dwLength);
			if (temp)
			{
				strcpy(temp, gFirstBuffer);
				strcat(temp, Text);
				free(gFirstBuffer);
				gFirstBuffer = temp;
			}
			else
				return E_FAIL;
		}

		sdstring = strstr(gFirstBuffer, "Stress has been shutdown.");
		while (sdstring)
		{
			CHAR *spare = (CHAR *)malloc(strlen(sdstring + strlen("Stress has been shutdown.")) + 1);
			if (!spare)
				break;
			strcpy(spare, (sdstring + strlen("Stress has been shutdown.")));
			free(gFirstBuffer);
			gFirstBuffer = spare;
			sdstring = strstr(gFirstBuffer, "Stress has been shutdown.");
		}

		sdstring = strstr(gFirstBuffer, "Page Avail=");
		while (sdstring)
		{
			CHAR *spare = (CHAR *)malloc(strlen(sdstring + strlen("Page Avail=")) + 1);
			if (!spare)
				break;
			strcpy(spare, (sdstring + strlen("Page Avail=")));
			free(gFirstBuffer);
			gFirstBuffer = spare;
			sdstring = strstr(gFirstBuffer, "Page Avail=");
		}

		sdstring = strstr(gFirstBuffer, "Waiting to Reconnect...");
		while (sdstring)
		{
			CHAR *spare = (CHAR *)malloc(strlen(sdstring + strlen("Waiting to Reconnect...")) + 1);
			if (!spare)
				break;
			strcpy(spare, (sdstring + strlen("Waiting to Reconnect...")));
			free(gFirstBuffer);
			gFirstBuffer = spare;
			sdstring = strstr(gFirstBuffer, "Waiting to Reconnect...");
		}

		sdstring = strstr(gFirstBuffer, "Send Break in");
		while (sdstring)
		{
			CHAR *spare = (CHAR *)malloc(strlen(sdstring + strlen("Send Break in")) + 1);
			if (!spare)
				break;
			strcpy(spare, (sdstring + strlen("Send Break in")));
			free(gFirstBuffer);
			gFirstBuffer = spare;
			sdstring = strstr(gFirstBuffer, "Send Break in");
		}

		sdstring = strstr(gFirstBuffer, "Loaded dbghelp extension DLL");
		while (sdstring)
		{
			CHAR *spare = (CHAR *)malloc(strlen(sdstring + strlen("Loaded dbghelp extension DLL")) + 1);
			if (!spare)
				break;
			strcpy(spare, (sdstring + strlen("Loaded dbghelp extension DLL")));
			free(gFirstBuffer);
			gFirstBuffer = spare;
			sdstring = strstr(gFirstBuffer, "Loaded dbghelp extension DLL");
		}

	}
	else
	{
		if (!gTransferBuffer)
			gTransferBuffer = _strdup(Text);
		else
		{
			DWORD dwLength = strlen(Text) + strlen(gTransferBuffer) + 1;
			CHAR *temp = (CHAR *)malloc(dwLength);
			if (temp)
			{
				strcpy(temp, gTransferBuffer);
				strcat(temp, Text);
				free(gTransferBuffer);
				gTransferBuffer = temp;
			}
			else
				return E_FAIL;
		}

//		if (gPromptType == PROMPT_NTSD)
//		{
//			if (IsThereNTSDPropt(gTransferBuffer))
//				hRes = g_DbgClient->ExitDispatch(g_DbgClient);
//		}
	}

    return S_OK;
}


VOID AdjustPromptTypeBasedOnInitialBuffer(VOID)
{
	CHAR *sol = NULL;

	if (!gFirstBuffer)
		return;

	sol = strrchr(gFirstBuffer, '>');
	if (sol)
	{
		sol--;
		if (isdigit(*sol))
		{
			while (isdigit(*sol))
				sol--;

			if ((*sol) == ':')
			{
				sol--;
				if (isdigit(*sol))
				gPromptType = PROMPT_NTSD;				
				return;
			}
		}
	}
	else
	{
		// Put Assert stuff in here
	}

	return;
}

BOOL IsThereNTSDPropt(CHAR *InBuff)
{
	CHAR *sol = NULL;

	if (!InBuff)
		return FALSE;

	sol = strrchr(InBuff, '>');
	if (sol)
	{
		if (sol > InBuff)
			sol--;
		if (isdigit(*sol))
		{
			while ((isdigit(*sol))&&(sol > InBuff))
				sol--;

			if ((*sol) == ':')
			{
				if (sol > InBuff)
					sol--;
				if (isdigit(*sol))
				return TRUE;
			}
		}
	}

	return FALSE;
}
HRESULT ConnectEngine(CHAR *server, CHAR *srvpipename)
{
    HRESULT hRes = S_OK;
	CHAR RemoteOptions[MAX_PATH + 1];
    ULONG OutMask = 0;
	ULONG Class = DEBUG_CLASS_UNINITIALIZED;
	ULONG Qualifier = DEBUG_KERNEL_CONNECTION;

	gFirstBuffer = NULL;
	gTransferBuffer = NULL;
	g_DbgClient = NULL;
	// -remote npipe:server=yourmachine,pipe=foobar
	sprintf(RemoteOptions, "npipe:server=%s,pipe=%s", server, srvpipename);

    hRes = DebugConnect(RemoteOptions, 
						IID_IDebugClient,
                        (void **)&g_DbgClient);
	if FAILED(hRes)
        return hRes;

    hRes = g_DbgClient->QueryInterface(IID_IDebugControl,
                                         (void **)&g_DbgControl);
	if FAILED(hRes)
        return hRes;

       
	// Set our callbacks
    g_ConOutputCb = new ConOutputCallbacks;
    hRes = g_DbgClient->SetOutputCallbacks(g_ConOutputCb);
    
	// Shut off Verbose
    hRes = g_DbgClient->GetOutputMask(&OutMask);
    OutMask &= ~DEBUG_OUTPUT_VERBOSE;
    OutMask |= DEBUG_OUTPUT_PROMPT;
    OutMask |= DEBUG_OUTPUT_DEBUGGEE;
    OutMask |= DEBUG_OUTPUT_DEBUGGEE_PROMPT;
    hRes = g_DbgClient->SetOutputMask(OutMask);

	// Connect the session. This will cause the output callback
	// to be fired with hte initial buffer.
	gbGotFirst = FALSE;
//    hRes = g_DbgClient->ConnectSession(DEBUG_CONNECT_SESSION_DEFAULT);
	gbGotFirst = TRUE;

	// Figure out if this is KD or NTSD piped to KD.
	hRes = g_DbgControl->GetDebuggeeType(&Class, &Qualifier);
	if (Class == DEBUG_CLASS_USER_WINDOWS)
		gPromptType = PROMPT_NTSD;
	else
	{
		gPromptType = PROMPT_KD;
		AdjustPromptTypeBasedOnInitialBuffer();
	}

	return hRes;
}

HRESULT DisconnectEngine(VOID)
{
	if (g_DbgClient)
	{
        if (g_ConOutputCb)
        {
            delete g_ConOutputCb;
            g_ConOutputCb = NULL;
        }
		g_DbgClient->Release();
		if (g_DbgControl)
			g_DbgControl->Release();
	}
	g_DbgClient = NULL;
	g_DbgControl = NULL;

	if (gFirstBuffer)
		free(gFirstBuffer);
	gFirstBuffer = NULL;

	if (gTransferBuffer)
		free(gTransferBuffer);
	gTransferBuffer = NULL;

	return S_OK;
}
*/
/*
// This function actually sends commands to the pipe and gets back the results
// Options:
// 1.theCommand = NULL && theBuffer = NULL = EmptyPipe
// 2.theCommand = NULL && theBuffer ! NULL = Get existing pipe data
// 3.theCommand ! NULL && theBuffer = NULL = Send command, dont want results
//							(They are still cleared from pipe)
// 4.theCommand ! NULL && theBuffer ! NULL = Send Command get Results
HRESULT NewSendMyCommand(const CHAR *theCommand, 
						 CHAR **RetBuffer, 
						 ULONG Output,
						 ULONG ulTimeout)
{
	CHAR *temp = NULL;
	HRESULT hRes = S_OK;

	// Get rid of the special(easy) cases
	if (theCommand == NULL)
	{
		if (RetBuffer == NULL)
		{
			if (gFirstBuffer)
				free(gFirstBuffer);
			gFirstBuffer = NULL;
			
			goto exit;
		}
		else
		{
			*RetBuffer = gFirstBuffer;
			gFirstBuffer = NULL;

			goto addprompt;
		}
	}

	if (RetBuffer == NULL)
	{
		if (gPromptType == PROMPT_KD)
		{
			g_DbgControl->Execute(DEBUG_OUTCTL_ALL_OTHER_CLIENTS, 
								  theCommand,
								  DEBUG_EXECUTE_ECHO);
			g_DbgControl->OutputPrompt(DEBUG_OUTCTL_ALL_OTHER_CLIENTS, NULL);
		}
		else
		{
			g_DbgControl->ReturnInput(theCommand);
		}

		goto exit;
	}
	else
	{
		if (gPromptType == PROMPT_KD)
		{
			g_DbgControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, 
								  theCommand,
								  DEBUG_EXECUTE_ECHO);
			g_DbgControl->OutputPrompt(DEBUG_OUTCTL_ALL_OTHER_CLIENTS, NULL);
		}
		else
		{
			hRes = g_DbgControl->ReturnInput(theCommand);
			while (!IsThereNTSDPropt(gTransferBuffer))
				hRes = g_DbgClient->DispatchCallbacks(100);
			// This is because we get a empty string as the last callback
			g_DbgClient->DispatchCallbacks(100);
		}

		*RetBuffer = gTransferBuffer;
		gTransferBuffer = NULL;
		
		goto addprompt;
	}

addprompt:
	// We need to put a debug prompt in here to make this 
	// work with the old stuff.
	if (gPromptType == PROMPT_KD)
	{
		temp = (CHAR *)malloc(strlen(*RetBuffer) + 1 + 3);
		if (temp)
		{
			strcpy(temp, *RetBuffer);
			strcat(temp, "kd>");
			free(*RetBuffer);
			*RetBuffer = temp;
		}
	}

exit:
	return S_OK;
}

*/