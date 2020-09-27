/*++

Copyright (c) 1998-1999  Microsoft Corporation

Module Name:

    netsh.h

Abstract:
    This file contains definitions which are needed by all NetSh helper DLLs.

--*/

#ifndef _NETSH_H_
#define _NETSH_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Error codes
//

#define NETSH_ERROR_BASE                        15000
#define ERROR_NO_ENTRIES                        (NETSH_ERROR_BASE + 0)
#define ERROR_INVALID_SYNTAX                    (NETSH_ERROR_BASE + 1)
#define ERROR_PROTOCOL_NOT_IN_TRANSPORT         (NETSH_ERROR_BASE + 2)
#define ERROR_NO_CHANGE                         (NETSH_ERROR_BASE + 3)
#define ERROR_CMD_NOT_FOUND                     (NETSH_ERROR_BASE + 4)
#define ERROR_ENTRY_PT_NOT_FOUND                (NETSH_ERROR_BASE + 5)
#define ERROR_DLL_LOAD_FAILED                   (NETSH_ERROR_BASE + 6)
#define ERROR_INIT_DISPLAY                      (NETSH_ERROR_BASE + 7)
#define ERROR_TAG_ALREADY_PRESENT               (NETSH_ERROR_BASE + 8)
#define ERROR_INVALID_OPTION_TAG                (NETSH_ERROR_BASE + 9)
#define ERROR_NO_TAG                            (NETSH_ERROR_BASE + 10)
#define ERROR_MISSING_OPTION                    (NETSH_ERROR_BASE + 11)
#define ERROR_TRANSPORT_NOT_PRESENT             (NETSH_ERROR_BASE + 12)
#define ERROR_SHOW_USAGE                        (NETSH_ERROR_BASE + 13)
#define ERROR_INVALID_OPTION_VALUE              (NETSH_ERROR_BASE + 14)
#define ERROR_OKAY                              (NETSH_ERROR_BASE + 15)
#define ERROR_CONTINUE_IN_PARENT_CONTEXT        (NETSH_ERROR_BASE + 16)
#define ERROR_SUPPRESS_OUTPUT                   (NETSH_ERROR_BASE + 17)

#define NETSH_ERROR_END                ERROR_SUPPRESS_OUTPUT

// Mode change values

#define NETSH_COMMIT                    0
#define NETSH_UNCOMMIT                  1
#define NETSH_FLUSH                     2
#define NETSH_COMMIT_STATE              3
#define NETSH_SAVE                      4

#define ERROR_CONTEXT_SWITCH            0x55aa
#define ERROR_CONNECT_REMOTE_CONFIG     (ERROR_CONTEXT_SWITCH + 1)

#define MAX_NAME_LEN                    MAX_DLL_NAME

#define NETSH_VERSION_50                0x0005000

#define NETSH_ARG_DELIMITER             L"="
#define NETSH_CMD_DELIMITER             L" "

#define NETSH_MAX_TOKEN_LENGTH          64
#define NETSH_MAX_CMD_TOKEN_LENGTH      128

#define NETSH_ROOT_GUID { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } }


typedef
DWORD
(WINAPI NS_CONTEXT_COMMIT_FN)(
    IN  DWORD       dwAction
    );

typedef NS_CONTEXT_COMMIT_FN *PNS_CONTEXT_COMMIT_FN;

typedef
DWORD
(WINAPI NS_CONTEXT_ENTRY_FN)(
    IN  PWCHAR      pwszMachine,
    IN  WCHAR       **ppwcArguments,
    IN  DWORD       dwArgCount,
    IN  DWORD       dwFlags,
    IN  PVOID       pvData,
    OUT PWCHAR      pwcNewContext
    );

typedef NS_CONTEXT_ENTRY_FN *PNS_CONTEXT_ENTRY_FN;

typedef
DWORD
(WINAPI NS_CONTEXT_CONNECT_FN)(
    IN  PWCHAR      pwszMachine
    );

