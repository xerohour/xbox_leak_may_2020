/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  status.c

Abstract:

  This module queries the status of an xnet stress session

Author:

  Steven Kehrli (steveke) 8-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetStressNamespace;

NETSYNC_TYPE_CALLBACK StressStatusSession = {
    1,
    1,
    0,
    L"xnetstress_nt.dll",
    "XNetStressStatusMain",
    "XNetStressStatusStart",
    "XNetStressStatusStop"
};



LRESULT CALLBACK DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Window procedure for the main window

-----------------------------------------------------------------------------*/
{
    // wndclass is the window class of the dialog
    WNDCLASSEX  wndclass;
    // hDlg is the handle to the dialog
    HWND        hDlg = NULL;
    // msg is the window message
    MSG         msg;
    // dwErrorCode is the error code
    DWORD       dwErrorCode = ERROR_SUCCESS;



    // Initialize the common controls
    InitCommonControls();


    // Initialize the window class
    ZeroMemory(&wndclass, sizeof(wndclass));
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = DialogProc;
    wndclass.cbWndExtra = DLGWINDOWEXTRA;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STATUS_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) (COLOR_INACTIVEBORDER + 1);
    wndclass.lpszClassName = STATUS_CLASS_NAME;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STATUS_ICON));

    // Check if window is open
    hDlg = FindWindow(STATUS_CLASS_NAME, NULL);
    if (NULL != hDlg) {
        // Switch to the open window
        ShowWindow(hDlg, SW_RESTORE);
        SetForegroundWindow(hDlg);

        return 0;
    }

    // Register the class
    RegisterClassEx(&wndclass);

    // Create the dialog
    hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, NULL);

    // Initialize the dialog
    SendMessage(hDlg, UM_DIALOG_INITIALIZE, 0, 0);

    // Show the dialog
    ShowWindow(hDlg, iCmdShow);
    UpdateWindow(hDlg);

    // Pump the message handler
    while (0 != GetMessage(&msg, NULL, 0, 0)) {
        if (FALSE == IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Get the error code
    dwErrorCode = msg.wParam;

    return dwErrorCode;
}



int CALLBACK
SortClientObjects(
    LPARAM  lParam1,
    LPARAM  lParam2,
    LPARAM  lParamSort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sorts the client objects in the status list

Arguments:

  lParam1 - Specifies the first lParam (pointer to the first client object)
  lParam2 - Specifies the second lParam (pointer to the second client object)
  lParamSort - Specifies the sort method

Return Value:

  int:
    If the first item should precede the second, negative value (-1)
    If the second item should precede the first, positive value (+1)
    If the two items are equivalent, 0

------------------------------------------------------------------------------*/
{
    // szName1 is the name in the first lParam
    char  *szName1;
    // szName2 is the name in the second lParam
    char  *szName2;



    switch (lParamSort) {
        case eSortLocalNameAscending:
            // Get the names
            szName1 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->LocalAddr);
            szName2 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->RemoteAddr);

            return strcmp(szName1, szName2);

            break;

        case eSortLocalNameDescending:
            szName1 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->LocalAddr);
            szName2 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->RemoteAddr);

            return strcmp(szName2, szName1);

            break;

        case eSortRemoteNameAscending:
            szName1 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->LocalAddr);
            szName2 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->RemoteAddr);

            return strcmp(szName1, szName2);

            break;

        case eSortRemoteNameDescending:
            szName1 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->LocalAddr);
            szName2 = inet_ntoa(((PCLIENT_OBJECT) lParam1)->RemoteAddr);

            return strcmp(szName2, szName1);

            break;

        case eSortStressTypeAscending:
            if (((PCLIENT_OBJECT) lParam1)->dwStressType < ((PCLIENT_OBJECT) lParam2)->dwStressType) {
                return -1;
            }
            else if (((PCLIENT_OBJECT) lParam1)->dwStressType > ((PCLIENT_OBJECT) lParam2)->dwStressType) {
                return 1;
            }

            break;

        case eSortStressTypeDescending:
            if (((PCLIENT_OBJECT) lParam2)->dwStressType < ((PCLIENT_OBJECT) lParam1)->dwStressType) {
                return -1;
            }
            else if (((PCLIENT_OBJECT) lParam2)->dwStressType > ((PCLIENT_OBJECT) lParam1)->dwStressType) {
                return 1;
            }

            break;


        case eSortStatusAscending:
            if (((PCLIENT_OBJECT) lParam1)->dwStatus < ((PCLIENT_OBJECT) lParam2)->dwStatus) {
                return -1;
            }
            else if (((PCLIENT_OBJECT) lParam1)->dwStatus > ((PCLIENT_OBJECT) lParam2)->dwStatus) {
                return 1;
            }

            break;


        case eSortStatusDescending:
            if (((PCLIENT_OBJECT) lParam2)->dwStatus < ((PCLIENT_OBJECT) lParam1)->dwStatus) {
                return -1;
            }
            else if (((PCLIENT_OBJECT) lParam2)->dwStatus > ((PCLIENT_OBJECT) lParam1)->dwStatus) {
                return 1;
            }

            break;

        default:
            break;
    }

    return 0;
}



