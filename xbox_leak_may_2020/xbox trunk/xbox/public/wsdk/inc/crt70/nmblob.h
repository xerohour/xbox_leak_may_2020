//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1996-1999. All rights reserved.
//
//  MODULE: nmblob.h
//
//  External Header for the blob helpers
//=============================================================================

#ifndef _NMBLOB_H_
#define _NMBLOB_H_

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif


/////////////////////////////////////////////////////////////////////////////
// Blob Definitions - (they are all in npptypes.idl)
/////////////////////////////////////////////////////////////////////////////
#include "npptypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////////////
// Blob Constants
/////////////////////////////////////////////////////////////////////////////
#define INITIAL_RESTART_KEY 0xFFFFFFFF

/////////////////////////////////////////////////////////////////////////////
// Blob Core Helper Routines - these mess with internals (BlobCore.c)
/////////////////////////////////////////////////////////////////////////////
DWORD _cdecl CreateBlob(HBLOB * phBlob);

DWORD _cdecl DestroyBlob(HBLOB hBlob);

DWORD _cdecl SetStringInBlob(HBLOB  hBlob,         
                      const char * pOwnerName,    
                      const char * pCategoryName, 
                      const char * pTagName,      
                      const char * pString);      

DWORD _cdecl GetStringFromBlob(HBLOB   hBlob,
                        const char *  pOwnerName,
                        const char *  pCategoryName,
                        const char *  pTagName,
                        const char ** ppString);

DWORD _cdecl GetStringsFromBlob(HBLOB   hBlob,
                         const char * pRequestedOwnerName,
                         const char * pRequestedCategoryName,
                         const char * pRequestedTagName,
                         const char ** ppReturnedOwnerName,
                         const char ** ppReturnedCategoryName,
                         const char ** ppReturnedTagName,
                         const char ** ppReturnedString,
                         DWORD *       pRestartKey);

DWORD _cdecl RemoveFromBlob(HBLOB   hBlob,
                     const char *  pOwnerName,
                     const char *  pCategoryName,
                     const char *  pTagName);

DWORD _cdecl LockBlob(HBLOB hBlob);

DWORD _cdecl UnlockBlob(HBLOB hBlob);

DWORD _cdecl FindUnknownBlobCategories( HBLOB hBlob,
                                 const char *  pOwnerName,
                                 const char *  pKnownCategoriesTable[],
                                 HBLOB hUnknownCategoriesBlob);

/////////////////////////////////////////////////////////////////////////////
// Blob Helper Routines - these just call core helpers (BlobHelp.c)
/////////////////////////////////////////////////////////////////////////////
DWORD _cdecl MergeBlob(HBLOB hDstBlob,
                HBLOB hSrcBlob); 

DWORD _cdecl DuplicateBlob (HBLOB hSrcBlob,
					 HBLOB *hBlobThatWillBeCreated ); 

DWORD _cdecl WriteBlobToFile(HBLOB  hBlob,
                      const char * pFileName);

DWORD _cdecl ReadBlobFromFile(HBLOB* phBlob,
                       const char * pFileName);

DWORD _cdecl RegCreateBlobKey(HKEY hkey, const char* szBlobName, HBLOB hBlob);

DWORD _cdecl RegOpenBlobKey(HKEY hkey, const char* szBlobName, HBLOB* phBlob);

DWORD _cdecl MarshalBlob(HBLOB hBlob, DWORD* pSize, BYTE** ppBytes);

DWORD _cdecl UnMarshalBlob(HBLOB* phBlob, DWORD Size, BYTE* pBytes);

DWORD _cdecl SetDwordInBlob(HBLOB hBlob,
                     const char *  pOwnerName,
                     const char *  pCategoryName,
                     const char *  pTagName,
                     DWORD         Dword);

DWORD _cdecl GetDwordFromBlob(HBLOB   hBlob,
                       const char *  pOwnerName,
                       const char *  pCategoryName,
                       const char *  pTagName,
                       DWORD      *  pDword);