typedef NS_CONTEXT_CONNECT_FN *PNS_CONTEXT_CONNECT_FN;

typedef
DWORD
(WINAPI NS_CONTEXT_SUBENTRY_FN)(
    IN  PVOID                     pSubContext,
    IN  PWCHAR                    pwszMachine,
    IN  WCHAR                     **ppwcArguments,
    IN  DWORD                     dwArgCount,
    IN  DWORD                     dwFlags,
    IN  PVOID                     pvData,
    OUT PWCHAR                    pwcNewContext
    );

typedef NS_CONTEXT_SUBENTRY_FN *PNS_CONTEXT_SUBENTRY_FN;

typedef
DWORD
(WINAPI *PNS_CONTEXT_UNINIT_FN)(
    IN  DWORD   dwReserved
    );

typedef
DWORD
(WINAPI NS_CONTEXT_DUMP_FN)(
    IN  PWCHAR      pwszRouter,
    IN  WCHAR       **ppwcArguments,
    IN  DWORD       dwArgCount,
    IN  PVOID       pvData
    );

typedef NS_CONTEXT_DUMP_FN *PNS_CONTEXT_DUMP_FN;

typedef
DWORD
(WINAPI NS_DLL_STOP_FN)(
    IN  DWORD       dwReserved
    );

typedef NS_DLL_STOP_FN *PNS_DLL_STOP_FN;

typedef
DWORD
(WINAPI NS_HELPER_START_FN)(
    IN  CONST GUID *pguidParent,
    IN  PVOID       pfnRegisterContext
    );

typedef NS_HELPER_START_FN *PNS_HELPER_START_FN;

typedef
DWORD
(WINAPI NS_HELPER_STOP_FN)(
    IN  DWORD       dwReserved
    );

typedef NS_HELPER_STOP_FN *PNS_HELPER_STOP_FN;

typedef
DWORD
(WINAPI NS_DEREG_ALL_CONTEXTS_FN)(
    IN  CONST GUID *pguidChild
    );

typedef NS_DEREG_ALL_CONTEXTS_FN *PNS_DEREG_ALL_CONTEXTS_FN;

typedef
DWORD
(WINAPI NS_DISPLAY_PARENT_HELP)(
    IN  PWCHAR  pwszGroup,
    IN  DWORD   dwCmdFlags
    );

typedef NS_DISPLAY_PARENT_HELP *PNS_DISPLAY_PARENT_HELP;

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201)

typedef struct _NS_DLL_ATTRIBUTES
{
    union
    {
        ULONGLONG       _ullAlign;

        struct
        {
            DWORD       dwVersion;
            DWORD       dwReserved;
        };
    };

    PNS_DLL_STOP_FN     pfnStopFn;

}NS_DLL_ATTRIBUTES, *PNS_DLL_ATTRIBUTES;

typedef struct _NS_HELPER_ATTRIBUTES
{
    union
    {
        ULONGLONG       _ullAlign;

        struct
        {
            DWORD       dwVersion;
            DWORD       dwReserved;
        };
    };

    PNS_HELPER_START_FN       pfnStart;
    PNS_HELPER_STOP_FN        pfnStop;
    PVOID                     pfnRegisterContext;
    PNS_DEREG_ALL_CONTEXTS_FN pfnDeregisterAllContexts;

}NS_HELPER_ATTRIBUTES, *PNS_HELPER_ATTRIBUTES;

