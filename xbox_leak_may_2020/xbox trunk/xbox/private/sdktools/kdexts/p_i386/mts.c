/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    mts.c

Abstract:

    MikeTs's little KD extension.

Author:

    Michael Tsang (mikets) 18-November-1997

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

VOID ReadPci (
    IN PPCI_TYPE1_CFG_BITS      PciCfg1,
    OUT PUCHAR                  Buffer,
    IN ULONG                    Offset,
    IN ULONG                    Length
    );

#define CSOFFSET(x)     ((DWORD)(&((PPCI_COMMON_CONFIG)0)->x))

/*** PCI ConfigSpace Registers
 */

//PCI Command Register
char *ppszCmdRegNames[] = {"FB2BEnable", "SERREnable", "WaitCycle",
                           "PERREnable", "VGASnoop", "MemWriteEnable",
                           "SpecialCycle", "BusInitiate", "MemSpaceEnable",
                           "IOSpaceEnable"};
FMTBITS fmtCmdReg =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0x03ff, ppszCmdRegNames, NULL
};

//PCI Status Register
char *ppszStatusRegNames[] = {"PERR", "SERR", "InitiatorAbort", "TargetAbort",
                              "TargetDevAbort", "DataPERR", "FB2BCapable",
                              "UDFSupported", "66MHzCapable",
                              "CapList"};
FMTBITS fmt1StatusReg =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0xf9f0, ppszStatusRegNames, NULL
};

FMTNUM fmt2StatusReg =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "DEVSELTiming:"},
    0x0600, 9, cszHexFmt
};

//Cache Line Size Register
char szBurstDisabled[] = "BurstDisabled";
char szBurst4DW[] = "Burst4DW";
char *ppszCacheSizeRegNames[] = {szBurstDisabled, szBurstDisabled,
                                 szBurstDisabled, szBurstDisabled,
                                 szBurst4DW, szBurst4DW, szBurst4DW, szBurst4DW};
char szBurst8DW[] = "Burst8DW";
FMTENUM fmtCacheSizeReg =
{
    {FMT_ENUM, UNIT_BYTE, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xff, 0, 0, 0x07, ppszCacheSizeRegNames, szBurst8DW
};

char *ppszBridgeCtrlRegNames[] = {"WritePostEnable", "Mem1Prefetch",
                                  "Mem0Prefectch", "IRQRoutingEnable", "CBRst",
                                  "MasterAbort", "VGAEnable", "ISAEnable",
                                  "SERREnable", "PERREnable"};
FMTBITS fmtBridgeCtrlReg =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0x07ef, ppszBridgeCtrlRegNames, NULL
};

VOID PrintClassInfo(PBYTE pb, DWORD dwReg);

