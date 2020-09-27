/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    amrtpnet.h

Abstract:

    Definitions for ActiveMovie RTP Network Filters.

--*/
 


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// RTP Filter UUIDS                                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#if !defined(_AMRTPNET_H_)

#if defined(INITGUID)
#define _AMRTPNET_H_
#endif

DEFINE_GUID(CLSID_RTPRenderFilter,
0x00d20921, 0x7e20, 0x11d0, 0xb2, 0x91, 0x00, 0xc0, 0x4f, 0xc3, 0x1d, 0x18);

DEFINE_GUID(CLSID_RTPRenderFilterProperties,
0x00d20923, 0x7e20, 0x11d0, 0xb2, 0x91, 0x00, 0xc0, 0x4f, 0xc3, 0x1d, 0x18);

DEFINE_GUID(CLSID_RTPSourceFilter,
0x00d20920, 0x7e20, 0x11d0, 0xb2, 0x91, 0x00, 0xc0, 0x4f, 0xc3, 0x1d, 0x18);

DEFINE_GUID(CLSID_RTPSourceFilterProperties,
0x00d20922, 0x7e20, 0x11d0, 0xb2, 0x91, 0x00, 0xc0, 0x4f, 0xc3, 0x1d, 0x18);

DEFINE_GUID(IID_IRTPStream, 
0x00d20915, 0x7e20, 0x11d0, 0xb2, 0x91, 0x00, 0xc0, 0x4f, 0xc3, 0x1d, 0x18);

DEFINE_GUID(IID_IRTCPStream,
0x4b5678ad, 0x3515, 0x11d1, 0x82, 0x96, 0x00, 0xc0, 0x4f, 0xc2, 0xf6, 0x78);

DEFINE_GUID(IID_IRTPParticipant,
0x4b5678ae, 0x3515, 0x11d1, 0x82, 0x96, 0x00, 0xc0, 0x4f, 0xc2, 0xf6, 0x78);
#endif


#ifndef _INC_AMRTPNET
#define _INC_AMRTPNET

#include <rtp.h>


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// IRTPStream Interface                                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

DECLARE_INTERFACE_(IRTPStream, IUnknown) {

    STDMETHOD(GetAddress)(THIS_
        LPWORD  pwRtpLocalPort,
		LPWORD  pwRtpRemotePort,
	    LPDWORD pdwRtpRemoteAddr
        ) PURE;

    STDMETHOD(SetAddress)(THIS_
        WORD  wRtpLocalPort,
		WORD  wRtpRemotePort,
		DWORD dwRtpRemoteAddr
        ) PURE;

	STDMETHOD(SelectLocalIPAddress)(THIS_
		DWORD dwLocalAddr
		) PURE;
	
	STDMETHOD(SelectSharedSockets)(THIS_
		DWORD  dwfSahredSockets
		) PURE;
	
	STDMETHOD(GetSessionID)(THIS_
		DWORD *pdwID
		) PURE;

    STDMETHOD(GetMulticastScope)(THIS_
        LPDWORD pdwMulticastScope
        ) PURE;

    STDMETHOD(SetMulticastScope)(THIS_
        DWORD   dwMulticastScope
        ) PURE;

    STDMETHOD(GetMulticastLoopBack)(THIS_
        DWORD *pdwMulticastLoopBack
        ) PURE;

    STDMETHOD(SetMulticastLoopBack)(THIS_
		DWORD dwMulticastLoopBack
        ) PURE;

    STDMETHOD(SetQOSByName)(THIS_
		char *psQOSname,
		DWORD fFailIfNoQOS
		) PURE;

    STDMETHOD(GetQOSstate)(THIS_
        DWORD *pdwQOSstate
        ) PURE;

    STDMETHOD(SetQOSstate)(THIS_
        DWORD dwQOSstate
        ) PURE;

	STDMETHOD(SetQOSSendIfAllowed)(THIS_
		DWORD dwEnable
		) PURE;

	STDMETHOD(SetQOSSendIfReceivers)(THIS_
		DWORD dwEnable
		) PURE;

	STDMETHOD(GetQOSEventMask)(THIS_
		DWORD *pdwQOSEventMask
		) PURE;
	
	STDMETHOD(ModifyQOSEventMask)(THIS_
		DWORD dwSelectItems,
		DWORD dwEnableItems
		) PURE;

    STDMETHOD(GetSessionClassPriority)(THIS_
		long *plSessionClass,
		long *plSessionPriority
        ) PURE;

    STDMETHOD(GetDataClock)(THIS_
        DWORD *pdwDataClock
        ) PURE;

    STDMETHOD(SetDataClock)(THIS_
        DWORD dwDataClock
        ) PURE;

    STDMETHOD(SetSessionClassPriority)(THIS_
		long lSessionClass,
		long lSessionPriority
        ) PURE;

	STDMETHOD(SelectLocalIPAddressToDest)(THIS_
		LPBYTE pLocSAddr,
		DWORD  dwLocSAddrLen,
		LPBYTE pDestSAddr,
		DWORD  dwDestSAddrLen
		) PURE;
	
#if 0
	STDMETHOD(GetSecurityState)(THIS_
	    DWORD *pdwSecurityState
        ) PURE;

	STDMETHOD(SetSecurityState)(THIS_
	    DWORD dwSecurityState
        ) PURE;

    STDMETHOD(GetSecurityKey)(THIS_
	    LPBYTE *pbSecurityKey,
		int *piSecurityKeyLen
        ) PURE;

    STDMETHOD(SetSecurityKey)(THIS_
	    LPBYTE *pbSecurityKey,
		int iSecurityKeyLen
        ) PURE;

	STDMETHOD(GetSecurityType)(THIS_
	    DWORD *pdwRTPSecurityType,
	    DWORD *pdwRTCPSecurityType
        ) PURE;

	STDMETHOD(SetSecurityType)(THIS_
	    DWORD dwRTPSecurityType,
	    DWORD dwRTCPSecurityType
        ) PURE;
#endif
};

