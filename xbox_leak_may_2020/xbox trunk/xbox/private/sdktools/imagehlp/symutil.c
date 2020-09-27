#include <assert.h>
#include "symutil.h"
#define PDB_LIBRARY
#include "pdb.h"
#include "dbghelp.h"
#include "cvinfo.h"
#include "cvexefmt.h"
#include "exe_vxd.h"
#include "share.h"
#include "winbase.h"


// Stuff for Checking symbols


#define DBGSYM 2
#define PDBSYM 3

BOOL CheckPrivate=FALSE;
BOOL CheckCodeView=TRUE;
PEXCLUDE_LIST pErrorFilterList=NULL;
P_LIST pCDIncludeList=NULL;
BOOL DbgOnly = FALSE;
BOOL Recurse = FALSE;
BOOL LogCheckSumErrors = FALSE;

// Typedefs

typedef struct _NB10I {
    DWORD   dwSig;    // NB10
    DWORD   dwOffset; // offset, always 0
    SIG     sig;
    AGE     age;
    char    szPDB[_MAX_PATH];
}NB10I, *PNB10I;

typedef struct _FILE_INFO {
    DWORD       TimeDateStamp;
    DWORD       SizeOfImage;
    DWORD       CheckSum;
    TCHAR       szName[MAX_PATH];
} FILE_INFO, *PFILE_INFO;


PIMAGE_DOS_HEADER
MapFileHeader (
              LPTSTR szFileName,
              PHANDLE phFile,
              PSYM_ERR pSymErr
              );

PIMAGE_NT_HEADERS
GetNtHeader (
            PIMAGE_DOS_HEADER pDosHeader,
            HANDLE hDosFile,
            LPTSTR szFileName,
            PSYM_ERR pSymErr
            );

BOOL
ResourceOnlyDll(
               PVOID pImageBase,
               BOOLEAN bMappedAsImage
               );

PIMAGE_SEPARATE_DEBUG_HEADER
MapDbgHeader (
             LPTSTR szFileName,
             PHANDLE phFile,
             PSYM_ERR pSymErr
             );

BOOL
UnmapFile(
         LPCVOID phFileMap,
         HANDLE hFile
         );

PIMAGE_DEBUG_DIRECTORY
GetDebugDirectoryInExe(
                      PIMAGE_DOS_HEADER pDosHeader,
                      ULONG *NumberOfDebugDirectories
                      );

PIMAGE_DEBUG_DIRECTORY
GetDebugDirectoryInDbg(
                      PIMAGE_SEPARATE_DEBUG_HEADER pDbgHeader,
                      ULONG *NumberOfDebugDirectories
                      );

PIMAGE_SEPARATE_DEBUG_HEADER
MapMatchingDbgFile(
                  LPTSTR szSearchPath,
                  PFILE_INFO pFileInfo,
                  LPTSTR szFoundPath
                  );

BOOL
VerifyCV(
        LPTSTR szFileName,
        PCHAR pImageBase,
        PIMAGE_DEBUG_DIRECTORY pDbgDir,
        LPTSTR szExtPath,
        LPTSTR szSearchPath,
        PSYM_ERR pSymErr
        );

LPSTR
GetMiscFile(
           PCHAR pImageBase,
           PIMAGE_DEBUG_DIRECTORY pDbgDir
           );

LPTSTR
SearchForSymbolFile (
                    LPTSTR szSearchPath,
                    LPTSTR szSymName,
                    LPTSTR szPathExt,
                    PSYM_ERR pSymErr,
                    DWORD SymType,
                    PVOID  pelem1
                    );

USHORT
__cdecl
CompDbg(
       PVOID pelem1,
       LPTSTR szSymName,
       PSYM_ERR pSymErr
       );

USHORT
__cdecl
CompPdb(
       PVOID pelem1,
       LPTSTR szSymName,
       PSYM_ERR pSymErr
       );

BOOL
AddToSymbolsCDLog(
                 FILE *hSymCDLog,
                 PSYM_ERR pSymErr,
                 LPTSTR szSymbolPath,
                 LPTSTR szFileExt
                 );

BOOL PDBPrivateStripped(
                       PDB *ppdb,
                       DBI *pdbi
                       );

int __cdecl
SymComp(
       const void *e1,
       const void *e2
       );

PEXCLUDE_LIST
GetExcludeList(
              LPTSTR szFileName
              )

{
    PEXCLUDE_LIST pExcList;

    FILE  *fFile;
    TCHAR szCurFile[_MAX_FNAME+1], *c;
    TCHAR fname[_MAX_FNAME+1], ext[_MAX_EXT+1];
    DWORD i;
    LPTSTR szEndName;


    pExcList = (PEXCLUDE_LIST)malloc(sizeof(EXCLUDE_LIST));

    pExcList->dNumFiles = 0;
    if (  (fFile = _tfopen(szFileName,_T("r") )) == NULL ) {
        // printf( "Cannot open the exclude file %s\n",szFileName );
        return FALSE;
    }

    while ( _fgetts(szCurFile,_MAX_FNAME,fFile) ) {
        if ( szCurFile[0] == ';' ) continue;
        (pExcList->dNumFiles)++;
    }

    // Go back to the beginning of the file
    fseek(fFile,0,0);
    pExcList->szExcList = (LPTSTR*)malloc( sizeof(LPTSTR) *
                                           (pExcList->dNumFiles));
    if (pExcList->szExcList == NULL) return FALSE;

    i = 0;
    while ( i < pExcList->dNumFiles ) {
        memset(szCurFile,'\0',sizeof(TCHAR) * (_MAX_FNAME+1) );
        _fgetts(szCurFile,_MAX_FNAME,fFile);

        // Replace the \n with \0
        c = NULL;
        c  = _tcschr(szCurFile, '\n');
        if ( c != NULL) *c='\0';

        if ( szCurFile[0] == ';' ) continue;

        if ( _tcslen(szCurFile) > _MAX_FNAME ) {
            printf("File %s has a string that is too large\n",szFileName);
            return(NULL);
        }

        // Allow for spaces and a ; after the file name
        // Move the '\0' back until it has erased the ';' and any
        // tabs and spaces that might come before it

        szEndName = _tcschr(szCurFile, ';');
        if (szEndName != NULL ) {
            while ( *szEndName == ';' || *szEndName == ' '
                    || *szEndName == '\t' ) {
                *szEndName = '\0';
                if ( szEndName > szCurFile ) szEndName--;
            }
        }

        pExcList->szExcList[i]=(LPTSTR)
                               malloc( sizeof(TCHAR) * (_tcslen(szCurFile)+1) );

        if (pExcList->szExcList[i] == NULL ) {
            printf("Malloc failed for %s\n",szCurFile);
        }

        _tsplitpath(szCurFile,NULL,NULL,fname,ext);

        _tcscpy(pExcList->szExcList[i],fname);
        _tcscat(pExcList->szExcList[i],ext);

        i++;
    }

    fclose(fFile);


    // Sort the List
    qsort( (void*)pExcList->szExcList, (size_t)pExcList->dNumFiles,
           (size_t)sizeof(LPTSTR), SymComp );


    return (pExcList);

}

BOOL
InExcludeList(
             LPTSTR szFileName,
             PEXCLUDE_LIST pExcludeList
             )

{
    DWORD i;
    int High;
    int Low;
    int Middle;
    int Result;

    // Lookup the name using a binary search
    if ( pExcludeList == NULL ) return FALSE;
    if ( pExcludeList->dNumFiles == 0 ) return FALSE;

    Low = 0;
    High = pExcludeList->dNumFiles - 1;
    while ( High >= Low ) {

        Middle = (Low + High) >> 1;
        Result = _tcsicmp( szFileName, pExcludeList->szExcList[Middle] );

        if ( Result < 0 ) {
            High = Middle - 1;

        } else if ( Result > 0 ) {
            Low = Middle + 1;

        } else {
            break;
        }
    }

    if ( High < Low ) return FALSE;

    return TRUE;
}


