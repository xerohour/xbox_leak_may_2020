//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1989 - 1994.
//
//  File:       build.c
//
//  Contents:   Parameter processing and main entry point for Build.exe
//
//  History:    16-May-89      SteveWo         Created
//              ...   See SLM log
//              26-Jul-94      LyleC           Cleanup/Add Pass0 support
//              05-Dec-00      sbonev          See SD changelist 2317
//
//----------------------------------------------------------------------------

#include "build.h"

//
// Increase critical section timeout so people don't get
// frightened when the CRT takes a long time to acquire
// its critical section.
//
IMAGE_LOAD_CONFIG_DIRECTORY _load_config_used = {
    0,                          // Reserved
    0,                          // Reserved
    0,                          // Reserved
    0,                          // Reserved
    0,                          // GlobalFlagsClear
    0,                          // GlobalFlagsSet
    1000 * 60 * 60 * 24,        // CriticalSectionTimeout (milliseconds)
    0,                          // DeCommitFreeBlockThreshold
    0,                          // DeCommitTotalFreeThreshold
    0,                          // LockPrefixTable
    0, 0, 0, 0, 0, 0, 0         // Reserved
};

//
// Target machine info:
//
//  SourceSubDirMask, Description, Switch, MakeVariable,
//  SourceVariable, ObjectVariable, AssociateDirectory,
//  SourceDirectory, ObjectDirectory
//

TARGET_MACHINE_INFO AlphaTargetMachine = {
    TMIDIR_ALPHA, "Alpha", "-alpha", "ALPHA=1",
    "ALPHA_SOURCES", "ALPHA_OBJECTS", "alpha",
    "alpha", "alphadirs", { "alpha" },
    DIR_INCLUDE_ALPHA | DIR_INCLUDE_RISC | DIR_INCLUDE_WIN32
};

TARGET_MACHINE_INFO Alpha64TargetMachine = {
    TMIDIR_ALPHA64, "Alpha64", "-alpha64", "ALPHA64=1",
    "ALPHA64_SOURCES", "ALPHA64_OBJECTS", "alpha",
    "alpha64", "alpha64dirs", { "alpha64" },
    DIR_INCLUDE_ALPHA64 | DIR_INCLUDE_RISC | DIR_INCLUDE_WIN64
};

TARGET_MACHINE_INFO i386TargetMachine = {
    TMIDIR_I386, "i386", "-386", "386=1",
    "i386_SOURCES", "386_OBJECTS", "i386",
    "i386", "i386dirs", { "i386" },
    DIR_INCLUDE_X86 | DIR_INCLUDE_WIN32
};

TARGET_MACHINE_INFO ia64TargetMachine = {
    TMIDIR_IA64, "IA64", "-ia64", "IA64=1",
    "IA64_SOURCES", "IA64_OBJECTS", "ia64",
    "ia64", "ia64dirs", { "ia64" },
    DIR_INCLUDE_IA64 | DIR_INCLUDE_RISC | DIR_INCLUDE_WIN64
};

TARGET_MACHINE_INFO *PossibleTargetMachines[MAX_TARGET_MACHINES] = {
    &AlphaTargetMachine,
    &Alpha64TargetMachine,
    &i386TargetMachine,
    &ia64TargetMachine
};

//
// Machine specific target dirs default. If one there is only one build
// target and a target specific dirs file is selected, then this gets
// filled with a pointer to the target specific dirs filename.
//

LPSTR pszTargetDirs = "";

#define AltDirMaxSize 10            // Maximum size for alternate obj dir name

CHAR LogFileName[sizeof("build.log") + AltDirMaxSize] = "build";
CHAR WrnFileName[sizeof("build.wrn") + AltDirMaxSize] = "build";
CHAR ErrFileName[sizeof("build.err") + AltDirMaxSize] = "build";
CHAR IncFileName[sizeof("build.inc") + AltDirMaxSize] = "build";
CHAR XMLFileName[sizeof("build.xml") + AltDirMaxSize] = "build";

CHAR szObjDir[sizeof("obj") + AltDirMaxSize] = "obj";
CHAR szObjDirSlash[sizeof("obj\\") + AltDirMaxSize] = "obj\\";
CHAR szObjDirSlashStar[sizeof("obj\\*") + AltDirMaxSize] = "obj\\*";

CHAR szObjDirD[] = "objd";
CHAR szObjDirSlashD[] = "objd\\";
CHAR szObjDirSlashStarD[] = "objd\\*";

CHAR *pszObjDir = szObjDir;
CHAR *pszObjDirSlash = szObjDirSlash;
CHAR *pszObjDirSlashStar = szObjDirSlashStar;

BOOL fCheckedBuild = TRUE;
ULONG iObjectDir = 0;

ULONG DefaultProcesses = 0;
CHAR *szBuildTag;

#define MAX_ENV_ARG 512

char szNewLine[] = "\n";
char szUsage[] =
    "Usage: BUILD [-?] display this message\n"
    "\t[-0] pass 0 generation only, no compile, no link\n"
    "\t[-3] same as -Z but will add a third pass zero pass\n"
    "\t[-a] allows synchronized blocks and drains during link pass\n"
    "\t[-b] displays full error message text (doesn't truncate)\n"
    "\t[-B [baseline]] Checks the build failures against a baseline\n"
    "\t\tIf there is no baseline,terminates the build on the first error\n"
    "\t[-c] deletes all object files\n"
    "\t[-C] deletes all .lib files only\n"
#if DBG
    "\t[-d] display debug information\n"
#endif
    "\t[-D] Display warnings/errors/summary in color\n"
    "\t[-e] generates build.log, build.wrn & build.err files\n"
    "\t[-E] always keep the log/wrn/err files (use with -z)\n"
    "\t[-f] force rescan of all source and include files\n"
    "\t[-F] when displaying errors/warnings to stdout, print the full path\n"
    "\t[-G] enables target specific dirs files iff one target\n"
    "\t[-i] ignore extraneous dependency warning messages\n"
    "\t[-k] keep (don't delete) out-of-date targets\n"
    "\t[-l] link only, no compiles\n"
    "\t[-L] compile only, no link phase\n"
    "\t[-m] run build in the idle priority class\n"
    "\t[-M [n]] Multiprocessor build (for MP machines)\n"
    "\t[-n] display thread index if multiprocessor build\n"
    "\t[-o] display out-of-date files\n"
    "\t[-O] generate _objects.mac file for current directory\n"
    "\t[-p] pause' before compile and link phases\n"
    "\t[-P] Print elapsed time after every directory\n"
    "\t[-q] query only, don't run NMAKE\n"
    "\t[-r dirPath] restarts clean build at specified directory path\n"
    "\t[-s] display status line at top of display\n"
    "\t[-S] display status line with include file line counts\n"
    "\t[-t] display the first level of the dependency tree\n"
    "\t[-T] display the complete dependency tree\n"
    "\t[-$] display the complete dependency tree hierarchically\n"
    "\t[-u] display unused BUILD_OPTIONS\n"
    "\t[-v] enable include file version checking\n"
    "\t[-w] show warnings on screen\n"
    "\t[-X] generates build.xml file\n"
    "\t[-Xv] generates verbose build.xml file\n"
    "\t[-y] show files scanned\n"
    "\t[-z] no dependency checking or scanning of source files -\n"
        "\t\tone pass compile/link\n"
    "\t[-Z] no dependency checking or scanning of source files -\n"
        "\t\ttwo passes\n"
    "\t[-why] list reasons for building targets\n"
    "\n"
    "\t[-all] same as -386, -alpha64, -alpha, and -ia64\n"
    "\t[-alpha] build targets for alpha\n"
    "\t[-alpha64] build targets for alpha64\n"
    "\t[-386] build targets for i386\n"
    "\t[-ia64] build targets for IA64\n"
    "\n"
    "\t[-x filename] exclude include file from dependency checks\n"
    "\t[-j filename] use 'filename' as the name for log files\n"
    "\t[-nmake arg] argument to pass to NMAKE\n"
    "\t[-clean] equivalent to '-nmake clean'\n"
    "\tNon-switch parameters specify additional source directories\n";