DWORD WINAPI QueryThreadProc (LPVOID lpv)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to query a netsync server

Return Value:

  DWORD - exit code

-----------------------------------------------------------------------------*/
{
    // hDlg is the handle to the dialog
    HWND                            hDlg = (HWND) lpv;
    // szHostName is the host name
    CHAR                            szHostName[__max(MAX_COMPUTERNAME_LENGTH, MAX_IP_LENGTH) + 1];
    // pHostEntry is a pointer to the hostent structure
    HOSTENT                         *pHostEntry = NULL;
    // uHostAddr is the host name address
    ULONG                           uHostAddr = 0;

    // hNetsyncObject is the handle to the netsync object
    HANDLE                          hNetsyncObject = INVALID_HANDLE_VALUE;
    // uNetsyncInAddr is the netsync address
    ULONG                           uNetsyncInAddr = 0;
    // bNetsyncSucceeded indicates if the netsync session is found
    BOOL                            bNetsyncSucceeded = FALSE;

    // StressStatusRequestMessage is the stress status request message
    STRESS_STATUS_REQUEST_MESSAGE   StressStatusRequestMessage;
    // pStressStatusReplyMessage is a pointer to the stress status reply message
    PSTRESS_STATUS_REPLY_MESSAGE    pStressStatusReplyMessage;
    // pStressStatusSessionMessage is a pointer to the stress status session message
    PSTRESS_STATUS_SESSION_MESSAGE  pStressStatusSessionMessage;
    // pStressStatusClientMessage is a pointer to the stress status client message
    PSTRESS_STATUS_CLIENT_MESSAGE   pStressStatusClientMessage;
    // dwMessageSize is the size of the message
    DWORD                           dwMessageSize;
    // dwSession is a counter to enumerate each session
    DWORD                           dwSession;
    // dwClient is a counter to enumerate each client
    DWORD                           dwClient;

    // szText is the text string buffer
    WCHAR                           szText[256];
    // szError is the error string buffer
    WCHAR                           szError[256];



    // Disable the UI
    SendMessage(hDlg, UM_DIALOG_ENABLE, 0, (LPARAM) FALSE);

    // Delete the items in the list view
    SendMessage(hDlg, UM_DIALOG_DELETE, 0, 0);

    // Get the computer name
    if (0 == GetDlgItemTextA(hDlg, IDC_HOST_NAME_TEXT, szHostName, sizeof(szHostName))) {
        // Get the resource string buffer
        GetResourceString(IDS_NAME_GET_FAILED, szError, sizeof(szError) / sizeof(WCHAR));

        goto QueryThreadProc_Exit;
    }

    // Get the IP address
    uHostAddr = inet_addr(szHostName);
    if (INADDR_NONE == uHostAddr) {
        // Resolve the computer name
        pHostEntry = gethostbyname(szHostName);
        if (NULL == pHostEntry) {
            // Get the resource string buffer
            GetResourceString(IDS_NAME_RESOLVE_FAILED, szError, sizeof(szError) / sizeof(WCHAR), GetLastError());

            goto QueryThreadProc_Exit;
        }

        // Get the IP address
        CopyMemory(&uHostAddr, pHostEntry->h_addr, sizeof(uHostAddr));
    }

    // Connect to the session
    hNetsyncObject = NetsyncCreateClient(uHostAddr, NETSYNC_SESSION_CALLBACK, &StressStatusSession, &uNetsyncInAddr, NULL, NULL, NULL);
    if (INVALID_HANDLE_VALUE == hNetsyncObject) {
        // Get the resource string buffer
        GetResourceString(IDS_NAME_NETSYNC_FAILED, szError, sizeof(szError) / sizeof(WCHAR), GetLastError());

        goto QueryThreadProc_Exit;
    }

    // Send the request message
    ZeroMemory(&StressStatusRequestMessage, sizeof(StressStatusRequestMessage));
    StressStatusRequestMessage.dwMessageId = STRESS_STATUS_REQUEST_MSG;
    NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(StressStatusRequestMessage), (char *) &StressStatusRequestMessage);

    // Receive the reply message
    NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &uNetsyncInAddr, NULL, &dwMessageSize, (char **) &pStressStatusReplyMessage);
    for (dwSession = 0; dwSession < pStressStatusReplyMessage->dwNumStressSessions; dwSession++) {
        // Receive the session message
        NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &uNetsyncInAddr, NULL, &dwMessageSize, (char **) &pStressStatusSessionMessage);
        for (dwClient = 0; dwClient < pStressStatusSessionMessage->dwNumStressClients; dwClient++) {
            // Receive the client message
            NetsyncReceiveClientMessage(hNetsyncObject, INFINITE, &uNetsyncInAddr, NULL, &dwMessageSize, (char **) &pStressStatusClientMessage);

            // Add the client
            SendMessage(hDlg, UM_DIALOG_ADD, 0, (LPARAM) pStressStatusClientMessage);

            // Free the client message
            NetsyncFreeMessage((char *) pStressStatusClientMessage);
        }

        // Free the session message
        NetsyncFreeMessage((char *) pStressStatusSessionMessage);
    }

    // Free the reply message
    NetsyncFreeMessage((char *) pStressStatusReplyMessage);

    // Close the session
    NetsyncCloseClient(hNetsyncObject);

    // Session succeeded
    bNetsyncSucceeded = TRUE;