BOOL
InList(
      LPTSTR szFileName,
      P_LIST pExcludeList
      )

{
    DWORD i;
    int High;
    int Low;
    int Middle;
    int Result;

    // Lookup the name using a binary search
    if ( pExcludeList == NULL ) return FALSE;
    if ( pExcludeList->dNumFiles == 0 ) return FALSE;

    Low = 0;
    High = pExcludeList->dNumFiles - 1;
    while ( High >= Low ) {

        Middle = (Low + High) >> 1;
        Result = _tcsicmp( szFileName, pExcludeList->List[Middle].Path );

        if ( Result < 0 ) {
            High = Middle - 1;

        } else if ( Result > 0 ) {
            Low = Middle + 1;

        } else {
            break;
        }
    }

    if ( High < Low ) return FALSE;

    return TRUE;
}


int __cdecl
SymComp(
       const void *e1,
       const void *e2
       )
{
    LPTSTR* p1;
    LPTSTR* p2;

    p1 = (LPTSTR*)e1;
    p2 = (LPTSTR*)e2;

    return ( _tcsicmp(*p1,*p2) );
}


BOOL
CheckSymbols (
             LPTSTR ErrMsg,
             LPTSTR szSearchPath,
             LPTSTR szFileName,
             FILE   *hSymCDLog,
             ULONG SymchkFlag,
             BOOL Verbose
             )
/*++

Routine Description:

    This function accepts a file name and a symbol search path
    (delimited by ;) and determines if its symbols match.

Arguments:
    szSearchPath    Search path delimited by ;

    szFileName      Full path and name of the file to verify symbols for

    szCopyDest      If this is not a NULL value, then generate actually copy the
                    symbols to this destination

    SymchkFlag      Specifies action to take according to whether symbols are
                    split or not split.

                    Possible values:
                        ERROR_IF_SPLIT      Print an error if the image is split already
                        ERROR_IF_NOT_SPLIT  Error if image is not split
                        IGNORE_IF_SPLIT     Don't check symbols for split images

    Verbose         If true, print an output line for every file
                    If false, print output only for files that fail

Return Value:
    TRUE    if symbols are correct, or symbol checking was ignored
    FALSE   if symbols are not correct

--*/