BOOL
ProcessParameters(int argc, LPSTR argv[]);

VOID
GetEnvParameters(
    LPSTR EnvVarName,
    LPSTR DefaultValue,
    int *pargc,
    int maxArgc,
    LPSTR argv[]);

VOID
FreeEnvParameters(int argc, LPSTR argv[]);

VOID
FreeCmdStrings(VOID);

VOID
MungePossibleTarget(
    PTARGET_MACHINE_INFO pti
    );

VOID
GetIncludePatterns(
    LPSTR EnvVarName,
    int maxArgc,
    LPSTR argv[]);

VOID
FreeIncludePatterns(
    int argc,
    LPSTR argv[]);

BOOL
LoadBaselineFile(VOID);

VOID
FreeBaselineFile(VOID);

//+---------------------------------------------------------------------------
//
//  Function:   main
//
//----------------------------------------------------------------------------

int
__cdecl main(
    int argc,
    LPSTR argv[]
    )
{
    char c;
    PDIRREC DirDB;
    UINT i;
    int EnvArgc;
    LPSTR EnvArgv[ MAX_ENV_ARG ];
    LPSTR s, s1;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    time_t ltime;
#if DBG
    BOOL fDebugSave;
#endif
    EnvArgc = 0;

#if DBG

    fDebug = 0;
#endif

    for (i=3; i<_NFILE; i++) {
        _close( i );
    }

    pGetFileAttributesExA = (BOOL (WINAPI *)(LPCSTR, GET_FILEEX_INFO_LEVELS, LPVOID))
                                GetProcAddress(GetModuleHandle("kernel32.dll"), "GetFileAttributesExA");

    if (pGetFileAttributesExA) {
        pDateTimeFile = DateTimeFile2;
    } else {
        pDateTimeFile = DateTimeFile;
    }

    InitializeCriticalSection(&TTYCriticalSection);

    s1 = getenv("COMSPEC");
    if (s1) {
        cmdexe = s1;
    } else {
        cmdexe = ( _osver & 0x8000 ) ? "command.com" : "cmd.exe";
    }

    memset(&RunningTotals, 0, sizeof(RunningTotals));

    MakeParameters[ 0 ] = '\0';
    MakeTargets[ 0 ] = '\0';
    EnvArgv[ 0 ] = "";
    EnvArgc = 1;
    CountExcludeIncs = 0;

    CountTargetMachines = 0;
    CountOptionalDirs = 0;
    CountExcludeDirs = 0;

    CountPassZeroDirs = 0;
    CountCompileDirs = 0;
    CountLinkDirs = 0;
    CountShowDirs = 0;
    CountIncludeDirs = 0;
    CountSystemIncludeDirs = 0;
    IncludeDirs[CountIncludeDirs++] = NULL;     // Placeholder for compiler
                                                // specific include directory
    AllDirs = NULL;

    fPassZeroOnly = FALSE;
    
    fErrorBaseline = FALSE;
    BaselinePathName[0] = '\0';
    bBaselineFailure = FALSE;
    pvBaselineContent = NULL;
    cbBaselineContentSize = 0;

    fXMLOutput = FALSE;
    fXMLVerboseOutput = FALSE;
    NumberPasses = 0;

    fXMLFragment = FALSE;

    memset(&BuildMetrics, 0, sizeof(BuildMetrics));

    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);

    {
        SYSTEMTIME st;
        FILETIME   ft;

        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);

        FileTimeToDosDateTime( &ft,
                               ((LPWORD)&BuildStartTime)+1,
                               (LPWORD)&BuildStartTime
                             );
    }

    BigBufSize = 0xFFF0;
    AllocMem(BigBufSize, &BigBuf, MT_IOBUFFER);

    // All env parsing should happen here (after the cmd line is processed)

    s = getenv("_NTROOT");
    if (!s)
        s = "\\nt";

    s1 = getenv("_NTDRIVE");
    if (!s1)
        s1 = "";

    sprintf(NtRoot, "%s%s", s1, s);
    sprintf(DbMasterName, "%s\\%s", NtRoot, DBMASTER_NAME);

    s = getenv("BUILD_ALT_DIR");
    if (s) {
        if (strlen(s) > sizeof(szObjDir) - strlen(szObjDir) - 1) {
            BuildError("environment variable BUILD_ALT_DIR may not be longer than %d characters.\n",
                    sizeof(szObjDir) - strlen(szObjDir) - 1);
            exit(1);
        }
        strcat(szObjDir, s);
        strcpy(szObjDirSlash, szObjDir);
        strcpy(szObjDirSlashStar, szObjDir);
        strcat(szObjDirSlash, "\\");
        strcat(szObjDirSlashStar, "\\*");
        strcat(LogFileName, s);
        strcat(WrnFileName, s);
        strcat(ErrFileName, s);
        strcat(IncFileName, s);
        strcat(XMLFileName, s);
    }

    s = getenv("NTDEBUG");
    if (!s || *s == '\0' || strcmp(s, "retail") == 0 || strcmp(s, "ntsdnodbg") == 0) {
        fCheckedBuild = FALSE;
    }

    s = getenv("CRT_INC_PATH");
    if (s) {
        MakeString(&pszIncCrt, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncCrt, "%s\\public\\sdk\\inc\\crt", TRUE, MT_DIRSTRING);
    }
    s = getenv("SDK_INC_PATH");
    if (s) {
        MakeString(&pszIncSdk, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncSdk, "%s\\public\\sdk\\inc", TRUE, MT_DIRSTRING);
    }
    s = getenv("OAK_INC_PATH");
    if (s) {
        MakeString(&pszIncOak, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncOak, "%s\\public\\oak\\inc", TRUE, MT_DIRSTRING);
    }
    s = getenv("DDK_INC_PATH");
    if (s) {
        MakeString(&pszIncDdk, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncDdk, "%s\\public\\ddk\\inc", TRUE, MT_DIRSTRING);
    }
    s = getenv("WDM_INC_PATH");
    if (s) {
        MakeString(&pszIncWdm, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncWdm, "%s\\public\\ddk\\inc\\wdm", TRUE, MT_DIRSTRING);
    }
    s = getenv("ATL_INC_PATH");
    if (s) {
        MakeString(&pszIncAtl, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncAtl, "%s\\public\\ddk\\inc\\atl", TRUE, MT_DIRSTRING);
    }
    s = getenv("PRIVATE_INC_PATH");
    if (s) {
        MakeString(&pszIncPri, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncPri, "%s\\private\\inc", TRUE, MT_DIRSTRING);
    }
    s = getenv("MFC_INCLUDES");
    if (s) {
        MakeString(&pszIncMfc, s, TRUE, MT_DIRSTRING);
    } else {
        MakeString(&pszIncMfc, "%s\\public\\sdk\\inc\\mfc42", TRUE, MT_DIRSTRING);
    }

    szBuildTag = getenv("BUILD_TAG");

    strcpy( MakeParameters, "" );
    MakeParametersTail = AppendString( MakeParameters,
                                       "/c BUILDMSG=Stop.",
                                       FALSE);

    CountFullDebugDirs = 0;
    if (s = getenv("BUILD_FULL_DEBUG")) {
        while (*s) {
            while (*s == ' ') {
                s++;
            }
            if (!*s) {
                break;
            }
            if (CountFullDebugDirs >= MAX_FULL_DEBUG_DIRECTORIES) {
                BuildError(
                    "Ignoring BUILD_FULL_DEBUG list after first %u entries\n",
                    CountFullDebugDirs);
                break;
            }

            s1 = s;
            while (*s1 && *s1 != ' ') {
                s1++;
            }

            c = *s1;
            *s1 = '\0';
            MakeString(
                &FullDebugDirectories[CountFullDebugDirs++],
                s,
                TRUE,
                MT_CMDSTRING);

            *s1 = c;
            s = s1;
        }
    }

    RecurseLevel = 0;

