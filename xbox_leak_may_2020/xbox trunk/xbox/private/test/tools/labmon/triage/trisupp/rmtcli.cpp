#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "rmtcli.h"
#include "dbgr.h"

#define BUFFSIZE 4096
#define MAX_RETRIES 5
#define LINES_TO_READ 150

#define READEND 0
#define WRITEEND 1

#define VERSION 4

#define SERVER_READ_PIPE    "\\\\%s\\PIPE\\%sIN"   //Client Writes and Server Reads
#define SERVER_WRITE_PIPE   "\\\\%s\\PIPE\\%sOUT"  //Server Reads  and Client Writes

#define MAGICNUMBER     0x31109000

typedef struct
    {
    DWORD    Size;
    DWORD    Version;
    CHAR     ClientName[15];
    DWORD    LinesToSend;
    DWORD    Flag;
    }   SESSION_STARTUPINFO;

typedef struct
    {
    DWORD MagicNumber;      //New Remote
    DWORD Size;             //Size of structure
    DWORD FileSize;         //Num bytes sent
    }   SESSION_STARTREPLY;

HANDLE hPipes[2] = {NULL, NULL};
BOOL UseNewPipe = FALSE;

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ReadFixBytes(CHAR *Buffer, DWORD ToRead, DWORD TimeOut)
    {
    DWORD xyzBytesRead=0;
    DWORD xyzBytesToRead=ToRead;
    CHAR  *xyzbuff=Buffer;
    
    while(xyzBytesToRead!=0) {
        if (!ReadFile(hPipes[READEND],xyzbuff,xyzBytesToRead,&xyzBytesRead,NULL)) {
            return(xyzBytesToRead);
            }
        
        xyzBytesToRead-=xyzBytesRead;
        xyzbuff+=xyzBytesRead;
        }
    return(0);
    }

BOOL SendInfo(int LinesToSend)
    {
    DWORD  hostlen=14;
    WORD   BytesToSend=sizeof(SESSION_STARTUPINFO);
    DWORD  tmp = 15;
    SESSION_STARTUPINFO ssi;
    SESSION_STARTREPLY  ssr;
    
    RtlZeroMemory(&ssi,sizeof(ssi));
    RtlZeroMemory(&ssr,sizeof(ssr));
    
    ssi.Size=BytesToSend;
    ssi.Version=VERSION;
    
    tmp=sizeof(ssi.ClientName);
    //GetUserNameA(ssi.ClientName, &tmp);
    strcpy(ssi.ClientName, "xstress");
    ssi.ClientName[14]=0;
    ssi.LinesToSend=LinesToSend;
    
        {
        DWORD NewCode=MAGICNUMBER;
        char  Name[15];
        
        strcpy(Name,(char *)ssi.ClientName);
        memcpy(&Name[11],(char *)&NewCode,sizeof(NewCode));
        
        if (!WriteFile(hPipes[WRITEEND],(char *)Name,15,&tmp,NULL))
            return FALSE;
        if (!ReadFile(hPipes[READEND] ,(char *)&ssr.MagicNumber,sizeof(ssr.MagicNumber),&tmp,NULL))
            return FALSE;
        
        if (ssr.MagicNumber!=MAGICNUMBER) 
            {
            return FALSE;
            }
        
        //Get Rest of the info-its not the old server
        
        ReadFixBytes((char *)&ssr.Size,sizeof(ssr.Size),0);
        ReadFixBytes((char *)&ssr.FileSize,sizeof(ssr)-sizeof(ssr.FileSize)-sizeof(ssr.MagicNumber),0);
        
        }
        ssi.Flag=0x80000000;
        if (!WriteFile(hPipes[WRITEEND],(char *)&ssi,BytesToSend,&tmp,NULL)) {
            return(FALSE);
            }
        
        return(TRUE);
    }