{

    PIMAGE_NT_HEADERS pNtHeader = NULL;
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    PIMAGE_SEPARATE_DEBUG_HEADER pDbgHeader= NULL;

    PIMAGE_DEBUG_DIRECTORY DebugDirectory;
    PIMAGE_DEBUG_DIRECTORY pDbgDir=NULL;
    PIMAGE_DEBUG_MISC pDebugMisc;

    PTCHAR pImageBase;

    ULONG NumberOfDebugDirectories;
    FILE_INFO FileInfo;
    LPTSTR szDbgFileName;
    LPSTR szMiscFileName;
    UINT i;
    PNB10I pDebugCV;


    TCHAR path_buffer[_MAX_PATH];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];

    HANDLE DbgFile = 0;
    HANDLE DosFile = 0;

    BOOL SymbolsOK = TRUE;
    BOOL CVFound = FALSE;
    BOOL MiscFound = FALSE;
    BOOL RawDataFound = FALSE;

    SYM_ERR SymErr;

    // Get the file name and the extension
    _tsplitpath( szFileName,drive,dir,fname,ext);


    // Initialize SymErr
    memset( &SymErr,0,sizeof(SYM_ERR) );
    SymErr.Verbose = Verbose;
    _tcscpy( SymErr.szFileName, szFileName );
    _tcscpy( SymErr.szSymbolSearchPath, szSearchPath );
    _tcscpy( SymErr.szSymbolFileName, _T("") );
    _tcscpy( SymErr.szPdbErr, _T("") );
    _tcscpy( SymErr.szPdbFileName, _T("") );

    pDosHeader = MapFileHeader( szFileName, &DosFile, &SymErr );
    if ( SymErr.ErrNo > 0 ) {
        LogError( ErrMsg, &SymErr, 0);
        return TRUE;
    }

    pNtHeader = GetNtHeader( pDosHeader, DosFile, szFileName, &SymErr );
    if ( SymErr.ErrNo > 0 ) {
        LogError( ErrMsg, &SymErr,0 );
        return TRUE;
    }

    __try {
        // Resource Dll's shouldn't have symbols
        if ( ResourceOnlyDll((PVOID)pDosHeader, FALSE) ) {
            LogError( ErrMsg, &SymErr,RESOURCE_ONLY_DLL);
            SymbolsOK = TRUE;
            __leave;
        }

        // Get info out of file header for comparison later
        if (pNtHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            FileInfo.CheckSum = ((PIMAGE_NT_HEADERS32)pNtHeader)->OptionalHeader.CheckSum;
            FileInfo.SizeOfImage = ((PIMAGE_NT_HEADERS32)pNtHeader)->OptionalHeader.SizeOfImage;
        } else {
            if (pNtHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
                FileInfo.CheckSum = ((PIMAGE_NT_HEADERS64)pNtHeader)->OptionalHeader.CheckSum;
                FileInfo.SizeOfImage = ((PIMAGE_NT_HEADERS64)pNtHeader)->OptionalHeader.SizeOfImage;
            } else {
                FileInfo.CheckSum = -1;
            }
        }
        FileInfo.TimeDateStamp = pNtHeader->FileHeader.TimeDateStamp;
        _tcscpy(FileInfo.szName,szFileName);


        // Locate the Debug Directory in this file
        DebugDirectory = NULL;
        DebugDirectory = GetDebugDirectoryInExe(
                                               pDosHeader,
                                               &NumberOfDebugDirectories
                                               );
        if (!DebugDirectory) {
            LogError( ErrMsg, &SymErr,NO_DEBUG_DIRECTORIES);
            SymbolsOK = FALSE;
            __leave;
        }
        pImageBase = (PCHAR) pDosHeader;

        // Do some checks on the Debug Directories
        /* Commenting this out for a few year because none of the debuggers
           really care about this stuff.  Also, BBT had a bug and didn't write
           the size of the debug directory correctly, so INVALID_POINTERTORAWDATA_
           ZERO_DEBUG was showing up as an error for some MFC files.  BryanT wanted
           me to comment this out.

            for ( i=0; i<NumberOfDebugDirectories; i++ ) {
                pDbgDir = DebugDirectory + i;

                if ( pDbgDir->SizeOfData != 0 ) {

                    if ( pDbgDir->PointerToRawData == 0 ) {
                        LogError( ErrMsg, &SymErr, INVALID_POINTERTORAWDATA_NON_ZERO );
                        return(FALSE);
                    }

                } else {

                    if ( pDbgDir->AddressOfRawData != 0 ) {
                        LogError( ErrMsg, &SymErr, INVALID_ADDRESSOFRAWDATA_ZERO_DEBUG );
                        return(FALSE);
                    }

                    if ( pDbgDir->PointerToRawData != 0 ) {
                        LogError( ErrMsg, &SymErr, INVALID_POINTERTORAWDATA_ZERO_DEBUG );
                        return(FALSE);
                    }
                }
            }
        */

        // Debug Information is stripped, we need the misc directory to find
        // out the DBG file name
        if ( (pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED) ) {
            if (SymchkFlag == ERROR_IF_SPLIT) {
                // Image is split, log error and continue;
                LogError( ErrMsg, &SymErr,ERROR_IF_SPLIT);
                SymbolsOK = FALSE;
                __leave;
            } else if (SymchkFlag == IGNORE_IF_SPLIT ) {
                // Image is split, don't check the symbols
                LogError( ErrMsg, &SymErr,IGNORE_IF_SPLIT);
                SymbolsOK = TRUE;
                __leave;
            }

            // Get the MISC entry
            i=0;
            while ( (i<NumberOfDebugDirectories) &&
                    ((DebugDirectory+i)->Type != IMAGE_DEBUG_TYPE_MISC) ) i++;

            if (i >= NumberOfDebugDirectories) {
                LogError( ErrMsg, &SymErr,NO_MISC_ENTRY);
                SymbolsOK = FALSE;
                __leave;
            }

            szMiscFileName = NULL;
            szMiscFileName = GetMiscFile(pImageBase,DebugDirectory + i);
            if (!szMiscFileName) {
                LogError( ErrMsg, &SymErr,NO_FILE_IN_MISC);
                SymbolsOK = FALSE;
                __leave;
            }

            // Debug info is stripped.
            // First, locate the correct DBG file
            szDbgFileName = NULL;
            szDbgFileName = SearchForSymbolFile (
                                                szSearchPath,
                                                szMiscFileName,
                                                ext+1, // Extension of the image
                                                &SymErr,
                                                DBGSYM,
                                                (PVOID)&FileInfo );

            if (!szDbgFileName) {
                LogDbgError(ErrMsg, &SymErr);
                SymbolsOK = FALSE;
                __leave;
            } else if ( hSymCDLog != NULL) {
                AddToSymbolsCDLog( hSymCDLog, &SymErr, SymErr.szSymbolFileName, ext+1);
            }

            // International builds need to call symchk and only grab the DBG and
            // write it to the symbols CD log.
            if (DbgOnly) {
                SymbolsOK = TRUE;
                __leave;
            }

            pDbgHeader = NULL;
            pDbgHeader = MapDbgHeader(szDbgFileName,&DbgFile, &SymErr);
            if (!pDbgHeader) {
                SymbolsOK = FALSE;
                // 0 means info is inside of SymErr
                LogError(ErrMsg, &SymErr,0);
                __leave;
            }

            DebugDirectory = NULL;
            DebugDirectory = GetDebugDirectoryInDbg(
                                                   pDbgHeader,
                                                   &NumberOfDebugDirectories
                                                   );
            if (!DebugDirectory) {
                LogError( ErrMsg, &SymErr,NO_DEBUG_DIRECTORIES_IN_DBG_HEADER);
                SymbolsOK = FALSE;
                __leave;
            }
            pImageBase = (PCHAR)pDbgHeader;
        }

        // Image is not split
        else {

            // VC 6 compiler always produces non-split images.
            // Check the debug directories to determine if this is a VC 6 image.

            RawDataFound = FALSE;
            CVFound = FALSE;
            MiscFound = FALSE;
            for ( i=0; i< NumberOfDebugDirectories; i++ ) {
                pDbgDir = DebugDirectory + i;
                switch (pDbgDir->Type) {
                    case IMAGE_DEBUG_TYPE_MISC:
                        MiscFound = TRUE;
                        break;

                    case IMAGE_DEBUG_TYPE_CODEVIEW:
                        CVFound = TRUE;
                        break;

                    default:
                        // Nothing except the CV entry should point to raw data
                        if ( pDbgDir->SizeOfData != 0 ) {
                            RawDataFound = TRUE;
                        }
                        break;
                }
            }

            if ( !MiscFound && CVFound ) {

                // This is using the 6.0 linker.  It does not
                // need to be split into iamge + dbg, so don't
                // give an error for it, unless there is another
                // debug directory with a non-null pointer to raw data.

                if ( RawDataFound ) {
                    LogError( ErrMsg, &SymErr, EXTRA_RAW_DATA_IN_6);
                    return ( FALSE);
                } else {
                    // Continue - image should not be split
                }
            } else {
                DWORD CertificateSize;
                PVOID pCertificates = ImageDirectoryEntryToData(
                                                     (PVOID)pDosHeader,
                                                     FALSE,
                                                     IMAGE_DIRECTORY_ENTRY_SECURITY,
                                                     &CertificateSize
                                                     );
                if ( pCertificates ) {
                    // Image is signed and non-split
                    // It should be split before it is signed
                    LogError( ErrMsg, &SymErr, SIGNED_AND_NON_SPLIT);
                    SymbolsOK = FALSE;
                    __leave;
                } else if (SymchkFlag == ERROR_IF_NOT_SPLIT) {
                    // Image isn't split, log error and continue;
                    LogError( ErrMsg, &SymErr,ERROR_IF_NOT_SPLIT);
                    SymbolsOK = FALSE;
                    __leave;
                }
            }

            // File with the DBG info is the original image file name
            // This line may not be necessary.
            _tcscpy( SymErr.szSymbolFileName, szFileName );
        }

        CVFound = FALSE;
        // Process the Debug Directories
        for ( i=0; i<NumberOfDebugDirectories; i++) {
            pDbgDir = DebugDirectory + i;
            switch (pDbgDir->Type) {

                // Don't need to process entry since directory table has
                // already been located.
                case IMAGE_DEBUG_TYPE_MISC:  break;

                case IMAGE_DEBUG_TYPE_CODEVIEW:
                    CVFound = TRUE;
                    if ( !VerifyCV(
                                  szFileName,
                                  pImageBase,
                                  pDbgDir,
                                  ext+1,
                                  szSearchPath,
                                  &SymErr
                                  )
                       ) {
                        LogPdbError(ErrMsg, &SymErr);
                        SymbolsOK = FALSE;
                    } else if (hSymCDLog != NULL && _tcscmp(SymErr.szPdbFileName, "") ) {
                        // Copy SymErr.szPdbFileName to Symbol Dest\ext
                        AddToSymbolsCDLog( hSymCDLog, &SymErr, SymErr.szPdbFileName, ext+1 );
                    }
                    break;
                default:    break;
            }
        }
        if (!CVFound && CheckCodeView ) {
            SymErr.ErrNo = NO_CODE_VIEW;
            LogPdbError(ErrMsg, &SymErr);
            SymbolsOK=FALSE;
        }
    } __finally {
        UnmapFile((LPCVOID)pDbgHeader,DbgFile);
        UnmapFile((LPCVOID)pDosHeader,DosFile);
    }

    if (SymbolsOK) {
        LogError( ErrMsg, &SymErr, IMAGE_PASSED );
    }

    return SymbolsOK;
}


PIMAGE_DOS_HEADER
MapFileHeader (
              LPTSTR szFileName,
              PHANDLE phFile,
              PSYM_ERR pSymErr
              )

{

    /*
        Creates a file mapping and returns Handle for the DOS_HEADER
        If the file does not have a DOS_HEADER, then it returns NULL.


    */
    HANDLE hFileMap;
    PIMAGE_DOS_HEADER pDosHeader;
    DWORD  dFileType;

    // phFile map needs to be returned, so it can be closed later
    (*phFile) = CreateFile( (LPCTSTR) szFileName,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL
                          );

    if (*phFile == INVALID_HANDLE_VALUE) {
        pSymErr->ErrNo  = CREATE_FILE_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        _tcscpy( pSymErr->szFileName, szFileName);
        return(NULL);
    }

    hFileMap = CreateFileMapping( *phFile,
                                  NULL,
                                  PAGE_READONLY,
                                  0,
                                  0,
                                  NULL
                                );

    if ( hFileMap == INVALID_HANDLE_VALUE) {
        pSymErr->ErrNo = CREATE_FILE_MAPPING_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        _tcscpy( pSymErr->szFileName, szFileName);
        CloseHandle(*phFile);
        return(NULL);
    }

    pDosHeader = (PIMAGE_DOS_HEADER) MapViewOfFile( hFileMap,
                                                    FILE_MAP_READ,
                                                    0,  // high
                                                    0,  // low
                                                    0   // whole file
                                                  );

    CloseHandle(hFileMap);

    if ( !pDosHeader ) {
        pSymErr->ErrNo = MAPVIEWOFFILE_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        _tcscpy( pSymErr->szFileName, szFileName);
        UnmapFile((LPCVOID)pDosHeader, *phFile);
        return(NULL);
    }

    //
    // Check to determine if this is an NT image (PE format)

    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        pSymErr->ErrNo = NO_DOS_HEADER;
        UnmapFile((LPCVOID)pDosHeader,*phFile);
        return(NULL);
    }

    return (pDosHeader);
}


