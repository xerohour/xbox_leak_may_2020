// ----------------------------------------------------------------------------
// nicw.h
//
// Software implementation of a NIC for simulating an Ethernet network
//
// Copyright (C) Microsoft Corporation
// ----------------------------------------------------------------------------

#ifndef __NICW_H__
#define __NICW_H__

// ---------------------------------------------------------------------------------------
// CXnNic
// ---------------------------------------------------------------------------------------

class NOVTABLE CXnNic : public CXnBase
{
    friend void WINAPI VLanRecv(BYTE *, UINT, void *);

    // Definitions -----------------------------------------------------------------------

    struct RecvDesc
    {
        DWORD       cbDat;
        BYTE        rgbBuf[ENET_LINK_MAXSIZE + ENET_DATA_MAXSIZE];
    };

    // External --------------------------------------------------------------------------

public:

    HAL_DECLARE_NEW_DELETE(CXnNic)

    BOOL            NicXmitReady();
    void            NicXmit(CPacket * ppkt);
    VIRTUAL void    NicTimer() {}

protected:

    NTSTATUS        NicInit(XNetInitParams * pxnip);
    INT             NicConfig(const XNetConfigParams * pxncp);
    void            NicStart();
    void            NicFlush();
    void            NicStop();
    void            NicTerm();

    VIRTUAL void    NicRecvFrame(void * pv, DWORD cbData);

    VIRTUAL void    EnetRecv(CPacket * ppkt, UINT uiType) VPURE;
    VIRTUAL void    EnetPush() VPURE;

    // Internal --------------------------------------------------------------------------

private:

    RecvDesc *      NextPrd(RecvDesc * prd);
    void            XmitPush();
    void            RecvPush();
    void            SetInterrupts(BOOL fEnable);
    static void     DpcCallback(struct _KDPC *, void * pthis, void *, void *);

    INT             NicVLanAttach();
    void            NicVLanDetach(BOOL fTerm);

    // Data ------------------------------------------------------------------------------

protected:

    CEnetAddr       _ea;

private:

    LIST_ENTRY      _le;
    KDPC            _dpcRecv;
    BOOL            _fIntr;
    BOOL            _fXmitFull;
    LONG            _cxdBusy;
    LONG            _cxdPool;
    CPacketQueue    _pqXmit;
    LONG            _crdBusy;
    LONG            _crdPool;
    RecvDesc *      _prdFirst;
    RecvDesc *      _prdRead;
    RecvDesc *      _prdWrite;
    RecvDesc *      _prdLast;
    BOOL            _fVLanInit;
    BOOL            _fVLanAttach;

};

// ---------------------------------------------------------------------------------------

#endif
