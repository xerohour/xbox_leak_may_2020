/**************************************************************************************************************
*
*   File:  rm.h
*
*   Description:
*       Definitions for the use of and entry points into the resource manager library from the miniport.
*
**************************************************************************************************************/

// notify callback action
#define NV_OS_WRITE_THEN_AWAKEN     0x00000001

// power state definitions used by RmSet/GetPowerState() and RmGetCurrentPowerState()
#define NV_POWER_ADAPTER_STATE_0            0x00000001 // full on
#define NV_POWER_ADAPTER_STATE_1            0x00000002 // slowed clocks
#define NV_POWER_ADAPTER_STATE_2            0x00000004 // slowed clocks, mobile style *** not all devices supported ***
#define NV_POWER_ADAPTER_STATE_3            0x00000008 // very slow clocks, state saved (regs & instance memory)
#define NV_POWER_ADAPTER_STATE_4            0x00000010 // future lower adapter power state
#define NV_POWER_ADAPTER_STATE_5            0x00000020 // future lower adapter power state
#define NV_POWER_ADAPTER_STATE_6            0x00000040 // future lower adapter power state
#define NV_POWER_ADAPTER_STATE_7            0x00000080 // future lower adapter power state
#define NV_POWER_ADAPTER_FEATURE_CTXSAVED   0x00000100 // lowest power consumption state saves device context
#define NV_POWER_ADAPTER_STATE_HIBERNATE    0x00000200 // adapter hibernate
#define NV_POWER_ADAPTER_FEATURE_UNDEF_2    0x00000400 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_3    0x00000800 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_4    0x00001000 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_5    0x00002000 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_6    0x00004000 // future feature
#define NV_POWER_ADAPTER_FEATURE_UNDEF_7    0x00008000 // future feature
#define NV_POWER_MONITOR_STATE_0            0x00010000 // full on
#define NV_POWER_MONITOR_STATE_1            0x00020000 // no VSYNC nor HSYNC
#define NV_POWER_MONITOR_STATE_2            0x00040000 // future lower monitor power state 

#define NV_POWER_MONITOR_STATE_3            0x00080000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_4            0x00100000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_5            0x00200000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_6            0x00400000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_7            0x00800000 // future lower monitor power state
#define NV_POWER_MONITOR_STATE_HIBERNATE    0x01000000 // monitor hibernate
#define NV_POWER_UNDEF_1                    0x02000000 // future attribute
#define NV_POWER_UNDEF_2                    0x04000000 // future attribute
#define NV_POWER_UNDEF_3                    0x08000000 // future attribute
#define NV_POWER_UNDEF_4                    0x10000000 // future attribute
#define NV_POWER_UNDEF_5                    0x20000000 // future attribute
#define NV_POWER_UNDEF_6                    0x40000000 // future attribute
#define NV_POWER_UNDEF_7                    0x80000000 // future attribute

// prototype of callback fct -- the parameter is the device extension
typedef VOID (*MINIPORT_CALLBACK)(VOID*);

BOOL    RmInitRm            (VOID);
BOOL    RmInitNvMapping     (VOID*, U032, PHWREG, U032*);
BOOL    RmPostNvDevice      (U032, PHWREG, PHWREG);
BOOL    RmInitNvDevice      (U032, U032, PHWREG);
BOOL    RmInitNvHal         (U032);
BOOL    RmLoadState         (U032, U032, U032, U032, U032, BOOL);
BOOL    RmUnloadState       (U032);
BOOL    RmUpdateAGPConfig   (U032);
BOOL    RmSetMode           (U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032,U032);
BOOL    RmIsr               (U032);
BOOL    RmConfigGetKernel   (U032, U032, U032*);
BOOL    RmConfigGetExKernel (U032, U032, VOID*, U032);
BOOL    RmConfigSetKernel   (U032, U032, U032, U032*);
BOOL    RmConfigSetExKernel (U032, U032, VOID*, U032);
#if (_WIN32_WINNT >= 0x0500)
VOID    RmSetAgpServices    (U032, VOID*);
#endif
BOOL    RmPreModeSet        (U032, U032);
BOOL    RmPostModeSet       (U032, U032);

BOOL    RmSetDisplayPowerState      (U032, U032, BOOL);
BOOL    RmEnableHead                (U032, U032);
BOOL    RmSetPowerState             (U032 deviceReference, U032 head, U032 state);
BOOL    RmGetCurrentPowerState      (U032 deviceReference, U032 head, U032* pState);
BOOL    RmGetPowerCaps              (U032 deviceReference, U032 head, U032* pCaps);
BOOL    RmSetDriverHotkeyHandling   (U032 deviceReference, BOOL enabled);
VP_STATUS  RMVideoResetDevice       (U032 deviceReference);

VOID    Nv01AllocRoot       (NVOS01_PARAMETERS*);
VOID    Nv01AllocDevice     (NVOS06_PARAMETERS*);
VOID    Nv01AllocContextDma (NVOS03_PARAMETERS*);
VOID    Nv01AllocChannelPio (NVOS04_PARAMETERS*);
VOID    Nv03AllocChannelDma (NVOS07_PARAMETERS*);
VOID    Nv01AllocMemory     (NVOS02_PARAMETERS*);
VOID    Nv01AllocObject     (NVOS05_PARAMETERS*);
VOID    Nv01AllocEvent      (NVOS10_PARAMETERS*);
VOID    Nv04Alloc           (NVOS21_PARAMETERS*);
VOID    Nv01Free            (NVOS00_PARAMETERS*);
VOID    Nv03DmaFlowControl  (NVOS08_PARAMETERS*);
VOID    Nv03ArchHeap        (NVOS11_PARAMETERS*);
VOID    Nv01ConfigVersion   (NVOS12_PARAMETERS*);
VOID    Nv01ConfigGet       (NVOS13_PARAMETERS*);
VOID    Nv01ConfigSet       (NVOS14_PARAMETERS*);
VOID    Nv04ConfigGetEx     (NVOS_CONFIG_GET_EX_PARAMS*);
VOID    Nv04ConfigSetEx     (NVOS_CONFIG_SET_EX_PARAMS*);
VOID    Nv01ConfigUpdate    (NVOS15_PARAMETERS*);
VOID    Nv01DebugControl    (NVOS20_PARAMETERS*);
VOID    Nv04I2CAccess       (NVOS_I2C_ACCESS_PARAMS *);
VOID    Nv04DirectMethodCall(NVOS1D_PARAMETERS*);