DWORD _cdecl SetBoolInBlob(HBLOB   hBlob,
                    const char *  pOwnerName,
                    const char *  pCategoryName,
                    const char *  pTagName,
                    BOOL          Bool);

DWORD _cdecl GetBoolFromBlob(HBLOB   hBlob,
                      const char *  pOwnerName,
                      const char *  pCategoryName,
                      const char *  pTagName,
                      BOOL       *  pBool);

DWORD _cdecl GetMacAddressFromBlob(HBLOB   hBlob,
                            const char *  pOwnerName,
                            const char *  pCategoryName,
                            const char *  pTagName,
                            BYTE *  pMacAddress);

DWORD _cdecl SetMacAddressInBlob(HBLOB   hBlob,
                          const char *  pOwnerName,
                          const char *  pCategoryName,
                          const char *  pTagName,
                          const BYTE *  pMacAddress);

DWORD _cdecl FindUnknownBlobTags( HBLOB hBlob,
                           const char *  pOwnerName,
                           const char *  pCategoryName,
                           const char *  pKnownTagsTable[],
                           HBLOB hUnknownTagsBlob);

/////////////////////////////////////////////////////////////////////////////
// Blob NPP Helper Routines - these are specific to the NPP interfaces (BlobNPP.c)
/////////////////////////////////////////////////////////////////////////////
DWORD _cdecl SetNetworkInfoInBlob(HBLOB hBlob, 
                           LPNETWORKINFO lpNetworkInfo);

DWORD _cdecl GetNetworkInfoFromBlob(HBLOB hBlob, 
                             LPNETWORKINFO lpNetworkInfo);

DWORD _cdecl CreateNPPInterface ( HBLOB hBlob,
                           REFIID iid,
                           void ** ppvObject);

DWORD _cdecl SetClassIDInBlob(HBLOB hBlob,
                       const char* pOwnerName,
                       const char* pCategoryName,
                       const char* pTagName,
                       const CLSID*  pClsID);

DWORD _cdecl GetClassIDFromBlob(HBLOB hBlob,
                         const char* pOwnerName,
                         const char* pCategoryName,
                         const char* pTagName,
                         CLSID * pClsID);

DWORD _cdecl SetNPPPatternFilterInBlob( HBLOB hBlob,
                                 LPEXPRESSION pExpression,
                                 HBLOB hErrorBlob);

DWORD _cdecl GetNPPPatternFilterFromBlob( HBLOB hBlob,
                                   LPEXPRESSION pExpression,
                                   HBLOB hErrorBlob);

DWORD _cdecl SetNPPAddressFilterInBlob( HBLOB hBlob,
                                 LPADDRESSTABLE pAddressTable);

DWORD _cdecl GetNPPAddressFilterFromBlob( HBLOB hBlob,
                                   LPADDRESSTABLE pAddressTable,
                                   HBLOB hErrorBlob);

DWORD _cdecl SetNPPTriggerInBlob( HBLOB hBlob,
                           LPTRIGGER   pTrigger,
                           HBLOB hErrorBlob);

DWORD _cdecl GetNPPTriggerFromBlob( HBLOB hBlob,
                             LPTRIGGER   pTrigger,
                             HBLOB hErrorBlob);

DWORD _cdecl SetNPPEtypeSapFilter(HBLOB  hBlob, 
                           WORD   nSaps,
                           WORD   nEtypes,
                           LPBYTE lpSapTable,
                           LPWORD lpEtypeTable,
                           DWORD  FilterFlags,
                           HBLOB  hErrorBlob);

DWORD _cdecl GetNPPEtypeSapFilter(HBLOB  hBlob, 
                           WORD   *pnSaps,
                           WORD   *pnEtypes,
                           LPBYTE *ppSapTable,
                           LPWORD *ppEtypeTable,
                           DWORD  *pFilterFlags,
                           HBLOB  hErrorBlob);