PIMAGE_NT_HEADERS
GetNtHeader ( PIMAGE_DOS_HEADER pDosHeader,
              HANDLE hDosFile,
              LPTSTR szFileName,
              PSYM_ERR pSymErr
            )
{

    /*
        Returns the pointer the address of the NT Header.  If there isn't
        an NT header, it returns NULL
    */
    PIMAGE_NT_HEADERS pNtHeader;
    LPBY_HANDLE_FILE_INFORMATION lpFileInfo;


    lpFileInfo = (LPBY_HANDLE_FILE_INFORMATION) malloc
                 (sizeof(BY_HANDLE_FILE_INFORMATION) );
    if (!GetFileInformationByHandle( hDosFile, lpFileInfo)) {
        pSymErr->ErrNo = GET_FILE_INFO_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        free(lpFileInfo);
        return(NULL);
    }

    if ( ((ULONG)(pDosHeader->e_lfanew) & 3) != 0) {

        //
        // The image header is not aligned on a long boundary.
        // Report this as an invalid protect mode image.
        //

        pSymErr->ErrNo = HEADER_NOT_ON_LONG_BOUNDARY;
        free(lpFileInfo);
        return (NULL);
    }


    if ((ULONG)(pDosHeader->e_lfanew) > lpFileInfo->nFileSizeLow) {
        pSymErr->ErrNo = IMAGE_BIGGER_THAN_FILE;
        free(lpFileInfo);
        return (NULL);
    }


    pNtHeader = (PIMAGE_NT_HEADERS)((PCHAR)pDosHeader +
                                    (ULONG)pDosHeader->e_lfanew);

    if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
        free(lpFileInfo);
        pSymErr->ErrNo = NOT_NT_IMAGE;
        return(NULL);
    }

    free(lpFileInfo);
    return (pNtHeader);
}


PIMAGE_SEPARATE_DEBUG_HEADER
MapDbgHeader (
             LPTSTR szFileName,
             PHANDLE phFile,
             PSYM_ERR pSymErr
             )
{
    HANDLE hFileMap;
    PIMAGE_SEPARATE_DEBUG_HEADER pDbgHeader;
    DWORD  dFileType;

    (*phFile) = CreateFile( (LPCTSTR) szFileName,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL
                          );

    if (*phFile == INVALID_HANDLE_VALUE) {
        pSymErr->ErrNo  = CREATE_FILE_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        _tcscpy( pSymErr->szFileName, szFileName);
        CloseHandle(*phFile);
        return(NULL);
    }

    hFileMap = CreateFileMapping( *phFile,
                                  NULL,
                                  PAGE_READONLY,
                                  0,
                                  0,
                                  NULL
                                );

    if ( hFileMap == INVALID_HANDLE_VALUE) {
        pSymErr->ErrNo = CREATE_FILE_MAPPING_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        _tcscpy( pSymErr->szFileName, szFileName);
        CloseHandle(*phFile);
        CloseHandle(hFileMap);
        return(NULL);
    }


    pDbgHeader = (PIMAGE_SEPARATE_DEBUG_HEADER) MapViewOfFile( hFileMap,
                                                               FILE_MAP_READ,
                                                               0,  // high
                                                               0,  // low
                                                               0   // whole file
                                                             );
    CloseHandle(hFileMap);

    if ( !pDbgHeader ) {
        pSymErr->ErrNo = MAPVIEWOFFILE_FAILED;
        pSymErr->ErrNo2 = GetLastError();
        _tcscpy( pSymErr->szFileName, szFileName);
        UnmapFile((LPCVOID)pDbgHeader, *phFile);
        return(NULL);
    }

    return (pDbgHeader);
}

BOOL
UnmapFile( LPCVOID phFileMap, HANDLE hFile )
{
    if ((PHANDLE)phFileMap != NULL) {
        UnmapViewOfFile( phFileMap );
    }
    if (hFile) {
        CloseHandle(hFile);
    }
    return TRUE;
}


PIMAGE_DEBUG_DIRECTORY
GetDebugDirectoryInExe(
                      PIMAGE_DOS_HEADER pDosHeader,
                      ULONG *NumberOfDebugDirectories
                      )
{

    /* Exe is already mapped and a pointer to the base is
       passed in. Find a pointer to the Debug Directories
    */
    ULONG size;

    PIMAGE_DEBUG_DIRECTORY pDebugDirectory = NULL;
    ULONG DebugDirectorySize;
    PIMAGE_SECTION_HEADER pSectionHeader;

    size = sizeof(IMAGE_DEBUG_DIRECTORY);

    pDebugDirectory = (PIMAGE_DEBUG_DIRECTORY)
                      ImageDirectoryEntryToDataEx (
                                                  (PVOID)pDosHeader,
                                                  FALSE,
                                                  IMAGE_DIRECTORY_ENTRY_DEBUG,
                                                  &DebugDirectorySize,
                                                  &pSectionHeader );

    if (pDebugDirectory) {
        (*NumberOfDebugDirectories) =
        DebugDirectorySize / sizeof(IMAGE_DEBUG_DIRECTORY);
        return (pDebugDirectory);
    } else return(NULL);
}

PIMAGE_DEBUG_DIRECTORY
GetDebugDirectoryInDbg(
                      PIMAGE_SEPARATE_DEBUG_HEADER pDbgHeader,
                      ULONG *NumberOfDebugDirectories
                      )
/*  Dbg is already mapped and a pointer to the base is
    passed in.  Returns a pointer to the Debug directories
*/
{
    PIMAGE_DEBUG_DIRECTORY pDebugDirectory = NULL;

    pDebugDirectory = (PIMAGE_DEBUG_DIRECTORY) ((PCHAR)pDbgHeader +
                                                sizeof(IMAGE_SEPARATE_DEBUG_HEADER) +
                                                pDbgHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER) +
                                                pDbgHeader->ExportedNamesSize);

    if (!pDebugDirectory) {
        return(NULL);
    }

    (*NumberOfDebugDirectories) =   pDbgHeader->DebugDirectorySize /
                                    sizeof(IMAGE_DEBUG_DIRECTORY);
    return (pDebugDirectory);

}


BOOL
VerifyCV(
        LPTSTR szFileName,
        PCHAR pImageBase,
        PIMAGE_DEBUG_DIRECTORY pDbgDir,
        LPTSTR szExtPath,
        LPTSTR szSearchPath,
        PSYM_ERR pSymErr
        )