typedef struct _NS_BASE_CONTEXT_ATTRIBUTES
{
    union
    {
        ULONGLONG       _ullAlign;

        struct
        {
            DWORD       dwVersion;
            DWORD       dwReserved;
        };
    };

    //
    // Name of the context
    //

    PWCHAR                   pwszContext;

    // GUID of the helper servicing this context

    GUID                     guidHelper;

    //
    // Flags limiting when context is available.
    // See CMD_FLAG_xxx in netsh.h.
    //

    DWORD                    dwFlags;

    // Priority field is only relevant if CMD_FLAG_PRIORITY is set in dwFlags

    ULONG                    ulPriority;

    // Number of top-level commands

    ULONG                    ulNumTopCmds; 

    // Array of top-level commands

    struct _CMD_ENTRY        (*pTopCmds)[];

    // Number of command groups

    ULONG                    ulNumGroups;

    // Array of command groups

    struct _CMD_GROUP_ENTRY  (*pCmdGroups)[];

    // Pointer to number of subcontexts

    ULONG                    ulNumContexts;

    // Pointer to array of contexts

    UINT_PTR                *pContextTable;

    // Size of a context entry

    ULONG                    ulContextSize;

    // Function to display parent help

    PNS_DISPLAY_PARENT_HELP  pfnDisplayParentHelp;

}NS_BASE_CONTEXT_ATTRIBUTES, *PNS_BASE_CONTEXT_ATTRIBUTES;

typedef struct _NS_CONTEXT_ATTRIBUTES
{
#ifdef __cplusplus
    NS_BASE_CONTEXT_ATTRIBUTES bca;
#else
    NS_BASE_CONTEXT_ATTRIBUTES;
#endif

    PNS_CONTEXT_ENTRY_FN     pfnEntryFn;
    PNS_CONTEXT_COMMIT_FN    pfnCommitFn;
    PNS_CONTEXT_DUMP_FN      pfnDumpFn;
    PNS_CONTEXT_CONNECT_FN   pfnConnectFn;
    PNS_CONTEXT_SUBENTRY_FN  pfnSubEntryFn;

}NS_CONTEXT_ATTRIBUTES, *PNS_CONTEXT_ATTRIBUTES;

#define DEFAULT_CONTEXT_PRIORITY 100

typedef
DWORD
(WINAPI *PNS_IF_NAME_MAPPER)(
    IN  HANDLE hMprConfig,
    IN  PWCHAR pwszName,
    OUT PWCHAR pwszBuffer,
    IN  PDWORD pdwBufSize
    );

typedef
BOOL
(WINAPI *PNS_MATCH_CMD_LINE)(
    IN  PWCHAR  *ppwcArguments,
    IN  DWORD   dwArgCount,
    IN  PWCHAR  pwszCmdToken,
    OUT PDWORD  pdwNumMatched
    );

typedef
BOOL
(WINAPI *PNS_MATCH_TOKEN)(
    IN  PWCHAR  pwszUserToken,
    IN  PWCHAR  pwszCmdToken
    );



typedef struct _TOKEN_VALUE
{
    PWCHAR  pwszToken; // literal token string
    DWORD   dwValue;   // ID of info string

}TOKEN_VALUE, *PTOKEN_VALUE;

#define NUM_TOKENS_IN_TABLE(TokenArray) \
    sizeof(TokenArray)/sizeof(TOKEN_VALUE)

typedef struct _VALUE_TOKEN
{
    DWORD   dwValue;
    PWCHAR  pwszToken;

}VALUE_TOKEN, *PVALUE_TOKEN;

typedef struct _VALUE_STRING
{
    DWORD   dwValue;
    DWORD   dwStringId;

}VALUE_STRING, *PVALUE_STRING;
#define NUM_VALUES_IN_TABLE(TokenArray) \
    sizeof(TokenArray)/sizeof(VALUE_STRING)


typedef
DWORD
(WINAPI *PNS_MATCH_ENUM_TAG)(
    IN  HANDLE      hModule,
    IN  PWCHAR      pwcArg,
    IN  DWORD       dwNumArg,
    IN  PTOKEN_VALUE pEnumTable,
    OUT PDWORD      pdwValue
    );

typedef
DWORD
(WINAPI *PNS_GET_DISPLAY_STRING)(
    IN  HANDLE      hModule,
    IN  DWORD       dwValue,
    IN  PTOKEN_VALUE pEnumTable,
    IN  DWORD       dwNumArgs,
    OUT PWCHAR      ptszString
    );