// GetNPPMacTypeAsNumber maps the tag NPP:NetworkInfo:MacType to the MAC_TYPE_*
// defined in the NPPTYPES.h.  If the tag is unavailable, the API returns MAC_TYPE_UNKNOWN.
DWORD _cdecl GetNPPMacTypeAsNumber(HBLOB hBlob, 
                            LPDWORD lpMacType);

// See if a remote catagory exists... and make sure that the remote computername
// isn't the same as the local computername.
BOOL  _cdecl IsRemoteNPP ( HBLOB hBLOB);

/////////////////////////////////////////////////////////////////////////////
// Blob Errors
/////////////////////////////////////////////////////////////////////////////

// basic blob errors
#define NMERR_BLOB_NOT_INITIALIZED          1000

#define NMERR_INVALID_BLOB                  1001

#define NMERR_UPLEVEL_BLOB                  1002

#define NMERR_BLOB_ENTRY_ALREADY_EXISTS     1003

#define NMERR_BLOB_ENTRY_DOES_NOT_EXIST     1004

#define NMERR_AMBIGUOUS_SPECIFIER           1005

#define NMERR_BLOB_OWNER_NOT_FOUND          1006

#define NMERR_BLOB_CATEGORY_NOT_FOUND       1007

#define NMERR_UNKNOWN_CATEGORY              1008

#define NMERR_UNKNOWN_TAG                   1009

#define NMERR_BLOB_CONVERSION_ERROR         1010

#define NMERR_ILLEGAL_TRIGGER               1011

#define NMERR_BLOB_STRING_INVALID           1012

// FINDER errors -- see also nmerr.h, finder.h
#define NMERR_UNABLE_TO_LOAD_LIBRARY        1013

#define NMERR_UNABLE_TO_GET_PROCADDR        1014

#define NMERR_CLASS_NOT_REGISTERED          1015

#define NMERR_INVALID_REMOTE_COMPUTERNAME   1016

#define NMERR_RPC_REMOTE_FAILURE            1017

//////////////////////////////////////////////////////////////////////////////
// npp tag definitions
//////////////////////////////////////////////////////////////////////////////
#define OWNER_NPP               "NPP"

#define CATEGORY_NETWORKINFO        "NetworkInfo"
#define TAG_RAS                         "Dial-up Connection"
#define TAG_MACTYPE						"MacType"
#define TAG_CURRENTADDRESS              "CurrentAddress"
#define TAG_LINKSPEED                   "LinkSpeed"
#define TAG_MAXFRAMESIZE                "MaxFrameSize"
#define TAG_FLAGS                       "Flags"
#define TAG_TIMESTAMPSCALEFACTOR        "TimeStampScaleFactor"
#define TAG_COMMENT                     "Comment"
#define TAG_NODENAME                    "NodeName"
#define TAG_NAME                        "Name"
#define TAG_FAKENPP                     "Fake"
#define TAG_PROMISCUOUS_MODE            "PMode"

#define CATEGORY_LOCATION           "Location"
#define TAG_MACADDRESS                  "MacAddress"
#define TAG_CLASSID                     "ClassID"
#define TAG_NAME                        "Name"

#define CATEGORY_CONFIG             "Config"
#define TAG_FRAME_SIZE                  "FrameSize"
#define TAG_UPDATE_FREQUENCY            "UpdateFreq"
#define TAG_BUFFER_SIZE                 "BufferSize"
#define TAG_DRIVE_LETTER                "DriveLetter"
#define TAG_PATTERN_DESIGNATOR          "PatternMatch"
#define TAG_PATTERN                     "Pattern"
#define TAG_ADDRESS_PAIR                "AddressPair"
#define TAG_CONNECTIONFLAGS             "ConnectionFlags"
#define TAG_ETYPES                      "Etypes"
#define TAG_SAPS                        "Saps"
#define TAG_NO_CONVERSATION_STATS       "NoConversationStats"
#define TAG_NO_STATS_FRAME              "NoStatsFrame"
#define TAG_DONT_DELETE_EMPTY_CAPTURE   "DontDeleteEmptyCapture"
#define TAG_WANT_PROTOCOL_INFO          "WantProtocolInfo"
#define TAG_INTERFACE_DELAYED_CAPTURE   "IDdC"
#define TAG_INTERFACE_REALTIME_CAPTURE  "IRTC"
#define TAG_INTERFACE_STATS             "ISts"
#define TAG_INTERFACE_EXPERT_STATS      "IESP"
#define TAG_LOCAL_ONLY                  "LocalOnly"
// Is_Remote is set to TRUE by NPPs that go remote.  Note that when you
//  are looking for a remote NPP, you probably also need to ask for 
//  blobs that have the TAG_GET_SPECIAL_BLOBS bool set
#define TAG_IS_REMOTE                   "IsRemote"