{
    PNB10I pDebugCV;
    LPTSTR szPdbFileName;

    TCHAR szFName[_MAX_FNAME];
    TCHAR szExt[_MAX_EXT];

    pDebugCV = (PNB10I) (pImageBase +
                         pDbgDir->PointerToRawData );

    // Initialize pSymErr
    pSymErr->PdbFileFound = FALSE;
    pSymErr->PdbValid = FALSE;
    _tcscpy(pSymErr->szPdbErr,_T("") );
    _tcscpy(pSymErr->szPdbFileName,_T("") );

    if ( pDebugCV->dwSig == '05BN') {
        // CVpack failed
        // Symbols are not OK
        _tcscpy(pSymErr->szPdbErr,_T("Codeview type is NB05") );
        return (FALSE);

    }
    if ( (pDebugCV->dwSig == '90BN') ||
         ( pDebugCV->dwSig == '11BN') ) {
        // For NB09 and NB11, the codeview info is in the image file
        // Thus, no need to locate symbols.  They are probably OK.
        return(TRUE);
    } else if ( pDebugCV->dwSig == '01BN') {
        // Symbols are type NB10
        // Locate the PDB information

        szPdbFileName = NULL;

        szPdbFileName = SearchForSymbolFile (
                                            szSearchPath,
                                            pDebugCV->szPDB, // File Name
                                            szExtPath,
                                            pSymErr,
                                            PDBSYM,
                                            (PVOID)pDebugCV
                                            );
        if (szPdbFileName) {
            // SearchForSymbolFile already gave pSymErr->szPdbFilename its value
            return TRUE;
        } else {
            _tsplitpath(pDebugCV->szPDB,NULL,NULL,szFName,szExt);
            _tcscpy(pSymErr->szPdbFileName,szFName);
            _tcscat(pSymErr->szPdbFileName,szExt);
            return FALSE;
        }
    } else {
        _tcscpy(pSymErr->szPdbErr,
                _T("Codeview info is not NB09, NB10, or NB11\n") );
        return(FALSE);
    }
}


LPSTR
GetMiscFile(
           PCHAR pImageBase,
           PIMAGE_DEBUG_DIRECTORY pDbgDir
           )

{
    PIMAGE_DEBUG_MISC pDebugMisc;
    LPSTR szData;

    // Nothing to verify, but here is how to get to the DBG entry
    // Get the debug directory

    pDebugMisc = (PIMAGE_DEBUG_MISC)(pImageBase +
                                     pDbgDir->PointerToRawData );
    szData = (LPSTR) &pDebugMisc->Data[0];
    return(szData);
}


LPTSTR SearchForSymbolFile ( LPTSTR     szSearchPath, // ; separated search path
                             LPTSTR     szSymName,    // Symbol File to search for
                             LPTSTR     szPathExt,    // Extension directory
                             PSYM_ERR   pSymErr,
                             DWORD      dSymType,
                             PVOID      pelem1
                           )

{
    /*++

    Routine Description:
       For each symbol path in szSearchPath, this function looks in the path
       and in the path concatenated with the szPathExt subdirectory.

    Arguments:
        szSearchPath    ; separated symbol path

        szSymName       Symbol file to search for

        szPathExt       Extension for the image that the symbol file matches

        pSymErr         Error structure

        dSymType        Type of symbol.  Possible values: DBGSYM, PDBSYM

        pelem1          Pointer to structure needed for comparing the symbols


    Return Value:
        If the symbol file is found and verifies, it returns the full path and
        name of the symbol file.  Otherwise, it returns NULL.

    --*/

    TCHAR szTmpSearchPath[_MAX_PATH];
    TCHAR szExtPath[_MAX_PATH];
    TCHAR szFName[_MAX_FNAME];
    TCHAR szExt[_MAX_EXT];

    LPTSTR szResultFileName;
    LPTSTR szResult;
    LPTSTR seps = _T(";");
    LPTSTR szCurPath;
    DWORD  FoundLength;
    DWORD  CurLen, ExtLen;
    DWORD  dwResultLength;

    dwResultLength = _MAX_PATH;
    szResult = (LPTSTR)malloc( _MAX_PATH * sizeof(TCHAR) );

    // Make a copy of szSearchPath because tcstok alters it
    _tcscpy(szTmpSearchPath,(LPCTSTR)szSearchPath);

    // Strip FileName from its path
    _tsplitpath(szSymName, NULL, NULL, szFName, szExt);

    // Try each search path
    szCurPath = _tcstok(szTmpSearchPath,seps);

    while ( szCurPath != NULL ) {

        FoundLength = SearchPath( szCurPath,
                                  szFName,
                                  szExt,
                                  dwResultLength,
                                  szResult,
                                  &szResultFileName
                                );
        if (FoundLength > 0 && FoundLength < dwResultLength) {

            switch ( dSymType ) {
                case DBGSYM:
                    // Status info, a match was found
                    pSymErr->SymbolFileFound = TRUE;
                    _tcscpy(pSymErr->szSymbolFileName, szResult);

                    // Validate the DBG file
                    if ( CompDbg(pelem1, szResult, pSymErr) ) {
                        return (szResult);
                    }
                    break;

                case PDBSYM:
                    // Status info, a match was found
                    pSymErr->PdbFileFound = TRUE;
                    _tcscpy(pSymErr->szPdbFileName, szResult);

                    // Validate the PDB file
                    if ( CompPdb(pelem1, szResult, pSymErr) ) {
                        return (szResult);
                    }
                    break;

                default: break;
            }
        }

        // Now try the path with szPathExt at the end of it
        // Don't try it if current path already has the extension
        // at the end of it
        CurLen = _tcslen(szCurPath);
        ExtLen = _tcslen(szPathExt);

        if ( _tcscmp ( (szCurPath + CurLen - ExtLen), szPathExt ) ) {

            _tcscpy(szExtPath, szCurPath);
            _tcscat(szExtPath, _T("\\") );
            _tcscat(szExtPath, szPathExt);

            FoundLength = SearchPath( szExtPath,
                                      szFName,
                                      szExt,
                                      dwResultLength,
                                      szResult,
                                      &szResultFileName
                                    );
            if (FoundLength > 0 && FoundLength < dwResultLength) {
                switch ( dSymType ) {
                    case DBGSYM:
                        // Status info, a match was found
                        pSymErr->SymbolFileFound = TRUE;
                        _tcscpy(pSymErr->szSymbolFileName, szResult);

                        // Validate the DBG file
                        if ( CompDbg(pelem1, szResult, pSymErr) ) {
                            return (szResult);
                        }
                        break;
                    case PDBSYM:
                        // Status info, a match was found
                        pSymErr->PdbFileFound = TRUE;
                        _tcscpy(pSymErr->szPdbFileName, szResult);

                        // Validate the PDB file
                        if ( CompPdb(pelem1, szResult, pSymErr) ) {
                            return (szResult);
                        }
                        break;
                    default: break;
                }
            }
        }
        szCurPath = _tcstok(NULL,seps);
    }

    // Symbol wasn't found.  Put name of file into SymErr for error message
    switch ( dSymType ) {
        case DBGSYM:
            _tcscpy(pSymErr->szSymbolFileName,szFName);
            _tcscat(pSymErr->szSymbolFileName,szExt);
            break;
        case PDBSYM:
            _tcscpy(pSymErr->szPdbFileName, szFName);
            _tcscat(pSymErr->szPdbFileName, szExt);
            break;
        default: break;
    }

    free(szResult);
    return NULL;
}


USHORT
__cdecl
CompDbg(
       PVOID pelem1,
       LPTSTR szSymName,
       PSYM_ERR pSymErr
       )