#define NS_REQ_PRESENT        0x01
#define NS_REQ_ALLOW_MULTIPLE 0x02
#define NS_REQ_ZERO         0
#define NS_REQ_ONE          1
#define NS_REQ_ZERO_OR_MORE 2
#define NS_REQ_ONE_OR_MORE  3

typedef struct _TAG_TYPE
{
    PWCHAR   pwszTag;       // tag string
    DWORD    dwRequired;    // required or not
    BOOL     bPresent;      // present or not
}TAG_TYPE, *PTAG_TYPE;

typedef
DWORD
(WINAPI *PNS_MATCH_TAGS_IN_CMD_LINE)(
    IN  HANDLE      hModule,
    IN  PWCHAR      *ppwcArguments,
    IN  DWORD       dwCurrentIndex,
    IN  DWORD       dwArgCount,
    IN  PTAG_TYPE   pEnumTable,
    IN  DWORD       dwNumTags,
    OUT PDWORD      pdwOut
    );

#define NUM_TAGS_IN_TABLE(TagsArray) \
    sizeof(TagsArray)/sizeof(TAG_TYPE)

typedef
DWORD
(WINAPI *PNS_PREPROCESS_COMMAND)(
    IN  HANDLE    hModule,
    IN  PWCHAR   *ppwcArguments,
    IN  DWORD     dwCurrentIndex,
    IN  DWORD     dwArgCount,

    IN  TAG_TYPE *pttTags,
    IN  DWORD     dwTagCount,
    IN  DWORD     dwMinArgs,
    IN  DWORD     dwMaxArgs,
    OUT DWORD    *pdwTagType
    );


typedef
PWCHAR
(WINAPI *PNS_GET_ENUM_STRING)(
    IN  HANDLE          hModule,
    IN  DWORD           dwValue,
    IN  DWORD           dwNumVal,
    IN  PTOKEN_VALUE    pEnumTable
    );

typedef
PWCHAR
(WINAPI *PNS_MAKE_STRING)(
    IN  HANDLE  hModule,
    IN  DWORD   dwMsgId
    );

typedef
VOID
(WINAPI *PNS_FREE_STRING)(
    IN  PWCHAR  pwszMadeString
    );

typedef
PWCHAR
(WINAPI *PNS_MAKE_QUOTED_STRING)(
    IN  PWCHAR  pwszString
    );

typedef
VOID
(WINAPI *PNS_FREE_QUOTED_STRING)(
    IN  PWCHAR  pwszMadeString
    );

typedef
VOID
(*PNS_DISPLAY_ERR)(
    IN  HANDLE  hModule, OPTIONAL
    IN  DWORD   dwErrId,
    ...
    );

typedef
DWORD
(*PNS_DISPLAY_MSG)(
    IN  HANDLE  hModule,
    IN  DWORD   dwMsgId,
    ...
    );

typedef PNS_DISPLAY_MSG PNS_DISPLAY_MSG_M;

typedef
DWORD
(*PNS_DISPLAY_MSG_T)(
    IN  PWCHAR  pwszFormat,
    ...
    );

typedef
BOOL
(WINAPI *PNS_INIT_CONSOLE)(
    IN    OUT    PDWORD    pdwRR,
    OUT          HANDLE    *phMib,
    OUT          HANDLE    *phConsole
    );

typedef
DWORD
(*PNS_DISPLAY_MSG_CONSOLE)(
    IN  HANDLE  hModule,
    IN  HANDLE  hConsole,
    IN  DWORD   dwMsgId,
    ...
    );

typedef
DWORD
(WINAPI *PNS_REFRESH_CONSOLE)(
    IN    HANDLE    hMib,
    IN    HANDLE    hConsole,
    IN    DWORD     dwRR
    );

typedef
DWORD
(WINAPI *PNS_REGISTER_HELPER)(
    IN    CONST GUID           *pguidChildContext,
    IN    CONST GUID           *pguidParentContext,
    IN    PVOID                 pfnRegisterSubContext
    );

