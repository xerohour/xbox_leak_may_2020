// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     XBoxNotify.cpp
// Contents: Contains code to handle notification messages sent from the Xbox.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_fWriteException    -- If an access violation occurred, 'true' here means it was a write
//                         violation; 'false' means a read violation occurred.
bool g_fWriteException;

// g_pvAVAddress        -- If an access violation occurred, this contains the address that was
//                         incorrectly read or written.
void *g_pvAVAddress;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  NotifyProc
// Purpose:   This is a callback routine registered by DmNotify.  It is called by XboxDbg for
//            various types of notification event
// Arguments: Notification      -- Type of notification, DM_DEBUGSTR for example
//            Parameter         -- Optional parameter based on notification type
// Returns:   Always zero
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD WINAPI NotifyProc(DWORD dwNotification, DWORD dwParameter)
{
    char           szBuf[4192] = {0}, szBuf2[4192] = {0};
    PDMN_DEBUGSTR  pdmnds;
    PDMN_BREAK     pdmnb;
    PDMN_DATABREAK pdmndb;
    PDMN_EXCEPTION pdmne;
    char           *pch;
    static BOOL fConnected = FALSE;

    // UNDONE-WARN: Remove flags
    dwNotification &= 0x0FFFFFFF;

    switch(dwNotification)
    {
    case DM_EXEC:
        // Output information about the Exec event.
        // The first time we get exec, we need to print that we successfully
        // connected
        if(!fConnected) {
            fConnected = TRUE;
            OutputMsg("xbwatson: Connection to Xbox successful.\r\n");
        } else if(dwParameter == DMN_EXEC_REBOOT)
            OutputMsg("xbwatson: Xbox is restarting.\r\n");
        break;
        
    case DM_DEBUGSTR:
    case DM_ASSERT:
    case DM_RIP:
        pdmnds = (PDMN_DEBUGSTR)dwParameter;
        if (pdmnds->Length)
        {
            strncpy(szBuf2, pdmnds->String, min(pdmnds->Length, 1024));
            strncpy(szBuf, pdmnds->String, min(pdmnds->Length, 1024));
            switch(dwNotification)
            {
            case DM_DEBUGSTR:
                // Replace the '\n' newline that XboxDbg appends since the edit control hates it
                pch = &szBuf2[min(pdmnds->Length, 1023) - 1];
                if (*pch == '\n')
                {
                    *pch = '\0';
                    OutputMsg("%s\r\n", szBuf2);
                }
                else
                    OutputMsg("%s", szBuf2);
                break;

            case DM_ASSERT:
                // Shrink the assert text for clarity of output
                for (pch = szBuf; *pch; pch++)
                    if (*pch == '\n') *pch = '\0';
                
                // Output the assert information to the log window
                strcat(szBuf, "\r\n");
                OutputMsg("Assert: %s", szBuf);

                // Pop up the Assert dialog and let the user choose what to do.
                HandleAssert(szBuf2, pdmnds->ThreadId);
                break;

            case DM_RIP:
                // Output the RIP information to the log window
                OutputMsg("RIP: %s\r\n", szBuf2);

                // Pop up the RIP dialog and let the user choose to reboot the machine, etc.
                HandleRIP(pdmnds->ThreadId, szBuf2);
                break;
            }            
        }
        break;

    case DM_BREAK:
        pdmnb = (PDMN_BREAK)dwParameter;

        // Output the breakpoint information to the log window
        OutputMsg("Break: 0x%08X 0x%08X\r\n", pdmnb->Address, pdmnb->ThreadId);

        // Pop up the exception dialog and let the user choose to dump the state, reboot the machine, etc
        HandleException(pdmnb->ThreadId, 0x80000003, pdmnb->Address);
        break;

    case DM_DATABREAK:
        pdmndb = (PDMN_DATABREAK)dwParameter;
        OutputMsg("Databreak: 0x%08X 0x%08X 0x%08X 0x%08X\r\n", pdmndb->Address,
                pdmndb->ThreadId, pdmndb->BreakType, pdmndb->DataAddress);

        // Pop up the exception dialog and let the user choose to dump the state, reboot the machine, etc
        HandleException(pdmndb->ThreadId, 0x80000003, pdmndb->Address);
        break;

    case DM_EXCEPTION:
        pdmne = (PDMN_EXCEPTION)dwParameter;
        if (!(pdmne->Flags & DM_EXCEPT_FIRSTCHANCE))
        {
            // output the exception information to the log window
            OutputMsg("Exception: 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X\r\n", pdmne->ThreadId,
                    pdmne->Code, pdmne->Address, pdmne->Flags, pdmne->Information[0], pdmne->Information[1]);

            // Store information about the access violation for the exception dialog
            g_fWriteException = pdmne->Information[0] ? true : false;
            g_pvAVAddress     = (void*)pdmne->Information[1];

            // Pop up the exception dialog and let the user choose to dump the state, reboot the machine, etc
            HandleException(pdmne->ThreadId, pdmne->Code, pdmne->Address);
        }
        break;

    default:
        // All notification messages are handled above, but in case a new one is added later and it somehow
        // gets to us, just output it...
        OutputMsg("%s\r\n", szBuf2);
        break;
    }

    return 0;
}