/*++

    Routine Description:
        Validates the DBG against the original image

    Return Value:
        TRUE -  DBG matches
        FALSE - DBG doesn't match timedatestamp and checksum


--*/
{

    PFILE_INFO pFileInfo;
    PIMAGE_SEPARATE_DEBUG_HEADER pDbgHeader;
    HANDLE hFile;

    pFileInfo = (PFILE_INFO)(pelem1);
    pDbgHeader = MapDbgHeader ( szSymName, &hFile, pSymErr );

    if (pDbgHeader == NULL) {
        printf("ERROR: CompDbg(), %s was not opened successfully\n",szSymName);
        UnmapFile((LPCVOID)pDbgHeader, hFile);
        return FALSE;
    }
    if (pDbgHeader->Signature != IMAGE_SEPARATE_DEBUG_SIGNATURE) {
        printf("ERROR: CompDbg(), %s doesn't have Debug signature in header\n");
        UnmapFile((LPCVOID)pDbgHeader, hFile);
        return FALSE;
    }

    // Record specifics of whether timedatestamp and checksum matched
    if ( pDbgHeader->SizeOfImage == pFileInfo->SizeOfImage ) {
        pSymErr->SizeOfImageMatch = TRUE;
    } else pSymErr->SizeOfImageMatch = FALSE;

    if ( pDbgHeader->TimeDateStamp == pFileInfo->TimeDateStamp ) {
        pSymErr->TimeDateStampsMatch = TRUE;
    } else pSymErr->TimeDateStampsMatch = FALSE;

    if ( pDbgHeader->CheckSum == pFileInfo->CheckSum ) {
        pSymErr->CheckSumsMatch = TRUE;
    } else pSymErr->CheckSumsMatch = FALSE;

    // Just check the timedatestamp for determining if the DBG matches
    // VC and KD don't pay attention to the checksum anymore.
    if ( pSymErr->TimeDateStampsMatch  &&
         pSymErr->SizeOfImageMatch &&
         ( !LogCheckSumErrors || pSymErr->CheckSumsMatch ) ) {
        UnmapFile((LPCVOID)pDbgHeader, hFile);
        return TRUE;
    } else {
        UnmapFile((LPCVOID)pDbgHeader, hFile);
        return FALSE;
    }
}

USHORT
__cdecl
CompPdb(
       PVOID pelem1,
       LPTSTR szSymName,
       PSYM_ERR pSymErr
       )

/*++

    Routine Description:
        Validates the PDB

    Return Value:
        TRUE    PDB validates
        FALSE   PDB doesn't validate
--*/

{

    PNB10I pPdbInfo;
    PNB10I pCurPdb;

    BOOL valid;
    PDB *pdb;
    EC ec;
    char szError[cbErrMax] = _T("");
    SIG sig;
    AGE age;

    DBI *pdbi;

    pdb=NULL;

    pPdbInfo = (PNB10I)(pelem1);

    valid = PDBOpenValidate(szSymName,
                            NULL,
                            _T("r"),
                            pPdbInfo->sig,
                            pPdbInfo->age,
                            &ec,
                            szError,
                            &pdb
                           );

    /*
      valid = PDBOpen( pPdbInfo->szPDB,
                       _T("r"),
                       pPdbInfo->sig,
                       &ec,
                       szError,
                       &pdb
                       );
    */

    if ( !valid ) {
        pSymErr->PdbValid = FALSE;
        return FALSE;
    } else pSymErr->PdbValid = TRUE;

    //  sig = PDBQuerySignature(pdb);
    //  age = PDBQueryAge(pdb);


    if (!CheckPrivate) {
        PDBClose(pdb);
        return (USHORT)pSymErr->PdbValid;
    }

    // Verify that private information is stripped
    // Verify that line information is removed

    if ( !PDBOpenDBI(pdb, pdbRead, NULL, &pdbi) ) {
        // OpenDBI failed
        pSymErr->ErrNo = PDB_MAY_BE_CORRUPT;
        return FALSE;
    }

    if ( !PDBPrivateStripped(pdb, pdbi) ) {
        //Error - Source line info is not stripped
        pSymErr->PdbValid = FALSE;
        pSymErr->ErrNo = PRIVATE_INFO_NOT_REMOVED;
    }

    PDBClose(pdb);
    return (USHORT)pSymErr->PdbValid;


}


BOOL
ResourceOnlyDll(
               PVOID pImageBase,
               BOOLEAN bMappedAsImage
               )

/*++

Routine Description:

    Returns true if the image is a resource only dll.

--*/

{

    PVOID pExports, pImports, pResources;
    DWORD dwExportSize, dwImportSize, dwResourceSize;
    BOOL fResourceOnlyDll;

    pExports = ImageDirectoryEntryToData(pImageBase,
                                         bMappedAsImage,
                                         IMAGE_DIRECTORY_ENTRY_EXPORT,
                                         &dwExportSize);

    pImports = ImageDirectoryEntryToData(pImageBase,
                                         bMappedAsImage,
                                         IMAGE_DIRECTORY_ENTRY_IMPORT,
                                         &dwImportSize);

    pResources = ImageDirectoryEntryToData(pImageBase,
                                           bMappedAsImage,
                                           IMAGE_DIRECTORY_ENTRY_RESOURCE,
                                           &dwResourceSize);

    if (pResources && dwResourceSize &&
        !pImports && !dwImportSize &&
        !pExports && !dwExportSize) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}


BOOL
LogError(
        LPTSTR ErrMsg,
        PSYM_ERR pSymErr,
        UINT ErrNo )
{

    TCHAR szFName[_MAX_FNAME];
    TCHAR szExt[_MAX_EXT];
    TCHAR szCurName[_MAX_FNAME];


    if (pSymErr->ErrNo != 0) ErrNo = pSymErr->ErrNo;

    // Get the file name without any path info:
    if (Recurse) {
        _tcscpy(szCurName, pSymErr->szFileName);
    } else {
        _tsplitpath(pSymErr->szFileName,NULL,NULL,szFName,szExt);
        _tcscpy(szCurName,szFName);
        _tcscat(szCurName,szExt);
    }

    _tcscpy(ErrMsg,_T(""));

    // See if this is a file that we aren't supposed to report errors
    // for.  If it is, just return without writing an error.

    if ( pErrorFilterList != NULL ) {
        if ( InExcludeList( szCurName, pErrorFilterList ) ) {
            return(TRUE);
        }
    }

    switch (ErrNo) {
        case NO_DEBUG_DIRECTORIES:
            _stprintf(ErrMsg, "%-20s FAILED  - Built with no debugging information\n",
                      szCurName);
            return(TRUE);

        case NO_DEBUG_DIRECTORIES_IN_DBG_HEADER:
            _stprintf(ErrMsg, "%-20s FAILED  - Header for %s has no debugging information\n",
                      szCurName);
            return(TRUE);

        case NO_MISC_ENTRY:
            _stprintf(ErrMsg, "%-20s FAILED  - No MISC entry in debug directories\n",
                      szCurName);
            return(TRUE);

        case NO_FILE_IN_MISC:
            _stprintf(ErrMsg, "%-20s FAILED  - MISC entry contains no .dbg file information\n",
                      szCurName);
            return(TRUE);

        case ERROR_IF_SPLIT:
            _stprintf(ErrMsg, "%-20s FAILED  - Debug information is stripped from image\n",
                      szCurName);
            return(TRUE);

        case ERROR_IF_NOT_SPLIT:
            _stprintf(ErrMsg, "%-20s FAILED  - Image still contains data that should be in a .DBG file\n",
                      szCurName);
            return(TRUE);

        case EXTRA_RAW_DATA_IN_6:
            _stprintf(ErrMsg, "%-20s FAILED  - No misc entry, but debug directories point to non-Codeview data\n",
                      szCurName );
            return (TRUE);

        case IGNORE_IF_SPLIT:
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image is already split\n",szCurName);
            }
            return(TRUE);


        case NO_CODE_VIEW:
            _stprintf(ErrMsg, "%-20s FAILED  - %s does not have a pointer to CodeView information\n",
                      szCurName, pSymErr->szSymbolFileName);
            return(TRUE);

        case FILE_NOT_FOUND:
            _stprintf(ErrMsg, "%-20s FAILED  - File does not exist\n",szCurName);
            return(TRUE);

        case CREATE_FILE_FAILED :
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image is not a valid NT image.\n", szCurName);
            }
            return(TRUE);

        case CREATE_FILE_MAPPING_FAILED :
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image is not a valid NT image.\n", szCurName);
            }
            return(TRUE);

        case MAPVIEWOFFILE_FAILED :
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image is not a valid NT image.\n", szCurName);
            }
            return(TRUE);

        case GET_FILE_INFO_FAILED :
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image is not a valid NT image.\n", szCurName);
            }
            return(TRUE);

        case HEADER_NOT_ON_LONG_BOUNDARY:
            // Not an NT image - ignore the error
            if (pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - This is either corrupt or a DOS image\n", szCurName);
            }
            return(TRUE);

        case IMAGE_BIGGER_THAN_FILE:
            if (pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - This is either corrupt or a DOS image\n", szCurName);
            }
            return(TRUE);

        case INVALID_ADDRESSOFRAWDATA_ZERO_DEBUG:
            _stprintf(ErrMsg, "%-20s FAILED  - Invalid AddressOfRawData for zero sized debug info\n",
                      szCurName);
            return(TRUE);

        case INVALID_POINTERTORAWDATA_NON_ZERO:
            _stprintf(ErrMsg, "%-20s FAILED  - Invalid PointerToRawData for non-zero sized debug info\n",
                      szCurName);
            return(TRUE);

        case INVALID_POINTERTORAWDATA_ZERO_DEBUG:
            _stprintf(ErrMsg, "%-20s FAILED  - Invalid PointerToRawData for zero sized debug info\n",
                      szCurName);
            return(TRUE);

        case NO_DOS_HEADER:
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image does not have a DOS header\n",
                          szCurName);
            }
            return(TRUE);

        case NOT_NT_IMAGE:
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Image does not have an NT header\n",
                          szCurName);
            }
            return(TRUE);
        case IMAGE_PASSED :
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s PASSED \n",szCurName);
            }
            return(TRUE);

        case RESOURCE_ONLY_DLL:
            if ( pSymErr->Verbose) {
                _stprintf(ErrMsg, "%-20s IGNORED - Resource only DLL\n",szCurName);
            }
            return(TRUE);

        case SIGNED_AND_NON_SPLIT:
            _stprintf(ErrMsg, "%-20s FAILED  - It is signed and debug info can't be stripped without invalidating the signature\n",
                      szCurName);
            return(TRUE);

        default:
            return(TRUE);
    }
    return (FALSE);
}