DWORD rmConnectToRemote(CHAR *server, CHAR *srvpipename)
    {
    CHAR	pipenameSrvIn[200];
    CHAR	pipenameSrvOut[200];
    int		tries = 0;
    BOOL	ret = TRUE;
    DWORD   dwOldPipeError = 0;
    HRESULT hRes = S_OK;
    
    if ((server == NULL)||(srvpipename == NULL))
        return ERROR_INVALID_FUNCTION;
    
    hPipes[READEND] = INVALID_HANDLE_VALUE;
    hPipes[WRITEEND] = INVALID_HANDLE_VALUE;
    
    /*	UseNewPipe = TRUE;
    
      hRes = ConnectEngine(server, srvpipename);  
      if (SUCCEEDED(hRes))
      return(ERROR_SUCCESS);
      
        UseNewPipe = FALSE;
    */
    sprintf(pipenameSrvIn ,SERVER_READ_PIPE ,server,srvpipename);
    sprintf(pipenameSrvOut,SERVER_WRITE_PIPE,server,srvpipename);
    
    hPipes[READEND]=CreateFile(pipenameSrvOut,GENERIC_READ ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hPipes[READEND]==INVALID_HANDLE_VALUE) 
        {
        tries = 1;
        while ((hPipes[READEND]==INVALID_HANDLE_VALUE) && (tries <= MAX_RETRIES))
            {
            hPipes[READEND]=CreateFile(pipenameSrvOut,GENERIC_READ ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            tries++;
            }
        
        if (hPipes[READEND]==INVALID_HANDLE_VALUE) 
            {
            dwOldPipeError = GetLastError();
            goto failed;
            }
        }
    
    hPipes[WRITEEND]=CreateFile(pipenameSrvIn ,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hPipes[WRITEEND]==INVALID_HANDLE_VALUE) 
        {
        tries = 1;
        while ((hPipes[WRITEEND]==INVALID_HANDLE_VALUE) && (tries <= MAX_RETRIES))
            {
            hPipes[WRITEEND]=CreateFile(pipenameSrvIn ,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            tries++;
            }
        if (hPipes[WRITEEND]==INVALID_HANDLE_VALUE) 
            {
            CloseHandle(hPipes[READEND]);
            dwOldPipeError = GetLastError();
            goto failed;
            }
        }
    
    if (!(ret = SendInfo(LINES_TO_READ))) 
        {
        tries =1;
        while ((tries <= MAX_RETRIES)&&(!ret))
            {
            ret = SendInfo(LINES_TO_READ);
            tries++;
            }
        if (!ret)
            {
            CloseHandle(hPipes[READEND]);
            CloseHandle(hPipes[WRITEEND]);
            dwOldPipeError = GetLastError();
            goto failed;
            }
        }
    return(ERROR_SUCCESS);
    
failed:
    
    hPipes[READEND] = INVALID_HANDLE_VALUE;
    hPipes[WRITEEND] = INVALID_HANDLE_VALUE;
    
    return dwOldPipeError;
    }

BOOL rmCloseRemote()
    {
    if (hPipes[READEND] != INVALID_HANDLE_VALUE)
        CloseHandle(hPipes[READEND]);
    if (hPipes[WRITEEND] != INVALID_HANDLE_VALUE)
        CloseHandle(hPipes[WRITEEND]);
    
    hPipes[READEND] = INVALID_HANDLE_VALUE;
    hPipes[WRITEEND] = INVALID_HANDLE_VALUE;
    
    //DisconnectEngine();
    
    return TRUE;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL mPeekNamedPipe(LPVOID lpBuffer, DWORD nBufferSize,  
                    LPDWORD lpBytesRead, LPDWORD lpTotalBytesAvail, LPDWORD lpBytesLeftThisMessage)
    {
    BOOL ret = TRUE, ret2 = TRUE;
    DWORD x = 0;
    DWORD BytesLeftThisMessage = 0;
    
    *lpTotalBytesAvail = 0;
    
    ret = PeekNamedPipe(hPipes[READEND], lpBuffer, nBufferSize, lpBytesRead, lpTotalBytesAvail, &BytesLeftThisMessage);
    
    if (lpBytesLeftThisMessage)
        *lpBytesLeftThisMessage = BytesLeftThisMessage;
    
    x = GetLastError();
    return TRUE;
    }

BOOL EmptyPipe()
    {
    CHAR tempbuff[BUFFSIZE] = {'\0'};
    DWORD bytesAvailable = 0;
    DWORD bytesRead = 0;
    
    // we do this to clean up the pipe
    mPeekNamedPipe(NULL, 0, NULL, &bytesAvailable, NULL);
    while (bytesAvailable)
        {
        if (!ReadFile(hPipes[READEND], &tempbuff, BUFFSIZE - 1, &bytesRead, NULL))
            return FALSE;
        mPeekNamedPipe(NULL, 0, NULL, &bytesAvailable, NULL);
        }
    
    return TRUE;
    }

BOOL WriteOutCommand(const CHAR *theCommand)
    {
    DWORD bytesWritten = 0;
    
    if (!WriteFile(hPipes[WRITEEND], theCommand, strlen(theCommand), &bytesWritten, NULL)) 
        {
        return FALSE;
        }
    Sleep(1000);
    return TRUE;
    }

BOOL WeHaveAPrompt(CHAR *theBuffer)
    {
    CHAR *sol = NULL;
    
    sol = strrchr(theBuffer, '>');
    if (sol)
        {
        sol--;
        if ((*sol) == 'd')
            {
            if (*(--sol) == 'k')
                return TRUE;
            }
        else if (isdigit(*sol))
            {
            while (isdigit(*sol))
                sol--;
            
            if ((*sol) == ':')
                {
                sol--;
                if (isdigit(*sol))
                    return TRUE;
                }
            }
        }
    return FALSE;
    }

    
/*
DWORD ReadData(char *buffer, unsigned bytesToRead, unsigned maxBuffSize)
    {
    DWORD bytesAvailable = bytesToRead;
    DWORD bytesRead;

    while(bytesAvailable)
        {
        if(!ReadFile(hPipes[READEND], buffer, bytesAvailable, &bytesRead, NULL))
            break;
        bytesAvailable = bytesAvailable - bytesRead;
        buffer += bytesRead;
        buffer[0] = '\0';
        }

    return bytesToRead-bytesAvailable;
    }
*/


// This function actually sends commands to the pipe and gets back the results
// Options:
// 1.theCommand = NULL && theBuffer = NULL then EmptyPipe
// 2.theCommand = NULL && theBuffer ! NULL then Get existing pipe data
// 3.theCommand ! NULL && theBuffer = NULL then Send command, dont want results
//							(They are still cleared from pipe)
// 4.theCommand ! NULL && theBuffer ! NULL then Send Command get Results
BOOL SendMyCommand(const CHAR *theCommand, CHAR **RetBuffer, time_t tWaitTime)
    {
    time_t	tTheTime = 0;
    CHAR *pcurr = NULL;
    CHAR *buf2 = NULL;
    DWORD bytesRead = 0;
    DWORD bytesAvailable = 0;
    BOOL done = FALSE;
    CHAR *theBuffer = NULL;	
    unsigned maxChunks = 5;

    // NO pipes no send
    if((hPipes[READEND] == NULL) || (hPipes[WRITEEND] == NULL))
        return FALSE;
    
    if(RetBuffer != NULL)
        *RetBuffer = NULL;
    
    // Get rid of the special(easy) case
    if((theCommand == NULL) && (RetBuffer == NULL))
        {
        EmptyPipe();
        return TRUE;
        }
    
    if(theCommand != NULL)
        {
        EmptyPipe();
        if(!WriteOutCommand(theCommand))
            return FALSE;
        }
    
    tTheTime = time(NULL) + tWaitTime;
    
    // Lets wait for some bytes to come available
    while(!bytesAvailable)
        {
        mPeekNamedPipe(NULL, 0, NULL, &bytesAvailable, NULL);
        if(time(NULL) > tTheTime)
            return FALSE;
        if(!bytesAvailable)
            Sleep(500);
        }
    
    // Get That first chunk
    if(pcurr = theBuffer = (CHAR *)malloc(bytesAvailable + 1))
        {
        while(bytesAvailable)
            {
            if(!ReadFile(hPipes[READEND], pcurr, bytesAvailable, &bytesRead, NULL))
                return FALSE;
            bytesAvailable = bytesAvailable - bytesRead;
            pcurr+=bytesRead;
            pcurr[0] = '\0';
            }
        }
    else
        return FALSE;
    
    // below code will max processor for tWaitTime on unbroken boxes
    // There could be some more coming
    do
        {
        bytesAvailable = 0;
        mPeekNamedPipe(NULL, 0, NULL, &bytesAvailable, NULL);
        
        while (bytesAvailable)
            {
            if (buf2 = (CHAR *)malloc((strlen(theBuffer) * sizeof(CHAR)) + 1 + bytesAvailable)) 
                {
                ZeroMemory(buf2, ((strlen(theBuffer) * sizeof(CHAR)) + 1 + bytesAvailable));
                strcpy(buf2, theBuffer);
                free(theBuffer);
                theBuffer=buf2;
                pcurr = theBuffer + strlen(theBuffer);
                
                while (bytesAvailable)
                    {
                    if (!ReadFile(hPipes[READEND], pcurr, bytesAvailable, &bytesRead, NULL))
                        {
                        free(theBuffer);
                        return FALSE;
                        }
                    bytesAvailable = bytesAvailable - bytesRead;
                    pcurr+=bytesRead;
                    pcurr[0] = '\0';
                    }	// while
                }
            else
                {
                free(theBuffer);
                return FALSE;
                }
            mPeekNamedPipe(NULL, 0, NULL, &bytesAvailable, NULL);
            if (time(NULL) > (tTheTime + tWaitTime))
                break;
            }
        
        // this should get kd> and #:{###}#>
        if (time(NULL) > tTheTime)
            done = TRUE;
        else
            done = WeHaveAPrompt(theBuffer);
        } while (!done);
        

    if((time(NULL) >= tTheTime)&& !WeHaveAPrompt(theBuffer))
        {
        // ctrl-v wont get a prompt but we want the buffer
        if((theCommand != NULL)&&(!strcmp(theCommand, "\026\n")))
            {
            if (RetBuffer != NULL)
                *RetBuffer = theBuffer;
            return FALSE;
            }

        // a breakpoint wont have a prompt but it will be waiting for a reply
        else if((strstr((theBuffer), "(bipt)?"))||
            (strstr((theBuffer), "[gbwxpft?]"))||
            (strstr((theBuffer), "(boipt)?"))||
            (strstr((theBuffer), "(bizro)?"))||
            (strstr((theBuffer), "(bizrd)?"))||
            (strstr((theBuffer), "Assertion failed")))
            {}
        else
            {
            free(theBuffer);
            return FALSE;
            }
        }
    
    if(RetBuffer != NULL)
        {
        *RetBuffer = theBuffer;
        }
    else
        {
        if (theBuffer)
            free(theBuffer);
        }
    
    return TRUE;
    }


