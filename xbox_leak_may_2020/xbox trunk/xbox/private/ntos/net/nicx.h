// ---------------------------------------------------------------------------------------
// nicx.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef _NICX_H_
#define _NICX_H_

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

struct NICSTATS
{
    ULONG               isrCount;
    ULONG               dpcCount;
    ULONG               txGoodFrames;
    ULONG               rxGoodFrames;
    ULONG               txUnderflowErrors;
    ULONG               txLateCollisions;
    ULONG               txLostCarriers;
    ULONG               txDefers;
    ULONG               txExcessiveDefers;
    ULONG               txRetryErrors;
    ULONG               rxMinSizeErrors;
    ULONG               rxFramingErrors;
    ULONG               rxOverFlowErrors;
    ULONG               rxCrcErrors;
    ULONG               rxLengthErrors;
    ULONG               rxMaxFrameErrors;
    ULONG               rxLateCollisions;
    ULONG               rxRunts;
    ULONG               rxExtraByteErrors;
    ULONG               rxMissedFrames;
    ULONG               rxEndOfFrameErrors;

};

#define NIC_VENDORID    0x10DE      // nVidia Vendor ID
#define NIC_DEVICEID    0x01C3      // nVidia Device ID

#define RXDESC_OWN      BIT(31)     // 0 - host owns; 1 - controller owns
#define RXDESC_ERR      BIT(30)     // receive error
#define RXDESC_FRAM     BIT(29)     // framing error
#define RXDESC_OFOL     BIT(28)     // overflow error
#define RXDESC_CRC      BIT(27)     // CRC error
#define RXDESC_LFER     BIT(26)     // length field error
#define RXDESC_MAX      BIT(25)     // received frame larger than max size
#define RXDESC_LCOL     BIT(24)     // late collision
#define RXDESC_RUNT     BIT(23)     // runt packet received
#define RXDESC_PAM      BIT(22)     // physical address match
#define RXDESC_MAM      BIT(21)     // multicast address match
#define RXDESC_BAM      BIT(20)     // broadcast address match
#define RXDESC_EXTRA    BIT(18)     // received frame length has an extra byte
#define RXDESC_MISS     BIT(17)     // missed data reception
#define RXDESC_REND     BIT(16)     // end of receive frame

#define TXDESC_OWN      BIT(31)     // 0 - host owns; 1 - controller owns
#define TXDESC_ERR      BIT(30)     // transmit error
#define TXDESC_UFLO     BIT(29)     // underflow error
#define TXDESC_LCOL     BIT(28)     // late collision
#define TXDESC_LCAR     BIT(27)     // loss of carrier
#define TXDESC_DEF      BIT(26)     // deferred
#define TXDESC_EXDEF    BIT(25)     // excessive deferral
#define TXDESC_INTEN    BIT(24)     // interrupt override
#define TXDESC_RTRY     BIT(19)     // retry error (23-20: retry count)
#define TXDESC_TEND     BIT(16)     // end of transmit frame

struct XmitDesc
{
    DWORD       _dwPhyAddr;         // physical address of xmit buffer
    DWORD       _dwFlagsCount;      // HIWORD - control/status bits; LOWORD - byte count minus 1
};

struct RecvDesc
{
    DWORD       _dwPhyAddr;         // physical address of recv buffer
    DWORD       _dwFlagsCount;      // HIWORD - control/status bits; LOWORD - byte count
};

#define NIC_FRAME_SIZE      (PAGE_SIZE/2)
#define NIC_FRAME_ALIGNMENT 2

// ---------------------------------------------------------------------------------------
// CXnNic
// ---------------------------------------------------------------------------------------

class NOVTABLE CXnNic : public CXnBase
{
    // Definitions -----------------------------------------------------------------------

    struct XNADDRXBDM
    {
        BYTE        bSizeOfStruct;                  // sizeof(XNADDR)
        BYTE        bFlags;                         // XNET_XNADDR_* flags below
        BYTE        abEnet[6];                      // Ethernet MAC address
        IN_ADDR     ina;                            // IP address (zero if not static/DHCP)
        IN_ADDR     inaOnline;                      // Online IP address (zero if not online)
        WORD        wPortOnline;                    // Online port (zero if not online)
        BYTE        abIdOnline[20];                 // Online identifier (zero if not online)
    };

    // External --------------------------------------------------------------------------

public:

    HAL_DECLARE_NEW_DELETE(CXnNic)

    BOOL                NicXmitReady();
    void                NicXmit(CPacket * ppkt);
    VIRTUAL void        NicTimer();

#if defined(XNET_FEATURE_XBDM_CLIENT) || defined(XNET_FEATURE_XBDM_SERVER)
    DWORD               NicGetOtherXnAddr(XNADDR * pxna);
#endif

protected:

    NTSTATUS            NicInit(XNetInitParams * pxnip);
    INLINE INT          NicConfig(const XNetConfigParams * pxncp) { return(0); }
    void                NicStart();
    void                NicFlush();
    void                NicStop();
    void                NicTerm();

    VIRTUAL void        EnetRecv(CPacket * ppkt, UINT uiType) VPURE;
    VIRTUAL void        EnetPush() VPURE;

    // Internal --------------------------------------------------------------------------

private:

    INLINE XmitDesc *   PxdNext(XmitDesc * pxd) { return(pxd == _pxdLast ? _pxdFirst : (pxd + 1)); }
    INLINE RecvDesc *   PrdNext(RecvDesc * prd) { return(prd == _prdLast ? _prdFirst : (prd + 1)); }
    INLINE DWORD_PTR    PhyAddr(void * pv)      { return((DWORD_PTR)pv + _dwPhyOff); }
    INLINE void *       VirAddr(DWORD_PTR dw)   { return((void *)(dw - _dwPhyOff)); }

