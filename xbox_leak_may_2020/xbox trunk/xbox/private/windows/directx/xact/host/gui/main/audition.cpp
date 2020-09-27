/***************************************************************************
 *
 *  Copyright (C) 2/22/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       audition.cpp
 *  Content:    Audition objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/22/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactapp.h"


/****************************************************************************
 *
 *  CRemoteAudition
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteAudition::CRemoteAudition"

CRemoteAudition::CRemoteAudition
(
    void
)
{
    m_pxbdm = NULL;
}


/****************************************************************************
 *
 *  ~CRemoteAudition
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteAudition::~CRemoteAudition"

CRemoteAudition::~CRemoteAudition
(
    void
)
{
    Disconnect();
}


/****************************************************************************
 *
 *  Connect
 *
 *  Description:
 *      Connects to a remote instance of Audio Console.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteAudition::Connect"

BOOL
CRemoteAudition::Connect
(
    void
)
{
    CMRU                    MRU(g_pApplication->m_Registry, REGKEY_XBOX_MRU);

    static const LPCTSTR    pszCommandTemplate  = TEXT("XAUD!connect %s");
    CXboxConnectionDialog   Dialog;
    XBOXCONNECTDATA         ConnectData;
    TCHAR                   szDefault[0x100];
    TCHAR                   szCommand[0x100];
    int                     nResult;
    BOOL                    fSuccess;
    DWORD                   dwSize;
    
    if(m_pxbdm)
    {
        return TRUE;
    }

    _stprintf(szCommand, pszCommandTemplate, g_szAppTitle);

    //
    // Get the default Xbox name and make sure it's in the MRU
    //

    dwSize = NUMELMS(szDefault);

    if(XBDM_NOERR == (nResult = DmGetXboxName(szDefault, &dwSize)))
    {
        MRU.AddItem(szDefault);
    }
    else
    {
        DPF_ERROR("DmGetXboxName failed with %x", nResult);
    }

    //
    // Get MRU data
    //

    ConnectData.pszAddressList = MRU.GetItems();

    //
    // Enter the connection loop
    //

    while(TRUE)
    {
        //
        // Show the connection dialog.  The dialog will write directly to
        // our m_szAddress buffer.
        //

        if(!(fSuccess = Dialog.DoModal(&g_pApplication->m_MainFrame, &ConnectData)))
        {
            break;
        }

        //
        // If the address is valid, use it
        //

        if(fSuccess && ConnectData.szAddress[0])
        {
            DmSetXboxNameNoRegister(ConnectData.szAddress);
        }

        //
        // Connect to the Xbox
        //

        if(fSuccess)
        {
            if(XBDM_NOERR != (nResult = DmOpenConnection(&m_pxbdm)))
            {
                DPF_ERROR("DmOpenConnection failed with %x", nResult);
                fSuccess = FALSE;
            }
        }

        //
        // Connect to AudConsole
        //

        if(fSuccess)
        {
            if(XBDM_NOERR != (nResult = DmSendCommand(m_pxbdm, szCommand, NULL, NULL)))
            {
                DPF_ERROR("DmSendCommand(%s) failed with %x", szCommand, nResult);
                fSuccess = FALSE;
            }
        }

        //
        // If anything went wrong, try again
        //

        if(fSuccess)
        {
            break;
        }
        else
        {
            DmCloseConnection(m_pxbdm);
            m_pxbdm = NULL;
        }
    }

    //
    // Add the address to the MRU
    //

    if(fSuccess && ConnectData.szAddress[0])
    {
        MRU.AddItem(ConnectData.szAddress);
    }

    //
    // Update commands
    //

    if(fSuccess)
    {
        g_pApplication->m_MainFrame.EnableCommand(ID_CONNECT, FALSE);
        g_pApplication->m_MainFrame.EnableCommand(ID_DISCONNECT, TRUE);
    }

    //
    // Clean up
    //

    MEMFREE(ConnectData.pszAddressList);

    return fSuccess;
}


/****************************************************************************
 *
 *  Disconnect
 *
 *  Description:
 *      Disconnects from a remote instance of Audio Console.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteAudition::Disconnect"

void
CRemoteAudition::Disconnect
(
    void
)
{
    static const LPCTSTR    pszCommandTemplate  = TEXT("XAUD!disconnect %s");
    TCHAR                   szCommand[0x100];
    int                     nResult;
    
    if(!m_pxbdm)
    {
        return;
    }

    //
    // Disconnect
    //
    
    _stprintf(szCommand, pszCommandTemplate, g_szAppTitle);

    DmSendCommand(m_pxbdm, szCommand, NULL, NULL);
    DmCloseConnection(m_pxbdm);

    m_pxbdm = NULL;

    //
    // Update commands
    //

    g_pApplication->m_MainFrame.EnableCommand(ID_CONNECT, TRUE);
    g_pApplication->m_MainFrame.EnableCommand(ID_DISCONNECT, FALSE);
}


/****************************************************************************
 *
 *  Play
 *
 *  Description:
 *      Auditions an entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteAudition::Play"

BOOL
CRemoteAudition::Play
(
    CWaveBankEntry *        pEntry
)
{
    static const LPCSTR     pszCommandTemplate      = "XAUD!WaveBank.PlayEntry \"%s\"";
    static const LPCSTR     pszDstPathTemplate      = "T:\\%s";
    static const DWORD      dwCopyBufferSize        = 256 * 1024;
    DWORD                   dwOffset                = 0;
    LPVOID                  pvCopyBuffer            = NULL;
    TCHAR                   szCurDir[MAX_PATH];
    TCHAR                   szSrcFile[MAX_PATH];
    TCHAR                   szDstFile[MAX_PATH];
    TCHAR                   szFileTitle[MAX_PATH];
    CHAR                    szCommand[0x400];
    CStdFileStream          DataFile;
    WAVEBANKENTRY           Entry;
    BOOL                    fSuccess;
    int                     nResult;

    //
    // Make sure we're connected
    //

    if(!Connect())
    {
        return FALSE;
    }

    //
    // Open a temporary file
    //

    GetCurrentDirectory(NUMELMS(szCurDir), szCurDir);
    GetTempFileName(szCurDir, TEXT("XAC"), 0, szSrcFile);

    fSuccess = SUCCEEDED(DataFile.Open(szSrcFile, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN));

    //
    // Get entry meta-data and write it to the file
    //

    if(fSuccess)
    {
        fSuccess = SUCCEEDED(pEntry->GetMetaData(&Entry, 0, CWaveBank::m_dwMinAlignment, &dwOffset));
    }

    if(fSuccess)
    {
        fSuccess = SUCCEEDED(DataFile.Write(&Entry, sizeof(Entry)));
    }
    
    //
    // Allocate the copy buffer
    //

    if(fSuccess)
    {
        fSuccess = MAKEBOOL(pvCopyBuffer = MEMALLOC(BYTE, dwCopyBufferSize));
    }

    //
    // Get wave data
    //

    if(fSuccess)
    {
        fSuccess = SUCCEEDED(pEntry->CommitWaveData(NULL, &DataFile, sizeof(Entry), &Entry, pvCopyBuffer, dwCopyBufferSize));
    }

    //
    // Close the file so XBDM can read it
    //

    if(fSuccess)
    {
        DataFile.Close();
    }

    //
    // Send the file to the Xbox
    //

    if(fSuccess)
    {
        _stprintf(szDstFile, pszDstPathTemplate, pEntry->m_pParent->m_szBankName);

        DmMkdir(szDstFile);
    }
    
    if(fSuccess)
    {
        _splitpath(pEntry->m_szFileName, NULL, NULL, szFileTitle, NULL);
        
        _tcscat(szDstFile, TEXT("\\"));
        _tcscat(szDstFile, szFileTitle);
        
        if(XBDM_NOERR != (nResult = DmSendFile(szSrcFile, szDstFile)))
        {
            DPF_ERROR("DmSendFile(%s, %s) failed with %x", szSrcFile, szDstFile, nResult);
            fSuccess = FALSE;
        }
    }

    //
    // Tell the Xbox to play the file
    //

    if(fSuccess)
    {
        sprintf(szCommand, pszCommandTemplate, szDstFile);
        
        if(XBDM_NOERR != (nResult = DmSendCommand(m_pxbdm, szCommand, NULL, NULL)))
        {
            DPF_ERROR("DmSendCommand(%s) failed with %x", szCommand, nResult);
            fSuccess = FALSE;
        }
    }

    //
    // Clean up
    //

    DeleteFile(szSrcFile);
    
    MEMFREE(pvCopyBuffer);

    return fSuccess;
}


/****************************************************************************
 *
 *  StopAll
 *
 *  Description:
 *      Cancels any autitioning entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteAudition::StopAll"

void
CRemoteAudition::StopAll
(
    void
)
{
    DmSendCommand(m_pxbdm, "XAUD!WaveBank.StopAll", NULL, NULL);
}


/****************************************************************************
 *
 *  CXboxConnectionDialog
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CXboxConnectionDialog::CXboxConnectionDialog"

CXboxConnectionDialog::CXboxConnectionDialog
(
    void
)
:   CDialog(IDD)
{
}


/****************************************************************************
 *
 *  ~CXboxConnectionDialog
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CXboxConnectionDialog::~CXboxConnectionDialog"

CXboxConnectionDialog::~CXboxConnectionDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CXboxConnectionDialog::OnCreate"

BOOL
CXboxConnectionDialog::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    LPTSTR                  pszAddressList;
    
    if(CDialog::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Save connection data
    //

    ASSERT(pvContext);
    m_pData = (LPXBOXCONNECTDATA)pvContext;

    //
    // Add addresses from the address list
    //

    if(m_pData->pszAddressList)
    {
        pszAddressList = m_pData->pszAddressList;

        while(*pszAddressList)
        {
            SendDlgItemMessage(IDC_ADDRESS_LIST, CB_ADDSTRING, 0, (LPARAM)pszAddressList);

            pszAddressList += _tcslen(pszAddressList) + 1;
        }
    }

    //
    // Set the default list item
    //

    SendDlgItemMessage(IDC_ADDRESS_LIST, CB_SETCURSEL, 0);

    return FALSE;
}


/****************************************************************************
 *
 *  OnCmdOK
 *
 *  Description:
 *      Handles the OK command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CXboxConnectionDialog::OnCmdOK"

void
CXboxConnectionDialog::OnCmdOK
(
    void
)
{
    //
    // Get the address
    //

    GetDlgItemText(IDC_ADDRESS_LIST, m_pData->szAddress, NUMELMS(m_pData->szAddress));

    //
    // Hand off to the base class
    //

    CDialog::OnCmdOK();
}