#if DBG
    if ((s = getenv("BUILD_DEBUG_FLAG")) != NULL) {
        i = atoi(s);
        if (!isdigit(*s)) {
            i = 1;
        }
        BuildMsg("Debug Output Enabled: %u ==> %u\n", fDebug, fDebug | i);
        fDebug |= i;
    }
#endif

    if (!(MakeProgram = getenv( "BUILD_MAKE_PROGRAM" ))) {
        MakeProgram = "NMAKE.EXE";
    }

    SystemIncludeEnv = getenv( "INCLUDE" );
    GetCurrentDirectory( sizeof( CurrentDirectory ), CurrentDirectory );

    for (i = 0; i < MAX_TARGET_MACHINES; i++) {
        TargetMachines[i] = NULL;
        TargetToPossibleTarget[i] = 0;
        MungePossibleTarget(PossibleTargetMachines[i]);
    }

    // prepare the command line in the XML buffer in case we need it
    strcpy(szXMLBuffer, "CMDLINE=\"");
    XMLEncodeBuiltInEntitiesCopy(GetCommandLine(), szXMLBuffer + strlen(szXMLBuffer));
    strcat(szXMLBuffer, "\"");

    if (!ProcessParameters( argc, argv )) {
        fUsage = TRUE;
    } else {
        int CurrentEnvArgc = EnvArgc;
        GetEnvParameters( "BUILD_DEFAULT", NULL, &EnvArgc, MAX_ENV_ARG, EnvArgv );
        if (CurrentEnvArgc != EnvArgc) {
            strcat(szXMLBuffer, " BUILD_DEFAULT=\"");
            while (CurrentEnvArgc < EnvArgc) {
                XMLEncodeBuiltInEntitiesCopy(EnvArgv[CurrentEnvArgc], szXMLBuffer + strlen(szXMLBuffer));
                strcat(szXMLBuffer, " ");
                CurrentEnvArgc++;
            }
            strcat(szXMLBuffer, "\"");
        }
        CurrentEnvArgc = EnvArgc;
        GetEnvParameters( "BUILD_OPTIONS", NULL, &EnvArgc, MAX_ENV_ARG, EnvArgv );
        if (CurrentEnvArgc != EnvArgc) {
            strcat(szXMLBuffer, " BUILD_OPTIONS=\"");
            while (CurrentEnvArgc < EnvArgc) {
                XMLEncodeBuiltInEntitiesCopy(EnvArgv[CurrentEnvArgc], szXMLBuffer + strlen(szXMLBuffer));
                strcat(szXMLBuffer, " ");
                CurrentEnvArgc++;
            }
            strcat(szXMLBuffer, "\"");
        }
        if (CountTargetMachines == 0) {
            if ( getenv("PROCESSOR_ARCHITECTURE") == NULL ) {
                BuildError("environment variable PROCESSOR_ARCHITECTURE must be defined\n");
                exit(1);
            }

            CurrentEnvArgc = EnvArgc;
            if (!strcmp(getenv("PROCESSOR_ARCHITECTURE"), "ALPHA64"))
                GetEnvParameters( "BUILD_DEFAULT_TARGETS", "-alpha64", &EnvArgc, MAX_ENV_ARG, EnvArgv );
            else
            if (!strcmp(getenv("PROCESSOR_ARCHITECTURE"), "ALPHA"))
                GetEnvParameters( "BUILD_DEFAULT_TARGETS", "-alpha", &EnvArgc, MAX_ENV_ARG, EnvArgv );
            else
            if (!strcmp(getenv("PROCESSOR_ARCHITECTURE"), "IA64"))
                GetEnvParameters( "BUILD_DEFAULT_TARGETS", "-ia64", &EnvArgc, MAX_ENV_ARG, EnvArgv );
            else
                GetEnvParameters( "BUILD_DEFAULT_TARGETS", "-386", &EnvArgc, MAX_ENV_ARG, EnvArgv );
            if (CurrentEnvArgc != EnvArgc) {
                strcat(szXMLBuffer, " BUILD_DEFAULT_TARGETS=\"");
                while (CurrentEnvArgc < EnvArgc) {
                    XMLEncodeBuiltInEntitiesCopy(EnvArgv[CurrentEnvArgc], szXMLBuffer + strlen(szXMLBuffer));
                    strcat(szXMLBuffer, " ");
                    CurrentEnvArgc++;
                }
                strcat(szXMLBuffer, "\"");
            }
        }
        if (!ProcessParameters( EnvArgc, EnvArgv )) {
            fUsage = TRUE;
        }
    }
    FreeEnvParameters(EnvArgc, EnvArgv);

    GetIncludePatterns( "BUILD_ACCEPTABLE_INCLUDES", MAX_INCLUDE_PATTERNS, AcceptableIncludePatternList );
    GetIncludePatterns( "BUILD_UNACCEPTABLE_INCLUDES", MAX_INCLUDE_PATTERNS, UnacceptableIncludePatternList );

    if (( fCheckIncludePaths ) &&
        ( AcceptableIncludePatternList[ 0 ] == NULL ) &&
        ( UnacceptableIncludePatternList[ 0 ] == NULL )) {

        BuildMsgRaw( "WARNING: -# specified without BUILD_[UN]ACCEPTABLE_INCLUDES set\n" );
    }

    if (fCleanRestart) {
        if (fClean) {
            fClean = FALSE;
            fRestartClean = TRUE;
        }
        else
        if (fCleanLibs) {
            fCleanLibs = FALSE;
            fRestartCleanLibs = TRUE;
        }
        else {
            BuildError("/R switch only valid with /c or /C switch.\n");
            fUsage = TRUE;
        }
    }

    NumberProcesses = 1;
    if (fParallel || getenv("BUILD_MULTIPROCESSOR")) {
        SYSTEM_INFO SystemInfo;

        if (DefaultProcesses == 0) {
            GetSystemInfo(&SystemInfo);
            NumberProcesses = SystemInfo.dwNumberOfProcessors;
        } else {
            NumberProcesses = DefaultProcesses;
        }
        if (NumberProcesses == 1) {
            fParallel = FALSE;
        } else {
            fParallel = TRUE;
            BuildMsg("Using %d child processes\n", NumberProcesses);
        }
    }

    XMLStartTicks = GetTickCount();
    time(&ltime);
    if (fPrintElapsed) {
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
        BuildMsg("Start time: %s", ctime(&ltime));
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
    }

    if (fColorConsole)
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
    BuildMsg("Object root set to: ==> %s\n", szObjDir);
    if (fColorConsole)
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);

    if (fUsage) {
        BuildMsgRaw(
            "\nBUILD: Version %x.%02x for Xbox. (Based on build 4.20 for Visual Studio)\n\n",
            BUILD_VERSION >> 8,
            BUILD_VERSION & 0xFF);
        BuildMsgRaw(szUsage);
    }
    else
    if (CountTargetMachines != 0) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);

        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
        BuildError(
            "%s for ",
            fLinkOnly? "Link" : (fCompileOnly? "Compile" : "Compile and Link"));
        for (i = 0; i < CountTargetMachines; i++) {
            BuildErrorRaw(i==0? "%s" : ", %s", TargetMachines[i]->Description);
            AppendString(
                MakeTargets,
                TargetMachines[i]->MakeVariable,
                TRUE);
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
        }

        BuildErrorRaw(szNewLine);

        //
        // If there is one and only one build target and target dirs has
        // been enabled, then fill in the appropriate target dirs name.
        //

        if (CountTargetMachines == 1) {
            if (fTargetDirs == TRUE) {
                pszTargetDirs = TargetMachines[0]->TargetDirs;
                FileDesc[0].pszPattern = TargetMachines[0]->TargetDirs;
            }
        }

        if (DEBUG_1) {
            if (CountExcludeIncs) {
                BuildError("Include files that will be excluded:");
                for (i = 0; i < CountExcludeIncs; i++) {
                    BuildErrorRaw(i == 0? " %s" : ", %s", ExcludeIncs[i]);
                }
                BuildErrorRaw(szNewLine);
            }
            if (CountOptionalDirs) {
                BuildError("Optional Directories that will be built:");
                for (i = 0; i < CountOptionalDirs; i++) {
                    BuildErrorRaw(i == 0? " %s" : ", %s", OptionalDirs[i]);
                }
                BuildErrorRaw(szNewLine);
            }
            if (CountExcludeDirs) {
                BuildError("Directories that will be NOT be built:");
                for (i = 0; i < CountExcludeDirs; i++) {
                    BuildErrorRaw(i == 0? " %s" : ", %s", ExcludeDirs[i]);
                }
                BuildErrorRaw(szNewLine);
            }
            BuildMsg("MakeParameters == %s\n", MakeParameters);
            BuildMsg("MakeTargets == %s\n", MakeTargets);
        }