typedef
DWORD
(WINAPI *PNS_GET_HELPER_ENTRY)(
    IN    CONST GUID                     *pGuid,
    OUT   struct _NS_HELPER_TABLE_ENTRY **ppHelper
    );

typedef
DWORD
(WINAPI *PNS_GET_DLL_ENTRY)(
    IN    DWORD                          dwDllIndex,
    OUT   struct _NS_DLL_TABLE_ENTRY   **ppDll
    );

typedef
VOID
(WINAPI *PNS_CONVERT_GUID_TO_STRING)(
    IN    CONST GUID *pGuid,
    OUT   PWCHAR      pwszBuffer
    );

typedef
DWORD
(WINAPI *PNS_CONVERT_STRING_TO_GUID)(
    IN  PWCHAR  pwszGuid,
    IN  USHORT  usStringLen,
    OUT GUID    *pGuid
    );

typedef
DWORD
(WINAPI NS_PROCESS_HELPER_CMD)(
    IN     DWORD     dwArgCount,
    IN     PWCHAR   *argv,
    IN     DWORD     dwFlags,
//  IN OUT PWCHAR   *ppwszNewContext,
    OUT    BOOL     *pbDone
    );

typedef NS_PROCESS_HELPER_CMD *PNS_PROCESS_HELPER_CMD;

typedef
BOOL
(WINAPI *PNS_EVENT_FILTER)(
    IN  EVENTLOGRECORD  *pRecord,
    IN  PWCHAR          pwszLogName,
    IN  PWCHAR          pwszComponent,
    IN  PWCHAR          pwszSubComponent,
    IN  PVOID           pvFilterContext
    );
    
typedef
DWORD
(WINAPI *PNS_PRINT_EVENTLOG)(
    IN  PWCHAR              pwszLogName,
    IN  PWCHAR              pwszComponent,
    IN  PWCHAR              pwszSubComponent, OPTIONAL
    IN  DWORD               fFlags,
    IN  PVOID               pvHistoryInfo,
    IN  PNS_EVENT_FILTER    pfnEventFilter, OPTIONAL
    IN  PVOID               pvFilterContext
    );

#define NS_GET_EVENT_IDS_FN_NAME    "GetEventIds"

#define NS_EVENT_LOOP       0x00010000
#define NS_EVENT_LAST_N     0x00000001
#define NS_EVENT_LAST_SECS  0x00000002
#define NS_EVENT_FROM_N     0x00000004
#define NS_EVENT_FROM_START 0x00000008

typedef
DWORD
(WINAPI *PNS_GET_EVENT_IDS_FN)(
    IN  PWCHAR  pwszComponent,
    IN  PWCHAR  pwszSubComponent,
    OUT PDWORD  pdwEventIds,
    OUT PULONG  pulEventCount
    );


#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif

typedef DWORD (FN_HANDLE_CMD)(
    PWCHAR    pwszMachine,
    PWCHAR   *ppwcArguments,
    DWORD     dwCurrentIndex,
    DWORD     dwArgCount,
    DWORD     dwFlags,
    PVOID     pvData,
    BOOL     *pbDone
    );

typedef FN_HANDLE_CMD *PFN_HANDLE_CMD;

#define CMD_FLAG_PRIVATE     0x01 // not valid in sub-contexts
#define CMD_FLAG_INTERACTIVE 0x02 // not valid from outside netsh
#define CMD_FLAG_IMMEDIATE   0x04 // not valid from ancestor contexts
#define CMD_FLAG_LOCAL       0x08 // not valid from a remote machine
#define CMD_FLAG_ONLINE      0x10 // not valid in offline/non-commit mode

#define CMD_FLAG_LIMIT_MASK 0xffff

#define CMD_FLAG_PRIORITY   0x80000000 // ulPriority field is used