QueryThreadProc_Exit:
    if (FALSE == bNetsyncSucceeded) {
        // Display the error message
        GetResourceString(IDS_STATUS_CAPTION, szText, sizeof(szText) / sizeof(WCHAR));
        MessageBox(NULL, szError, szText, MB_OK | MB_ICONERROR);
    }

    // Enable the UI
    SendMessage(hDlg, UM_DIALOG_ENABLE, 0, (LPARAM) TRUE);

    return 0;
}


    
LRESULT CALLBACK DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Dialog procedure for the main window

-----------------------------------------------------------------------------*/
{
    // hMemObject is the handle to the memory object
    static HANDLE       hMemObject = INVALID_HANDLE_VALUE;
    // WSAData is the details of the Winsock implementation
    WSADATA             WSAData;
    // bWinsockInitialized indicates Winsock is initialized
    static BOOL         bWinsockInitialized = FALSE;
    // bDlgInitialized indicates the dialog is initialized
    static BOOL         bDlgInitialized = TRUE;
    // dwErrorCode is the last error code
    DWORD               dwErrorCode = ERROR_SUCCESS;

    // hWndList is a handle to the status list
    static HWND         hWndList = NULL;
    // eCurrentSortMethod is the current sort method of the status list
    static eSortMethod  eCurrentSortMethod = eSortLocalNameAscending;
    // rcClient is the WindowRect of the status list
    RECT                rcClient;
    // lvc specifies the attributes of a particular column of the status list
    LV_COLUMN           lvc;
    // lvi specifies the attributes of a particular item of the status list
    LV_ITEM             lvi;
    // dwItemCount is the number of items in the status list
    DWORD               dwItemCount;
    // dwItem is a counter to enumerate each item in the status list
    DWORD               dwItem;
    // pnmhdr is a pointer to the WM_NOTIFY message header
    LPNMHDR             pnmhdr;
    // pnmlistview is a pointer to the LVN_COLUMNCLICK message
    LPNMLISTVIEW        pnmlistview;

    // pClientObject is a pointer to the client object
    PCLIENT_OBJECT      pClientObject;

    // hQueryThread is the handle to the query thread
    static HANDLE       hQueryThread = NULL;
    // dwExitCode is the exit code of the query thread
    DWORD               dwExitCode;

    // szText is the text string buffer
    WCHAR               szText[256];
    // szError is the error string buffer
    WCHAR               szError[256];



    switch (iMsg) {

        case UM_DIALOG_INITIALIZE:
            // Limit the length of the text box
            SendMessage(GetDlgItem(hDlg, IDC_HOST_NAME_TEXT), EM_SETLIMITTEXT, __max(MAX_COMPUTERNAME_LENGTH, MAX_IP_LENGTH), 0);

            // Get the handle to the status list
            hWndList = GetDlgItem(hDlg, IDC_STATUS_LIST);

            // Set the status list to select the full row
            ListView_SetExtendedListViewStyle(hWndList, LVS_EX_FULLROWSELECT);

            // Get the Rect of the Status List
            GetWindowRect(hWndList, &rcClient);

            // Set common attributes for each column
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvc.fmt = LVCFMT_LEFT;

            // Local Name Column
            // Load the text
            GetResourceString(IDS_LOCALNAME_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Set the width of the column to be about 1 / 3 of the width of the status list, allowing for the width of the borders and scroll bar
            lvc.cx = (rcClient.right - rcClient.left - 4 * GetSystemMetrics(SM_CXBORDER) - GetSystemMetrics(SM_CXVSCROLL)) / 3;
            // Indicate this is the first column
            lvc.iSubItem = eColumnLocalName;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Remote Name Column
            // Load the text
            GetResourceString(IDS_REMOTENAME_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Indicate this is the second column
            lvc.iSubItem = eColumnRemoteName;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Stress Type Column
            // Load the text
            GetResourceString(IDS_STRESSTYPE_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Set the width of the column to be about 1 / 6 of the width of the status list, allowing for the width of the borders and scroll bar
            lvc.cx /= 2;
            // Indicate this is the third column
            lvc.iSubItem = eColumnStressType;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Status Column
            // Load the text
            GetResourceString(IDS_STATUS_LABEL, szText, sizeof(szText) / sizeof(WCHAR));
            // Set the column text
            lvc.pszText = szText;
            // Indicate this is the fourth column
            lvc.iSubItem = eColumnStatus;
            // Insert column
            ListView_InsertColumn(hWndList, lvc.iSubItem, &lvc);

            // Set the caption
            GetResourceString(IDS_STATUS_CAPTION, szText, sizeof(szText) / sizeof(WCHAR));
            SetWindowText(hDlg, szText);

            // Create the memory object
            hMemObject = xMemCreate();
            if (INVALID_HANDLE_VALUE == hMemObject) {
                // Get the last error
                dwErrorCode = GetLastError();

                // Get the resource string buffer
                GetResourceString(IDS_MEMORY_INIT_FAILED, szError, sizeof(szError) / sizeof(WCHAR), dwErrorCode);

                goto UM_DIALOG_INITIALIZE_Exit;
            }

            // Initialize Winsock
            ZeroMemory(&WSAData, sizeof(WSAData));
            dwErrorCode = WSAStartup(MAKEWORD(2, 2), &WSAData);
            if (ERROR_SUCCESS != dwErrorCode) {
                // Get the resource string buffer
                GetResourceString(IDS_WINSOCK_INIT_FAILED, szError, sizeof(szError) / sizeof(WCHAR), dwErrorCode);

                goto UM_DIALOG_INITIALIZE_Exit;
            }
            else {
                bWinsockInitialized = TRUE;
            }

UM_DIALOG_INITIALIZE_Exit:
            if (ERROR_SUCCESS != dwErrorCode) {
                // Display the error message
                GetResourceString(IDS_STATUS_CAPTION, szText, sizeof(szText) / sizeof(WCHAR));
                MessageBox(NULL, szError, szText, MB_OK | MB_ICONERROR);

                // End the dialog
                bDlgInitialized = FALSE;
                DestroyWindow(hDlg);
                PostQuitMessage(dwErrorCode);
            }


            break;

        case UM_DIALOG_ENABLE:
            // Enable \ Disable the UI
            EnableWindow(GetDlgItem(hDlg, IDC_HOST_NAME_STATIC), (BOOL) lParam);
            EnableWindow(GetDlgItem(hDlg, IDC_HOST_NAME_TEXT), (BOOL) lParam);
            EnableWindow(GetDlgItem(hDlg, IDC_QUERY_BUTTON), (BOOL) lParam);

            if (FALSE == (BOOL) lParam) {
                SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM) hWndList, MAKELONG(TRUE, 0));
            }

            break;

        case UM_DIALOG_ADD:
            // Allocate the memory for the client object
            pClientObject = (PCLIENT_OBJECT) xMemAlloc(hMemObject, sizeof(CLIENT_OBJECT));

            // Set the client object
            CopyMemory(&pClientObject->LocalAddr, &((PSTRESS_STATUS_CLIENT_MESSAGE) lParam)->StressClient.ClientXnAddr.ina, sizeof(pClientObject->LocalAddr));
            CopyMemory(&pClientObject->RemoteAddr, &((PSTRESS_STATUS_CLIENT_MESSAGE) lParam)->StressClient.RemoteXnAddr.ina, sizeof(pClientObject->RemoteAddr));
            pClientObject->dwStressType = ((PSTRESS_STATUS_CLIENT_MESSAGE) lParam)->StressClient.dwStressType;
            pClientObject->dwStatus = ((PSTRESS_STATUS_CLIENT_MESSAGE) lParam)->StressClient.dwStatus;

            // Insert the item

            // Indicate pszText and lParam are valid
            lvi.mask = LVIF_TEXT | LVIF_PARAM;

            // Set the text
            wsprintf(szText, L"%d.%d.%d.%d", pClientObject->LocalAddr.S_un.S_un_b.s_b1, pClientObject->LocalAddr.S_un.S_un_b.s_b2, pClientObject->LocalAddr.S_un.S_un_b.s_b3, pClientObject->LocalAddr.S_un.S_un_b.s_b4);
            lvi.pszText = szText;
            // Set the lParam
            lvi.lParam = (long) pClientObject;
            // Set the item number
            lvi.iItem = ListView_GetItemCount(hWndList);
            // Indicate this is the first column
            lvi.iSubItem = eColumnLocalName;
            // Insert item
            ListView_InsertItem(hWndList, &lvi);

            // Indicate only pszText is valid
            lvi.mask = LVIF_TEXT;

            // Set the text
            wsprintf(szText, L"%d.%d.%d.%d", pClientObject->RemoteAddr.S_un.S_un_b.s_b1, pClientObject->RemoteAddr.S_un.S_un_b.s_b2, pClientObject->RemoteAddr.S_un.S_un_b.s_b3, pClientObject->RemoteAddr.S_un.S_un_b.s_b4);
            lvi.pszText = szText;
            // Indicate this is the second column
            lvi.iSubItem = eColumnRemoteName;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Set the text
            wsprintf(szText, L"Type %d", pClientObject->dwStressType);
            lvi.pszText = szText;
            // Indicate this is the third column
            lvi.iSubItem = eColumnStressType;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Set the text
            if (STATUS_REGISTERED == pClientObject->dwStatus) {
                wcscpy(szText, L"Registered");
            }
            else if (STATUS_STARTED == pClientObject->dwStatus) {
                wcscpy(szText, L"Started");
            }
            else {
                wcscpy(szText, L"Waiting");
            }
            lvi.pszText = szText;
            // Indicate this is the fourth column
            lvi.iSubItem = eColumnStatus;
            // Set item
            ListView_SetItem(hWndList, &lvi);

            // Sort the status list
            ListView_SortItems(hWndList, SortClientObjects, eCurrentSortMethod);

            break;

        case UM_DIALOG_DELETE:
            // Delete the items in the list

            // Set the item mask
            lvi.mask = LVIF_PARAM;

            // Get the number of items in the list
            dwItemCount = ListView_GetItemCount(hWndList);
            for (dwItem = dwItemCount; dwItem > 0; dwItem--) {
                // Get the item
                lvi.iItem = dwItem - 1;
                ListView_GetItem(hWndList, &lvi);

                // Get the client message
                pClientObject = (PCLIENT_OBJECT) lvi.lParam;

                // Delete the item
                ListView_DeleteItem(hWndList, lvi.iItem);

                // Free the client object
                xMemFree(hMemObject, pClientObject);
            }

        case WM_NOTIFY:
            // Get the WM_NOTIFY message header
            pnmhdr = (LPNMHDR) lParam;

            if ((NULL != pnmhdr) && (hWndList == pnmhdr->hwndFrom) && (LVN_COLUMNCLICK == pnmhdr->code)) {
                // Get the LVN_COLUMNCLICK message
                pnmlistview = (LPNMLISTVIEW) lParam;

                // Update the sort method
                switch (pnmlistview->iSubItem) {
                    case eColumnLocalName:
                        if (eSortLocalNameAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortLocalNameAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortLocalNameDescending;
                        }

                        break;

                    case eColumnRemoteName:
                        if (eSortRemoteNameAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortRemoteNameAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortRemoteNameDescending;
                        }

                        break;

                    case eColumnStressType:
                        if (eSortStressTypeAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortStressTypeAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortStressTypeDescending;
                        }

                        break;

                    case eColumnStatus:
                        if (eSortStatusAscending != eCurrentSortMethod) {
                            eCurrentSortMethod = eSortStatusAscending;
                        }
                        else {
                            eCurrentSortMethod = eSortStatusDescending;
                        }

                        break;

                    default:
                        break;
                }

                // Sort the status list
                ListView_SortItems(hWndList, SortClientObjects, eCurrentSortMethod);
            }

            break;

        case WM_COMMAND:
            if (IDC_QUERY_BUTTON == LOWORD(wParam)) {
                // Close the query thread, if necessary
                if (NULL == hQueryThread) {
                    CloseHandle(hQueryThread);
                    hQueryThread = NULL;
                }

                // Create the query thread
                hQueryThread = CreateThread(NULL, 0, QueryThreadProc, (LPVOID) hDlg, 0, NULL);
            }

            break;

        case WM_QUERYENDSESSION:
        case WM_ENDSESSION:
        case WM_CLOSE:
        case WM_DESTROY:
            if (TRUE == bDlgInitialized) {
                // Delete the items in the list view
                SendMessage(hDlg, UM_DIALOG_DELETE, 0, 0);
            }

            // Check if query thread is still running
            if (hQueryThread != NULL) {
                if ((GetExitCodeThread(hQueryThread, &dwExitCode) == FALSE) || (dwExitCode == STILL_ACTIVE)) {
                    return FALSE;
                }

                CloseHandle(hQueryThread);
                hQueryThread = NULL;
            }

            // Terminate Winsock
            if (TRUE == bWinsockInitialized) {
                WSACleanup();
                bWinsockInitialized = FALSE;
            }

            // Free the memory object
            if (INVALID_HANDLE_VALUE != hMemObject) {
                xMemClose(hMemObject);
                hMemObject = INVALID_HANDLE_VALUE;
            }

            // End the dialog
            if (TRUE == bDlgInitialized) {
                bDlgInitialized = FALSE;
                DestroyWindow(hDlg);
                PostQuitMessage(ERROR_SUCCESS);
            }

            break;

    }

    return DefDlgProc(hDlg, iMsg, wParam, lParam);
}