#if DBG
        fDebugSave = fDebug;
        // fDebug = 0;
#endif

        //
        // Generate the _objects.mac file if requested
        //

        if (fGenerateObjectsDotMacOnly) {
            DIRSUP DirSup;
            ULONG DateTimeSources;

            DirDB = ScanDirectory( CurrentDirectory );

            if (DirDB && (DirDB->DirFlags & (DIRDB_DIRS | DIRDB_SOURCES))) {
                FreeBaselineFile();

                if (!ReadSourcesFile(DirDB, &DirSup, &DateTimeSources)) {
                    BuildError("Current directory not a SOURCES directory.\n");
                    return( 1 );
                }

                GenerateObjectsDotMac(DirDB, &DirSup, DateTimeSources);

                FreeDirSupData(&DirSup);
                ReportDirsUsage();
                FreeCmdStrings();
                ReportMemoryUsage();
                return(0);
            }
        }

        if (!fQuery && fErrorLog) {
            strcat(LogFileName, ".log");
            if (!MyOpenFile(".", LogFileName, "wb", &LogFile, TRUE)) {
                BuildError("(Fatal Error) Unable to open log file\n");
                exit( 1 );
            }
            CreatedBuildFile(".", LogFileName);

            strcat(WrnFileName, ".wrn");
            if (!MyOpenFile(".", WrnFileName, "wb", &WrnFile, FALSE)) {
                BuildError("(Fatal Error) Unable to open warning file\n");
                exit( 1 );
            }
            CreatedBuildFile(".", WrnFileName);

            strcat(ErrFileName, ".err");
            if (!MyOpenFile(".", ErrFileName, "wb", &ErrFile, FALSE)) {
                BuildError("(Fatal Error) Unable to open error file\n");
                exit( 1 );
            }
            CreatedBuildFile(".", ErrFileName);
            if ( fCheckIncludePaths ) {

                strcat( IncFileName, ".inc");
                if (!MyOpenFile( ".", IncFileName, "wb", &IncFile, FALSE ) ) {
                    BuildError( "(Fatal Error) Unable to open include log file\n");
                    exit( 1 );
                }
                CreatedBuildFile( ".", IncFileName );
            }
        }
        else {
            LogFile = NULL;
            WrnFile = NULL;
            ErrFile = NULL;
            IncFile = NULL;
        }

        // in case of query only we are not going to produce XML file
        if (fQuery) {
            fXMLOutput = FALSE;
        }

        // set the XML output file
        if (fXMLOutput) {
            strcat(XMLFileName, ".xml");
            if (!MyOpenFile(".", XMLFileName, "wb", &XMLFile, FALSE)) {
                BuildError("(Fatal Error) Unable to open XML file\n");
                exit( 1 );
            }
            CreatedBuildFile(".", XMLFileName);

            if (!XMLInit()) {
                exit( 1 );
            }
        }
        else {
            XMLFile = NULL;
        }

        sprintf(szXMLBuffer + strlen(szXMLBuffer), " TIME=\"%s\" CURRENTDIR=\"%s\"", ctime(&ltime), CurrentDirectory);
        if (fXMLOutput) {
            XMLGlobalWrite("<?xml version=\"1.0\"?>");
            XMLGlobalOpenTag("BUILD",  "xmlns=\"x-schema:buildschema.xml\"");
            XMLGlobalWrite("<START %s/>", szXMLBuffer);
            XMLUpdateEndTag(FALSE);
        }
        if (fXMLFragment) {
            XMLWriteFragmentFile("START", "<BUILD %s/>", szXMLBuffer);
        }

        //
        // The user should not have CHICAGO_PRODUCT in
        // their environment, as it can cause problems on other machines with
        // other users that don't have them set.  The following warning
        // messages are intended to alert the user to the presence of these
        // environment variables.
        //
        if (getenv("CHICAGO_PRODUCT") != NULL) {
            BuildError("CHICAGO_PRODUCT was detected in the environment.\n" );
            BuildMsg("   ALL directories will be built targeting Chicago!\n" );
            fChicagoProduct = TRUE;
        }

        if (!fQuicky) {
            LoadMasterDB();

            BuildError("Computing Include file dependencies:\n");

            ScanIncludeEnv(SystemIncludeEnv);
            ScanIncludeDir(pszIncMfc);
            ScanIncludeDir(pszIncOak);
            ScanIncludeDir(pszIncDdk);
            ScanIncludeDir(pszIncWdm);
            ScanIncludeDir(pszIncSdk);
            ScanIncludeDir(pszIncPri);
            CountSystemIncludeDirs = CountIncludeDirs;
        }

#if DBG
        fDebug = fDebugSave;