typedef struct _CMD_ENTRY
{
    //
    // The token for the command
    //

    const PWCHAR        pwszCmdToken;

    //
    // The function which handles this command
    //

    PFN_HANDLE_CMD      pfnCmdHandler;

    //
    // The short help message
    //

    DWORD               dwShortCmdHelpToken;

    //
    // The message to display if the only thing after the command is a
    // help token (HELP, /?, -?, ?)
    //

    DWORD               dwCmdHlpToken;

    // Flags (see CMD_FLAGS_xxx above)

    DWORD               dwFlags;

}CMD_ENTRY, *PCMD_ENTRY;

#define CREATE_CMD_ENTRY(t,f)   {CMD_##t, f, HLP_##t, HLP_##t##_EX, CMD_FLAG_PRIVATE}
#define CREATE_CMD_ENTRY_EX(t,f,i) {CMD_##t, f, HLP_##t, HLP_##t##_EX, i}

typedef struct _CMD_GROUP_ENTRY
{
    //
    // The token for the command verb
    //

    const PWCHAR    pwszCmdGroupToken;

    //
    // The message to display in a command listing.
    //

    DWORD           dwShortCmdHelpToken;

    //
    // The number of entries in the cmd table
    //

    ULONG           ulCmdGroupSize;

    // Flags (see CMD_FLAG_xxx)

    DWORD           dwFlags;

    //
    // The command table
    //

    PCMD_ENTRY      pCmdGroup;

}CMD_GROUP_ENTRY, *PCMD_GROUP_ENTRY;

#define CREATE_CMD_GROUP_ENTRY(t,s)     {CMD_##t, HLP_##t, sizeof(s)/sizeof(CMD_ENTRY), 0, s }
#define CREATE_CMD_GROUP_ENTRY_EX(t,s,i)     {CMD_##t, HLP_##t, sizeof(s)/sizeof(CMD_ENTRY), i, s }

typedef
DWORD
(WINAPI *PNS_EXECUTE_HANDLER)(
    IN  HANDLE     hModule,
    IN  PCMD_ENTRY pCmdEntry,
    IN  PWCHAR    *argv,
    IN  DWORD      dwNumMatched,
    IN  DWORD      dwArgCount,
    IN  DWORD      dwFlags,
    IN  PVOID      pvData,
    OUT BOOL      *pbDone
    );

typedef
CONST PWCHAR
(WINAPI *PNS_GET_CURRENT_CONTEXT)(VOID);

typedef
DWORD
(WINAPI NS_DISPLAY_HELP)(
    IN  HANDLE                     hModule,
    IN  PWCHAR                     pwszContext,
    IN  DWORD                      dwDisplayFlags,
    IN  DWORD                      dwCmdFlags,
    IN  DWORD                      dwArgsRemaining,
    IN  ULONG                      ulNumTopCmds,
    IN  PCMD_ENTRY                 pCmds,
    IN  ULONG                      ulNumGroups,
    IN  PCMD_GROUP_ENTRY           pCmdGroups,
    IN  ULONG                      ulNumContexts,
    IN  PBYTE                      pContexts,
    IN  DWORD                      dwContextSize,
    IN  PNS_DISPLAY_PARENT_HELP    DisplayParentHelp,
    IN  PWCHAR                     pwszGroup
    );

typedef NS_DISPLAY_HELP *PNS_DISPLAY_HELP;

typedef
DWORD
(WINAPI NS_UPDATE_NEW_CONTEXT)(
    IN PWCHAR  ppwszBuffer,
    IN PWCHAR  pwszNewToken,
    IN DWORD   dwArgs
    );

typedef NS_UPDATE_NEW_CONTEXT *PNS_UPDATE_NEW_CONTEXT;

typedef
DWORD
(WINAPI NS_GENERIC_ENTRY_FN)(
    IN    PNS_CONTEXT_ATTRIBUTES         pContext,
    IN    PWCHAR                         pwszMachine,
    IN    PWCHAR                         *ppwcArguments,
    IN    DWORD                          dwArgCount,
    IN    DWORD                          dwFlags,
    IN    PVOID                          pvData,
    OUT   PWCHAR                         pwcNewContext
    );