//////////////////////////////////////////////////////////////////////
// RTCPStream Interface
//////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(IRTCPStream, IUnknown) {

    STDMETHOD(GetRTCPAddress)(THIS_
		LPWORD  pwRtcpLocalPort,
		LPWORD  pwRtcpRemotePort,
		LPDWORD pdwRtcpRemoteAddr
        ) PURE;

    STDMETHOD(SetRTCPAddress)(THIS_
        WORD  wRtcpLocalPort,
		WORD  wRtcpRemotePort,
		DWORD dwRtcpRemoteAddr
        ) PURE;

    STDMETHOD(GetRTCPMulticastScope)(THIS_
        LPDWORD pdwMulticastScope
        ) PURE;

    STDMETHOD(SetRTCPMulticastScope)(THIS_
        DWORD   dwMulticastScope
        ) PURE;

	STDMETHOD(GetRTCPEventMask)(THIS_
		DWORD *pdwRTCPEventMask
		) PURE;
	
	STDMETHOD(ModifyRTCPEventMask)(THIS_
		DWORD dwSelectItems,
		DWORD dwEnableItems
		) PURE;

	STDMETHOD(GetSessionID)(THIS_
		DWORD *pdwID
		) PURE;
	
	STDMETHOD(GetLocalSDESItem)(THIS_
		DWORD   dwSDESItem,
		LPBYTE  psSDESData,
		LPDWORD pdwSDESLen
		) PURE;
	
	STDMETHOD(SetLocalSDESItem)(THIS_
		DWORD  dwSDESItem,
		LPBYTE psSDESData,
		DWORD  dwSDESLen
		) PURE;
	
	STDMETHOD(GetRTCPSDESMask)(THIS_
		DWORD *pdwSdesMask
		) PURE;
	
	STDMETHOD(ModifyRTCPSDESMask)(THIS_
		DWORD dwSelectItems,
		DWORD dwEnableItems
		) PURE;

#if 0
	STDMETHOD(SendRTCPReports)(THIS_
		LPDWORD pdwSendState
		) PURE;

	STDMETHOD(RecvRTCPReports)(THIS_
		LPDWORD pdwRecvState
		) PURE;

	STDMETHOD(RecvAsyncRTCPEvents)(THIS_
		void (*pPostRTCPEvents)(DWORD, DWORD),
		DWORD Par1
		) PURE;

	STDMETHOD(RecvAsyncSDESEvents)(THIS_
		void (*pPostSDESEvents)(DWORD, DWORD),
		DWORD Par1
		) PURE;
#endif	
};

//////////////////////////////////////////////////////////////////////
// RTPParticipant Interface
//////////////////////////////////////////////////////////////////////
DECLARE_INTERFACE_(IRTPParticipant, IUnknown) {

	STDMETHOD(EnumParticipants)(THIS_
		LPDWORD pdwSSRC,
		LPDWORD pdwNum
		) PURE;

	STDMETHOD(GetParticipantSDESItem)(THIS_
		DWORD   dwSSRC,
		DWORD   dwSDESItem,
		LPBYTE  psSDESData,
		LPDWORD pdwLen
		) PURE;

	STDMETHOD(GetParticipantSDESAll)(THIS_
	    DWORD      dwSSRC,
		PSDES_DATA pSdes,
		DWORD      dwNum
		) PURE;

	STDMETHOD(GetParticipantAddress)(THIS_
		DWORD  dwSSRC,
		LPBYTE pbAddr,
		int    *piAddrLen
		) PURE;

    STDMETHOD(GetMaxQOSEnabledParticipants)(THIS_
        DWORD *pdwMaxParticipants,
        DWORD *pdwMaxBandwidth
        ) PURE;

    STDMETHOD(SetMaxQOSEnabledParticipants)(THIS_
		DWORD dwMaxParticipants,
        DWORD dwMaxBandwidth,
		DWORD fSharedStyle
        ) PURE;

    STDMETHOD(GetParticipantQOSstate)(THIS_
		DWORD dwSSRC,
        DWORD *pdwQOSstate
        ) PURE;

    STDMETHOD(SetParticipantQOSstate)(THIS_
		DWORD dwSSRC,
        DWORD dwQOSstate
        ) PURE;
	
    STDMETHOD(ModifyQOSList)(THIS_
		DWORD *pdwSSRCList,
		DWORD dwNumSSRC,
		DWORD dwOperation
        ) PURE;

    STDMETHOD(GetQOSList)(THIS_
		DWORD *pdwSSRCList,
		DWORD *pdwNumSSRC
        ) PURE;

#if 0
	STDMETHOD(EnumFirstParticipant)(THIS_
		LPDWORD pdwSSRC
		) PURE;

	STDMETHOD(EnumNextParticipant)(THIS_
		LPDWORD pdwSSRC
		) PURE;
#endif	
};

#endif // _INC_AMRTPNET