#endif
        fFirstScan = TRUE;
        fPassZero  = FALSE;
        ScanSourceDirectories( CurrentDirectory );

        if (!fQuicky) {
            if (SaveMasterDB() == FALSE) {
                BuildError("Unable to save the dependency database: %s\n", DbMasterName);
            }
        }

        c = '\n';
        if (!fLinkOnly && CountPassZeroDirs && !bBaselineFailure) {
            if (!fQuicky) {
                TotalFilesToCompile = 0;
                TotalLinesToCompile = 0L;

                for (i=0; i<CountPassZeroDirs; i++) {
                    DirDB = PassZeroDirs[ i ];

                    TotalFilesToCompile += DirDB->CountOfPassZeroFiles;
                    TotalLinesToCompile += DirDB->PassZeroLines;
                    }

                if (CountPassZeroDirs > 1 &&
                    TotalFilesToCompile != 0 &&
                    TotalLinesToCompile != 0L) {

                    BuildMsgRaw(
                        "Total of %d pass zero files (%s lines) to compile in %d directories\n\n",
                         TotalFilesToCompile,
                         FormatNumber( TotalLinesToCompile ),
                         CountPassZeroDirs);
                }
            }

            TotalFilesCompiled    = 0;
            TotalLinesCompiled    = 0L;
            ElapsedCompileTime    = 0L;

            if (fPause) {
                BuildMsg("Press enter to continue with compilations (or 'q' to quit)...");
                c = (char)getchar();
            }

            if ((CountPassZeroDirs > 0) && (c == '\n') && !bBaselineFailure) {
                DWORD dwStartTime = GetTickCount();
                if (fXMLOutput) {
                    XMLGlobalOpenTag("PASS", "NUMBER=\"0\"");
                }
                if (fXMLFragment) {
                    XMLWriteFragmentFile("PASS0", "<PASS NUMBER=\"0\"/>");
                }
                memset(&PassMetrics, 0, sizeof(PassMetrics));
                CompilePassZeroDirectories();
                WaitForParallelThreads();

                AddBuildMetrics(&BuildMetrics, &PassMetrics);
                if (fXMLOutput || fXMLFragment) {
                    sprintf(szXMLBuffer, "DIRS=\"%d\" ELAPSED=\"%s\" ", CountPassZeroDirs, FormatElapsedTime(dwStartTime));
                    strcat(szXMLBuffer, XMLBuildMetricsString(&PassMetrics));
                    NumberPasses++;
                    if (fXMLOutput) {
                        XMLGlobalWrite("<PASSSUMMARY %s/>", szXMLBuffer);
                        XMLGlobalCloseTag();
                        XMLUpdateEndTag(FALSE);
                    }
                    if (fXMLFragment) {
                        XMLWriteFragmentFile("PASS0SUMMARY", "<PASSSUMMARY %s/>", szXMLBuffer);
                    }
                }

                //
                // Rescan now that we've generated all the generated files
                //
                CountPassZeroDirs = 0;
                CountCompileDirs = 0;
                CountLinkDirs = 0;

                UnsnapAllDirectories();

                fPassZero = FALSE;
                fFirstScan = FALSE;
                RecurseLevel = 0;

                if (!fPassZeroOnly) {
                    ScanSourceDirectories( CurrentDirectory );
                } else {
                    BuildMsg("Pass Zero Completed\n");
                }

                if (!fQuicky) {
                    if (SaveMasterDB() == FALSE) {
                        BuildError("Unable to save the dependency database: %s\n", DbMasterName);
                    }
                }
            }
        }

        if (!fPassZeroOnly && !fLinkOnly && (c == '\n') && !bBaselineFailure) {
            if (!fQuicky) {
                TotalFilesToCompile = 0;
                TotalLinesToCompile = 0L;

                for (i=0; i<CountCompileDirs; i++) {
                    DirDB = CompileDirs[ i ];

                    TotalFilesToCompile += DirDB->CountOfFilesToCompile;
                    TotalLinesToCompile += DirDB->SourceLinesToCompile;
                    }

                if (CountCompileDirs > 1 &&
                    TotalFilesToCompile != 0 &&
                    TotalLinesToCompile != 0L) {

                    BuildMsgRaw(
                        "Total of %d source files (%s lines) to compile in %d directories\n\n",
                         TotalFilesToCompile,
                         FormatNumber( TotalLinesToCompile ),
                         CountCompileDirs);
                }
            }

            TotalFilesCompiled    = 0;
            TotalLinesCompiled    = 0L;
            ElapsedCompileTime    = 0L;

            if (fPause) {
                BuildMsg("Press enter to continue with compilations (or 'q' to quit)...");
                c = (char)getchar();
            }

            if (c == '\n' && !bBaselineFailure) {
                DWORD dwStartTime = GetTickCount();
                if (fXMLOutput) {
                    XMLGlobalOpenTag("PASS", "NUMBER=\"1\"");
                }
                if (fXMLFragment) {
                    XMLWriteFragmentFile("PASS1", "<PASS NUMBER=\"1\"/>");
                }
                memset(&PassMetrics, 0, sizeof(PassMetrics));
                CompileSourceDirectories();
                WaitForParallelThreads();

                AddBuildMetrics(&BuildMetrics, &PassMetrics);
                if (fXMLOutput || fXMLFragment) {
                    sprintf(szXMLBuffer, "DIRS=\"%d\" ELAPSED=\"%s\" ", CountCompileDirs, FormatElapsedTime(dwStartTime));
                    strcat(szXMLBuffer, XMLBuildMetricsString(&PassMetrics));
                    NumberPasses++;
                    if (fXMLOutput) {
                        XMLGlobalWrite("<PASSSUMMARY %s/>", szXMLBuffer);
                        XMLGlobalCloseTag();
                        XMLUpdateEndTag(FALSE);
                    }
                    if (fXMLFragment) {
                        XMLWriteFragmentFile("PASS1SUMMARY", "<PASSSUMMARY %s/>", szXMLBuffer);
                    }
                }
            }
        }

        if (!fPassZeroOnly && !fCompileOnly && (c == '\n') && !bBaselineFailure) {
            DWORD dwStartTime = GetTickCount();
            if (fXMLOutput) {
                XMLGlobalOpenTag("PASS", "NUMBER=\"2\"");
            }
            if (fXMLFragment) {
                XMLWriteFragmentFile("PASS2", "<PASS NUMBER=\"2\"/>");
            }
            memset(&PassMetrics, 0, sizeof(PassMetrics));
            LinkSourceDirectories();
            WaitForParallelThreads();

            AddBuildMetrics(&BuildMetrics, &PassMetrics);
            if (fXMLOutput || fXMLFragment) {
                sprintf(szXMLBuffer, "DIRS=\"%d\" ELAPSED=\"%s\" ", CountLinkDirs, FormatElapsedTime(dwStartTime));
                strcat(szXMLBuffer, XMLBuildMetricsString(&PassMetrics));
                NumberPasses++;
                if (fXMLOutput) {
                    XMLGlobalWrite("<PASSSUMMARY %s/>", szXMLBuffer);
                    XMLGlobalCloseTag();
                    XMLUpdateEndTag(FALSE);
                }
                if (fXMLFragment) {
                    XMLWriteFragmentFile("PASS2SUMMARY", "<PASSSUMMARY %s/>", szXMLBuffer);
                }
            }
        }

        if (fShowTree) {
            for (i = 0; i < CountShowDirs; i++) {
                PrintDirDB(ShowDirs[i], 1|4);
            }
        }
    }
    else {
        BuildError("No target machine specified\n");
    }

    // moved the end time before the log files are closed so we can put it into the XML file
    time(&ltime);

    if (fXMLOutput) {
        XMLUpdateEndTag(TRUE);
        XMLGlobalCloseTag();    // BUILD
    }
    if (fXMLFragment) {
        sprintf(szXMLBuffer, "TIME=\"%s\" ELAPSED=\"%s\" PASSES=\"%d\" COMPLETED=\"1\" ", ctime(&ltime), FormatElapsedTime(XMLStartTicks), NumberPasses);
        strcat(szXMLBuffer, XMLBuildMetricsString(&BuildMetrics));
        XMLWriteFragmentFile("END", "<END %s/>", szXMLBuffer);
    }

    if (!fUsage && !fQuery && fErrorLog) {
        ULONG cbLogMin = 32;
        ULONG cbWarnMin = 0;

        if (!fAlwaysKeepLogfile) {
            if (fQuicky && !fSemiQuicky && ftell(ErrFile) == 0) {
                cbLogMin = cbWarnMin = ULONG_MAX;
            }
        }
        CloseOrDeleteFile(&LogFile, LogFileName, cbLogMin);
        CloseOrDeleteFile(&WrnFile, WrnFileName, cbWarnMin);
        CloseOrDeleteFile(&ErrFile, ErrFileName, 0L);
        if ( fCheckIncludePaths ) {
            CloseOrDeleteFile(&IncFile, IncFileName, cbLogMin);
        }
        CloseOrDeleteFile(&XMLFile, XMLFileName, 0L);
    }

    if (bBaselineFailure) {
        BuildError(BaselinePathName[0] != '\0' ? 
            "Diffs from baseline\n" :
            "Terminated at the first error encountered\n");
    }
    
    if (fPrintElapsed) {
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
        BuildMsg("Finish time: %s", ctime(&ltime));
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
    }

    if (fColorConsole)
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
    BuildError("Done\n\n");
    if (fColorConsole)
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);

    if (RunningTotals.NumberCompiles) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);

        BuildMsgRaw("    %d files compiled", RunningTotals.NumberCompiles);
        if (RunningTotals.NumberCompileWarnings) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            BuildMsgRaw(" - %d Warnings", RunningTotals.NumberCompileWarnings);
        }
        if (RunningTotals.NumberCompileErrors) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            BuildMsgRaw(" - %d Errors", RunningTotals.NumberCompileErrors);
        }

        if (ElapsedCompileTime) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            BuildMsgRaw(" - %5ld LPS", TotalLinesCompiled / ElapsedCompileTime);
        }

        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
        BuildMsgRaw(szNewLine);
    }

    if (RunningTotals.NumberLibraries) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);

        BuildMsgRaw("    %d libraries built", RunningTotals.NumberLibraries);
        if (RunningTotals.NumberLibraryWarnings) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            BuildMsgRaw(" - %d Warnings", RunningTotals.NumberLibraryWarnings);
        }
        if (RunningTotals.NumberLibraryErrors) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            BuildMsgRaw(" - %d Errors", RunningTotals.NumberLibraryErrors);
        }

        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
        BuildMsgRaw(szNewLine);
    }

    if (RunningTotals.NumberLinks) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);

        BuildMsgRaw("    %d executables built", RunningTotals.NumberLinks);
        if (RunningTotals.NumberLinkWarnings) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            BuildMsgRaw(" - %d Warnings", RunningTotals.NumberLinkWarnings);
        }
        if (RunningTotals.NumberLinkErrors) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            BuildMsgRaw(" - %d Errors", RunningTotals.NumberLinkErrors);
        }

        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
        BuildMsgRaw(szNewLine);
    }

    if (RunningTotals.NumberBSCMakes) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);
        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);

        BuildMsgRaw("    %d browse databases built", RunningTotals.NumberBSCMakes);
        if (RunningTotals.NumberBSCWarnings) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            BuildMsgRaw(" - %d Warnings", RunningTotals.NumberBSCWarnings);
        }
        if (RunningTotals.NumberBSCErrors) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            BuildMsgRaw(" - %d Errors", RunningTotals.NumberBSCErrors);
        }

        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
        BuildMsgRaw(szNewLine);
    }

    if (RunningTotals.NumberVSToolErrors + RunningTotals.NumberVSToolWarnings > 0)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);

        if (RunningTotals.NumberVSToolWarnings) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            BuildMsgRaw("    %d VS Tool Warnings\n", RunningTotals.NumberVSToolWarnings);
        }
        if (RunningTotals.NumberVSToolErrors) {
            if (fColorConsole)
                SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            BuildMsgRaw("    %d VS Tool Errors\n", RunningTotals.NumberVSToolErrors);
        }

        if (fColorConsole)
            SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), csbi.wAttributes);
    }

    ReportDirsUsage();
    if (fXMLOutput) XMLUnInit();
    FreeBaselineFile();
    FreeCmdStrings();
    FreeIncludePatterns( MAX_INCLUDE_PATTERNS, AcceptableIncludePatternList );
    FreeIncludePatterns( MAX_INCLUDE_PATTERNS, UnacceptableIncludePatternList );
    ReportMemoryUsage();

    if (bBaselineFailure)
        return 2;

    if (RunningTotals.NumberCompileErrors || RunningTotals.NumberLibraryErrors || RunningTotals.NumberLinkErrors || RunningTotals.NumberVSToolErrors) {
        return 1;
    }
    else {
        return( 0 );
    }
}