BOOL
LogDbgError(
           LPTSTR ErrMsg,
           PSYM_ERR pSymErr
           )

{

    TCHAR szFName[_MAX_FNAME];
    TCHAR szExt[_MAX_EXT];
    TCHAR szCurName[_MAX_FNAME];
    TCHAR szSymName[_MAX_FNAME];

    // Get the file name without any path info:
    if (Recurse) {
        _tcscpy(szCurName, pSymErr->szFileName);
    } else {
        _tsplitpath(pSymErr->szFileName,NULL,NULL,szFName,szExt);
        _tcscpy(szCurName,szFName);
        _tcscat(szCurName,szExt);
    }

    _tcscpy(ErrMsg,_T("") );

    // See if this is a file that we aren't supposed to report errors
    // for.  If it is, just return without writing an error.

    if ( pErrorFilterList != NULL ) {
        if ( InExcludeList( szCurName, pErrorFilterList ) ) {
            return(TRUE);
        }
    }

    // Get the file name without any path info:
    _tsplitpath(pSymErr->szSymbolFileName,NULL,NULL,szFName,szExt);
    _tcscpy(szSymName,szFName);
    _tcscat(szSymName,szExt);


    if ( !pSymErr->SymbolFileFound ) {
        _stprintf(ErrMsg, "%-20s FAILED  - Image is split correctly, but %s is missing\n",
                  szCurName, szSymName);
        return(TRUE);
    }

    if ( LogCheckSumErrors && !pSymErr->CheckSumsMatch) {
        _stprintf(ErrMsg, "%-20s FAILED  - Checksum doesn't match with %s\n",
                  szCurName, szSymName);
        return(TRUE);
    }

    if ( !pSymErr->SizeOfImageMatch) {
        _stprintf(ErrMsg, "%-20s FAILED  - Size of image doesn't match with %s\n",
                  szCurName, szSymName);
        return(TRUE);
    }

    if ( !pSymErr->TimeDateStampsMatch) {
        _stprintf(ErrMsg, "%-20s FAILED  - Timedate stamp doesn't match with %s\n",
                  szCurName, szSymName);
        return(TRUE);
    }

    return(TRUE);
}


BOOL
LogPdbError(
           LPTSTR ErrMsg,
           PSYM_ERR pSymErr
           )

{

    TCHAR szFName[_MAX_FNAME];
    TCHAR szExt[_MAX_EXT];
    TCHAR szCurName[_MAX_FNAME];
    TCHAR szSymName[_MAX_FNAME];
    TCHAR szPdbName[_MAX_FNAME];

    // Get the file name without any path info:
    if (Recurse) {
        _tcscpy(szCurName, pSymErr->szFileName);
    } else {
        _tsplitpath(pSymErr->szFileName,NULL,NULL,szFName,szExt);
        _tcscpy(szCurName,szFName);
        _tcscat(szCurName,szExt);
    }

    _tcscpy(ErrMsg,_T("") );

    // See if this is a file that we aren't supposed to report errors
    // for.  If it is, just return without writing an error.

    if ( pErrorFilterList != NULL ) {
        if ( InExcludeList( szCurName, pErrorFilterList ) ) {
            return(TRUE);
        }
    }

    // Get the file name without any path info:
    _tsplitpath(pSymErr->szSymbolFileName,NULL,NULL,szFName,szExt);
    _tcscpy(szSymName,szFName);
    _tcscat(szSymName,szExt);


    // Get the file name without any path info:
    _tsplitpath(pSymErr->szPdbFileName,NULL,NULL,szFName,szExt);
    _tcscpy(szPdbName,szFName);
    _tcscat(szPdbName,szExt);

    if (pSymErr->ErrNo == NO_CODE_VIEW) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s does not point to CodeView information\n",
                  szCurName, szSymName);
        return(TRUE);
    }
    if (pSymErr->ErrNo == PRIVATE_INFO_NOT_REMOVED) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s contains private information\n",
                  szCurName, szPdbName);
        return(TRUE);
    }

    if (pSymErr->ErrNo == PDB_MAY_BE_CORRUPT) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s may be corrupt\n",
                  szCurName, szPdbName);
        return(TRUE);
    }

    if ( _tcscmp(pSymErr->szPdbErr,_T("") ) ) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s\n",szCurName, pSymErr->szPdbErr );
        return(TRUE);
    }

    if ( pSymErr->SymbolFileFound  && !pSymErr->PdbFileFound ) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s is correct, but %s is missing\n",
                  szCurName, szSymName, szPdbName);
        return(TRUE);
    }

    if ( !pSymErr->PdbFileFound ) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s is missing \n",szCurName,szPdbName );
        return(TRUE);
    }

    // There was a DBG file, but PDB file didn't validate

    if ( pSymErr->SymbolFileFound && !pSymErr->PdbValid ) {
        _stprintf(ErrMsg, "%-20s FAILED  - %s and %s signatures do not match\n",
                  szCurName, szSymName, szPdbName);
        return(TRUE);
    }

    // There isn't supposed to be a DBG file.  PDB doesn't validate against
    // image.

    if ( !pSymErr->PdbValid ) {
        _stprintf(ErrMsg, "%-20s FAILED  - signature does not match %s\n",
                  szCurName, szPdbName );
        return(TRUE);
    }
    return(FALSE);
}