FMT afmtCfgSpaceCommon[] =
{
    {"VendorID=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"DeviceID=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"Command=",       (PFMTHDR)&fmtCmdReg,        NULL},
    {"Status=",        (PFMTHDR)&fmt1StatusReg,    NULL},
    {NULL,             (PFMTHDR)&fmt2StatusReg,    NULL},
    {"RevisionID=",    (PFMTHDR)&fmtHexByte,       NULL},
    {"ProgIF=",        (PFMTHDR)&fmtHexByteNoEOL,  PrintClassInfo},
    {"SubClass=",      (PFMTHDR)&fmtHexByteNoEOL,  PrintClassInfo},
    {"BaseClass=",     (PFMTHDR)&fmtHexByteNoEOL,  PrintClassInfo},
    {"CacheLineSize=", (PFMTHDR)&fmtCacheSizeReg,  NULL},
    {"LatencyTimer=",  (PFMTHDR)&fmtHexByte,       NULL},
    {"HeaderType=",    (PFMTHDR)&fmtHexByte,       NULL},
    {"BIST=",          (PFMTHDR)&fmtHexByte,       NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtCfgSpaceType0[] =
{
    {"BAR0=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"BAR1=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"BAR2=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"BAR3=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"BAR4=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"BAR5=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"CBCISPtr=",      (PFMTHDR)&fmtHexDWord,      NULL},
    {"SubSysVenID=",   (PFMTHDR)&fmtHexWord,       NULL},
    {"SubSysID=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"ROMBAR=",        (PFMTHDR)&fmtHexDWord,      NULL},
    {"CapPtr=",        (PFMTHDR)&fmtHexByte,       NULL},
    {cszLabelReserved, (PFMTHDR)&fmtHexByte,       NULL},
    {cszLabelReserved, (PFMTHDR)&fmtHexWord,       NULL},
    {cszLabelReserved, (PFMTHDR)&fmtHexDWord,      NULL},
    {"IntLine=",       (PFMTHDR)&fmtHexByte,       NULL},
    {"IntPin=",        (PFMTHDR)&fmtHexByte,       NULL},
    {"MinGnt=",        (PFMTHDR)&fmtHexByte,       NULL},
    {"MaxLat=",        (PFMTHDR)&fmtHexByte,       NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtCfgSpaceType1[] =
{
    {"BAR0=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"BAR1=",          (PFMTHDR)&fmtHexDWord,      NULL},
    {"PriBusNum=",     (PFMTHDR)&fmtHexByte,       NULL},
    {"SecBusNum=",     (PFMTHDR)&fmtHexByte,       NULL},
    {"SubBusNum=",     (PFMTHDR)&fmtHexByte,       NULL},
    {"SecLatencyTmr=", (PFMTHDR)&fmtHexByte,       NULL},
    {"IOBase=",        (PFMTHDR)&fmtHexByte,       NULL},
    {"IOLimit=",       (PFMTHDR)&fmtHexByte,       NULL},
    {"SecStatus=",     (PFMTHDR)&fmt1StatusReg,    NULL},
    {NULL,             (PFMTHDR)&fmt2StatusReg,    NULL},
    {"MemBase=",       (PFMTHDR)&fmtHexWord,       NULL},
    {"MemLimit=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"PrefMemBase=",   (PFMTHDR)&fmtHexWord,       NULL},
    {"PrefMemLimit=",  (PFMTHDR)&fmtHexWord,       NULL},
    {"PrefBaseHi=",    (PFMTHDR)&fmtHexDWord,      NULL},
    {"PrefLimitHi=",   (PFMTHDR)&fmtHexDWord,      NULL},
    {"IOBaseHi=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"IOLimitHi=",     (PFMTHDR)&fmtHexWord,       NULL},
    {"CapPtr=",        (PFMTHDR)&fmtHexByte,       NULL},
    {cszLabelReserved, (PFMTHDR)&fmtHexByte,       NULL},
    {cszLabelReserved, (PFMTHDR)&fmtHexWord,       NULL},
    {"ROMBAR=",        (PFMTHDR)&fmtHexDWord,      NULL},
    {"IntLine=",       (PFMTHDR)&fmtHexByte,       NULL},
    {"IntPin=",        (PFMTHDR)&fmtHexByte,       NULL},
    {"BridgeCtrl=",    (PFMTHDR)&fmtBridgeCtrlReg, NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtCfgSpaceType2[] =
{
    {"RegBaseAddr=",   (PFMTHDR)&fmtHexDWord,      NULL},
    {"CapPtr=",        (PFMTHDR)&fmtHexByte,       NULL},
    {cszLabelReserved, (PFMTHDR)&fmtHexByte,       NULL},
    {"SecStatus=",     (PFMTHDR)&fmt1StatusReg,    NULL},
    {NULL,             (PFMTHDR)&fmt2StatusReg,    NULL},
    {"PCIBusNum=",     (PFMTHDR)&fmtHexByte,       NULL},
    {"CBBusNum=",      (PFMTHDR)&fmtHexByte,       NULL},
    {"SubBusNum=",     (PFMTHDR)&fmtHexByte,       NULL},
    {"CBLatencyTimer=",(PFMTHDR)&fmtHexByte,       NULL},
    {"MemBase0=",      (PFMTHDR)&fmtHexDWord,      NULL},
    {"MemLimit0=",     (PFMTHDR)&fmtHexDWord,      NULL},
    {"MemBase1=",      (PFMTHDR)&fmtHexDWord,      NULL},
    {"MemLimit1=",     (PFMTHDR)&fmtHexDWord,      NULL},
    {"IOBase0=",       (PFMTHDR)&fmtHexDWord,      NULL},
    {"IOLimit0=",      (PFMTHDR)&fmtHexDWord,      NULL},
    {"IOBase1=",       (PFMTHDR)&fmtHexDWord,      NULL},
    {"IOLimit1=",      (PFMTHDR)&fmtHexDWord,      NULL},
    {"IntLine=",       (PFMTHDR)&fmtHexByte,       NULL},
    {"IntPin=",        (PFMTHDR)&fmtHexByte,       NULL},
    {"BridgeCtrl=",    (PFMTHDR)&fmtBridgeCtrlReg, NULL},
    {"SubSysVenID=",   (PFMTHDR)&fmtHexWord,       NULL},
    {"SubSysID=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"LegacyBaseAddr=",(PFMTHDR)&fmtHexDWord,      NULL},
    {cszNull,          (PFMTHDR)&fmt6HexDWord,     NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtCfgSpaceUnknown[] =
{
    {cszNull,          (PFMTHDR)&fmt4HexDWord,     NULL},       //0x10-0x1f
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},       //0x20-0x3f
    {NULL,             NULL,                       NULL}
};

FMT afmtCfgSpace40[] =
{
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtCfgSpace60[] =
{
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},       //0x60-0x7f
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},       //0x80-0x9f
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},       //0xa0-0xbf
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},       //0xc0-0xdf
    {cszNull,          (PFMTHDR)&fmt8HexDWord,     NULL},       //0xe0-0xff
    {NULL,             NULL,                       NULL}
};

char szUnknown[] = "Unknown";
char *ppszCapIDNames[] = {"PwrMgmt", "AGP", szUnknown, szUnknown, "MSI"};
FMTENUM fmtCapID =
{
    {FMT_ENUM, UNIT_BYTE, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xff, 0, 0x01, 0x05, ppszCapIDNames, szUnknown
};

FMT afmtCapHeader[] =
{
    {"CapID=",         (PFMTHDR)&fmtCapID,         NULL},
    {"NextPtr=",       (PFMTHDR)&fmtHexByte,       NULL},
    {NULL,             NULL,                       NULL}
};

char *ppszPwrMgmtCaps[] = {"PMED3Cold", "PMED3Hot", "PMED2", "PMED1",
                           "PMED0", "D2Support", "D1Support", "DSI",
                           "AUXPWR", "PMECLK"};
FMTBITS fmtPwrMgmtCaps =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0xfe38, ppszPwrMgmtCaps, NULL
};

FMTNUM fmtPwrMgmtVer =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "Ver:"},
    0x0007, 0, cszHexFmt
};

char *ppszPwrMgmtCtrl1[] = {"PMESTAT", "PMEEnable"};
FMTBITS fmtPwrMgmtCtrl1 =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0x8100, ppszPwrMgmtCtrl1, NULL
};

FMTNUM fmtPwrMgmtCtrl2 =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, "DataScale:"},
    0x6000, 13, cszHexFmt
};

FMTNUM fmtPwrMgmtCtrl3 =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, "DataSel:"},
    0x1e00, 9, cszHexFmt
};

char *ppszDStateNames[] = {"D0", "D1", "D2", "D3Hot"};
FMTENUM fmtPwrMgmtCtrl4 =
{
    {FMT_ENUM, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    0x0003, 0, 0x00, 0x03, ppszDStateNames, szUnknown
};

char *ppszPwrMgmtBridge[] = {"BPCCEnable", "D3HotStopClock"};
FMTBITS fmtPwrMgmtBridge =
{
    {FMT_BITS, UNIT_BYTE, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xc0, ppszPwrMgmtBridge, NULL
};

FMT afmtPwrMgmt[] =
{
    {"PwrMgmtCap=",    (PFMTHDR)&fmtPwrMgmtCaps,   NULL},
    {NULL,             (PFMTHDR)&fmtPwrMgmtVer,    NULL},
    {"PwrMgmtCtrl=",   (PFMTHDR)&fmtPwrMgmtCtrl1,  NULL},
    {NULL,             (PFMTHDR)&fmtPwrMgmtCtrl2,  NULL},
    {NULL,             (PFMTHDR)&fmtPwrMgmtCtrl3,  NULL},
    {NULL,             (PFMTHDR)&fmtPwrMgmtCtrl4,  NULL},
    {"PwrMgmtBridge=", (PFMTHDR)&fmtPwrMgmtBridge, NULL},
    {NULL,             NULL,                       NULL}
};

FMTNUM fmtAGPVerMajor =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, "Major:"},
    0x00f0, 4, cszHexFmt
};

FMTNUM fmtAGPVerMinor =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "Minor:"},
    0x000f, 0, cszHexFmt
};