VOID
ReportDirsUsage( VOID )
{
    ULONG i;
    BOOLEAN fHeaderPrinted;

    if (!fShowUnusedDirs) {
        return;
    }

    fHeaderPrinted = FALSE;
    for (i=0; i<CountOptionalDirs; i++) {
        if (!OptionalDirsUsed[i]) {
            if (!fHeaderPrinted) {
                printf( "Unused BUILD_OPTIONS:" );
                fHeaderPrinted = TRUE;
            }
            printf( " %s", OptionalDirs[i] );
        }
    }

    for (i=0; i<CountExcludeDirs; i++) {
        if (!ExcludeDirsUsed[i]) {
            if (!fHeaderPrinted) {
                printf( "Unused BUILD_OPTIONS:" );
                fHeaderPrinted = TRUE;
            }
            printf( " ~%s", ExcludeDirs[i] );
        }
    }

    if (fHeaderPrinted) {
        printf( "\n" );
    }

    fHeaderPrinted = FALSE;
    for (i = 0; i < CountFullDebugDirs; i++) {
        if (!FullDebugDirsUsed[i]) {
            if (!fHeaderPrinted) {
                printf( "Unused BUILD_FULL_DEBUG:" );
                fHeaderPrinted = TRUE;
            }
            printf( " %s", FullDebugDirectories[i] );
        }
    }
    if (fHeaderPrinted) {
        printf( "\n" );
    }
}


//+---------------------------------------------------------------------------
//
//  Function:   SetObjDir
//
//----------------------------------------------------------------------------

VOID
SetObjDir(BOOL fAlternate)
{
    iObjectDir = 0;
    if (fCheckedBuild) {
        if (fAlternate) {
            pszObjDir = szObjDirD;
            pszObjDirSlash = szObjDirSlashD;
            pszObjDirSlashStar = szObjDirSlashStarD;
            iObjectDir = 1;
        } else {
            pszObjDir = szObjDir;
            pszObjDirSlash = szObjDirSlash;
            pszObjDirSlashStar = szObjDirSlashStar;
        }
    }
}



//+---------------------------------------------------------------------------
//
//  Function:   AddTargetMachine
//
//----------------------------------------------------------------------------

VOID
AddTargetMachine(UINT iTarget)
{
    UINT i;

    for (i = 0; i < CountTargetMachines; i++) {
        if (TargetMachines[i] == PossibleTargetMachines[iTarget]) {
            assert(TargetToPossibleTarget[i] == iTarget);
            return;
        }
    }
    assert(CountTargetMachines < MAX_TARGET_MACHINES);
    TargetToPossibleTarget[CountTargetMachines] = iTarget;
    TargetMachines[CountTargetMachines++] = PossibleTargetMachines[iTarget];
}


//+---------------------------------------------------------------------------
//
//  Function:   ProcessParameters
//
//----------------------------------------------------------------------------