BOOL AddToSymbolsCDLog(
                      FILE *hSymCDLog,
                      PSYM_ERR pSymErr,
                      LPTSTR szSymbolPath,
                      LPTSTR szFileExt
                      )

/*
   szSymbolPath is the full path and name to the symbol file
   szFileExt is the extension of the image without the '.' at
   the front
*/

{

    LPTSTR szSrc;
    LPTSTR szDest;
    LPTSTR szTmp;

    TCHAR szFName[_MAX_FNAME+1];
    TCHAR szExt[_MAX_EXT+1];
    TCHAR szCurName[_MAX_FNAME + _MAX_EXT + 1];
    TCHAR szDestDir[_MAX_PATH];
    TCHAR szSymName[_MAX_FNAME + 1];
    TCHAR szSymExt[_MAX_EXT + 1];

    // If there is a list of the files that belong on the
    // CD, then only write this file to the log for the
    // symbol CD if the file is in the list
    //
    // Originally, this was used for the international
    // incremental builds.
    //
    if ( pCDIncludeList != NULL ) {
        if ( !InList( szSymbolPath, pCDIncludeList ) ) {
            return (TRUE);
        }
    }

    // Get the file name without any path info:
    _tsplitpath(pSymErr->szFileName,NULL,NULL,szFName,szExt);
    _tcscpy(szCurName,szFName);


    // Put the path below "binaries" as the source
    szSrc = _tcsstr(szSymbolPath, _T("symbols\\") );

    if (szSrc == NULL) {
        printf("%s: Cannot find \"symbols\\\" in the symbol file's path\n",
               szCurName);
        exit(1);
    }
    if ( _tcscmp( szSrc, _T("symbols\\") )  == 0 ) {
        printf("Symbol file name cannot end with \"symbols\\\"\n");
        exit(1);
    }

    // Move the Destination up to the directory after symbols.  If this is
    // the retail directory, don't include retail in the path.
    szDest = szSrc + _tcslen(_T("symbols\\"));

    if ( _tcsncmp( szDest, _T("retail\\"), _tcslen(_T("retail\\")) ) == 0 ) {
        szDest = szDest + _tcslen(_T("retail\\"));
    }

    _tsplitpath(szDest,NULL,szDestDir,NULL,NULL);

    // Remove the '\' from the end of the string;
    szTmp = szDestDir + _tcslen(szDestDir) - 1;
    while ( _tcscmp( szTmp, _T("\\")) == 0 ) {
        _tcscpy( szTmp, _T("") );
        szTmp--;
    }

    // get the symbol file name
    _tsplitpath(szSymbolPath,NULL,NULL,szSymName,szSymExt);


    fprintf(hSymCDLog, "%s%s,%s%s,%s,%s\n", szFName,szExt,
            szSymName,szSymExt,szSrc,szDestDir);

    return (TRUE);
}



P_LIST
GetList(
       LPTSTR szFileName
       )

{

    /* GetList gets the list and keeps the original file name which could
     * have included the path to the file
     * Note, it can be merged with GetExcludeList.  I first created it for
     * use in creating the symbols CD, and didn't want to risk entering a
     * bug into symchk
     */

    P_LIST pList;

    FILE  *fFile;
    TCHAR szCurFile[_MAX_FNAME+1], *c;
    TCHAR fname[_MAX_FNAME+1], ext[_MAX_EXT+1];
    DWORD i;
    LPTSTR szEndName;


    pList = (P_LIST)malloc(sizeof(LIST));

    pList->dNumFiles = 0;
    if (  (fFile = _tfopen(szFileName,_T("r") )) == NULL ) {
        // printf( "Cannot open the exclude file %s\n",szFileName );
        return FALSE;
    }

    while ( _fgetts(szCurFile,_MAX_FNAME,fFile) ) {
        if ( szCurFile[0] == ';' ) continue;
        (pList->dNumFiles)++;
    }

    // Go back to the beginning of the file
    fseek(fFile,0,0);
    pList->List = (LIST_ELEM*)malloc( sizeof(LIST_ELEM) *
                                      (pList->dNumFiles));
    if (pList->List == NULL) return FALSE;

    i = 0;
    while ( i < pList->dNumFiles ) {
        memset(szCurFile,'\0',sizeof(TCHAR) * (_MAX_FNAME+1) );
        _fgetts(szCurFile,_MAX_FNAME,fFile);

        // Replace the \n with \0
        c = NULL;
        c  = _tcschr(szCurFile, '\n');
        if ( c != NULL) *c='\0';

        if ( szCurFile[0] == ';' ) continue;

        if ( _tcslen(szCurFile) > _MAX_FNAME ) {
            printf("File %s has a string that is too large\n",szFileName);
            return(NULL);
        }

        // Allow for spaces and a ; after the file name
        // Move the '\0' back until it has erased the ';' and any
        // tabs and spaces that might come before it
        szEndName = _tcschr(szCurFile, ';');
        if (szEndName != NULL ) {
            while ( *szEndName == ';' || *szEndName == ' '
                    || *szEndName == '\t' ) {
                *szEndName = '\0';
                if ( szEndName > szCurFile ) szEndName--;
            }
        }

        _tcscpy(pList->List[i].Path,szCurFile);

        _tsplitpath(szCurFile,NULL,NULL,fname,ext);

        _tcscpy(pList->List[i].FName,fname);
        _tcscat(pList->List[i].FName,ext);

        i++;
    }

    fclose(fFile);


    // Sort the List
    qsort( (void*)pList->List, (size_t)pList->dNumFiles,
           (size_t)sizeof(LIST_ELEM), SymComp2 );


    return (pList);

}

int __cdecl
SymComp2(
        const void *e1,
        const void *e2
        )
{
    LPTSTR p1,n1;
    LPTSTR p2,n2;
    int rc;

    p1 = ((LIST_ELEM*)e1)->FName;
    p2 = ((LIST_ELEM*)e2)->FName;

    n1 = ((LIST_ELEM*)e1)->Path;
    n2 = ((LIST_ELEM*)e2)->Path;

    rc = _tcsicmp(p1,p2);
    if (rc == 0) return ( _tcsicmp(n1,n2) );
    else return (rc);
}



BOOL PDBPrivateStripped(
                       PDB *ppdb,
                       DBI *pdbi
                       )
{
    // Return values:
    // FALSE - Private Information has NOT been stripped
    // TRUE - Private Information has been stripped

    Mod *pmod;
    long cb;

    unsigned itsm;
    TPI *ptpi;
    TI  tiMin;
    TI  tiMac;

    pmod = NULL;
    while (DBIQueryNextMod(pdbi, pmod, &pmod) && pmod) {

        // Check that Source line info is removed
        ModQueryLines(pmod, NULL, &cb);

        if (cb != 0) {
            return FALSE;
        }

        // Check that local symbols are removed
        ModQuerySymbols(pmod, NULL, &cb);

        if (cb != 0) {
            return FALSE;
        }

    }

    // Check that types are removed
    for ( itsm = 0; itsm < 256; itsm++) {
        ptpi = 0;
        if (DBIQueryTypeServer(pdbi, (ITSM) itsm, &ptpi)) {
            continue;
        }
        if (!ptpi) {

            PDBOpenTpi(ppdb, pdbRead, &ptpi);
            tiMin = TypesQueryTiMinEx(ptpi);
            tiMac = TypesQueryTiMacEx(ptpi);
            if (tiMin < tiMac) {
                return FALSE;
            }
        }
    }
    return (TRUE);
}