#define CATEGORY_TRIGGER            "Trigger"
#define TAG_TRIGGER                     "Trigger"

#define CATEGORY_FINDER             "Finder"
#define TAG_ROOT                        "Root"
#define TAG_PROCNAME                    "ProcName"
#define TAG_DISP_STRING                 "Display"
#define TAG_DLL_FILENAME                "DLLName"
#define TAG_GET_SPECIAL_BLOBS           "Specials"

#define CATEGORY_REMOTE              "Remote"
#define TAG_REMOTECOMPUTER              "RemoteComputer"
#define TAG_REMOTECLASSID               "ClassID"

#define CATEGORY_ESP                "ESP"
#define TAG_ESP_GENERAL_ACTIVE          "ESPGeneralActive"
#define TAG_ESP_PROTOCOL_ACTIVE         "ESPProtocolActive"
#define TAG_ESP_MAC_ACTIVE              "ESPMacActive"
#define TAG_ESP_MAC2MAC_ACTIVE          "ESPMac2MacActive"
#define TAG_ESP_IP_ACTIVE               "ESPIpActive"
#define TAG_ESP_IP2IP_ACTIVE            "ESPIp2IpActive"
#define TAG_ESP_IP_APP_ACTIVE           "ESPIpAppActive"
#define TAG_ESP_IPX_ACTIVE              "ESPIpxActive"
#define TAG_ESP_IPX2IPX_ACTIVE          "ESPIpx2IpxActive"
#define TAG_ESP_IPX_APP_ACTIVE          "ESPIpxAppActive"
#define TAG_ESP_DEC_ACTIVE              "ESPDecActive"
#define TAG_ESP_DEC2DEC_ACTIVE          "ESPDec2DecActive"
#define TAG_ESP_DEC_APP_ACTIVE          "ESPDecAppActive"
#define TAG_ESP_APPLE_ACTIVE            "ESPAppleActive"
#define TAG_ESP_APPLE2APPLE_ACTIVE      "ESPApple2AppleActive"
#define TAG_ESP_APPLE_APP_ACTIVE        "ESPAppleAppActive"

#define TAG_ESP_UTIL_SIZE               "ESPUtilSize"
#define TAG_ESP_TIME_SIZE               "ESPTimeSize"
#define TAG_ESP_BPS_SIZE				"ESPBpsSize"
#define TAG_ESP_BPS_THRESH				"ESPBpsThresh"
#define TAG_ESP_FPS_THRESH				"ESPFpsThresh"

#define TAG_ESP_MAC                     "ESPMac"
#define TAG_ESP_IPX                     "ESPIpx"
#define TAG_ESP_IPXSPX                  "ESPIpxSpx"
#define TAG_ESP_NCP                     "ESPNcp"
#define TAG_ESP_IP                      "ESPIp"
#define TAG_ESP_UDP                     "ESPUdp"
#define TAG_ESP_TCP                     "ESPTcp"
#define TAG_ESP_ICMP                    "ESPIcmp"
#define TAG_ESP_ARP                     "ESPArp"
#define TAG_ESP_RARP                    "ESPRarp"
#define TAG_ESP_APPLE                   "ESPApple"
#define TAG_ESP_AARP                    "ESPAarp"
#define TAG_ESP_DEC                     "ESPDec"
#define TAG_ESP_NETBIOS                 "ESPNetbios"
#define TAG_ESP_SNA                     "ESPSna"
#define TAG_ESP_BPDU                    "ESPBpdu"
#define TAG_ESP_LLC                     "ESPLlc"
#define TAG_ESP_RPL                     "ESPRpl"
#define TAG_ESP_BANYAN                  "ESPBanyan"
#define TAG_ESP_LANMAN                  "ESPLanMan"
#define TAG_ESP_SNMP                    "ESPSnmp"
#define TAG_ESP_X25                     "ESPX25"
#define TAG_ESP_XNS                     "ESPXns"
#define TAG_ESP_ISO                     "ESPIso"
#define TAG_ESP_UNKNOWN                 "ESPUnknown"
#define TAG_ESP_ATP                     "ESPAtp"
#define TAG_ESP_ADSP                    "ESPAdsp"