BOOL
ProcessParameters(
    int argc,
    LPSTR argv[]
    )
{
    char c, *p;
    int i;
    BOOL Result;

    if (DEBUG_1) {
        BuildMsg("Parsing:");
        for (i=1; i<argc; i++) {
            BuildMsgRaw(" %s", argv[i]);
        }
        BuildMsgRaw(szNewLine);
    }

    Result = TRUE;
    while (--argc) {
        p = *++argv;
        if (*p == '/' || *p == '-') {
            if (DEBUG_1) {
                BuildMsg("Processing \"-%s\" switch\n", p+1);
            }

            for (i = 0; i < MAX_TARGET_MACHINES; i++) {
                if (!_stricmp(p, PossibleTargetMachines[i]->Switch)) {
                    AddTargetMachine(i);
                    break;
                }
            }

            if (i < MAX_TARGET_MACHINES) {
            }
            else
            if (!_stricmp(p + 1, "all")) {
                for (i = 0; i < MAX_TARGET_MACHINES; i++) {
                    AddTargetMachine(i);
                }
            }
            else
            if (!_stricmp(p + 1, "why")) {
                fWhyBuild = TRUE;
            }
            else
            while (c = *++p)
                switch (toupper( c )) {
            case '?':
                fUsage = TRUE;
                break;
                
            case '0':
                fPassZeroOnly = TRUE;
                break;
                
            case '3':
                fQuickZero = TRUE;
                fSemiQuicky = TRUE;
                fQuicky = TRUE;
                break;

            case 'A':
                fSyncLink = TRUE;
                break;

            case 'J': {

                argc--, argv++;

                // Clear it out
                memset(LogFileName, 0, sizeof(LogFileName));
                memset(WrnFileName, 0, sizeof(WrnFileName));
                memset(ErrFileName, 0, sizeof(ErrFileName));
                memset(IncFileName, 0, sizeof(IncFileName));
                memset(XMLFileName, 0, sizeof(XMLFileName));

                // And set it to the arg passed in.
                strncpy(LogFileName, *argv, sizeof(LogFileName) - 4);
                strncpy(WrnFileName, *argv, sizeof(WrnFileName) - 4);
                strncpy(ErrFileName, *argv, sizeof(ErrFileName) - 4);
                strncpy(IncFileName, *argv, sizeof(IncFileName) - 4);
                strncpy(XMLFileName, *argv, sizeof(XMLFileName) - 4);

                break;
            }

            case 'E':
                if (c == 'E') {
                    fAlwaysKeepLogfile = TRUE;
                }
                fErrorLog = TRUE;
                break;

            case 'S':
                fStatus = TRUE;
                if (c == 'S') {
                    fStatusTree = TRUE;
                }
                break;

            case 'T':
                fShowTree = TRUE;
                if (c == 'T') {
                    fShowTreeIncludes = TRUE;
                    }
                break;

            case 'U':
                fShowUnusedDirs = TRUE;
                break;

            case 'B':
                if (c == 'b') {
                    fFullErrors = TRUE;
                }
                else {
                    fErrorBaseline = TRUE;

                    if (--argc) {
                        ++argv;

                        if (**argv != '/' && **argv != '-') {   
                            if (ProbeFile(NULL, *argv) != -1) {
                                CanonicalizePathName(*argv, CANONICALIZE_ONLY, BaselinePathName);
                                Result = LoadBaselineFile();
                            }
                            else {
                                BuildError("The specified baseline file doesn't exist\n");
                                Result = FALSE;
                            }
                        }
                        else {
                            // the next parameter is a switch, reprocess it
                            --argv;
                            ++argc;
                        }
                    } else {
                        // no more parameters
                        ++argc;
                    }
                }
                break;

            case 'C':
                if (!_stricmp( p, "clean" )) {
                        MakeParametersTail = AppendString( MakeParametersTail,
                                                           "clean",
                                                           TRUE);
                        *p-- = '\0';
                }
                else
                if (c == 'C') {
                    fCleanLibs = TRUE;
                }
                else {
                    fClean = TRUE;
                }
                break;

            case 'R':
                if (--argc) {
                    fCleanRestart = TRUE;
                    ++argv;
                    CopyString(RestartDir, *argv, TRUE);
                }
                else {
                    argc++;
                    BuildError("Argument to /R switch missing\n");
                    Result = FALSE;
                }
                break;

            case 'D':
                if (c == 'D')
                    fColorConsole = TRUE;
#if DBG
                else
                    fDebug |= 1;
#endif
                break;

            case '$':
                fDebug += 2;    // yes, I want to *add* 2.
                break;

            case '#':
                fCheckIncludePaths = TRUE;
                fForce = TRUE;
                break;

            case 'O':
                if (c == 'O') {
                    fGenerateObjectsDotMacOnly = TRUE;
                }
                else {
                    fShowOutOfDateFiles = TRUE;
                }
                break;

            case 'P':
                if (c == 'P') {
                    fPrintElapsed = TRUE;
                } else {
                    fPause = TRUE;
                }
                break;

            case 'Q':
                fQuery = TRUE;
                break;

            case 'F':
                if (c == 'F') {
                    fAlwaysPrintFullPath = TRUE;
                } else {
                    fForce = TRUE;
                }
                break;

            case 'G':
                fTargetDirs = TRUE;
                break;

            case 'V':
                fEnableVersionCheck = TRUE;
                break;

            case 'I':
                fSilentDependencies = TRUE;
                break;

            case 'K':
                fKeep = TRUE;
                break;

            case 'M':
                if (c == 'M') {
                    fParallel = TRUE;
                    if (--argc) {
                        DefaultProcesses = atoi(*++argv);
                        if (DefaultProcesses == 0) {
                            --argv;
                            ++argc;
                        }
                    } else {
                        ++argc;
                    }
                } else {
                    SetPriorityClass(GetCurrentProcess(),IDLE_PRIORITY_CLASS);
                }
                break;

            case 'L':
                if (c == 'L') {
                    fCompileOnly = TRUE;
                }
                else {
                    fLinkOnly = TRUE;
                }
                break;

            case 'X':
                if (!strcmp(p, "Xf")) {
                    // The Xf switch produces XML fragments in a specified directory
                    if (--argc) {
                        ++argv;
                        if (!CanonicalizePathName(*argv, CANONICALIZE_DIR, XMLFragmentDirectory)) {
                            Result = FALSE;
                        }
                        else {
                            fXMLFragment = TRUE;
                        }
                    } else {
                        ++argc;
                        BuildError("Argument to /Xf switch missing\n");
                        Result = FALSE;
                    }
                }
                else
                if (c == 'X') {
                    fXMLOutput = TRUE;
                    if (p[1] == 'v' ) {
                        ++p;
                        fXMLVerboseOutput = TRUE;
                    }
                }
                else {
                    if (--argc) {
                        ++argv;
                        if (CountExcludeIncs >= MAX_EXCLUDE_INCS) {
                            static BOOL fError = FALSE;

                            if (!fError) {
                                BuildError(
                                    "-x argument table overflow, using first %u entries\n",
                                    MAX_EXCLUDE_INCS);
                                fError = TRUE;
                            }
                        }
                        else {
                            MakeString(
                                &ExcludeIncs[CountExcludeIncs++],
                                *argv,
                                TRUE,
                                MT_CMDSTRING);
                        }
                    }
                    else {
                        argc++;
                        BuildError("Argument to /X switch missing\n");
                        Result = FALSE;
                    }
                }
                break;

            case 'N':
                if (_stricmp( p, "nmake") == 0) {
                    if (--argc) {
                        ++argv;
                        MakeParametersTail = AppendString( MakeParametersTail,
                                                           *argv,
                                                           TRUE);
                    }
                    else {
                        argc++;
                        BuildError("Argument to /NMAKE switch missing\n");
                        Result = FALSE;
                    }
                    *p-- = '\0';
                    break;
                }
                else
                {
                    if (c == 'n') {
                       fThreadIndex = TRUE;
                    }
                }
                break;

            case 'W':
                fShowWarningsOnScreen = TRUE;
                break;

            case 'Y':
                fNoisyScan = TRUE;
                break;

            case 'Z':
                if (c == 'Z') {
                    fSemiQuicky = TRUE;
                }

                fQuicky = TRUE;
                break;

            default:
                BuildError("Invalid switch - /%c\n", c);
                Result = FALSE;
                break;
            }
        }
        else
        if (*p == '~') {
            if (CountExcludeDirs >= MAX_EXCLUDE_DIRECTORIES) {
                static BOOL fError = FALSE;

                if (!fError) {
                    BuildError(
                        "Exclude directory table overflow, using first %u entries\n",
                        MAX_EXCLUDE_DIRECTORIES);
                    fError = TRUE;
                }
            }
            else {
                MakeString(
                    &ExcludeDirs[CountExcludeDirs++],
                    p + 1,
                    TRUE,
                    MT_CMDSTRING);
            }
        }
        else {
            for (i = 0; i < MAX_TARGET_MACHINES; i++) {
                if (!_stricmp(p, PossibleTargetMachines[i]->MakeVariable)) {
                    AddTargetMachine(i);
                    break;
                }
            }
            if (i >= MAX_TARGET_MACHINES) {
                if (iscsym(*p) || *p == '.') {
                    if (CountOptionalDirs >= MAX_OPTIONAL_DIRECTORIES) {
                        static BOOL fError = FALSE;

                        if (!fError) {
                            BuildError(
                                "Optional directory table overflow, using first %u entries\n",
                                MAX_OPTIONAL_DIRECTORIES);
                            fError = TRUE;
                        }
                    }
                    else {
                        MakeString(
                            &OptionalDirs[CountOptionalDirs++],
                            p,
                            TRUE,
                            MT_CMDSTRING);
                    }
                }
                else {
                    MakeParametersTail = AppendString(
                                            MakeParametersTail,
                                            p,
                                            TRUE);
                }
            }
        }
    }
    return(Result);
}