FMTNUM fmtAGPStatus1 =
{
    {FMT_NUMBER, UNIT_DWORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, "MaxRQDepth:"},
    0xff000000, 24, cszHexFmt
};

char *ppszAGPStatus2[] = {"SBA"};
FMTBITS fmtAGPStatus2 =
{
    {FMT_BITS, UNIT_DWORD, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, NULL},
    0x00000200, ppszAGPStatus2, NULL
};

FMTNUM fmtAGPStatus3 =
{
    {FMT_NUMBER, UNIT_DWORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "Rate:"},
    0x00000003, 0, cszHexFmt
};

FMTNUM fmtAGPCommand1 =
{
    {FMT_NUMBER, UNIT_DWORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, "RQDepth:"},
    0xff000000, 24, cszHexFmt
};

char *ppszAGPCommand2[] = {"SBAEnable", "AGPEnable"};
FMTBITS fmtAGPCommand2 =
{
    {FMT_BITS, UNIT_DWORD, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, NULL},
    0x00000300, ppszAGPCommand2, NULL
};

FMTNUM fmtAGPCommand3 =
{
    {FMT_NUMBER, UNIT_DWORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "Rate:"},
    0x00000007, 0, cszHexFmt
};

FMT afmtAGP[] =
{
    {"Version=",       (PFMTHDR)&fmtAGPVerMajor,   NULL},
    {NULL,             (PFMTHDR)&fmtAGPVerMinor,   NULL},
    {"Status=",        (PFMTHDR)&fmtAGPStatus1,    NULL},
    {NULL,             (PFMTHDR)&fmtAGPStatus2,    NULL},
    {NULL,             (PFMTHDR)&fmtAGPStatus3,    NULL},
    {"Command=",       (PFMTHDR)&fmtAGPCommand1,   NULL},
    {NULL,             (PFMTHDR)&fmtAGPCommand2,   NULL},
    {NULL,             (PFMTHDR)&fmtAGPCommand3,   NULL},
    {NULL,             NULL,                       NULL}
};

char *ppszMsgCtrl1[] = {"64BitCapable", "MSIEnable"};
FMTBITS fmtMSIMsgCtrl1 =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0x0081, ppszMsgCtrl1, NULL
};

FMTNUM fmtMSIMsgCtrl2 =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, "MultipleMsgEnable:"},
    0x0070, 4, cszHexFmt
};

FMTNUM fmtMSIMsgCtrl3 =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "MultipleMsgCapable:"},
    0x000e, 1, cszHexFmt
};