//////////////////////////////////////////////////////////////////////////////
// npp value definitions
//////////////////////////////////////////////////////////////////////////////
// Mac types
#define PROTOCOL_STRING_ETHERNET_TXT   "ETHERNET"
#define PROTOCOL_STRING_TOKENRING_TXT  "TOKENRING"
#define PROTOCOL_STRING_FDDI_TXT       "FDDI"
#define PROTOCOL_STRING_ATM_TXT        "ATM"

// lower protocols
#define PROTOCOL_STRING_IP_TXT         "IP"
#define PROTOCOL_STRING_IPX_TXT        "IPX"
#define PROTOCOL_STRING_XNS_TXT        "XNS"
#define PROTOCOL_STRING_VINES_IP_TXT   "VINES IP"

// upper protocols
#define PROTOCOL_STRING_ICMP_TXT       "ICMP"
#define PROTOCOL_STRING_TCP_TXT        "TCP"
#define PROTOCOL_STRING_UDP_TXT        "UDP"
#define PROTOCOL_STRING_SPX_TXT        "SPX"
#define PROTOCOL_STRING_NCP_TXT        "NCP"

// pseudo protocols
#define PROTOCOL_STRING_ANY_TXT        "ANY"
#define PROTOCOL_STRING_ANY_GROUP_TXT  "ANY GROUP"
#define PROTOCOL_STRING_HIGHEST_TXT    "HIGHEST"
#define PROTOCOL_STRING_LOCAL_ONLY_TXT "LOCAL ONLY"
#define PROTOCOL_STRING_UNKNOWN_TXT    "UNKNOWN"
#define PROTOCOL_STRING_DATA_TXT       "DATA"
#define PROTOCOL_STRING_FRAME_TXT      "FRAME"
#define PROTOCOL_STRING_NONE_TXT       "NONE"
#define PROTOCOL_STRING_EFFECTIVE_TXT  "EFFECTIVE"


#define ADDRESS_PAIR_INCLUDE_TXT    "INCLUDE"
#define ADDRESS_PAIR_EXCLUDE_TXT    "EXCLUDE"

#define INCLUDE_ALL_EXCEPT_TXT      "INCLUDE ALL EXCEPT"
#define EXCLUDE_ALL_EXCEPT_TXT      "EXCLUDE ALL EXCEPT"

#define PATTERN_MATCH_OR_TXT        "OR("
#define PATTERN_MATCH_AND_TXT       "AND("

#define TRIGGER_PATTERN_TXT               "PATTERN MATCH"
#define TRIGGER_BUFFER_TXT                "BUFFER CONTENT"

#define TRIGGER_NOTIFY_TXT      "NOTIFY"
#define TRIGGER_STOP_TXT        "STOP"
#define TRIGGER_PAUSE_TXT       "PAUSE"

#define TRIGGER_25_PERCENT_TXT  "25 PERCENT"
#define TRIGGER_50_PERCENT_TXT  "50 PERCENT"
#define TRIGGER_75_PERCENT_TXT  "75 PERCENT"
#define TRIGGER_100_PERCENT_TXT "100 PERCENT"

#define PATTERN_MATCH_NOT_TXT   "NOT"


#ifdef __cplusplus
}
#endif

#endif // _BLOBHELP_H_