//+---------------------------------------------------------------------------
//
//  Function:   GetEnvParameters
//
//----------------------------------------------------------------------------

VOID
GetEnvParameters(
    LPSTR EnvVarName,
    LPSTR DefaultValue,
    int *pargc,
    int maxArgc,
    LPSTR argv[]
    )
{
    LPSTR p, p1, psz;

    if (!(p = getenv(EnvVarName))) {
        if (DefaultValue == NULL) {
            return;
        }
        else {
            p = DefaultValue;
        }
    }
    else {
        if (DEBUG_1) {
            BuildMsg("Using %s=%s\n", EnvVarName, p);
        }
    }

    MakeString(&psz, p, FALSE, MT_CMDSTRING);
    p1 = psz;
    while (*p1) {
        while (*p1 <= ' ') {
            if (!*p1) {
                break;
            }
            p1++;
        }
        p = p1;
        while (*p > ' ') {
            if (*p == '#') {
                *p = '=';
            }
            p++;
        }
        if (*p) {
            *p++ = '\0';
        }
        MakeString(&argv[*pargc], p1, FALSE, MT_CMDSTRING);
        if ((*pargc += 1) >= maxArgc) {
            BuildError("Too many parameters (> %d)\n", maxArgc);
            exit(1);
        }
        p1 = p;
    }
    FreeMem(&psz, MT_CMDSTRING);
}


//+---------------------------------------------------------------------------
//
//  Function:   FreeEnvParameters
//
//----------------------------------------------------------------------------

VOID
FreeEnvParameters(int argc, LPSTR argv[])
{
    while (--argc) {
        FreeMem(&argv[argc], MT_CMDSTRING);
    }
}


//+---------------------------------------------------------------------------
//
//  Function:   FreeCmdStrings
//
//----------------------------------------------------------------------------

VOID
FreeCmdStrings(VOID)
{
#if DBG
    UINT i;

    for (i = 0; i < CountExcludeIncs; i++) {
        FreeMem(&ExcludeIncs[i], MT_CMDSTRING);
    }
    for (i = 0; i < CountOptionalDirs; i++) {
        FreeMem(&OptionalDirs[i], MT_CMDSTRING);
    }
    for (i = 0; i < CountExcludeDirs; i++) {
        FreeMem(&ExcludeDirs[i], MT_CMDSTRING);
    }
    FreeMem(&pszIncMfc, MT_DIRSTRING);
    FreeMem(&pszIncSdk, MT_DIRSTRING);
    FreeMem(&pszIncCrt, MT_DIRSTRING);
    FreeMem(&pszIncOak, MT_DIRSTRING);
    FreeMem(&pszIncDdk, MT_DIRSTRING);
    FreeMem(&pszIncWdm, MT_DIRSTRING);
    FreeMem(&pszIncAtl, MT_DIRSTRING);
    FreeMem(&pszIncPri, MT_DIRSTRING);
#endif
}

//+---------------------------------------------------------------------------
//
//  Function:   MungePossibleTarget
//
//----------------------------------------------------------------------------

VOID
MungePossibleTarget(
    PTARGET_MACHINE_INFO pti
    )
{
    PCHAR s;
    char *pszDir;

    if (!pti) {
        return;
    }

    // save "i386" string

    pszDir = pti->ObjectDirectory[0];

    // Create "$(_OBJ_DIR)\i386" string

    s = malloc(12 + strlen(pszDir) + 1);
    sprintf(s, "$(_OBJ_DIR)\\%s", pszDir);
    pti->ObjectMacro = s;

    // Create "obj$(BUILD_ALT_DIR)\i386" string for default obj dir

    s = malloc(strlen(szObjDir) + 1 + strlen(pszDir) + 1);
    sprintf(s, "%s\\%s", szObjDir, pszDir);
    pti->ObjectDirectory[0] = s;

    // Create "objd\i386" string for alternate checked obj dir

    s = malloc(strlen(szObjDirD) + 1 + strlen(pszDir) + 1);
    sprintf(s, "%s\\%s", szObjDirD, pszDir);
    pti->ObjectDirectory[1] = s;
}

//+---------------------------------------------------------------------------
//
//  Function:   GetIncludePatterns
//
//----------------------------------------------------------------------------

VOID
GetIncludePatterns(
    LPSTR EnvVarName,
    int maxArgc,
    LPSTR argv[]
    )
{
    LPSTR p, p1, psz;
    int argc;

    argc = 0;

    if ( ( p = getenv(EnvVarName ) ) == NULL ) {
        return;
    }

    MakeString( &psz, p, FALSE, MT_DIRSTRING );

    p1 = psz;
    while ( *p1 ) {
        while ( *p1 == ';' || *p1 == ' ' ) {
            p1++;
        }
        p = p1;
        while ( *p && *p != ';' ) {
            p++;
        }
            if ( *p ) {
            *p++ = '\0';
        }
        MakeString( &argv[argc], p1, FALSE, MT_DIRSTRING );
        if ( ( argc += 1 ) == maxArgc ) {
            BuildError( "Too many include patterns ( > %d)\n", maxArgc );
            exit( 1 );
        }

        p1 = p;
    }

    FreeMem(&psz, MT_DIRSTRING);
}

//+---------------------------------------------------------------------------
//
//  Function:   FreeIncludePatterns
//
//----------------------------------------------------------------------------

VOID
FreeIncludePatterns(int argc, LPSTR argv[])
{
    while ( argc ) {
        if ( argv[--argc] ) {
            FreeMem( &argv[argc], MT_DIRSTRING );
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   LoadBaselineFile
//
//----------------------------------------------------------------------------
BOOL
LoadBaselineFile(VOID)
{
    BOOL Result = FALSE;
    FILE* FBase = NULL;
    long lSize = 0;

    if (BaselinePathName[0] == '\0')
        goto Cleanup;

    if (!MyOpenFile("", BaselinePathName, "rb", &FBase, FALSE))
        goto Cleanup;

    if (fseek(FBase, 0, SEEK_END))
        goto Cleanup;

    if ((lSize = ftell(FBase)) == -1)
        goto Cleanup;

    if (lSize == 0) {
        // if the baseline is zero-length file, do as if it weren't specified
        Result = TRUE;
        BaselinePathName[0] = '\0';
        goto Cleanup;
    }

    if (fseek(FBase, 0, SEEK_SET))
        goto Cleanup;

    if ((pvBaselineContent = malloc(lSize)) == NULL)
        goto Cleanup;

    if ((long) fread(pvBaselineContent, 1, lSize, FBase) != lSize)
    {
        free(pvBaselineContent);
        pvBaselineContent = NULL;
        goto Cleanup;
    }

    cbBaselineContentSize = (DWORD)lSize;
    Result = TRUE;

Cleanup:
    if (FBase != NULL)
        fclose(FBase);
    return Result;
}

VOID
FreeBaselineFile(VOID)
{
    if (NULL != pvBaselineContent)
        free(pvBaselineContent);
    pvBaselineContent = NULL;
    cbBaselineContentSize = 0;
}