    void                NicStartXmitRecv();
    void                NicStopXmitRecv();
    static BOOLEAN      NicIsr(PKINTERRUPT, PVOID pvContext);
    BOOLEAN             NicProcessIsr();
    static void         NicDpc(PKDPC dpc, void * pvContext, void * pvArg1, void * pvArg2);
    void                NicProcessDpc();
    void                NicMiiInterrupt(DWORD dwMintr, BOOL fInit);
    void                NicRecvInterrupt();
    void                NicXmitInterrupt();

    static void         HalShutdownNotification(HAL_SHUTDOWN_REGISTRATION * pHalShutdownReg);

    class CXbdmClient
    {
    public:
        INLINE CXnNic *         GetXnNic();
        virtual void            XmitComplete(void * pvPkt);
        virtual void            EnetRecv(UINT uiFlags, void * pv, UINT cb, UINT uiType);
        virtual void            EnetPush();
        virtual DWORD           GetXnAddr(XNADDRXBDM * pxnaXbdm);
    };

    class CXbdmServer
    {
    public:
        // This magic cookie on the XBDM server CXnNic protects the client from connecting
        // to an incompatible version.  We only change it when this interface changes in
        // a non-backward compatible way.
        #define XBDM_SERVER_COOKIE 'XBD6'
        CXbdmServer() { _dwCookie = XBDM_SERVER_COOKIE; }
        BOOL                    IsValidServer() { return(_dwCookie == XBDM_SERVER_COOKIE); }
        INLINE CXnNic *         GetXnNic();
        virtual void            NicStop();
        virtual NTSTATUS        InitClient(UINT cfgRecvQ, UINT cfgXmitQ, CEnetAddr * pea, BOOL * pfLinkIsUp);
        virtual void            AttachClient(CXbdmClient * pXbdmClient);
        virtual void            DetachClient();
        virtual DWORD           GetXnAddr(XNADDRXBDM * pxnaXbdm);
        virtual void            Xmit(void * pvPkt, void * pv, UINT cb);
        virtual BOOL            XmitReady();
        DWORD                   _dwCookie;
    };

    class CXbdmPacket : public CPacket
    {

    public:

        void *          _pvPkt;                 // Client's packet pointer
    };

#ifdef XNET_FEATURE_STATS
    #define STATINC(n)          StatInc(offsetof(NICSTATS, n))
    #define STATINC_(x,n)       if (x) STATINC(n)
    void                        StatInit();
    void                        StatTerm();
    void                        StatInc(UINT iStat);
    void                        StatTimer(CTimer * pt);
#else
    #define STATINC(n)
    #define STATINC_(x,n)
    INLINE void                 StatInit() {};
    INLINE void                 StatTerm() {};
#endif

    // Data ------------------------------------------------------------------------------

public:

    CEnetAddr                   _ea;                // Ethernet address

private:

    KDPC                        _dpc;               // DPC object
    ULONG                       _ulIntrVector;      // interrupt vector
    KIRQL                       _irqlIntr;          // interrupt IRQ level
    DWORD                       _dwRxPollFreq;      // Rx polling frequency
    DWORD                       _dwLinkState;       // last known link state (from PhyGetLinkState)
    BOOL                        _fXmitFull;         // TRUE if _cxdBusy >= _cxdPool
    DWORD_PTR                   _dwPhyOff;          // virtual to physical offset
    LONG                        _cxdBusy;           // count of busy transmit descriptors
    LONG                        _cxdPool;           // number of transmit descriptors (minus 1)
    XmitDesc *                  _pxdFirst;          // first transmit descriptor
    XmitDesc *                  _pxdLast;           // last transmit descriptor
    XmitDesc *                  _pxdBusy;           // first busy transmit descriptor
    XmitDesc *                  _pxdFree;           // first free transmit descriptor
    CPacketQueue                _pqXmit;            // queue of packets being transmitted
    LONG                        _crdPool;           // number of receive descriptors available
    RecvDesc *                  _prdFirst;          // first receive descriptor
    RecvDesc *                  _prdLast;           // last receive descriptor
    RecvDesc *                  _prdPtr;            // current receive descriptor
    HAL_SHUTDOWN_REGISTRATION   _HalShutdownReg;    // registration for system shutdown

#ifdef XNET_FEATURE_XBDM_CLIENT
    CXbdmServer *               _pXbdmServer;       // Non-NULL when XBDM is running in this process
    CXbdmClient                 _XbdmClient;        // The object we give to the XBDM server
#endif

#ifdef XNET_FEATURE_XBDM_SERVER
    CXbdmClient *               _pXbdmClient;       // Non-NULL when the title stack is attached
    CXbdmServer                 _XbdmServer;        // The object we give to the XBDM client
    CEnetAddr                   _eaClient;          // Ethernet address of title stack
    CPacketQueue                _pqClient;          // Packet queue of client packets
    CXbdmPacket *               _ppktXbdm;          // Vector of CXbdmPacket structures
#endif

#ifdef XNET_FEATURE_STATS
    NICSTATS                    _NicStats;          // statistics
    NICSTATS                    _NicStatsLast;      // statistics at last report
    CTimer                      _timerStats;        // statistics timer
#endif

private:

    static KINTERRUPT           s_InterruptObject;

};

#endif  // _NICX_H_