typedef NS_GENERIC_ENTRY_FN   *PNS_GENERIC_ENTRY_FN;

typedef struct _NETSH_ATTRIBUTES
{
    union
    {
        ULONGLONG       _ulAlign;

        struct
        {
            DWORD       dwVersion;
            DWORD       dwReserved;
        };
    };

    PNS_IF_NAME_MAPPER      pfnGetFriendlyNameFromIfName;
    PNS_IF_NAME_MAPPER      pfnGetIfNameFromFriendlyName;
    PNS_MATCH_CMD_LINE      pfnMatchCmdLine;
    PNS_MATCH_TOKEN         pfnMatchToken;
    PNS_MATCH_ENUM_TAG      pfnMatchEnumTag;
    PNS_MATCH_TAGS_IN_CMD_LINE     pfnMatchTagsInCmdLine;
    PNS_GET_ENUM_STRING     pfnGetEnumString;
    PNS_MAKE_STRING         pfnMakeString;
    PNS_FREE_STRING         pfnFreeString;

    PNS_MAKE_QUOTED_STRING  pfnMakeQuotedString;
    PNS_FREE_QUOTED_STRING  pfnFreeQuotedString;
    PNS_DISPLAY_ERR         pfnDisplayError;
    PNS_DISPLAY_MSG         pfnDisplayMessage;
    PNS_INIT_CONSOLE        pfnInitializeConsole;
    PNS_DISPLAY_MSG_CONSOLE pfnDisplayMessageToConsole;
    PNS_REFRESH_CONSOLE     pfnRefreshConsole;
    PNS_PROCESS_HELPER_CMD  pfnProcessHelperCommand;
    PNS_REGISTER_HELPER     pfnRegisterHelper;
    PNS_GET_HELPER_ENTRY    pfnGetHelperEntry;
    PNS_GET_DLL_ENTRY       pfnGetDllEntry;
    PNS_CONVERT_GUID_TO_STRING  pfnConvertGuidToString;
    PNS_CONVERT_STRING_TO_GUID  pfnConvertStringToGuid;
    PNS_DISPLAY_MSG_T       pfnDisplayMessageT;
    PNS_EXECUTE_HANDLER     pfnExecuteHandler;
    PNS_DISPLAY_HELP        pfnDisplayHelp;
    PNS_UPDATE_NEW_CONTEXT  pfnUpdateNewContext;
    PNS_PREPROCESS_COMMAND  pfnPreprocessCommand;
    PNS_PRINT_EVENTLOG      pfnPrintEventLog;
    PNS_DISPLAY_PARENT_HELP pfnDisplayRootHelp;
    PNS_GENERIC_ENTRY_FN    pfnGenericEntry;
    PNS_DISPLAY_MSG_M       pfnDisplayMessageM;

}NETSH_ATTRIBUTES, *PNETSH_ATTRIBUTES;

typedef
DWORD
(NS_DLL_INIT_FN)(
    IN  PNETSH_ATTRIBUTES           pUtilityTable,
    OUT PNS_DLL_ATTRIBUTES          pDllTable
    );

typedef NS_DLL_INIT_FN *PNS_DLL_INIT_FN;

typedef
DWORD
(WINAPI *PNS_HELPER_INIT_FN)(
    IN  PWCHAR                      pwszRouter,
    IN  PNETSH_ATTRIBUTES           pUtilityTable,
    OUT PNS_HELPER_ATTRIBUTES       pHelperTable
    );

typedef
DWORD
(WINAPI *PNS_REGISTER_CONTEXT)(
    IN    PNS_CONTEXT_ATTRIBUTES pAttributes,
    OUT   PVOID                  pvUnused     
    );

#ifdef __cplusplus
}
#endif

#endif // _NETSH_H_