FMT afmtMSI[] =
{
    {"MsgCtrl=",       (PFMTHDR)&fmtMSIMsgCtrl1,   NULL},
    {NULL,             (PFMTHDR)&fmtMSIMsgCtrl2,   NULL},
    {NULL,             (PFMTHDR)&fmtMSIMsgCtrl3,   NULL},
    {"MsgAddr=",       (PFMTHDR)&fmtHexDWord,      NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtMSI32[] =
{
    {"MsgData=",       (PFMTHDR)&fmtHexWord,       NULL},
    {NULL,             NULL,                       NULL}
};

FMT afmtMSI64[] =
{
    {"MsgAddrHi=",     (PFMTHDR)&fmtHexDWord,      NULL},
    {"MsgData=",       (PFMTHDR)&fmtHexWord,       NULL},
    {NULL,             NULL,                       NULL}
};

/*** CardBus Registers
 */

//Socket Event Register
char szPowerCycle[] = "PowerCycle";
char szCSTSCHG[] = "CSTSCHG";
char *ppszEventRegNames[] = {szPowerCycle, "/CCD2", "/CCD1", szCSTSCHG};
FMTBITS fmtSktEventReg =
{
    {FMT_BITS, UNIT_DWORD, 1, 0, 1, NULL, NULL, NULL},
    SKTEVENT_MASK, ppszEventRegNames, NULL
};

//Socket Mask Register
char *ppszMaskRegNames1[] = {szPowerCycle, szCSTSCHG};
char *ppszMaskRegNames2[] = {"CSCDisabled", cszUndefined, cszUndefined,
                             "CSCEnabled"};
FMTBITS fmt1SktMaskReg =
{
    {FMT_BITS, UNIT_DWORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    SKTMSK_POWERCYCLE | SKTMSK_CSTSCHG, ppszMaskRegNames1, NULL
};
FMTENUM fmt2SktMaskReg =
{
    {FMT_ENUM, UNIT_DWORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    SKTMSK_CCD, 1, 0, 3, ppszMaskRegNames2, NULL
};

//Socket Present State Register
char szCardMayPresent[] = "CardMayPresent";
char *ppszStateRegNames1[] = {"YVSkt", "XVSkt", "3VSkt", "5VSkt", "YVCard",
                              "XVCard", "3VCard", "5VCard", "BadVccReq",
                              "DataLoss", "NotACard", "CardInt", "CBCard",
                              "R2Card", szPowerCycle, szCSTSCHG};
char *ppszStateRegNames2[] = {"CardPresent", szCardMayPresent, szCardMayPresent,
                              "NoCard"};
FMTBITS fmt1SktStateReg =
{
    {FMT_BITS, UNIT_DWORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0xf0003ff9, ppszStateRegNames1, NULL
};
FMTENUM fmt2SktStateReg =
{
    {FMT_ENUM, UNIT_DWORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    0x00000006, 1, 0, 3, ppszStateRegNames2, NULL
};

//Socket Force Event Register
FMTNUM fmtSktForceReg =
{
    {FMT_NUMBER, UNIT_DWORD, 1, 0, 1, NULL, NULL, NULL},
    0xffffffff, 0, cszDWordFmt
};

//Socket Control Register
char *ppszCtrlRegNames1[] = {"ClockStopEnabled"};
char *ppszCtrlRegNames2[] = {"Vcc:Off", cszReserved, "Vcc:5V", "Vcc:3.3V",
                             "Vcc:X.XV", "Vcc:Y.YV"};
char *ppszCtrlRegNames3[] = {"Vpp:Off", "Vpp:12V", "Vpp:5V", "Vpp:3.3V",
                             "Vpp:X.XV", "Vpp:Y.YV"};
FMTBITS fmt1SktCtrlReg =
{
    {FMT_BITS, UNIT_DWORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    SKTPOWER_STOPCLOCK, ppszCtrlRegNames1, NULL
};
FMTENUM fmt2SktCtrlReg =
{
    {FMT_ENUM, UNIT_DWORD, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, NULL},
    SKTPOWER_VCC_CONTROL, 4, 0, 5, ppszCtrlRegNames2, cszReserved
};
FMTENUM fmt3SktCtrlReg =
{
    {FMT_ENUM, UNIT_DWORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    SKTPOWER_VPP_CONTROL, 0, 0, 5, ppszCtrlRegNames3, cszReserved
};

FMT afmtCBRegs[] =
{
    {"SktEvent=", (PFMTHDR)&fmtSktEventReg,  NULL},
    {"SktMask=",  (PFMTHDR)&fmt1SktMaskReg,  NULL},
    {NULL,        (PFMTHDR)&fmt2SktMaskReg,  NULL},
    {"SktState=", (PFMTHDR)&fmt1SktStateReg, NULL},
    {NULL,        (PFMTHDR)&fmt2SktStateReg, NULL},
    {"SktForce=", (PFMTHDR)&fmtSktForceReg,  NULL},
    {"SktCtrl=",  (PFMTHDR)&fmt1SktCtrlReg,  NULL},
    {NULL,        (PFMTHDR)&fmt2SktCtrlReg,  NULL},
    {NULL,        (PFMTHDR)&fmt3SktCtrlReg,  NULL},
    {NULL,        NULL,                      NULL}
};

/*** ExCA Registers
 */

//ID/Revision Register
char *ppszIDRegNames[] = {"IOOnly", "MemOnly", "IO&Mem"};
FMTENUM fmt1IDRevReg =
{
    {FMT_ENUM, UNIT_BYTE, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0xc0, 6, 0, 2, ppszIDRegNames, cszReserved
};
FMTNUM fmt2IDRevReg =
{
    {FMT_NUMBER, UNIT_BYTE, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "Rev:"},
    0x0f, 0, "%x"
};

//Interface Status Register
char *ppszISRegNames[] = {"VppValid", "PowerActive", "Ready", "WP", "CD2",
                          "CD1", "BVD2", "BVD1"};
FMTBITS fmtISReg =
{
    {FMT_BITS, UNIT_BYTE, 1, 0, 1, NULL, NULL, NULL},
    0xff, ppszISRegNames, NULL
};

//Power and RESETDRV Control Register
char *ppszPwrRegNames1[] = {"OutputEnable", "RESETDRVDisabled",
                            "AutoPwrEnabled", "PwrEnable"};
char *ppszPwrRegNames2[] = {"Vpp2=Off", "Vpp2=Vcc", "Vpp2=Vpp"};
char *ppszPwrRegNames3[] = {"Vpp1=Off", "Vpp1=Vcc", "Vpp1=Vpp"};
FMTBITS fmt1PwrReg =
{
    {FMT_BITS, UNIT_BYTE, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0xf0, ppszPwrRegNames1, NULL
};
FMTENUM fmt2PwrReg =
{
    {FMT_ENUM, UNIT_BYTE, 1, FMTF_MIDDLE_FIELD, 1, NULL, NULL, NULL},
    0x0c, 2, 0, 2, ppszPwrRegNames2, cszReserved
};
FMTENUM fmt3PwrReg =
{
    {FMT_ENUM, UNIT_BYTE, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    0x03, 0, 0, 2, ppszPwrRegNames3, cszReserved
};

//Interrupt and General Control Register
char *ppszIntCtrlRegNames[] = {"RingIndEnable", "ResetOff", "IOCard",
                               "INTREnable"};
FMTBITS fmt1IntCtrlReg =
{
    {FMT_BITS, UNIT_BYTE, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0xf0, ppszIntCtrlRegNames, NULL
};
FMTNUM fmt2IntCtrlReg =
{
    {FMT_NUMBER, UNIT_BYTE, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "CardIRQ:"},
    0x0f, 0, cszHexFmt
};

//Card Status Change Register
char *ppszCSCRegNames[] = {"CDC", "RDYC", "BATTWARN", "BATTDEAD"};
FMTBITS fmtCSCReg =
{
    {FMT_BITS, UNIT_BYTE, 1, 0, 1, NULL, NULL, NULL},
    0x0f, ppszCSCRegNames, NULL
};

//Card Status Change Interrupt Configuration Register
char *ppszCSCIntCfgRegNames[] = {"CDEnable", "RDYEnable", "BattWarnEnable",
                                 "BattDeadEnable"};
FMTNUM fmt1CSCIntCfgReg =
{
    {FMT_NUMBER, UNIT_BYTE, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, "CSCIRQ:"},
    0xf0, 4, cszHexFmt
};
FMTBITS fmt2CSCIntCfgReg =
{
    {FMT_BITS, UNIT_BYTE, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    0x0f, ppszCSCIntCfgRegNames, NULL
};

//Address Window Enable Register
char *ppszWinEnableRegNames[] = {"IO1Enable", "IO0Enable", "DecodeA23-A12",
                                 "Mem4Enable", "Mem3Enable", "Mem2Enable",
                                 "Mem1Enable", "Mem0Enable"};
FMTBITS fmtWinEnableReg =
{
    {FMT_BITS, UNIT_BYTE, 1, 0, 1, NULL, NULL, NULL},
    0xff, ppszWinEnableRegNames, NULL
};

//I/O Control Register
char *ppszIOCtrlRegNames[] = {"IO1WS", "IO1ZeroWS", "IO1CardIOCS", "IO116Bit",
                              "IO0WS", "IO0ZeroWS", "IO0CardIOCS", "IO016Bit"};
FMTBITS fmtIOCtrlReg =
{
    {FMT_BITS, UNIT_BYTE, 1, 0, 1, NULL, NULL, NULL},
    0xff, ppszIOCtrlRegNames, NULL
};

//Memory Address Registers
char *ppszMemStartNames[] = {"16Bit", "ZeroWS"};
char *ppszMemOffsetNames[] = {"WP", "RegActive"};
FMTNUM fmtMemAddr =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0x0fff, 0, cszWordFmt
};

FMTBITS fmtMemStartReg =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    0xc000, ppszMemStartNames, NULL
};

FMTNUM fmtMemEndReg =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, "WaitState:"},
    0xc000, 14, cszDecFmt
};

FMTNUM fmt1MemOffsetReg =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_FIRST_FIELD, 1, NULL, NULL, NULL},
    0x3fff, 0, cszWordFmt
};

FMTBITS fmt2MemOffsetReg =
{
    {FMT_BITS, UNIT_WORD, 1, FMTF_LAST_FIELD, 1, NULL, NULL, NULL},
    0xc000, ppszMemOffsetNames, NULL
};

FMT afmtExCARegs[] =
{
    {"IDRev=",            (PFMTHDR)&fmt1IDRevReg,     NULL},
    {NULL,                (PFMTHDR)&fmt2IDRevReg,     NULL},
    {"IFStatus=",         (PFMTHDR)&fmtISReg,         NULL},
    {"PwrCtrl=",          (PFMTHDR)&fmt1PwrReg,       NULL},
    {NULL,                (PFMTHDR)&fmt2PwrReg,       NULL},
    {NULL,                (PFMTHDR)&fmt3PwrReg,       NULL},
    {"IntGenCtrl=",       (PFMTHDR)&fmt1IntCtrlReg,   NULL},
    {NULL,                (PFMTHDR)&fmt2IntCtrlReg,   NULL},
    {"CardStatusChange=", (PFMTHDR)&fmtCSCReg,        NULL},
    {"IntConfig=",        (PFMTHDR)&fmt1CSCIntCfgReg, NULL},
    {NULL,                (PFMTHDR)&fmt2CSCIntCfgReg, NULL},
    {"WinEnable=",        (PFMTHDR)&fmtWinEnableReg,  NULL},
    {"IOWinCtrl=",        (PFMTHDR)&fmtIOCtrlReg,     NULL},
    {"IOWin0Start=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"IOWin0End=",        (PFMTHDR)&fmtHexWord,       NULL},
    {"IOWin1Start=",      (PFMTHDR)&fmtHexWord,       NULL},
    {"IOWin1End=",        (PFMTHDR)&fmtHexWord,       NULL},
    {"MemWin0Start=",     (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemStartReg,   NULL},
    {"MemWin0End=",       (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemEndReg,     NULL},
    {"MemWin0Offset=",    (PFMTHDR)&fmt1MemOffsetReg, NULL},
    {NULL,                (PFMTHDR)&fmt2MemOffsetReg, NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {"MemWin1Start=",     (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemStartReg,   NULL},
    {"MemWin1End=",       (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemEndReg,     NULL},
    {"MemWin1Offset=",    (PFMTHDR)&fmt1MemOffsetReg, NULL},
    {NULL,                (PFMTHDR)&fmt2MemOffsetReg, NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {"MemWin2Start=",     (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemStartReg,   NULL},
    {"MemWin2End=",       (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemEndReg,     NULL},
    {"MemWin2Offset=",    (PFMTHDR)&fmt1MemOffsetReg, NULL},
    {NULL,                (PFMTHDR)&fmt2MemOffsetReg, NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {"MemWin3Start=",     (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemStartReg,   NULL},
    {"MemWin3End=",       (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemEndReg,     NULL},
    {"MemWin3Offset=",    (PFMTHDR)&fmt1MemOffsetReg, NULL},
    {NULL,                (PFMTHDR)&fmt2MemOffsetReg, NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {"MemWin4Start=",     (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemStartReg,   NULL},
    {"MemWin4End=",       (PFMTHDR)&fmtMemAddr,       NULL},
    {NULL,                (PFMTHDR)&fmtMemEndReg,     NULL},
    {"MemWin4Offset=",    (PFMTHDR)&fmt1MemOffsetReg, NULL},
    {NULL,                (PFMTHDR)&fmt2MemOffsetReg, NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {cszLabelReserved,    (PFMTHDR)&fmtHexWord,       NULL},
    {NULL,                NULL,                       NULL}
};

FMT afmtExCAHiRegs[] =
{
    {"MemWin0High=",      (PFMTHDR)&fmtHexByte,       NULL},
    {"MemWin1High=",      (PFMTHDR)&fmtHexByte,       NULL},
    {"MemWin2High=",      (PFMTHDR)&fmtHexByte,       NULL},
    {"MemWin3High=",      (PFMTHDR)&fmtHexByte,       NULL},
    {"MemWin4High=",      (PFMTHDR)&fmtHexByte,       NULL},
    {"CLIOWin0High=",     (PFMTHDR)&fmtHexByte,       NULL},
    {"CLIOWin1High=",     (PFMTHDR)&fmtHexByte,       NULL},
    {NULL,                NULL,                       NULL}
};

/***LP  ReadExCAByte - Read ExCA byte register
 *
 *  ENTRY
 *      dwBaseAddr - Base port address
 *      dwReg - register offset
 *
 *  EXIT
 *      returns data read
 */

BYTE ReadExCAByte(DWORD dwBaseAddr, DWORD dwReg)
{
    BYTE bData;
    ULONG ulSize;

    ulSize = sizeof(BYTE);
    WriteIoSpace(dwBaseAddr, dwReg, &ulSize);
    ulSize = sizeof(BYTE);
    ReadIoSpace(dwBaseAddr + 1, (PULONG)&bData, &ulSize);

    return bData;
}       //ReadExCAByte

/***LP  GetClassDesc - Get class description string
 *
 *  ENTRY
 *      bBaseClass - Base Class code
 *      bSubClass - Sub Class code
 *      bProgIF - Program Interface code
 *
 *  EXIT-SUCCESS
 *      returns pointer to description string
 *  EXIT-FAILURE
 *      returns NULL
 */

PSZ GetClassDesc(BYTE bBaseClass, BYTE bSubClass, BYTE bProgIF)
{
    char *psz = NULL;
    int i;
    static struct classtab_s
    {
        BYTE bBaseClass;
        BYTE bSubClass;
        BYTE bProgIF;
        PSZ  pszDesc;
    } ClassTable[] =
        {
            {0x00, 0xff, 0xff, "Legacy controller"},
            {0x00, 0x00, 0x00, "All legacy controller except VGA"},
            {0x00, 0x01, 0x00, "All legacy VGA device"},

            {0x01, 0xff, 0xff, "Mass storage controller"},
            {0x01, 0x00, 0x00, "SCSI bus controller"},
            {0x01, 0x01, 0xff, "IDE controller"},
            {0x01, 0x02, 0x00, "Floppy disk controller"},
            {0x01, 0x03, 0x00, "IPI bus controller"},
            {0x01, 0x04, 0x00, "RAID controller"},
            {0x01, 0x80, 0x00, "Other mass storage controller"},

            {0x02, 0xff, 0xff, "Network controller"},
            {0x02, 0x00, 0x00, "Ethernet controller"},
            {0x02, 0x01, 0x00, "Token ring controller"},
            {0x02, 0x02, 0x00, "FDDI controller"},
            {0x02, 0x03, 0x00, "ATM controller"},
            {0x02, 0x80, 0x00, "Other network controller"},

            {0x03, 0xff, 0xff, "Display controller"},
            {0x03, 0x00, 0x00, "VGA compatible controller"},
            {0x03, 0x00, 0x01, "8514 compatible controller"},
            {0x03, 0x01, 0x00, "XGA controller"},
            {0x03, 0x80, 0x00, "Other display controller"},

            {0x04, 0xff, 0xff, "Multimedia device"},
            {0x04, 0x00, 0x00, "Video device"},
            {0x04, 0x01, 0x00, "Audio device"},
            {0x04, 0x80, 0x00, "Other multimedia device"},

            {0x05, 0xff, 0xff, "Memory controller"},
            {0x05, 0x00, 0x00, "RAM controller"},
            {0x05, 0x01, 0x00, "Flash controller"},
            {0x05, 0x80, 0x00, "Other memory controller"},

            {0x06, 0xff, 0xff, "Bridge device"},
            {0x06, 0x00, 0x00, "Host bridge"},
            {0x06, 0x01, 0x00, "ISA bridge"},
            {0x06, 0x02, 0x00, "EISA bridge"},
            {0x06, 0x03, 0x00, "MCA bridge"},
            {0x06, 0x04, 0x00, "PCI-PCI bridge"},
            {0x06, 0x05, 0x00, "PCMCIA bridge"},
            {0x06, 0x06, 0x00, "NuBus bridge"},
            {0x06, 0x07, 0x00, "CardBus bridge"},
            {0x06, 0x80, 0x00, "Other bridge device"},

            {0x07, 0xff, 0xff, "Simple com device"},
            {0x07, 0x00, 0x00, "Generic XT compatible serial controller"},
            {0x07, 0x00, 0x01, "16450 compatible serial controller"},
            {0x07, 0x00, 0x02, "16550 compatible serial controller"},
            {0x07, 0x01, 0x00, "Parallel port"},
            {0x07, 0x01, 0x01, "Bidirectional parallel port"},
            {0x07, 0x01, 0x02, "ECP 1.X compliant parallel port"},
            {0x07, 0x80, 0x00, "Other communication device"},

            {0x08, 0xff, 0xff, "Base system peripherals"},
            {0x08, 0x00, 0x00, "Generic 8259 PIC"},
            {0x08, 0x00, 0x01, "ISA PIC"},
            {0x08, 0x00, 0x02, "EISA PIC"},
            {0x08, 0x01, 0x00, "Generic 8237 DMA controller"},
            {0x08, 0x01, 0x01, "ISA DMA controller"},
            {0x08, 0x01, 0x02, "EISA DMA controller"},
            {0x08, 0x02, 0x00, "Generic 8254 system timer"},
            {0x08, 0x02, 0x01, "ISA system timer"},
            {0x08, 0x02, 0x02, "EISA system timer"},
            {0x08, 0x03, 0x00, "Generic RTC controller"},
            {0x08, 0x03, 0x01, "ISA RTC controller"},
            {0x08, 0x80, 0x00, "Other system peripheral"},

            {0x09, 0xff, 0xff, "Input device"},
            {0x09, 0x00, 0x00, "Keyboard controller"},
            {0x09, 0x01, 0x00, "Digitizer (pen)"},
            {0x09, 0x02, 0x00, "Mouse controller"},
            {0x09, 0x80, 0x00, "Other input controller"},

            {0x0a, 0xff, 0xff, "Docking station"},
            {0x0a, 0x00, 0x00, "Generic docking station"},
            {0x0a, 0x80, 0x00, "Other type of docking station"},

            {0x0b, 0xff, 0xff, "Processor"},
            {0x0b, 0x00, 0x00, "386"},
            {0x0b, 0x01, 0x00, "486"},
            {0x0b, 0x02, 0x00, "Pentium"},
            {0x0b, 0x10, 0x00, "Alpha"},
            {0x0b, 0x20, 0x00, "PowerPC"},
            {0x0b, 0x40, 0x00, "Co-processor"},

            {0x0c, 0xff, 0xff, "Serial bus controller"},
            {0x0c, 0x00, 0x00, "FireWire (IEEE 1394)"},
            {0x0c, 0x01, 0x00, "ACCESS bus"},
            {0x0c, 0x02, 0x00, "SSA"},
            {0x0c, 0x03, 0x00, "Universal Serial Bus (USB)"},
            {0x0c, 0x04, 0x00, "Fibre Channel"},

            {0xff, 0xff, 0xff, "Unknown"},
            {0x00, 0x00, 0x00, NULL}
        };

    for (i = 0; ClassTable[i].pszDesc != NULL; ++i)
    {
        if ((ClassTable[i].bBaseClass == bBaseClass) &&
            (ClassTable[i].bSubClass == bSubClass) &&
            (ClassTable[i].bProgIF == bProgIF))
        {
            psz = ClassTable[i].pszDesc;
        }
    }

    return psz;
}       //GetClassDesc

/***LP  PrintClassInfo - Print device class info.
 *
 *  ENTRY
 *      pb -> ConfigSpace
 *      dwReg - ConfigSpace register
 *
 *  EXIT
 *      None
 */

VOID PrintClassInfo(PBYTE pb, DWORD dwReg)
{
    BYTE bBaseClass, bSubClass, bProgIF;
    PSZ psz;

    if (dwReg == CSOFFSET(BaseClass))
    {
        bBaseClass = pb[CSOFFSET(BaseClass)];
        bSubClass = 0xff;
        bProgIF = 0xff;
    }
    else if (dwReg == CSOFFSET(SubClass))
    {
        bBaseClass = pb[CSOFFSET(BaseClass)];
        bSubClass = pb[CSOFFSET(SubClass)];
        bProgIF = 0xff;
    }
    else        //must be CFGSPACE_CLASSCODE_PI
    {
        bBaseClass = pb[CSOFFSET(BaseClass)];
        bSubClass = pb[CSOFFSET(SubClass)];
        bProgIF = pb[CSOFFSET(ProgIf)];
    }

    if ((psz = GetClassDesc(bBaseClass, bSubClass, bProgIF)) != NULL)
        dprintf(" (%s)", psz);
    else if ((bBaseClass == 0x01) && (bSubClass == 0x01) && (bProgIF != 0xff) &&
             (bProgIF != 0x00))
    {
        dprintf(" (");
        if (bProgIF & 0x80)
            dprintf("MasterIDE ");
        if (bProgIF & 0x02)
            dprintf("PriNativeCapable ");
        if (bProgIF & 0x01)
            dprintf("PriNativeMode ");
        if (bProgIF & 0x08)
            dprintf("SecNativeCapable ");
        if (bProgIF & 0x04)
            dprintf("SecNativeMode");
        dprintf(")");
    }

    dprintf("\n");
}       //PrintClassInfo

VOID
DumpCfgSpace (
    IN PPCI_COMMON_CONFIG pcs
    )
{
    int rc = FERR_NONE;
    BYTE bHeaderType = pcs->HeaderType & ~PCI_MULTIFUNCTION;
    PFMT pafmt;
    DWORD dwOffset;
    PSZ pszDataFmt = "%02x: ";

    switch (bHeaderType)
    {
        case PCI_DEVICE_TYPE:
            pafmt = afmtCfgSpaceType0;
            break;

        case PCI_BRIDGE_TYPE:
            pafmt = afmtCfgSpaceType1;
            break;

        case PCI_CARDBUS_BRIDGE_TYPE:
            pafmt = afmtCfgSpaceType2;
            break;

        default:
            pafmt = afmtCfgSpaceUnknown;
    }

    dwOffset = 0;
    if (((rc = BinPrintf(NULL, afmtCfgSpaceCommon, (PBYTE)pcs, &dwOffset,
                         pszDataFmt)) == FERR_NONE) &&
        ((rc = BinPrintf(NULL, pafmt, (PBYTE)pcs, &dwOffset, pszDataFmt)) ==
         FERR_NONE))
    {
        if (bHeaderType != PCI_CARDBUS_BRIDGE_TYPE)
        {
            rc = BinPrintf(NULL, afmtCfgSpace40, (PBYTE)pcs, &dwOffset,
                           pszDataFmt);
        }

        if (rc == FERR_NONE)
        {
            rc = BinPrintf(NULL, afmtCfgSpace60, (PBYTE)pcs, &dwOffset,
                           pszDataFmt);
        }

        if ((rc == FERR_NONE) && (pcs->Status & PCI_STATUS_CAPABILITIES_LIST))
        {
            if (bHeaderType == PCI_DEVICE_TYPE)
            {
                dwOffset = pcs->u.type0.CapabilitiesPtr;
            }
            else if (bHeaderType == PCI_BRIDGE_TYPE)
            {
                dwOffset = pcs->u.type1.CapabilitiesPtr;
            }
            else if (bHeaderType == PCI_CARDBUS_BRIDGE_TYPE)
            {
                dwOffset = pcs->u.type2.CapabilitiesPtr;
            }
            else
            {
                dwOffset = 0;
            }

            while ((rc == FERR_NONE) && (dwOffset != 0))
            {
                PPCI_CAPABILITIES_HEADER pCap;

                pCap = (PPCI_CAPABILITIES_HEADER)&((PBYTE)pcs)[dwOffset];

                rc = BinPrintf(NULL,
                               afmtCapHeader,
                               (PBYTE)pcs,
                               &dwOffset,
                               pszDataFmt);

                if (rc == FERR_NONE)
                {
                    switch (pCap->CapabilityID)
                    {
                        case PCI_CAPABILITY_ID_POWER_MANAGEMENT:
                            rc = BinPrintf(NULL,
                                           afmtPwrMgmt,
                                           (PBYTE)pcs,
                                           &dwOffset,
                                           pszDataFmt);
                            break;

                        case PCI_CAPABILITY_ID_AGP:
                            rc = BinPrintf(NULL,
                                           afmtAGP,
                                           (PBYTE)pcs,
                                           &dwOffset,
                                           pszDataFmt);
                            break;

                        case PCI_CAPABILITY_ID_MSI:
                            rc = BinPrintf(NULL,
                                           afmtMSI,
                                           (PBYTE)pcs,
                                           &dwOffset,
                                           pszDataFmt);
                            if (rc == FERR_NONE)
                            {
                                PFMT pfmt;

                                if (((PCI_MSI_CAPABILITY *)pCap)->MessageControl.CapableOf64Bits)
                                {
                                    pfmt = afmtMSI64;
                                }
                                else
                                {
                                    pfmt = afmtMSI32;
                                }
                                rc = BinPrintf(NULL,
                                               pfmt,
                                               (PBYTE)pcs,
                                               &dwOffset,
                                               pszDataFmt);

                            }
                            break;
                    }

                    dwOffset = pCap->Next;
                }
            }
        }
    }

    if (rc != FERR_NONE)
    {
        dprintf("dump PCI ConfigSpace failed\n");
    }
}

/***LP  DumpCBRegs - Dump CardBus registers
 *
 *  ENTRY
 *      pbBuff -> register base
 *
 *  EXIT
 *      None
 */

VOID DumpCBRegs(PBYTE pbBuff)
{
    PSZ pszDataFmt = "%02x: ";

    if (BinPrintf(NULL, afmtCBRegs, pbBuff, NULL, pszDataFmt) != FERR_NONE)
    {
        dprintf("dump CardBus registers failed\n");
    }
}       //DumpCBRegs

/***LP  DumpExCARegs - Dump ExCA registers
 *
 *  ENTRY
 *      pbBuff -> buffer
 *      dwSize - size of buffer
 *
 *  EXIT
 *      None
 */

VOID DumpExCARegs(PBYTE pbBuff, DWORD dwSize)
{
    DWORD dwOffset = 0;
    char *pszDataFmt = "%02x: ";

    if (BinPrintf(NULL, afmtExCARegs, pbBuff, &dwOffset, pszDataFmt) !=
        FERR_NONE)
    {
        dprintf("dump ExCA registers failed\n");
    }
    else if ((dwSize > 0x40) &&
             (BinPrintf(NULL, afmtExCAHiRegs, pbBuff, &dwOffset, pszDataFmt) !=
              FERR_NONE))
    {
        dprintf("dump ExCA high registers failed\n");
    }
}       //DumpExCARegs

DECLARE_API( dcs )
/*++

Routine Description:

    Dumps PCI ConfigSpace

Arguments:

    args - Supplies the Bus.Dev.Fn numbers

Return Value:

    None

--*/
{
    LONG lcArgs;
    DWORD dwBus = 0;
    DWORD dwDev = 0;
    DWORD dwFn = 0;

    lcArgs = sscanf(args, "%lx.%lx.%lx", &dwBus, &dwDev, &dwFn);
    if (lcArgs != 3)
    {
        dprintf("invalid command syntax\n"
                "Usage: dcs <Bus>.<Dev>.<Func>\n");
    }
    else
    {
        PCI_TYPE1_CFG_BITS PciCfg1;
        PCI_COMMON_CONFIG  cs;

        PciCfg1.u.AsULONG = 0;
        PciCfg1.u.bits.BusNumber = dwBus;
        PciCfg1.u.bits.DeviceNumber = dwDev;
        PciCfg1.u.bits.FunctionNumber = dwFn;
        PciCfg1.u.bits.Enable = TRUE;

        ReadPci(&PciCfg1, (PUCHAR)&cs, 0, sizeof(cs));
        DumpCfgSpace(&cs);
    }
}

DECLARE_API( cbreg )
/*++

Routine Description:

    Dumps CardBus registers

Arguments:

    args - Supplies the address in hex.

Return Value:

    None

--*/
{
    BOOL rc = TRUE;
    LONG lcArgs;
    BOOL fPhysical = FALSE;
    DWORD dwAddr = 0;

    if (args == NULL)
    {
        dprintf("invalid command syntax\n"
                "Usage: cbreg <RegBaseAddr>\n");
        rc = FALSE;
    }
    else if ((args[0] == '%') && (args[1] == '%'))
    {
        lcArgs = sscanf(&args[2], "%lx", &dwAddr);
        fPhysical = TRUE;
    }
    else
    {
        lcArgs = sscanf(args, "%lx", &dwAddr);
    }

    if ((rc == TRUE) && (lcArgs == 1))
    {
        BYTE abCBRegs[0x14];
        BYTE abExCARegs[0x47];
        DWORD dwSize;

        if (fPhysical)
        {
            ULONG64 phyaddr = 0;

            phyaddr = dwAddr;
            ReadPhysical(phyaddr, abCBRegs, sizeof(abCBRegs), &dwSize);
            if (dwSize != sizeof(abCBRegs))
            {
                dprintf("failed to read physical CBRegs (SizeRead=%x)\n",
                        dwSize);
                rc = FALSE;
            }
            else
            {
                phyaddr += 0x800;
                ReadPhysical(phyaddr, abExCARegs, sizeof(abExCARegs), &dwSize);
                if (dwSize != sizeof(abExCARegs))
                {
                    dprintf("failed to read physical ExCARegs (SizeRead=%x)\n",
                            dwSize);
                    rc = FALSE;
                }
            }
        }
        else if (!ReadMemory(dwAddr, abCBRegs, sizeof(abCBRegs), &dwSize) ||
                 (dwSize != sizeof(abCBRegs)))
        {
            dprintf("failed to read CBRegs (SizeRead=%x)\n", dwSize);
            rc = FALSE;
        }
        else if (!ReadMemory(dwAddr + 0x800, abExCARegs, sizeof(abExCARegs),
                             &dwSize) ||
                 (dwSize != sizeof(abExCARegs)))
        {
            dprintf("failed to read CBRegs (SizeRead=%x)\n", dwSize);
            rc = FALSE;
        }

        if (rc == TRUE)
        {
            dprintf("\nCardBus Registers:\n");
            DumpCBRegs(abCBRegs);
            dprintf("\nExCA Registers:\n");
            DumpExCARegs(abExCARegs, sizeof(abExCARegs));
        }
    }
}

DECLARE_API( exca )
/*++

Routine Description:

    Dumps CardBus ExCA registers

Arguments:

    args - Supplies <BasePort>.<SktNum>

Return Value:

    None

--*/
{
    LONG lcArgs;
    DWORD dwBasePort = 0;
    DWORD dwSktNum = 0;

    lcArgs = sscanf(args, "%lx.%lx", &dwBasePort, &dwSktNum);
    if (lcArgs != 2)
    {
        dprintf("invalid command syntax\n"
                "Usage: exca <BasePort>.<SocketNum>\n");
    }
    else
    {
        int i;
        BYTE abExCARegs[0x40];

        for (i = 0; i < sizeof(abExCARegs); ++i)
        {
            abExCARegs[i] = ReadExCAByte(dwBasePort,
                                         (ULONG)(dwSktNum*0x40 + i));
        }

        DumpExCARegs(abExCARegs, sizeof(abExCARegs));
    }
}
