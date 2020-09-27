/***************************************************************************
 *
 *  Copyright (C) 2/22/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       audition.h
 *  Content:    Audition objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/22/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __AUDITION_H__
#define __AUDITION_H__

#ifdef __cplusplus

//
// Connection dialog data
//

BEGIN_DEFINE_STRUCT()
    LPTSTR      pszAddressList;                     // Initial address list
    TCHAR       szAddress[0x100];                   // Address output buffer
END_DEFINE_STRUCT(XBOXCONNECTDATA);

//
// Remote audition object
//

class CRemoteAudition
{
protected:
    PDM_CONNECTION              m_pxbdm;            // Xbox debugger connection

public:
    CRemoteAudition(void);
    virtual ~CRemoteAudition(void);

public:
    // Connection
    virtual BOOL Connect(void);
    virtual void Disconnect(void);

    // Auditioning
    virtual BOOL Play(CWaveBankEntry *pEntry);
    virtual void StopAll(void);
};

//
// Xbox connection dialog
//

class CXboxConnectionDialog
    : public CDialog
{
public:
    enum
    {
        IDD = IDD_CONNECT_XBOX
    };

protected:
    LPXBOXCONNECTDATA       m_pData;            // Connection data

public:
    CXboxConnectionDialog(void);
    virtual ~CXboxConnectionDialog(void);

protected:
    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);

    // Command handlers
    virtual void OnCmdOK(void);
};

#endif // __cplusplus

#endif // __AUDITION_H__
