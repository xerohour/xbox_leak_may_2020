/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    iniapis.c

Abstract:

    This module contains the client side of the Win32 Initialization
    File APIs

Author:

    Steve Wood (stevewo) 24-Sep-1990

Revision History:

    08-Mar-2000     schanbai

          Port from base to XBox test library and remove mapping routines,
          client/server calls to CSR, registry-related routines.

    10-Mar-2000     schanbai

          Remove DBCS support from .ini file, even the file is ANSI

--*/

#include "precomp.h"
#include "misc.h"


#define BYTE_ORDER_MARK           0xFEFF
#define REVERSE_BYTE_ORDER_MARK   0xFFFE

#pragma warning( disable : 4201 )
#pragma warning( disable : 4213 )

typedef enum _INIFILE_OPERATION {
    FlushProfiles,
    ReadKeyValue,
    WriteKeyValue,
    DeleteKey,
    ReadKeyNames,
    ReadSectionNames,
    ReadSection,
    WriteSection,
    DeleteSection,
    RefreshIniFileMapping
} INIFILE_OPERATION;


typedef struct _INIFILE_CACHE {
    struct _INIFILE_CACHE *Next;
    ULONG EnvironmentUpdateCount;
    UNICODE_STRING NtFileName;
    HANDLE FileHandle;
    BOOLEAN WriteAccess;
    BOOLEAN UnicodeFile;
    BOOLEAN LockedFile;
    ULONG EndOfFile;
    PVOID BaseAddress;
    SIZE_T CommitSize;
    SIZE_T RegionSize;
    ULONG UpdateOffset;
    ULONG UpdateEndOffset;
    ULONG DirectoryInformationLength;
    FILE_NETWORK_OPEN_INFORMATION NetworkInformation;
} INIFILE_CACHE, *PINIFILE_CACHE;


typedef struct _INIFILE_PARAMETERS {
    INIFILE_OPERATION Operation;
    BOOLEAN WriteOperation;
    BOOLEAN Unicode;
    BOOLEAN ValueBufferAllocated;
    PINIFILE_CACHE IniFile;
    UNICODE_STRING FileName;
    UNICODE_STRING NtFileName;
    ANSI_STRING ApplicationName;
    ANSI_STRING VariableName;
    UNICODE_STRING ApplicationNameU;
    UNICODE_STRING VariableNameU;
    BOOLEAN MultiValueStrings;
    union {
        //
        // This structure filled in for write operations
        //
        struct {
            LPSTR ValueBuffer;
            ULONG ValueLength;
            PWSTR ValueBufferU;
            ULONG ValueLengthU;
        };
        //
        // This structure filled in for read operations
        //
        struct {
            ULONG ResultChars;
            ULONG ResultMaxChars;
            LPSTR ResultBuffer;
            PWSTR ResultBufferU;
        };
    };


    //
    // Remaining fields only valid when parsing an on disk .INI file mapped into
    // memory.
    //

    PVOID TextCurrent;
    PVOID TextStart;
    PVOID TextEnd;

    ANSI_STRING SectionName;
    ANSI_STRING KeywordName;
    ANSI_STRING KeywordValue;
    PANSI_STRING AnsiSectionName;
    PANSI_STRING AnsiKeywordName;
    PANSI_STRING AnsiKeywordValue;
    UNICODE_STRING SectionNameU;
    UNICODE_STRING KeywordNameU;
    UNICODE_STRING KeywordValueU;
    PUNICODE_STRING UnicodeSectionName;
    PUNICODE_STRING UnicodeKeywordName;
    PUNICODE_STRING UnicodeKeywordValue;
} INIFILE_PARAMETERS, *PINIFILE_PARAMETERS;


ULONG BaseIniFileUpdateCount = 0;


#if DBG
BOOLEAN BaseDllDumpIniCalls;

char *xOperationNames[] = {
    "FlushProfiles",
    "ReadKeyValue",
    "WriteKeyValue",
    "DeleteKey",
    "ReadKeyNames",
    "ReadSectionNames",
    "ReadSection",
    "WriteSection",
    "DeleteSection",
    "RefreshIniFileMapping"
};
#endif

ULONG
BaseDllIniFileNameLength(
                        IN BOOLEAN Unicode,
                        IN PVOID *Name
                        )
{
    if (Unicode) {
        PWSTR p;

        p = *Name;
        while (*p == L' ') {
            p++;
        }
        *Name = p;
        while (*p != UNICODE_NULL) {
            p++;
        }

        if (p > (PWSTR)*Name) {
            while (*--p == L' ') {
            }
            p++;
        }

        return (ULONG)(p - (PWSTR)*Name);
    } else {
        PCH p;

        p = *Name;
        while (*p == ' ') {
            p++;
        }
        *Name = p;
        while (*p != '\0') {
            p++;
        }

        if (p > (PCH)*Name) {
            while (*--p == ' ') {
            }
            p++;
        }

        return (ULONG)(p - (PCH)*Name);
    }
}


NTSTATUS
BaseDllCaptureIniFileParameters(
                               BOOLEAN Unicode,
                               INIFILE_OPERATION Operation,
                               BOOLEAN WriteOperation,
                               BOOLEAN MultiValueStrings,
                               PVOID FileName OPTIONAL,
                               PVOID ApplicationName OPTIONAL,
                               PVOID VariableName OPTIONAL,
                               PVOID VariableValue OPTIONAL,
                               PULONG ResultMaxChars OPTIONAL,
                               PINIFILE_PARAMETERS *ReturnedParameterBlock
                               )
{
    NTSTATUS Status;
    PBYTE s;
    PWSTR p, p1;
    ULONG TotalSize,
    CharSize,
    NtFileNameLength,
    FileNameLength,
    ApplicationNameLength,
    VariableNameLength,
    VariableValueLength;
    ANSI_STRING AnsiString;
    PINIFILE_PARAMETERS a;

    if (ARGUMENT_PRESENT( FileName )) {
        if (Unicode) {
            FileNameLength = wcslen( FileName );
        } else {
            FileNameLength = strlen( FileName );
        }
    } else {
        FileNameLength = 0;
    }

    if (ARGUMENT_PRESENT( ApplicationName )) {
        ApplicationNameLength = BaseDllIniFileNameLength( Unicode, &ApplicationName );
    } else {
        ApplicationNameLength = 0;
        VariableName = NULL;
    }

    if (ARGUMENT_PRESENT( VariableName )) {
        VariableNameLength = BaseDllIniFileNameLength( Unicode, &VariableName );
    } else {
        VariableNameLength = 0;
    }

    if (ARGUMENT_PRESENT( VariableValue )) {
        if (ARGUMENT_PRESENT( ResultMaxChars )) {
            VariableValueLength = 0;
        } else {
            if (!MultiValueStrings) {
                if (Unicode) {
                    VariableValueLength = wcslen( VariableValue );
                } else {
                    VariableValueLength = strlen( VariableValue );
                }
            } else {
                if (Unicode) {
                    p = (PWSTR)VariableValue;
                    while (*p) {
                        while (*p++) {
                            ;
                        }
                    }

                    VariableValueLength = (ULONG)(p - (PWSTR)VariableValue);
                } else {
                    s = (PBYTE)VariableValue;
                    while (*s) {
                        while (*s++) {
                            ;
                        }
                    }

                    VariableValueLength = (ULONG)(s - (PBYTE)VariableValue);
                }
            }
        }
    } else {
        VariableValueLength = 0;
    }

    NtFileNameLength = xxx_RtlGetLongestNtPathLength() * sizeof( WCHAR );

    TotalSize = sizeof( *a ) + NtFileNameLength;
    if (!Unicode) {
        TotalSize += (FileNameLength + 1 ) * sizeof( WCHAR );
    }

    // We have to allocate enough buffer for DBCS string.
    CharSize = (Unicode ? sizeof(WORD) : sizeof( WCHAR ));
    TotalSize += (ApplicationNameLength + 1 +
                  VariableNameLength + 1 +
                  VariableValueLength + 1
                 ) * CharSize;
    a = AllocateKernelPool( TotalSize );
    if (a == NULL) {
        KdPrint(( "BASE: Unable to allocate IniFile parameter buffer of %u bytes\n", TotalSize ));
        return STATUS_NO_MEMORY;
    }

    a->Operation = Operation;
    a->WriteOperation = WriteOperation;
    a->Unicode = Unicode;
    a->IniFile = NULL;
    a->ValueBufferAllocated = FALSE;
    a->MultiValueStrings = MultiValueStrings;

    p = (PWSTR)(a + 1);
    a->NtFileName.Buffer = p;
    a->NtFileName.Length = 0;
    a->NtFileName.MaximumLength = (USHORT)NtFileNameLength;
    p = (PWSTR)((PCHAR)p + NtFileNameLength);

    if (ARGUMENT_PRESENT( FileName )) {
        a->FileName.MaximumLength = (USHORT)((FileNameLength + 1) * sizeof( UNICODE_NULL ));
        if (Unicode) {
            a->FileName.Length = (USHORT)(FileNameLength * sizeof( WCHAR ));
            a->FileName.Buffer = FileName;
        } else {
            AnsiString.Buffer = FileName;
            AnsiString.Length = (USHORT)FileNameLength;
            AnsiString.MaximumLength = (USHORT)(AnsiString.Length + 1);
            a->FileName.Buffer = p;
            a->FileName.Length = 0;
            p += FileNameLength + 1;
            Status = RtlAnsiStringToUnicodeString( &a->FileName, &AnsiString, FALSE );
            if (!NT_SUCCESS( Status )) {
                FreeKernelPool( a );
                return Status;
            }
        }

    } else {
        RtlInitUnicodeString( &a->FileName, L"testini.ini" );
    }

    if (ARGUMENT_PRESENT( ApplicationName )) {
        // We have to keep enough buffer for DBCS string.
        a->ApplicationName.MaximumLength = (USHORT)((ApplicationNameLength * sizeof(WORD)) + 1);
        a->ApplicationNameU.MaximumLength = (USHORT)(a->ApplicationName.MaximumLength * sizeof( UNICODE_NULL ));
        if (Unicode) {
            a->ApplicationNameU.Buffer = ApplicationName;
            a->ApplicationNameU.Length = (USHORT)(ApplicationNameLength * sizeof( UNICODE_NULL ));
            a->ApplicationName.Buffer = (PBYTE)p;
            a->ApplicationName.Length = 0;
            p = (PWSTR)((PCHAR)p + (ApplicationNameLength * sizeof(WORD)) + 1);
        } else {
            a->ApplicationName.Buffer = ApplicationName;
            a->ApplicationName.Length = (USHORT)ApplicationNameLength;
            a->ApplicationNameU.Buffer = p;
            a->ApplicationNameU.Length = 0;
            p += ApplicationNameLength + 1;
        }
    } else {
        RtlInitAnsiString( &a->ApplicationName, NULL );
        RtlInitUnicodeString( &a->ApplicationNameU, NULL );
    }

    if (ARGUMENT_PRESENT( VariableName )) {
        // We have to keep enough buffer for DBCS string.
        a->VariableName.MaximumLength = (USHORT)((VariableNameLength *sizeof(WORD)) + 1);
        a->VariableNameU.MaximumLength = (USHORT)(a->VariableName.MaximumLength * sizeof( UNICODE_NULL ));
        if (Unicode) {
            a->VariableNameU.Buffer = VariableName;
            a->VariableNameU.Length = (USHORT)(VariableNameLength * sizeof( UNICODE_NULL ));
            a->VariableName.Buffer = (PBYTE)p;
            a->VariableName.Length = 0;
            p = (PWSTR)((PCHAR)p + (VariableNameLength * sizeof(WORD)) + 1);
        } else {
            a->VariableName.Buffer = VariableName;
            a->VariableName.Length = (USHORT)VariableNameLength;
            a->VariableNameU.Buffer = p;
            a->VariableNameU.Length = 0;
            p += VariableNameLength + 1;
        }
    } else {
        RtlInitAnsiString( &a->VariableName, NULL );
        RtlInitUnicodeString( &a->VariableNameU, NULL );
    }

    if (ARGUMENT_PRESENT( VariableValue )) {
        if (a->WriteOperation) {
            if (Unicode) {
                a->ValueBufferU = VariableValue;
                a->ValueLengthU = VariableValueLength * sizeof( WCHAR );
                *(PBYTE)p = '\0';
                a->ValueBuffer = (PBYTE)p;
                a->ValueLength = 0;
            } else {
                a->ValueBuffer = VariableValue;
                a->ValueLength = VariableValueLength;
                *p = UNICODE_NULL;
                a->ValueBufferU = p;
                a->ValueLengthU = 0;
            }
        } else {
            if (ARGUMENT_PRESENT( ResultMaxChars )) {
                a->ResultMaxChars = *ResultMaxChars;
            } else {
                a->ResultMaxChars = 0;
            }
            a->ResultChars = 0;
            if (Unicode) {
                a->ResultBufferU = VariableValue;
                a->ResultBuffer = NULL;
            } else {
                a->ResultBuffer = VariableValue;
                a->ResultBufferU = NULL;
            }
        }
    } else {
        if (a->WriteOperation) {
            a->ValueBuffer = NULL;
            a->ValueLength = 0;
            a->ValueBufferU = NULL;
            a->ValueLengthU = 0;
        } else {
            a->ResultMaxChars = 0;
            a->ResultChars = 0;
            a->ResultBufferU = NULL;
            a->ResultBuffer = NULL;
        }
    }

    *ReturnedParameterBlock = a;
    return STATUS_SUCCESS;
}


NTSTATUS
BaseDllAppendBufferToResultBuffer(
                                 IN PINIFILE_PARAMETERS a,
                                 IN PBYTE Buffer OPTIONAL,
                                 IN PWSTR BufferU OPTIONAL,
                                 IN ULONG Chars,
                                 IN BOOLEAN IncludeNull
                                 )
{
    NTSTATUS Status, OverflowStatus;
    ULONG Index;

    OverflowStatus = STATUS_SUCCESS;
    if (ARGUMENT_PRESENT( Buffer )) {
        if (ARGUMENT_PRESENT( BufferU )) {
            return STATUS_INVALID_PARAMETER;
        } else {
            ULONG CharsMbcs = Chars;
            //
            // In this point, Chars does not contains proper value for Unicode.
            // because. Chars was computed based on DBCS string length,
            // This is correct, sources string is DBCS, then
            // if the source is not DBCS. we just adjust it here.
            //
            if (a->Unicode) {
                Status = RtlMultiByteToUnicodeSize(&Chars,Buffer,Chars);
                if (!NT_SUCCESS( Status )) {
                    KdPrint(( "BASEDLL: AnsiToUnicodeSize of %.*s failed (%08x)\n", Chars, Buffer, Status ));
                    return Status;
                }
                Chars /= sizeof(WCHAR);
            }
            if (a->ResultChars + Chars >= a->ResultMaxChars) {
                OverflowStatus = STATUS_BUFFER_OVERFLOW;
                Chars = a->ResultMaxChars - a->ResultChars;
                if (Chars) {
                    Chars -= 1;
                }
            }

            if (Chars) {
                if (a->Unicode) {
                    Status = RtlMultiByteToUnicodeN( a->ResultBufferU + a->ResultChars,
                                                     Chars * sizeof( WCHAR ),
                                                     &Index,
                                                     Buffer,
                                                     CharsMbcs
                                                   );
                    if (!NT_SUCCESS( Status )) {
                        KdPrint(( "BASEDLL: AnsiToUnicode of %.*s failed (%08x)\n", Chars, Buffer, Status ));
                        return Status;
                    }
                } else {
                    memcpy( a->ResultBuffer + a->ResultChars, Buffer, Chars );
                }

                a->ResultChars += Chars;
            }
        }
    } else if (ARGUMENT_PRESENT( BufferU )) {
        ULONG CharsUnicode = Chars;
        //
        // In this point, Chars does not contains proper value for DBCS.
        // because. Chars was computed by just devide Unicode string length
        // by two. This is correct, sources string is Unicode, then
        // if the source is not Unicode. we just adjust it here.
        //
        if (!(a->Unicode)) {
            Status = RtlUnicodeToMultiByteSize(&Chars,BufferU,Chars * sizeof(WCHAR));
            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: UnicodeToAnsiSize of %.*ws failed (%08x)\n", Chars, BufferU, Status ));
                return Status;
            }
        }
        if (a->ResultChars + Chars >= a->ResultMaxChars) {
            OverflowStatus = STATUS_BUFFER_OVERFLOW;
            Chars = a->ResultMaxChars - a->ResultChars;
            if (Chars) {
                Chars -= 1;
            }
        }

        if (Chars) {
            if (a->Unicode) {
                memcpy( a->ResultBufferU + a->ResultChars, BufferU, Chars * sizeof( WCHAR ) );
            } else {
                Status = RtlUnicodeToMultiByteN( a->ResultBuffer + a->ResultChars,
                                                 Chars,
                                                 &Index,
                                                 BufferU,
                                                 CharsUnicode * sizeof( WCHAR )
                                               );
                if (!NT_SUCCESS( Status )) {
                    KdPrint(( "BASEDLL: UnicodeToAnsi of %.*ws failed (%08x)\n", Chars, BufferU, Status ));
                    return Status;
                }
            }

            a->ResultChars += Chars;
        }
    }

    if (IncludeNull) {
        if (a->ResultChars + 1 >= a->ResultMaxChars) {
            return STATUS_BUFFER_OVERFLOW;
        }

        if (a->Unicode) {
            a->ResultBufferU[ a->ResultChars ] = UNICODE_NULL;
        } else {
            a->ResultBuffer[ a->ResultChars ] = '\0';
        }

        a->ResultChars += 1;
    }

    return OverflowStatus;
}


NTSTATUS
BaseDllAppendNullToResultBuffer(
                               IN PINIFILE_PARAMETERS a
                               )
{
    return BaseDllAppendBufferToResultBuffer( a,
                                              NULL,
                                              NULL,
                                              0,
                                              TRUE
                                            );
}


NTSTATUS
BaseDllOpenIniFileOnDisk(
                        IN PINIFILE_PARAMETERS a
                        )
{
    NTSTATUS Status;
    PINIFILE_CACHE IniFile;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    LARGE_INTEGER ByteOffset, Length;
    OBJECT_STRING ObjectString;
    OCHAR ObjectBuffer[MAX_PATH];

    if ( a->FileName.Length > sizeof(WCHAR) && a->FileName.Buffer[0] == L'\\' ) {
        //
        // If FileName begin with backslash, this is NT-style path
        //
        a->NtFileName.Length = 0;
    } else {
        //
        // Convert to DOS-style path
        //
        wcscpy( a->NtFileName.Buffer, L"\\??\\t:\\" );
        if ( a->FileName.Length > sizeof(WCHAR) && a->FileName.Buffer[1] == L':' ) {
            a->NtFileName.Length = 8;
        } else {
            a->NtFileName.Length = 14;
        }
    }

    Status = RtlAppendUnicodeStringToString( &a->NtFileName, &a->FileName );

    IniFile = NULL;
    if (NT_SUCCESS( Status )) {
        IniFile = AllocateKernelPool( sizeof( *IniFile ) + a->NtFileName.MaximumLength );
        if (IniFile == NULL) {
            return STATUS_NO_MEMORY;
        }
        IniFile->NtFileName.Buffer = (PWSTR)(IniFile + 1);
        IniFile->NtFileName.MaximumLength = a->NtFileName.MaximumLength;
        RtlCopyUnicodeString( &IniFile->NtFileName, &a->NtFileName );
        IniFile->WriteAccess = a->WriteOperation;

        ObjectString.Buffer = ObjectBuffer;
        ObjectString.Length = 0;
        ObjectString.MaximumLength = sizeof(ObjectBuffer);
        RtlUnicodeStringToAnsiString( &ObjectString, &IniFile->NtFileName, FALSE );
        InitializeObjectAttributes( &ObjectAttributes,
                                    &ObjectString,
                                    OBJ_CASE_INSENSITIVE,
                                    NULL,
                                    NULL
                                  );
        if (IniFile->WriteAccess) {
            Status = NtCreateFile( &IniFile->FileHandle,
                                   SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                                   &ObjectAttributes,
                                   &IoStatusBlock,
                                   0,
                                   FILE_ATTRIBUTE_NORMAL,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   FILE_OPEN_IF,
                                   FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE
                                 );
        } else {
            Status = NtOpenFile( &IniFile->FileHandle,
                                 SYNCHRONIZE | GENERIC_READ,
                                 &ObjectAttributes,
                                 &IoStatusBlock,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 FILE_SYNCHRONOUS_IO_NONALERT |
                                 FILE_NON_DIRECTORY_FILE
                               );
        }

#if DBG
        if (!NT_SUCCESS( Status )) {
            if (BaseDllDumpIniCalls) {
                KdPrint(( "BASEDLL: Unable to open %wZ - Status == %x\n", &a->NtFileName, Status ));
            }
        }
#endif // DBG
    }

    if (NT_SUCCESS( Status )) {
        IniFile->LockedFile = FALSE;
        ByteOffset.QuadPart = 0;
        Length.QuadPart = -1;

        if (NT_SUCCESS( Status )) {
            Status = NtQueryInformationFile( IniFile->FileHandle,
                                             &IoStatusBlock,
                                             &IniFile->NetworkInformation,
                                             sizeof( IniFile->NetworkInformation ),
                                             FileNetworkOpenInformation
                                           );
            if (Status == STATUS_BUFFER_OVERFLOW) {
                Status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: Unable to QueryInformation for %wZ - Status == %x\n", &a->NtFileName, Status ));
            }

            if ( NT_SUCCESS(Status) && IniFile->NetworkInformation.EndOfFile.HighPart != 0 ) {
                Status = STATUS_SECTION_TOO_BIG;
            }
        }
    }

    if (!NT_SUCCESS( Status )) {
#if DBG
        if (BaseDllDumpIniCalls) {
            KdPrint(( "BASEDLL: Open of %wZ failed - Status == %x\n",
                      &IniFile->NtFileName,
                      Status
                    ));
        }
#endif // DBG

        if (IniFile != NULL) {

            NtClose( IniFile->FileHandle );
            FreeKernelPool( IniFile );
        }

        return Status;
    }

    ASSERT( IniFile->NetworkInformation.EndOfFile.HighPart == 0 );
    IniFile->EndOfFile = IniFile->NetworkInformation.EndOfFile.LowPart;
    IniFile->CommitSize = IniFile->EndOfFile + (4 * (IniFile->UnicodeFile ? sizeof( WCHAR ) : 1));
    IniFile->RegionSize = IniFile->CommitSize + 0x100000; // Room for 256KB of growth
    Status = NtAllocateVirtualMemory( &IniFile->BaseAddress,
                                      0,
                                      &IniFile->RegionSize,
                                      MEM_RESERVE,
                                      PAGE_READWRITE
                                    );
    if (NT_SUCCESS( Status )) {
        Status = NtAllocateVirtualMemory( &IniFile->BaseAddress,
                                          0,
                                          &IniFile->CommitSize,
                                          MEM_COMMIT,
                                          PAGE_READWRITE
                                        );
        if (NT_SUCCESS( Status )) {
            Status = NtReadFile( IniFile->FileHandle,
                                 NULL,
                                 NULL,
                                 NULL,
                                 &IoStatusBlock,
                                 IniFile->BaseAddress,
                                 IniFile->EndOfFile,
                                 NULL
                               );
            if (NT_SUCCESS( Status ) && IoStatusBlock.Information != IniFile->EndOfFile) {
                Status = STATUS_END_OF_FILE;
            }
        }
    }

    if (NT_SUCCESS( Status )) {
        IniFile->UpdateOffset = 0xFFFFFFFF;
        IniFile->UpdateEndOffset = 0;

        IniFile->UnicodeFile = xxx_RtlIsTextUnicode( IniFile->BaseAddress, IniFile->EndOfFile, 0 );

        if (IniFile->UnicodeFile) {
            PWSTR Src;

            Src = (PWSTR)((PCHAR)IniFile->BaseAddress + IniFile->EndOfFile);
            while (Src > (PWSTR)IniFile->BaseAddress && Src[ -1 ] <= L' ') {
                if (Src[-1] == L'\r' || Src[-1] == L'\n') {
                    break;
                }

                IniFile->EndOfFile -= sizeof( WCHAR );
                Src -= 1;
            }

            Src = (PWSTR)((PCHAR)IniFile->BaseAddress + IniFile->EndOfFile);
            if (Src > (PWSTR)IniFile->BaseAddress) {
                if (Src[-1] != L'\n') {
                    *Src++ = L'\r';
                    *Src++ = L'\n';
                    IniFile->UpdateOffset = IniFile->EndOfFile;
                    IniFile->UpdateEndOffset = IniFile->UpdateOffset + 2 * sizeof( WCHAR );
                    IniFile->EndOfFile = IniFile->UpdateEndOffset;
                }
            }
        } else {
            PBYTE Src;

            Src = (PBYTE)((PCHAR)IniFile->BaseAddress + IniFile->EndOfFile);
            while (Src > (PBYTE)IniFile->BaseAddress && Src[ -1 ] <= ' ') {
                if (Src[-1] == '\r' || Src[-1] == '\n') {
                    break;
                }

                IniFile->EndOfFile -= 1;
                Src -= 1;
            }

            Src = (PBYTE)((PCHAR)IniFile->BaseAddress + IniFile->EndOfFile);
            if (Src > (PBYTE)IniFile->BaseAddress) {
                if (Src[-1] != '\n') {
                    *Src++ = '\r';
                    *Src++ = '\n';
                    IniFile->UpdateOffset = IniFile->EndOfFile;
                    IniFile->UpdateEndOffset = IniFile->UpdateOffset + 2;
                    IniFile->EndOfFile = IniFile->UpdateEndOffset;
                }
            }
        }

        a->IniFile = IniFile;
    } else {
        KdPrint(( "BASEDLL: Read of %wZ failed - Status == %x\n",
                  &IniFile->NtFileName,
                  Status
                ));

        NtClose( IniFile->FileHandle );

        FreeKernelPool( IniFile );
    }

    return Status;
}


#define STOP_AT_SECTION 1
#define STOP_AT_KEYWORD 2
#define STOP_AT_NONSECTION 3


NTSTATUS
BaseDllAdvanceTextPointer(
                         IN PINIFILE_PARAMETERS a,
                         IN ULONG StopAt
                         )
{
    BOOLEAN AllowNoEquals;

    if (StopAt == STOP_AT_NONSECTION) {
        StopAt = STOP_AT_KEYWORD;
        AllowNoEquals = TRUE;
    } else {
        AllowNoEquals = FALSE;
    }

    if (a->IniFile->UnicodeFile) {
        PWSTR Src, EndOfLine, EqualSign, EndOfFile;
        PWSTR Name, EndOfName, Value, EndOfValue;

#define INI_TEXT(quote) L##quote

        Src = a->TextCurrent;
        EndOfFile = a->TextEnd;
        while (Src < EndOfFile) {
            //
            // Find first non-blank character on a line.  Skip blank lines
            //

            while (Src < EndOfFile && *Src <= INI_TEXT(' ')) {
                Src++;
            }

            if (Src >= EndOfFile) {
                a->TextCurrent = Src;
                break;
            }

            EndOfLine = Src;
            EqualSign = NULL;
            a->TextStart = Src;
            while (EndOfLine < EndOfFile) {
                if (EqualSign == NULL && *EndOfLine == INI_TEXT('=')) {
                    EqualSign = ++EndOfLine;
                } else
                    if (*EndOfLine == INI_TEXT('\r') || *EndOfLine == INI_TEXT('\n')) {
                    if (*EndOfLine == INI_TEXT('\r')) {
                        EndOfLine++;
                    }

                    if (*EndOfLine == INI_TEXT('\n')) {
                        EndOfLine++;
                    }

                    break;
                } else {
                    EndOfLine++;
                }
            }

            if (*Src != INI_TEXT(';')) {
                if (*Src == INI_TEXT('[')) {
                    Name = Src + 1;
                    while (Name < EndOfLine && *Name <= INI_TEXT(' ')) {
                        Name++;
                    }
                    EndOfName = Name;
                    while (EndOfName < EndOfLine && *EndOfName != INI_TEXT(']')) {
                        EndOfName++;
                    }

                    while (EndOfName > Name && EndOfName[ -1 ] <= INI_TEXT(' ')) {
                        EndOfName--;
                    }
                    a->SectionNameU.Buffer = Name;
                    a->SectionNameU.Length = (USHORT)((PCHAR)EndOfName - (PCHAR)Name);
                    a->SectionNameU.MaximumLength = a->SectionNameU.Length;
                    a->AnsiSectionName = NULL;
                    a->UnicodeSectionName = &a->SectionNameU;
                    if (StopAt == STOP_AT_SECTION) {
                        a->TextCurrent = EndOfLine;
                        return STATUS_MORE_ENTRIES;
                    } else
                        if (StopAt == STOP_AT_KEYWORD) {
                        return STATUS_NO_MORE_ENTRIES;
                    }
                } else
                    if (AllowNoEquals || (EqualSign != NULL) ) {

                    if (EqualSign != NULL) {
                        Name = Src;
                        EndOfName = EqualSign - 1;
                        while (EndOfName > Name && EndOfName[ -1 ] <= INI_TEXT(' ')) {
                            EndOfName--;
                        }

                        a->KeywordNameU.Buffer = Name;
                        a->KeywordNameU.Length = (USHORT)((PCHAR)EndOfName - (PCHAR)Name);
                        a->KeywordNameU.MaximumLength = a->KeywordNameU.Length;
                        a->AnsiKeywordName = NULL;
                        a->UnicodeKeywordName = &a->KeywordNameU;

                        Value = EqualSign;
                    } else {
                        Value = Src;
                        a->AnsiKeywordName = NULL;
                        a->UnicodeKeywordName = NULL;
                    }

                    EndOfValue = EndOfLine;
                    while (EndOfValue > Value && EndOfValue[ -1 ] <= INI_TEXT(' ')) {
                        EndOfValue--;
                    }
                    a->KeywordValueU.Buffer = Value;
                    a->KeywordValueU.Length = (USHORT)((PCHAR)EndOfValue - (PCHAR)Value);
                    a->KeywordValueU.MaximumLength = a->KeywordValueU.Length;
                    a->AnsiKeywordValue = NULL;
                    a->UnicodeKeywordValue = &a->KeywordValueU;
                    if (StopAt == STOP_AT_KEYWORD) {
                        a->TextCurrent = EndOfLine;
                        return STATUS_MORE_ENTRIES;
                    }
                }
            }

            Src = EndOfLine;
        }
    } else {
        PBYTE Src, EndOfLine, EqualSign, EndOfFile;
        PBYTE Name, EndOfName, Value, EndOfValue;

#undef INI_TEXT
#define INI_TEXT(quote) quote

        Src = a->TextCurrent;
        EndOfFile = a->TextEnd;
        while (Src < EndOfFile) {
            //
            // Find first non-blank character on a line.  Skip blank lines
            //

            while (Src < EndOfFile && *Src <= INI_TEXT(' ')) {
                Src++;
            }

            if (Src >= EndOfFile) {
                a->TextCurrent = Src;
                break;
            }

            EndOfLine = Src;
            EqualSign = NULL;
            a->TextStart = Src;
            while (EndOfLine < EndOfFile) {
                if (EqualSign == NULL && *EndOfLine == INI_TEXT('=')) {
                    EqualSign = ++EndOfLine;
                } else
                    if (*EndOfLine == INI_TEXT('\r') || *EndOfLine == INI_TEXT('\n')) {
                    if (*EndOfLine == INI_TEXT('\r')) {
                        EndOfLine++;
                    }

                    if (*EndOfLine == INI_TEXT('\n')) {
                        EndOfLine++;
                    }

                    break;
                } else {
                    EndOfLine++;
                }
            }

            if (*Src != INI_TEXT(';')) {
                if (*Src == INI_TEXT('[')) {
                    Name = Src + 1;
                    while (Name < EndOfLine && *Name <= INI_TEXT(' ')) {
                        Name++;
                    }
                    EndOfName = Name;
                    while (EndOfName < EndOfLine) {
                        if (*EndOfName == INI_TEXT(']')) {
                            break;
                        }
                        //if (IsDBCSLeadByte(*EndOfName)) {
                            //EndOfName++;
                        //}
                        EndOfName++;
                    }
                    while (EndOfName > Name && EndOfName[ -1 ] <= INI_TEXT(' ')) {
                        EndOfName--;
                    }
                    a->SectionName.Buffer = Name;
                    a->SectionName.Length = (USHORT)((PCHAR)EndOfName - (PCHAR)Name);
                    a->SectionName.MaximumLength = a->SectionName.Length;
                    a->AnsiSectionName = &a->SectionName;
                    a->UnicodeSectionName = NULL;
                    if (StopAt == STOP_AT_SECTION) {
                        a->TextCurrent = EndOfLine;
                        return STATUS_MORE_ENTRIES;
                    } else
                        if (StopAt == STOP_AT_KEYWORD) {
                        return STATUS_NO_MORE_ENTRIES;
                    }
                } else
                    if (AllowNoEquals || (EqualSign != NULL)) {

                    if (EqualSign != NULL) {
                        Name = Src;
                        EndOfName = EqualSign - 1;
                        while (EndOfName > Name && EndOfName[ -1 ] <= INI_TEXT(' ')) {
                            EndOfName--;
                        }

                        a->KeywordName.Buffer = Name;
                        a->KeywordName.Length = (USHORT)((PCHAR)EndOfName - (PCHAR)Name);
                        a->KeywordName.MaximumLength = a->KeywordName.Length;
                        a->AnsiKeywordName = &a->KeywordName;
                        a->UnicodeKeywordName = NULL;

                        Value = EqualSign;
                    } else {
                        Value = Src;
                        a->AnsiKeywordName = NULL;
                        a->UnicodeKeywordName = NULL;
                    }

                    EndOfValue = EndOfLine;
                    while (EndOfValue > Value && EndOfValue[ -1 ] <= INI_TEXT(' ')) {
                        EndOfValue--;
                    }
                    a->KeywordValue.Buffer = Value;
                    a->KeywordValue.Length = (USHORT)((PCHAR)EndOfValue - (PCHAR)Value);
                    a->KeywordValue.MaximumLength = a->KeywordValue.Length;
                    a->AnsiKeywordValue = &a->KeywordValue;
                    a->UnicodeKeywordValue = NULL;
                    if (StopAt == STOP_AT_KEYWORD) {
                        a->TextCurrent = EndOfLine;
                        return STATUS_MORE_ENTRIES;
                    }
                }
            }

            Src = EndOfLine;
        }
    }

    return STATUS_NO_MORE_ENTRIES;
}


NTSTATUS
BaseDllAppendStringToResultBuffer(
                                 IN PINIFILE_PARAMETERS a,
                                 IN PANSI_STRING String OPTIONAL,
                                 IN PUNICODE_STRING StringU OPTIONAL,
                                 IN BOOLEAN IncludeNull
                                 )
{
    if (ARGUMENT_PRESENT( String )) {
        if (ARGUMENT_PRESENT( StringU )) {
            return STATUS_INVALID_PARAMETER;
        } else {
            return BaseDllAppendBufferToResultBuffer( a,
                                                      String->Buffer,
                                                      NULL,
                                                      String->Length,
                                                      IncludeNull
                                                    );
        }
    } else if (ARGUMENT_PRESENT( StringU )) {
        return BaseDllAppendBufferToResultBuffer( a,
                                                  NULL,
                                                  StringU->Buffer,
                                                  StringU->Length / sizeof( WCHAR ),
                                                  IncludeNull
                                                );
    } else {
        return STATUS_INVALID_PARAMETER;
    }
}


NTSTATUS
BaseDllReadSectionNames(
                       IN PINIFILE_PARAMETERS a
                       )
{
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    while (NT_SUCCESS( Status )) {
        Status = BaseDllAdvanceTextPointer( a, STOP_AT_SECTION );
        if (Status == STATUS_MORE_ENTRIES) {
            Status = BaseDllAppendStringToResultBuffer( a,
                                                        a->AnsiSectionName,
                                                        a->UnicodeSectionName,
                                                        TRUE
                                                      );
        } else {
            if (Status == STATUS_NO_MORE_ENTRIES) {
                Status = STATUS_SUCCESS;
            }
            break;
        }
    }

    return Status;
}


BOOLEAN
BaseDllGetApplicationName(
                         IN PINIFILE_PARAMETERS a,
                         OUT PANSI_STRING *ApplicationName OPTIONAL,
                         OUT PUNICODE_STRING *ApplicationNameU OPTIONAL
                         )
{
    NTSTATUS Status;

    if (ARGUMENT_PRESENT( ApplicationName )) {
        if (a->ApplicationName.Length == 0) {
            Status = RtlUnicodeStringToAnsiString( &a->ApplicationName, &a->ApplicationNameU, FALSE );
            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: UnicodeToAnsi of %wZ failed (%08x)\n", &a->ApplicationNameU, Status ));
                return FALSE;
            }
        }

        *ApplicationName = &a->ApplicationName;
        return TRUE;
    }

    if (ARGUMENT_PRESENT( ApplicationNameU )) {
        if (a->ApplicationNameU.Length == 0) {
            Status = RtlAnsiStringToUnicodeString( &a->ApplicationNameU, &a->ApplicationName, FALSE );
            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: AnsiToUnicode of %Z failed (%08x)\n", &a->ApplicationName, Status ));
                return FALSE;

            }
        }
        *ApplicationNameU = &a->ApplicationNameU;
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
BaseDllGetVariableName(
                      IN PINIFILE_PARAMETERS a,
                      OUT PANSI_STRING *VariableName OPTIONAL,
                      OUT PUNICODE_STRING *VariableNameU OPTIONAL
                      )
{
    NTSTATUS Status;

    if (ARGUMENT_PRESENT( VariableName )) {
        if (a->VariableName.Length == 0) {
            Status = RtlUnicodeStringToAnsiString( &a->VariableName, &a->VariableNameU, FALSE );
            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: UnicodeToAnsi of %wZ failed (%08x)\n", &a->VariableNameU, Status ));
                return FALSE;
            }
        }

        *VariableName = &a->VariableName;
        return TRUE;
    }

    if (ARGUMENT_PRESENT( VariableNameU )) {
        if (a->VariableNameU.Length == 0) {
            Status = RtlAnsiStringToUnicodeString( &a->VariableNameU, &a->VariableName, FALSE );
            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: AnsiToUnicode of %Z failed (%08x)\n", &a->VariableName, Status ));
                return FALSE;

            }
        }
        *VariableNameU = &a->VariableNameU;
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
BaseDllGetVariableValue(
                       IN PINIFILE_PARAMETERS a,
                       OUT PBYTE *VariableValue OPTIONAL,
                       OUT PWSTR *VariableValueU OPTIONAL,
                       OUT PULONG VariableValueLength
                       )
{
    NTSTATUS Status;
    ULONG Index;

    if (ARGUMENT_PRESENT( VariableValue )) {
        if (a->ValueLength == 0) {
            if (a->ValueBufferU == NULL || a->ValueLengthU == 0) {
                *VariableValue = "";
                *VariableValueLength = 1;
                return TRUE;
            }

            a->ValueLength = a->ValueLengthU;
            Status = RtlUnicodeToMultiByteN( a->ValueBuffer,
                                             a->ValueLength,
                                             &Index,
                                             a->ValueBufferU,
                                             a->ValueLengthU
                                           );

            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: UnicodeToAnsi of %.*ws failed (%08x)\n",
                          a->ValueLengthU / sizeof( WCHAR ), a->ValueBufferU, Status
                        ));
                return FALSE;
            }

            // Set real converted size
            a->ValueLength = Index;
            a->ValueBuffer[ Index ] = '\0';       // Null terminate converted value
        } else {
            Index = a->ValueLength;
        }

        *VariableValue = a->ValueBuffer;
        *VariableValueLength = Index + 1;
        return TRUE;
    }

    if (ARGUMENT_PRESENT( VariableValueU )) {
        if (a->ValueLengthU == 0) {
            if (a->ValueBuffer == NULL || a->ValueLength == 0) {
                *VariableValueU = L"";
                *VariableValueLength = sizeof( UNICODE_NULL );
                return TRUE;
            }

            a->ValueLengthU = a->ValueLength * sizeof( WCHAR );
            Status = RtlMultiByteToUnicodeN( a->ValueBufferU,
                                             a->ValueLengthU,
                                             &Index,
                                             a->ValueBuffer,
                                             a->ValueLength
                                           );


            if (!NT_SUCCESS( Status )) {
                KdPrint(( "BASEDLL: AnsiToUnicode of %.*s failed (%08x)\n",
                          a->ValueLength, a->ValueBuffer, Status
                        ));
                return FALSE;
            }

            Index = Index / sizeof( WCHAR );
            a->ValueBufferU[ Index ] = UNICODE_NULL;    // Null terminate converted value
        } else {
            Index = a->ValueLengthU / sizeof( WCHAR );
        }

        *VariableValueU = a->ValueBufferU;
        *VariableValueLength = (Index + 1) * sizeof( WCHAR );
        return TRUE;
    }

    return FALSE;
}


NTSTATUS
BaseDllFindSection(
                  IN PINIFILE_PARAMETERS a
                  )
{
    NTSTATUS Status;
    PANSI_STRING AnsiSectionName;
    PUNICODE_STRING UnicodeSectionName;
    BOOL FreeUnicodeBuffer;

    while (TRUE) {
        Status = BaseDllAdvanceTextPointer( a, STOP_AT_SECTION );
        if (Status == STATUS_MORE_ENTRIES) {
            FreeUnicodeBuffer = FALSE;
            if (a->AnsiSectionName) {
                // Ansi ini file
                if (a->Unicode) {
                    // Unicode parm - convert the ansi sectio name to unicode
                    if (!BaseDllGetApplicationName( a, NULL, &UnicodeSectionName )) {
                        return STATUS_INVALID_PARAMETER;
                    }

                    a->UnicodeSectionName = &a->SectionNameU;
                    Status = RtlAnsiStringToUnicodeString( a->UnicodeSectionName,
                                                           a->AnsiSectionName,
                                                           TRUE
                                                         );
                    if (!NT_SUCCESS( Status )) {
                        KdPrint(( "BASEDLL: AnsiToUnicode of %Z failed (%08x)\n", a->AnsiSectionName, Status ));
                        return Status;
                    }

                    FreeUnicodeBuffer = TRUE;
                } else {
                    // Ansi parm
                    if (!BaseDllGetApplicationName( a, &AnsiSectionName, NULL )) {
                        return STATUS_INVALID_PARAMETER;
                    }
                }
            } else {
                // Doesn't matter - Unicode ini, get the Unicode section name.
                if (!BaseDllGetApplicationName( a, NULL, &UnicodeSectionName )) {
                    return STATUS_INVALID_PARAMETER;
                }
            }

            if (a->AnsiSectionName == NULL || a->Unicode) {
                if (RtlEqualUnicodeString( UnicodeSectionName,
                                           a->UnicodeSectionName,
                                           TRUE
                                         )
                   ) {
                    Status = STATUS_SUCCESS;
                } else {
                    Status = STATUS_MORE_ENTRIES;
                }
            } else {
                if (RtlEqualString( AnsiSectionName, a->AnsiSectionName, TRUE )) {
                    Status = STATUS_SUCCESS;
                } else {
                    Status = STATUS_MORE_ENTRIES;
                }
            }

            if (FreeUnicodeBuffer) {
                RtlFreeUnicodeString( a->UnicodeSectionName );
            }

            if (Status != STATUS_MORE_ENTRIES) {
                return Status;
            }
        } else {
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }
    }
}

NTSTATUS
BaseDllFindKeyword(
                  IN PINIFILE_PARAMETERS a
                  )
{
    NTSTATUS Status;
    PANSI_STRING AnsiKeywordName;
    PUNICODE_STRING UnicodeKeywordName;
    BOOL FreeUnicodeBuffer;

    while (TRUE) {
        Status = BaseDllAdvanceTextPointer( a, STOP_AT_KEYWORD );
        if (Status == STATUS_MORE_ENTRIES) {
            FreeUnicodeBuffer = FALSE;

            // Here's the deal.  We want to compare Unicode if possible.  If the
            // The ini is Ansi and the input parm is ansi, use ansi.  Otherwise
            // use Unicode for everything.

            if (a->AnsiKeywordName) {
                // Ansi ini file.
                if (a->Unicode) {
                    // Unicode parm - convert the ansi ini keyword to unicode.
                    if (!BaseDllGetVariableName( a, NULL, &UnicodeKeywordName )) {
                        return STATUS_INVALID_PARAMETER;
                    }

                    a->UnicodeKeywordName = &a->KeywordNameU;
                    Status = RtlAnsiStringToUnicodeString( a->UnicodeKeywordName,
                                                           a->AnsiKeywordName,
                                                           TRUE
                                                         );
                    if (!NT_SUCCESS( Status )) {
                        KdPrint(( "BASEDLL: AnsiToUnicode of %Z failed (%08x)\n", a->AnsiKeywordName, Status ));
                        return Status;
                    }

                    FreeUnicodeBuffer = TRUE;
                } else {
                    // Ansi param
                    if (!BaseDllGetVariableName( a, &AnsiKeywordName, NULL )) {
                        return STATUS_INVALID_PARAMETER;
                    }
                }
            } else {
                // Doesn't matter - Unicode ini, get the Unicode parm.
                if (!BaseDllGetVariableName( a, NULL, &UnicodeKeywordName )) {
                    return STATUS_INVALID_PARAMETER;
                }
            }

            if (a->AnsiKeywordName == NULL || a->Unicode) {
                if (RtlEqualUnicodeString( UnicodeKeywordName,
                                           a->UnicodeKeywordName,
                                           TRUE
                                         )
                   ) {
                    Status = STATUS_SUCCESS;
                } else {
                    Status = STATUS_MORE_ENTRIES;
                }
            } else {
                if (RtlEqualString( AnsiKeywordName, a->AnsiKeywordName, TRUE )) {
                    Status = STATUS_SUCCESS;
                } else {
                    Status = STATUS_MORE_ENTRIES;
                }
            }

            if (FreeUnicodeBuffer) {
                RtlFreeUnicodeString( a->UnicodeKeywordName );
            }

            if (Status != STATUS_MORE_ENTRIES) {
                return Status;
            }
        } else {
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }
    }
}


NTSTATUS
BaseDllReadKeywordValue(
                       IN PINIFILE_PARAMETERS a
                       )
{
    NTSTATUS Status;

    Status = BaseDllFindSection( a );
    if (!NT_SUCCESS( Status )) {
        return Status;
    }

    Status = BaseDllFindKeyword( a );
    if (!NT_SUCCESS( Status )) {
        return Status;
    }

    if (a->IniFile->UnicodeFile) {
        PWSTR Src;

        Src = (PWSTR)a->UnicodeKeywordValue->Buffer;
        while (*Src <= L' ' && a->UnicodeKeywordValue->Length) {
            Src += 1;
            a->UnicodeKeywordValue->Buffer = Src;
            a->UnicodeKeywordValue->Length -= sizeof( WCHAR );
            a->UnicodeKeywordValue->MaximumLength -= sizeof( WCHAR );
        }

        if (a->UnicodeKeywordValue->Length >= (2 * sizeof( WCHAR )) &&
            (Src[ 0 ] == Src[ (a->UnicodeKeywordValue->Length - sizeof( WCHAR )) / sizeof( WCHAR ) ]) &&
            (Src[ 0 ] == L'"' || Src[ 0 ] == L'\'')
           ) {
            a->UnicodeKeywordValue->Buffer += 1;
            a->UnicodeKeywordValue->Length -= (2 * sizeof( WCHAR ));
            a->UnicodeKeywordValue->MaximumLength -= (2 * sizeof( WCHAR ));
        }
    } else {
        PBYTE Src;

        Src = (PBYTE)a->AnsiKeywordValue->Buffer;
        while (*Src <= ' ' && a->AnsiKeywordValue->Length) {
            Src += 1;
            a->AnsiKeywordValue->Buffer = Src;
            a->AnsiKeywordValue->Length -= sizeof( UCHAR );
            a->AnsiKeywordValue->MaximumLength -= sizeof( UCHAR );
        }

        if (a->AnsiKeywordValue->Length >= (2 * sizeof( UCHAR )) &&
            (Src[ 0 ] == Src[ (a->AnsiKeywordValue->Length - sizeof( UCHAR )) / sizeof( UCHAR ) ]) &&
            (Src[ 0 ] == '"' || Src[ 0 ] == '\'')
           ) {
            a->AnsiKeywordValue->Buffer += 1;
            a->AnsiKeywordValue->Length -= (2 * sizeof( UCHAR ));
            a->AnsiKeywordValue->MaximumLength -= (2 * sizeof( UCHAR ));
        }
    }

    return BaseDllAppendStringToResultBuffer( a,
                                              a->AnsiKeywordValue,
                                              a->UnicodeKeywordValue,
                                              TRUE
                                            );
}


NTSTATUS
BaseDllReadKeywordNames(
                       IN PINIFILE_PARAMETERS a
                       )
{
    NTSTATUS Status;

    Status = BaseDllFindSection( a );
    while (NT_SUCCESS( Status )) {
        Status = BaseDllAdvanceTextPointer( a, STOP_AT_KEYWORD );
        if (Status == STATUS_MORE_ENTRIES) {
            Status = BaseDllAppendStringToResultBuffer( a,
                                                        a->AnsiKeywordName,
                                                        a->UnicodeKeywordName,
                                                        TRUE
                                                      );
        } else {
            if (Status == STATUS_NO_MORE_ENTRIES) {
                Status = STATUS_SUCCESS;
            }
            break;
        }
    }

    return Status;
}


NTSTATUS
BaseDllReadSection(
                  IN PINIFILE_PARAMETERS a
                  )
{
    NTSTATUS Status;

    Status = BaseDllFindSection( a );
    if (!NT_SUCCESS( Status )) {
        return Status;
    }

    while (TRUE) {
        Status = BaseDllAdvanceTextPointer( a, STOP_AT_NONSECTION );
        if (Status == STATUS_MORE_ENTRIES) {
            if (a->AnsiKeywordName || a->UnicodeKeywordName) {
                Status = BaseDllAppendStringToResultBuffer( a,
                                                            a->AnsiKeywordName,
                                                            a->UnicodeKeywordName,
                                                            FALSE
                                                          );
                if (!NT_SUCCESS( Status )) {
                    return Status;
                }

                Status = BaseDllAppendBufferToResultBuffer( a,
                                                            a->Unicode ? NULL : "=",
                                                            a->Unicode ? L"=" : NULL,
                                                            1,
                                                            FALSE
                                                          );
                if (!NT_SUCCESS( Status )) {
                    return Status;
                }
            }

            if (a->IniFile->UnicodeFile) {
                PWSTR Src;

                Src = (PWSTR)a->UnicodeKeywordValue->Buffer;
                while (*Src <= L' ' && a->UnicodeKeywordValue->Length) {
                    Src += 1;
                    a->UnicodeKeywordValue->Buffer = Src;
                    a->UnicodeKeywordValue->Length -= sizeof( WCHAR );
                    a->UnicodeKeywordValue->MaximumLength -= sizeof( WCHAR );
                }
            } else {
                PBYTE Src;

                Src = (PBYTE)a->AnsiKeywordValue->Buffer;
                while (*Src <= ' ' && a->AnsiKeywordValue->Length) {
                    Src += 1;
                    a->AnsiKeywordValue->Buffer = Src;
                    a->AnsiKeywordValue->Length -= sizeof( UCHAR );
                    a->AnsiKeywordValue->MaximumLength -= sizeof( UCHAR );
                }
            }

            Status = BaseDllAppendStringToResultBuffer( a,
                                                        a->AnsiKeywordValue,
                                                        a->UnicodeKeywordValue,
                                                        TRUE
                                                      );
            if (!NT_SUCCESS( Status )) {
                return Status;
            }
        } else {
            if (Status == STATUS_NO_MORE_ENTRIES) {
                Status = STATUS_SUCCESS;
            }

            break;
        }
    }

    return Status;
}


NTSTATUS
BaseDllCalculateDeleteLength(
                            IN PINIFILE_PARAMETERS a
                            )
{
    ULONG DeleteLength;

    if (a->IniFile->UnicodeFile) {
        DeleteLength = (ULONG)((PCHAR)a->TextCurrent -
                               (PCHAR)a->UnicodeKeywordName->Buffer);
    } else {
        DeleteLength = (ULONG)((PCHAR)a->TextCurrent -
                               a->AnsiKeywordName->Buffer);
    }

    return DeleteLength;
}


NTSTATUS
BaseDllModifyMappedFile(
                       IN PINIFILE_PARAMETERS a,
                       IN PVOID AddressInFile,
                       IN ULONG SizeToRemove,
                       IN PVOID InsertBuffer,
                       IN ULONG InsertAmount
                       )
{
    NTSTATUS Status;
    ULONG NewEndOfFile, UpdateOffset, UpdateLength;

    NewEndOfFile = a->IniFile->EndOfFile - SizeToRemove + InsertAmount;
    if (NewEndOfFile > a->IniFile->CommitSize) {
        if (NewEndOfFile > a->IniFile->RegionSize) {
            return STATUS_BUFFER_OVERFLOW;
        }

        a->IniFile->CommitSize = NewEndOfFile;
        Status = NtAllocateVirtualMemory( &a->IniFile->BaseAddress,
                                          0,
                                          &a->IniFile->CommitSize,
                                          MEM_COMMIT,
                                          PAGE_READWRITE
                                        );
        if (!NT_SUCCESS( Status )) {
            KdPrint(( "BASEDLL: Unable to allocate memory to grow %wZ - Status == %x\n",
                      &a->IniFile->NtFileName,
                      Status
                    ));

            return Status;
        }

        a->IniFile->EndOfFile = NewEndOfFile;
    }

    UpdateOffset = (ULONG)((PCHAR)AddressInFile - (PCHAR)(a->IniFile->BaseAddress)),
                   UpdateLength = (ULONG)((PCHAR)a->TextEnd - (PCHAR)AddressInFile) + InsertAmount - SizeToRemove;
    //
    // Are we deleting more than we are inserting?
    //
    if (SizeToRemove > InsertAmount) {
        //
        // Yes copy over insert string.
        //
        RtlMoveMemory( AddressInFile, InsertBuffer, InsertAmount );

        //
        // Delete remaining text after insertion string by moving it
        // up
        //

        RtlMoveMemory( (PCHAR)AddressInFile + InsertAmount,
                       (PCHAR)AddressInFile + SizeToRemove,
                       UpdateLength - InsertAmount
                     );
    } else
        if (InsertAmount > 0) {
        //
        // Are we deleting less than we are inserting?
        //
        if (SizeToRemove < InsertAmount) {
            //
            // Move text down to make room for insertion
            //

            RtlMoveMemory( (PCHAR)AddressInFile + InsertAmount - SizeToRemove,
                           (PCHAR)AddressInFile,
                           UpdateLength - InsertAmount + SizeToRemove
                         );
        } else {
            //
            // Deleting and inserting same amount, update just that text as
            // no shifting was done.
            //

            UpdateLength = InsertAmount;
        }

        //
        // Copy over insert string
        //

        RtlMoveMemory( AddressInFile, InsertBuffer, InsertAmount );
    } else {
        //
        // Nothing to change, as InsertAmount and SizeToRemove are zero
        //
        return STATUS_SUCCESS;
    }

    if (a->IniFile->EndOfFile != NewEndOfFile) {
        a->IniFile->EndOfFile = NewEndOfFile;
    }

    if (UpdateOffset < a->IniFile->UpdateOffset) {
        a->IniFile->UpdateOffset = UpdateOffset;
    }

    if ((UpdateOffset + UpdateLength) > a->IniFile->UpdateEndOffset) {
        a->IniFile->UpdateEndOffset = UpdateOffset + UpdateLength;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
BaseDllWriteKeywordValue(
                        IN PINIFILE_PARAMETERS a,
                        IN PUNICODE_STRING VariableName OPTIONAL
                        )
{
    NTSTATUS Status;
    BOOLEAN InsertSectionName;
    BOOLEAN InsertKeywordName;
    PVOID InsertBuffer;
    ULONG InsertAmount, n;
    PANSI_STRING AnsiSectionName;
    PANSI_STRING AnsiKeywordName;
    PUNICODE_STRING UnicodeSectionName;
    PUNICODE_STRING UnicodeKeywordName;
    PBYTE AnsiKeywordValue;
    PWSTR UnicodeKeywordValue;
    ULONG ValueLength;
    ULONG DeleteLength;
    PVOID AddressInFile = NULL;

    InsertAmount = 0;
    Status = BaseDllFindSection( a );
    if (!NT_SUCCESS( Status )) {
        if (a->Operation == DeleteKey) {
            return STATUS_SUCCESS;
        }

        AddressInFile = a->TextEnd;
        if (a->IniFile->UnicodeFile) {
            if (!BaseDllGetApplicationName( a, NULL, &UnicodeSectionName )) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // Add in size of [SectionName]\r\n
            //

            InsertAmount += (1 + 1 + 2) * sizeof( WCHAR );
            InsertAmount += UnicodeSectionName->Length;
        } else {
            if (!BaseDllGetApplicationName( a, &AnsiSectionName, NULL )) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // Add in size of [SectionName]\r\n
            //

            InsertAmount += (1 + 1 + 2) * sizeof( UCHAR );
            InsertAmount += AnsiSectionName->Length;
        }

        InsertSectionName = TRUE;
    } else {
        InsertSectionName = FALSE;
        Status = BaseDllFindKeyword( a );
    }

    if (!NT_SUCCESS( Status )) {
        if (a->Operation == DeleteKey) {
            return STATUS_SUCCESS;
        }

        if (!InsertSectionName) {
            AddressInFile = a->TextCurrent;
        }

        if (a->IniFile->UnicodeFile) {
            if (!BaseDllGetVariableName( a, NULL, &UnicodeKeywordName )) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // Add in size of Keyword=\r\n
            //

            InsertAmount += (1 + 2) * sizeof( WCHAR );
            InsertAmount += UnicodeKeywordName->Length;
        } else {
            if (!BaseDllGetVariableName( a, &AnsiKeywordName, NULL )) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // Add in size of Keyword=\r\n
            //

            InsertAmount += (1 + 2) * sizeof( UCHAR );
            InsertAmount += AnsiKeywordName->Length;
        }

        InsertKeywordName = TRUE;
    } else {
        if (a->IniFile->UnicodeFile) {
            if (a->Operation == DeleteKey) {
                DeleteLength = BaseDllCalculateDeleteLength( a );
                return BaseDllModifyMappedFile( a,
                                                a->UnicodeKeywordName->Buffer,
                                                DeleteLength,
                                                NULL,
                                                0
                                              );
            } else {
                AddressInFile = a->UnicodeKeywordValue->Buffer;
            }
        } else {
            if (a->Operation == DeleteKey) {
                DeleteLength = BaseDllCalculateDeleteLength( a );
                return BaseDllModifyMappedFile( a,
                                                a->AnsiKeywordName->Buffer,
                                                DeleteLength,
                                                NULL,
                                                0
                                              );
            } else {
                AddressInFile = a->AnsiKeywordValue->Buffer;
            }
        }
        InsertKeywordName = FALSE;
    }

    if (a->IniFile->UnicodeFile) {
        if (!BaseDllGetVariableValue( a, NULL, &UnicodeKeywordValue, &ValueLength )) {
            return STATUS_INVALID_PARAMETER;
        }
        ValueLength -= sizeof( WCHAR );

        if (InsertAmount == 0) {
            return BaseDllModifyMappedFile( a,
                                            a->UnicodeKeywordValue->Buffer,
                                            a->UnicodeKeywordValue->Length,
                                            UnicodeKeywordValue,
                                            ValueLength
                                          );
        }

        //
        // Add in size of value
        //

        InsertAmount += ValueLength;
    } else {
        if (!BaseDllGetVariableValue( a, &AnsiKeywordValue, NULL, &ValueLength )) {
            return STATUS_INVALID_PARAMETER;
        }
        ValueLength -= sizeof( UCHAR );

        if (InsertAmount == 0) {
            return BaseDllModifyMappedFile( a,
                                            a->AnsiKeywordValue->Buffer,
                                            a->AnsiKeywordValue->Length,
                                            AnsiKeywordValue,
                                            ValueLength
                                          );
        }

        //
        // Add in size of value
        //

        InsertAmount += ValueLength;
    }

    InsertBuffer = AllocateKernelPool( InsertAmount + sizeof(UNICODE_NULL) );
    if (InsertBuffer == NULL) {
        return STATUS_NO_MEMORY;
    }

    if (a->IniFile->UnicodeFile) {
        PWSTR Src, Dst;

        Dst = InsertBuffer;
        if (InsertSectionName) {
            *Dst++ = L'[';
            Src = UnicodeSectionName->Buffer;
            n = UnicodeSectionName->Length / sizeof( WCHAR );
            while (n--) {
                *Dst++ = *Src++;
            }
            *Dst++ = L']';
            *Dst++ = L'\r';
            *Dst++ = L'\n';
        }

        if (InsertKeywordName) {
            Src = UnicodeKeywordName->Buffer;
            n = UnicodeKeywordName->Length / sizeof( WCHAR );
            while (n--) {
                *Dst++ = *Src++;
            }
            *Dst++ = L'=';
        }

        Src = UnicodeKeywordValue;
        n = ValueLength / sizeof( WCHAR );
        while (n--) {
            *Dst++ = *Src++;
        }

        if (InsertKeywordName) {
            *Dst++ = L'\r';
            *Dst++ = L'\n';
        }
    } else {
        PBYTE Src, Dst;

        Dst = InsertBuffer;
        if (InsertSectionName) {
            *Dst++ = '[';
            Src = AnsiSectionName->Buffer;
            n = AnsiSectionName->Length;
            while (n--) {
                *Dst++ = *Src++;
            }
            *Dst++ = ']';
            *Dst++ = '\r';
            *Dst++ = '\n';
        }

        if (InsertKeywordName) {
            Src = AnsiKeywordName->Buffer;
            n = AnsiKeywordName->Length;
            while (n--) {
                *Dst++ = *Src++;
            }
            *Dst++ = '=';
        }

        Src = AnsiKeywordValue;
        n = ValueLength;
        while (n--) {
            *Dst++ = *Src++;
        }

        if (InsertKeywordName) {
            *Dst++ = '\r';
            *Dst++ = '\n';
        }
    }

    Status = BaseDllModifyMappedFile( a,
                                      AddressInFile,
                                      0,
                                      InsertBuffer,
                                      InsertAmount
                                    );
    FreeKernelPool( InsertBuffer );
    return Status;
}


NTSTATUS
BaseDllWriteSection(
                   IN PINIFILE_PARAMETERS a
                   )
{
    NTSTATUS Status;
    BOOLEAN InsertSectionName;
    PVOID InsertBuffer;
    ULONG InsertAmount, n;
    PANSI_STRING AnsiSectionName;
    PUNICODE_STRING UnicodeSectionName;
    PBYTE AnsiKeywordValue, s;
    PWSTR UnicodeKeywordValue, w;
    ULONG ValueLength, SizeToRemove = 0;
    PVOID AddressInFile;

    InsertAmount = 0;
    Status = BaseDllFindSection( a );
    if (!NT_SUCCESS( Status )) {
        if (a->Operation == DeleteSection) {
            return STATUS_SUCCESS;
        }

        AddressInFile = a->TextEnd;
        if (a->IniFile->UnicodeFile) {
            if (!BaseDllGetApplicationName( a, NULL, &UnicodeSectionName )) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // Add in size of [SectionName]\r\n
            //

            InsertAmount += (1 + 1 + 2) * sizeof( WCHAR );
            InsertAmount += UnicodeSectionName->Length;
        } else {
            if (!BaseDllGetApplicationName( a, &AnsiSectionName, NULL )) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // Add in size of [SectionName]\r\n
            //

            InsertAmount += (1 + 1 + 2) * sizeof( UCHAR );
            InsertAmount += AnsiSectionName->Length;
        }

        InsertSectionName = TRUE;
        SizeToRemove = 0;
    } else {
        if (a->Operation == DeleteSection) {
            AddressInFile = a->TextStart;
        } else {
            AddressInFile = a->TextCurrent;
        }
        while (TRUE) {
            //
            // For delete operations need to iterate all lines in section,
            // not just those that have an = on them. Otherwise sections like
            // [foo]
            // a
            // b = c
            // d
            //
            // don't get deleted properly.
            //
            Status = BaseDllAdvanceTextPointer(
                                              a,
                                              (a->Operation == DeleteSection) ? STOP_AT_NONSECTION : STOP_AT_KEYWORD
                                              );

            if (Status == STATUS_MORE_ENTRIES) {
            } else
                if (Status == STATUS_NO_MORE_ENTRIES) {
                SizeToRemove = (ULONG)((PCHAR)a->TextCurrent - (PCHAR)AddressInFile);
                break;
            } else {
                return Status;
            }
        }

        InsertSectionName = FALSE;
    }

    if (a->Operation == DeleteSection) {
        InsertBuffer = NULL;
    } else {
        if (a->IniFile->UnicodeFile) {
            if (!BaseDllGetVariableValue( a, NULL, &UnicodeKeywordValue, &ValueLength )) {
                return STATUS_INVALID_PARAMETER;
            }
            ValueLength -= sizeof( WCHAR );

            //
            // Add in size of value, + \r\n for each line
            //

            w = UnicodeKeywordValue;
            InsertAmount += ValueLength;
            while (*w) {
                while (*w++) {
                }
                InsertAmount += (2-1) * sizeof( WCHAR );    // Subtract out NULL byte already in ValueLength
            }
        } else {
            if (!BaseDllGetVariableValue( a, &AnsiKeywordValue, NULL, &ValueLength )) {
                return STATUS_INVALID_PARAMETER;
            }
            ValueLength -= sizeof( UCHAR );

            //
            // Add in size of value, + \r\n for each line
            //

            s = AnsiKeywordValue;
            InsertAmount += ValueLength;
            while (*s) {
                while (*s++) {
                }
                InsertAmount += 2 - 1;      // Subtract out NULL byte already in ValueLength
            }
        }

        InsertBuffer = AllocateKernelPool( InsertAmount + sizeof(UNICODE_NULL) );
        if (InsertBuffer == NULL) {
            return STATUS_NO_MEMORY;
        }

        if (a->IniFile->UnicodeFile) {
            PWSTR Src, Dst;

            Dst = InsertBuffer;
            if (InsertSectionName) {
                *Dst++ = L'[';
                Src = UnicodeSectionName->Buffer;
                n = UnicodeSectionName->Length / sizeof( WCHAR );
                while (n--) {
                    *Dst++ = *Src++;
                }
                *Dst++ = L']';
                *Dst++ = L'\r';
                *Dst++ = L'\n';
            }

            Src = UnicodeKeywordValue;
            while (*Src) {
                while ( 0 != (*Dst = *Src++) ) {
                    Dst += 1;
                }

                *Dst++ = L'\r';
                *Dst++ = L'\n';
            }
        } else {
            PBYTE Src, Dst;

            Dst = InsertBuffer;
            if (InsertSectionName) {
                *Dst++ = '[';
                Src = AnsiSectionName->Buffer;
                n = AnsiSectionName->Length;
                while (n--) {
                    *Dst++ = *Src++;
                }
                *Dst++ = ']';
                *Dst++ = '\r';
                *Dst++ = '\n';
            }

            Src = AnsiKeywordValue;
            while (*Src) {
                while ( 0 != (*Dst = *Src++) ) {
                    Dst += 1;
                }

                *Dst++ = '\r';
                *Dst++ = '\n';
            }
        }
    }

    Status = BaseDllModifyMappedFile( a,
                                      AddressInFile,
                                      SizeToRemove,
                                      InsertBuffer,
                                      InsertAmount
                                    );
    FreeKernelPool( InsertBuffer );
    return Status;
}


NTSTATUS
BaseDllCloseIniFileOnDisk(
                         IN PINIFILE_PARAMETERS a
                         )
{
    PINIFILE_CACHE IniFile;
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    ULONG UpdateLength;
    LARGE_INTEGER ByteOffset, Length;

    Status = STATUS_SUCCESS;
    IniFile = a->IniFile;
    if (IniFile != NULL) {
        if (IniFile->BaseAddress != NULL) {
            if (IniFile->UpdateOffset != 0xFFFFFFFF && IniFile->WriteAccess) {
                ByteOffset.HighPart = 0;
                ByteOffset.LowPart = IniFile->UpdateOffset;
                UpdateLength = IniFile->UpdateEndOffset - IniFile->UpdateOffset;
                Status = NtWriteFile( IniFile->FileHandle,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &IoStatusBlock,
                                      (PCHAR)(IniFile->BaseAddress) + IniFile->UpdateOffset,
                                      UpdateLength,
                                      &ByteOffset
                                    );
                if (NT_SUCCESS( Status )) {
                    if (IoStatusBlock.Information != UpdateLength) {
                        Status = STATUS_DISK_FULL;
                    } else {
                        Length.QuadPart = IniFile->EndOfFile;
                        Status = NtSetInformationFile( IniFile->FileHandle,
                                                       &IoStatusBlock,
                                                       &Length,
                                                       sizeof( Length ),
                                                       FileEndOfFileInformation
                                                     );
                    }
                }

                if (!NT_SUCCESS( Status )) {
                    KdPrint(( "BASEDLL: Unable to write changes for %wZ to disk - Status == %x\n",
                              &IniFile->NtFileName,
                              Status
                            ));
                }
            }

            NtFreeVirtualMemory( &IniFile->BaseAddress,
                                 &IniFile->RegionSize,
                                 MEM_RELEASE
                               );
            IniFile->BaseAddress = NULL;
            IniFile->CommitSize = 0;
            IniFile->RegionSize = 0;
        }

        if (IniFile->FileHandle != NULL) {

            Status = NtClose( IniFile->FileHandle );
            IniFile->FileHandle = NULL;
        }

        FreeKernelPool( IniFile );
    }

    return Status;
}


NTSTATUS
BaseDllReadWriteIniFileOnDisk(
                             IN PINIFILE_PARAMETERS a
                             )
{
    NTSTATUS Status;
    ULONG PartialResultChars = 0;

    if (!a->WriteOperation) {
        PartialResultChars = a->ResultChars;
    }

    Status = BaseDllOpenIniFileOnDisk( a );
    if (NT_SUCCESS( Status )) {
        try {
            a->TextEnd = (PCHAR)a->IniFile->BaseAddress + a->IniFile->EndOfFile;
            a->TextCurrent = a->IniFile->BaseAddress;
            if (a->IniFile->UnicodeFile &&
                ((*(PWCHAR)a->TextCurrent == BYTE_ORDER_MARK) ||
                 (*(PWCHAR)a->TextCurrent == REVERSE_BYTE_ORDER_MARK)))
            {
                // Skip past the BOM.
                ((PWCHAR)a->TextCurrent)++;
            }

            if (a->Operation == ReadSectionNames) {
                Status = BaseDllReadSectionNames( a );
            } else
                if (a->Operation == ReadKeyValue) {
                Status = BaseDllReadKeywordValue( a );
            } else
                if (a->Operation == ReadKeyNames) {
                Status = BaseDllReadKeywordNames( a );
            } else
                if (a->Operation == ReadSection) {
                Status = BaseDllReadSection( a );
            } else
                if (a->Operation == WriteKeyValue || a->Operation == DeleteKey) {
                Status = BaseDllWriteKeywordValue( a, NULL );
            } else
                if (a->Operation == WriteSection || a->Operation == DeleteSection) {
                Status = BaseDllWriteSection( a );
            } else {
                Status = STATUS_INVALID_PARAMETER;
            }
        }
        finally {
            BaseDllCloseIniFileOnDisk( a );
        }
    }

    if (Status == STATUS_OBJECT_NAME_NOT_FOUND &&
        !a->WriteOperation &&
        PartialResultChars != 0
       ) {
        Status = STATUS_SUCCESS;
    }

    return Status;
}


NTSTATUS
BaseDllReadWriteIniFile(
                       IN BOOLEAN Unicode,
                       IN BOOLEAN WriteOperation,
                       IN BOOLEAN SectionOperation,
                       IN PVOID FileName OPTIONAL,
                       IN PVOID ApplicationName OPTIONAL,
                       IN PVOID VariableName OPTIONAL,
                       IN OUT PVOID VariableValue OPTIONAL,
                       IN OUT PULONG VariableValueLength OPTIONAL
                       )
{
    BOOLEAN MultiValueStrings;
    INIFILE_OPERATION Operation;
    PINIFILE_PARAMETERS a;
    NTSTATUS Status;

    if (SectionOperation) {
        VariableName = NULL;
    }

    MultiValueStrings = FALSE;
    if (WriteOperation) {
        if (ARGUMENT_PRESENT( ApplicationName )) {
            if (ARGUMENT_PRESENT( VariableName )) {
                if (ARGUMENT_PRESENT( VariableValue )) {
                    Operation = WriteKeyValue;
                } else {
                    Operation = DeleteKey;
                }
            } else {
                if (ARGUMENT_PRESENT( VariableValue )) {
                    Operation = WriteSection;
                    MultiValueStrings = TRUE;
                } else {
                    Operation = DeleteSection;
                }
            }
        } else {
#if DBG
            if (ARGUMENT_PRESENT( VariableName ) ||
                ARGUMENT_PRESENT( VariableValue )
               ) {
                return STATUS_INVALID_PARAMETER;
            } else
#endif
                if (ARGUMENT_PRESENT( FileName )) {
                Operation = RefreshIniFileMapping;
            } else {
                Operation = FlushProfiles;
            }
        }
    } else {
        if (ARGUMENT_PRESENT( ApplicationName )) {
            if (!ARGUMENT_PRESENT( VariableValue )) {
                return STATUS_INVALID_PARAMETER;
            } else
                if (ARGUMENT_PRESENT( VariableName )) {
                Operation = ReadKeyValue;
            } else
                if (SectionOperation) {
                Operation = ReadSection;
                MultiValueStrings = TRUE;
            } else {
                Operation = ReadKeyNames;
                MultiValueStrings = TRUE;
            }
        } else
            if (SectionOperation || !ARGUMENT_PRESENT( VariableValue )) {
            return STATUS_INVALID_PARAMETER;
        } else {
            VariableName = NULL;
            Operation = ReadSectionNames;
            MultiValueStrings = TRUE;
        }
    }

#if DBG
    if (WriteOperation) {
        ASSERT( Operation == WriteKeyValue || Operation == WriteSection || Operation == DeleteKey || Operation == DeleteSection || Operation == FlushProfiles || Operation == RefreshIniFileMapping );
    } else {
        ASSERT( Operation == ReadKeyValue || Operation == ReadKeyNames || Operation == ReadSectionNames || Operation == ReadSection );
    }
#endif

    Status = BaseDllCaptureIniFileParameters( Unicode,
                                              Operation,
                                              WriteOperation,
                                              MultiValueStrings,
                                              FileName,
                                              ApplicationName,
                                              VariableName,
                                              VariableValue,
                                              VariableValueLength,
                                              &a
                                            );
    if (!NT_SUCCESS( Status )) {
        return Status;
    }

#if DBG
    if (BaseDllDumpIniCalls) {
        DbgPrint( "BASEDLL: called with profile operation\n" );
        DbgPrint( "    Operation: %s  Write: %u\n", xOperationNames[ a->Operation ], a->WriteOperation );
        DbgPrint( "    FileName: %wZ\n", &a->FileName );
        DbgPrint( "    NtFileName: %wZ\n", &a->NtFileName );
        DbgPrint( "    ApplicationName: %wZ (%Z)\n", &a->ApplicationNameU, &a->ApplicationName );
        DbgPrint( "    VariableName: %wZ (%Z)\n", &a->VariableNameU, &a->VariableName );
        if (a->WriteOperation) {
            DbgPrint( "    VariableValue: %ws (%s)\n", a->ValueBufferU, a->ValueBuffer );
        }
    }
#endif // DBG

    Status = BaseDllReadWriteIniFileOnDisk( a );
#if DBG
    if (BaseDllDumpIniCalls) {
        if (NT_SUCCESS( Status ) ||
            Status == STATUS_BUFFER_OVERFLOW
           ) {
            if (!a->WriteOperation) {
                if (a->Unicode) {
                    if (a->Operation == ReadKeyValue) {
                        DbgPrint( "BASEDLL: Returning value from disk - '%.*ws' (%u)\n", a->ResultChars, a->ResultBufferU, a->ResultChars );
                    } else {
                        PWSTR s;

                        DbgPrint( "BASEDLL: Return multi-value from disk: (%u)\n", a->ResultChars );
                        s = a->ResultBufferU;
                        s[ a->ResultChars ] = UNICODE_NULL;
                        while (*s) {
                            DbgPrint( "    %ws\n", s );
                            while (*s++) {
                            }
                        }
                    }
                } else {
                    if (a->Operation == ReadKeyValue) {
                        DbgPrint( "BASEDLL: Returning value from disk - '%.*s' (%u)\n", a->ResultChars, a->ResultBuffer, a->ResultChars );
                    } else {
                        PBYTE s;

                        DbgPrint( "BASEDLL: Return multi-value from disk: (%u)\n", a->ResultChars );
                        s = a->ResultBuffer;
                        s[ a->ResultChars ] = '\0';
                        while (*s) {
                            DbgPrint( "    (%s)\n", s );
                            while (*s++) {
                            }
                        }
                    }
                }

                if (Status == STATUS_BUFFER_OVERFLOW) {
                    DbgPrint( "    *** above result partial as buffer too small.\n" );
                }
            } else {
                DbgPrint( "BASEDLL: Returning success for above write operation.\n" );
            }
        } else {
            DbgPrint( "BASEDLL: Profile operation %s failed: Status == %x\n", xOperationNames[ a->Operation ], Status );
        }
    }
#endif // DBG

    //if (BaseRunningInServerProcess || a->Operation == FlushProfiles) {
        //BaseDllFlushRegistryCache();
    //}

    if (NT_SUCCESS( Status )) {
        if (a->Operation == ReadSectionNames ||
            a->Operation == ReadKeyNames ||
            a->Operation == ReadSection
           ) {
            BaseDllAppendNullToResultBuffer( a );
        }
    }

    if (NT_SUCCESS( Status ) || Status == STATUS_BUFFER_OVERFLOW) {
        if (a->WriteOperation) {
            BaseIniFileUpdateCount++;
        } else
            if (ARGUMENT_PRESENT( VariableValueLength )) {
            *VariableValueLength = a->ResultChars;
        }
    }

    if (a->ValueBufferAllocated) {
        if (a->Unicode) {
            FreeKernelPool( a->ValueBufferU );
        } else {
            FreeKernelPool( a->ValueBuffer );
        }
    }

    FreeKernelPool( a );

    return Status;
}


DWORD
WINAPI
GetPrivateProfileStringA(
                        LPCSTR lpAppName,
                        LPCSTR lpKeyName,
                        LPCSTR lpDefault,
                        LPSTR lpReturnedString,
                        DWORD nSize,
                        LPCSTR lpFileName
                        )
{
    NTSTATUS Status;
    ULONG n;

    if (lpDefault == NULL) {
        lpDefault = "";
    }

    n = nSize;
    Status = BaseDllReadWriteIniFile( FALSE,    // Unicode,
                                      FALSE,    // WriteOperation
                                      FALSE,    // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      (PVOID)lpKeyName,
                                      (PVOID)lpReturnedString,
                                      &n
                                    );
    if (NT_SUCCESS( Status ) || Status == STATUS_BUFFER_OVERFLOW) {
        if (NT_SUCCESS( Status )) {
            SetLastError( NO_ERROR );
            n--;
        } else
            if (!lpAppName || !lpKeyName) {
            if (nSize >= 2) {
                n = nSize - 2;
                lpReturnedString[ n+1 ] = '\0';
                //
                // GetPrivateProfileString(): don't leave 1st byte of double byte char alone
                //
                lpReturnedString[ n ] = '\0';
                if ( n > 0 ) {
                    LPSTR pc = lpReturnedString;
                    LPSTR pcEnd = lpReturnedString + n - 1;
                    //
                    // if the last character is the 1st byte of
                    // double byte character, erase it.
                    //
                    while ( pc <= pcEnd ) {
                        //pc += IsDBCSLeadByte( *pc ) ? 2 : 1;
                        pc += 1;
                    }
                    if ( (pc - pcEnd ) == 2 ) {
                        *pcEnd = '\0';
                    }
                }

                return ( n );
            } else {
                n = 0;
            }
        } else {
            if (nSize >= 1) {
                n = nSize - 1;
            } else {
                n = 0;
            }
        }
    } else {
        n = strlen( lpDefault );
        while (n > 0 && lpDefault[n-1] == ' ') {
            n -= 1;
        }

        if (n >= nSize) {
            n = nSize;
        }

        strncpy( lpReturnedString, lpDefault, n );
    }

    if (n < nSize) {
        lpReturnedString[ n ] = '\0';
    } else
        if (nSize > 0) {
        lpReturnedString[ nSize-1 ] = '\0';
    }

    return( n );
}


DWORD
WINAPI
GetPrivateProfileSectionNamesA(
                              LPSTR lpszReturnBuffer,
                              DWORD nSize,
                              LPCSTR lpFileName
                              )
{
    return GetPrivateProfileStringA( NULL,
                                     NULL,
                                     NULL,
                                     lpszReturnBuffer,
                                     nSize,
                                     lpFileName
                                   );
}


DWORD
WINAPI
GetPrivateProfileStringW(
                        LPCWSTR lpAppName,
                        LPCWSTR lpKeyName,
                        LPCWSTR lpDefault,
                        LPWSTR lpReturnedString,
                        DWORD nSize,
                        LPCWSTR lpFileName
                        )
{
    NTSTATUS Status;
    ULONG n;

    if (lpDefault == NULL) {
        lpDefault = L"";
    }

    n = nSize;
    Status = BaseDllReadWriteIniFile( TRUE,     // Unicode,
                                      FALSE,    // WriteOperation
                                      FALSE,    // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      (PVOID)lpKeyName,
                                      (PVOID)lpReturnedString,
                                      &n
                                    );
    if (NT_SUCCESS( Status ) || Status == STATUS_BUFFER_OVERFLOW) {
        if (NT_SUCCESS( Status )) {
            SetLastError( NO_ERROR );
            n--;
        } else
            if (!lpAppName || !lpKeyName) {
            if (nSize >= 2) {
                n = nSize - 2;
                lpReturnedString[ n+1 ] = UNICODE_NULL;
            } else {
                n = 0;
            }
        } else {
            if (nSize >= 1) {
                n = nSize - 1;
            } else {
                n = 0;
            }
        }
    } else {
        n = wcslen( lpDefault );
        while (n > 0 && lpDefault[n-1] == L' ') {
            n -= 1;
        }

        if (n >= nSize) {
            n = nSize;
        }

        wcsncpy( lpReturnedString, lpDefault, n );
    }

    if (n < nSize) {
        lpReturnedString[ n ] = UNICODE_NULL;
    } else
        if (nSize > 0) {
        lpReturnedString[ nSize-1 ] = UNICODE_NULL;
    }

    return( n );
}


DWORD
WINAPI
GetPrivateProfileSectionNamesW(
                              LPWSTR lpszReturnBuffer,
                              DWORD nSize,
                              LPCWSTR lpFileName
                              )
{
    return GetPrivateProfileStringW( NULL,
                                     NULL,
                                     NULL,
                                     lpszReturnBuffer,
                                     nSize,
                                     lpFileName
                                   );
}

#define NibbleToChar(x) (N2C[x])
#define CharToNibble(x) ((x)>='0'&&(x)<='9' ? (x)-'0' : ((10+(x)-'A')&0x000f))
char N2C[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

BOOL
WINAPI
GetPrivateProfileStructA(
                        LPCSTR lpszSection,
                        LPCSTR lpszKey,
                        LPVOID lpStruct,
                        UINT uSizeStruct,
                        LPCSTR szFile
                        )
{
    UCHAR szBuf[256];
    LPSTR lpBuf, lpBufTemp, lpFreeBuffer;
    UINT nLen;
    BYTE checksum;
    BOOL Result;

    nLen = uSizeStruct*2 + 10;
    if (nLen < uSizeStruct) {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (nLen > sizeof( szBuf )) {
        lpFreeBuffer = (LPSTR)AllocateKernelPool( nLen );
        if (!lpFreeBuffer) {
            return FALSE;
        }
        lpBuf = lpFreeBuffer;
    } else {
        lpFreeBuffer = NULL;
        lpBuf = (LPSTR)szBuf;
    }

    Result = FALSE;
    nLen = GetPrivateProfileStringA( lpszSection,
                                     lpszKey,
                                     NULL,
                                     lpBuf,
                                     nLen,
                                     szFile
                                   );

    if (nLen == uSizeStruct*2+2) {
        // Room for the one byte check sum
        uSizeStruct+=1;
        checksum = 0;
        for (lpBufTemp=lpBuf; uSizeStruct!=0; --uSizeStruct) {
            BYTE bStruct;
            BYTE cTemp;

            cTemp = *lpBufTemp++;
            bStruct = (BYTE)CharToNibble(cTemp);
            cTemp = *lpBufTemp++;
            bStruct = (BYTE)((bStruct<<4) | CharToNibble(cTemp));

            if (uSizeStruct == 1) {
                if (checksum == bStruct) {
                    Result = TRUE;
                } else {
                    SetLastError( ERROR_INVALID_DATA );
                }

                break;
            }

            checksum = (BYTE)(checksum + bStruct);
            *((LPBYTE)lpStruct)++ = bStruct;
        }
    } else {
        SetLastError( ERROR_BAD_LENGTH );
    }

    if (lpFreeBuffer) {
        FreeKernelPool( lpFreeBuffer );
    }

    return Result;
}

BOOL
WINAPI
GetPrivateProfileStructW(
                        LPCWSTR lpszSection,
                        LPCWSTR lpszKey,
                        LPVOID   lpStruct,
                        UINT     uSizeStruct,
                        LPCWSTR szFile
                        )
{
    WCHAR szBuf[256];
    PWSTR lpBuf, lpBufTemp, lpFreeBuffer;
    UINT nLen;
    BYTE checksum;
    BOOL Result;

    nLen = uSizeStruct*2 + 10;
    if (nLen < uSizeStruct) {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if ((nLen * sizeof( WCHAR )) > sizeof( szBuf )) {
        lpFreeBuffer = (PWSTR)AllocateKernelPool( nLen * sizeof(WCHAR) );
        if (!lpFreeBuffer) {
            return FALSE;
        }
        lpBuf = lpFreeBuffer;
    } else {
        lpFreeBuffer = NULL;
        lpBuf = (PWSTR)szBuf;
    }

    Result = FALSE;
    nLen = GetPrivateProfileStringW( lpszSection,
                                     lpszKey,
                                     NULL,
                                     lpBuf,
                                     nLen,
                                     szFile
                                   );

    if (nLen == uSizeStruct*2+2) {
        // Room for the one byte check sum
        uSizeStruct+=1;
        checksum = 0;
        for (lpBufTemp=lpBuf; uSizeStruct!=0; --uSizeStruct) {
            BYTE bStruct;
            WCHAR cTemp;

            cTemp = *lpBufTemp++;
            bStruct = (BYTE)CharToNibble(cTemp);
            cTemp = *lpBufTemp++;
            bStruct = (BYTE)((bStruct<<4) | CharToNibble(cTemp));

            if (uSizeStruct == 1) {
                if (checksum == bStruct) {
                    Result = TRUE;
                } else {
                    SetLastError( ERROR_INVALID_DATA );
                }

                break;
            }

            checksum = (BYTE)(checksum + bStruct);
            *((LPBYTE)lpStruct)++ = bStruct;
        }
    } else {
        SetLastError( ERROR_BAD_LENGTH );
    }

    if (lpFreeBuffer) {
        FreeKernelPool( lpFreeBuffer );
    }

    return Result;
}


BOOL
WINAPI
WritePrivateProfileStringA(
                          LPCSTR lpAppName,
                          LPCSTR lpKeyName,
                          LPCSTR lpString,
                          LPCSTR lpFileName
                          )
{
    NTSTATUS Status;

    Status = BaseDllReadWriteIniFile( FALSE,    // Unicode,
                                      TRUE,     // WriteOperation
                                      FALSE,    // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      (PVOID)lpKeyName,
                                      (PVOID)(lpKeyName == NULL ? NULL : lpString),
                                      NULL
                                    );
    if (NT_SUCCESS( Status )) {
        return( TRUE );
    } else {
        if (Status == STATUS_INVALID_IMAGE_FORMAT) {
            SetLastError( ERROR_INVALID_DATA );
        } else {
            SetLastError( RtlNtStatusToDosError(Status) );
        }
        return( FALSE );
    }
}


BOOL
WINAPI
WritePrivateProfileStructA(
                          LPCSTR lpszSection,
                          LPCSTR lpszKey,
                          LPVOID lpStruct,
                          UINT   uSizeStruct,
                          LPCSTR szFile
                          )
{
    UCHAR szBuf[256];
    LPSTR lpBuf, lpBufTemp, lpFreeBuffer;
    UINT nLen;
    BOOL Result;
    BYTE checksum;

    if (lpStruct == NULL) {
        return WritePrivateProfileStringA( lpszSection,
                                           lpszKey,
                                           NULL,
                                           szFile
                                         );
    }


    nLen = uSizeStruct*2 + 3;
    if (nLen < uSizeStruct) {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (nLen > sizeof( szBuf )) {
        lpFreeBuffer = (LPSTR)AllocateKernelPool( nLen );
        if (!lpFreeBuffer) {
            return FALSE;
        }
        lpBuf = lpFreeBuffer;
    } else {
        lpFreeBuffer = NULL;
        lpBuf = (LPSTR)szBuf;
    }

    checksum = 0;
    for (lpBufTemp=lpBuf; uSizeStruct != 0; --uSizeStruct) {
        BYTE bStruct;

        bStruct = *((LPBYTE)lpStruct)++;
        checksum = (BYTE)(checksum + bStruct);

        *lpBufTemp++ = NibbleToChar((bStruct>>4)&0x000f);
        *lpBufTemp++ = NibbleToChar(bStruct&0x000f);
    }
    *lpBufTemp++ = NibbleToChar((checksum>>4)&0x000f);
    *lpBufTemp++ = NibbleToChar(checksum&0x000f);
    *lpBufTemp = '\0';

    Result = WritePrivateProfileStringA( lpszSection,
                                         lpszKey,
                                         lpBuf,
                                         szFile
                                       );

    if (lpFreeBuffer) {
        FreeKernelPool( lpFreeBuffer );
    }

    return Result;
}


BOOL
WINAPI
WritePrivateProfileStringW(
                          LPCWSTR lpAppName,
                          LPCWSTR lpKeyName,
                          LPCWSTR lpString,
                          LPCWSTR lpFileName
                          )
{
    NTSTATUS Status;

    Status = BaseDllReadWriteIniFile( TRUE,     // Unicode,
                                      TRUE,     // WriteOperation
                                      FALSE,    // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      (PVOID)lpKeyName,
                                      (PVOID)(lpKeyName == NULL ? NULL : lpString),
                                      NULL
                                    );
    if (NT_SUCCESS( Status )) {
        return( TRUE );
    } else {
        if (Status == STATUS_INVALID_IMAGE_FORMAT) {
            SetLastError( ERROR_INVALID_DATA );
        } else {
            SetLastError( RtlNtStatusToDosError(Status) );
        }
        return( FALSE );
    }
}


BOOL
WINAPI
WritePrivateProfileStructW(
                          LPCWSTR lpszSection,
                          LPCWSTR lpszKey,
                          LPVOID  lpStruct,
                          UINT    uSizeStruct,
                          LPCWSTR szFile
                          )
{
    WCHAR szBuf[256];
    PWSTR lpBuf, lpBufTemp, lpFreeBuffer;
    UINT nLen;
    BOOL Result;
    BYTE checksum;

    if (lpStruct == NULL) {
        return WritePrivateProfileStringW( lpszSection,
                                           lpszKey,
                                           NULL,
                                           szFile
                                         );
    }


    nLen = uSizeStruct*2 + 3;
    if (nLen < uSizeStruct) {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if ((nLen * sizeof( WCHAR )) > sizeof( szBuf )) {
        lpFreeBuffer = (PWSTR)AllocateKernelPool( nLen * sizeof(WCHAR) );
        if (!lpFreeBuffer) {
            return FALSE;
        }
        lpBuf = lpFreeBuffer;
    } else {
        lpFreeBuffer = NULL;
        lpBuf = (PWSTR)szBuf;
    }

    checksum = 0;
    for (lpBufTemp=lpBuf; uSizeStruct != 0; --uSizeStruct) {
        BYTE bStruct;

        bStruct = *((LPBYTE)lpStruct)++;
        checksum = (BYTE)(checksum + bStruct);

        *lpBufTemp++ = (WCHAR)NibbleToChar((bStruct>>4)&0x000f);
        *lpBufTemp++ = (WCHAR)NibbleToChar(bStruct&0x000f);
    }
    *lpBufTemp++ = (WCHAR)NibbleToChar((checksum>>4)&0x000f);
    *lpBufTemp++ = (WCHAR)NibbleToChar(checksum&0x000f);
    *lpBufTemp = L'\0';

    Result = WritePrivateProfileStringW( lpszSection,
                                         lpszKey,
                                         lpBuf,
                                         szFile
                                       );

    if (lpFreeBuffer) {
        FreeKernelPool( lpFreeBuffer );
    }

    return Result;
}


UINT
WINAPI
GetPrivateProfileIntA(
                     LPCSTR lpAppName,
                     LPCSTR lpKeyName,
                     INT nDefault,
                     LPCSTR lpFileName
                     )
{
    NTSTATUS Status;
    ULONG ReturnValue;
    UCHAR ValueBuffer[ 256 ];
    ULONG cb;

    ReturnValue = 0;
    cb = GetPrivateProfileStringA( lpAppName,
                                   lpKeyName,
                                   NULL,
                                   ValueBuffer,
                                   sizeof( ValueBuffer ),
                                   lpFileName
                                 );
    if (cb == 0) {
        ReturnValue = nDefault;
    } else {
        Status = RtlCharToInteger( ValueBuffer, 0, &ReturnValue );
        if (!NT_SUCCESS( Status )) {
            SetLastError( RtlNtStatusToDosError(Status) );
        } else {
            SetLastError( NO_ERROR );
        }
    }

    return ReturnValue;
}


UINT
WINAPI
GetProfileIntA(
              LPCSTR lpAppName,
              LPCSTR lpKeyName,
              INT nDefault
              )
{
    return( GetPrivateProfileIntA( lpAppName,
                                   lpKeyName,
                                   nDefault,
                                   NULL
                                 )
          );
}

DWORD
WINAPI
GetProfileStringA(
                 LPCSTR lpAppName,
                 LPCSTR lpKeyName,
                 LPCSTR lpDefault,
                 LPSTR lpReturnedString,
                 DWORD nSize
                 )
{
    return( GetPrivateProfileStringA( lpAppName,
                                      lpKeyName,
                                      lpDefault,
                                      lpReturnedString,
                                      nSize,
                                      NULL
                                    )
          );
}

BOOL
WINAPI
WriteProfileStringA(
                   LPCSTR lpAppName,
                   LPCSTR lpKeyName,
                   LPCSTR lpString
                   )
{
    return( WritePrivateProfileStringA( lpAppName,
                                        lpKeyName,
                                        lpString,
                                        NULL
                                      )
          );
}


DWORD
WINAPI
GetPrivateProfileSectionA(
                         LPCSTR lpAppName,
                         LPSTR lpReturnedString,
                         DWORD nSize,
                         LPCSTR lpFileName
                         )
{
    NTSTATUS Status;
    ULONG n;

    n = nSize;
    Status = BaseDllReadWriteIniFile( FALSE,    // Unicode,
                                      FALSE,    // WriteOperation
                                      TRUE,     // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      NULL,
                                      (PVOID)lpReturnedString,
                                      &n
                                    );
    if (NT_SUCCESS( Status ) || Status == STATUS_BUFFER_OVERFLOW) {
        if (NT_SUCCESS( Status )) {
            SetLastError( NO_ERROR );
            n--;
        } else
            if (nSize >= 2) {
            n = nSize - 2;
            lpReturnedString[ n+1 ] = '\0';
        } else {
            n = 0;
        }
    } else {
        if (Status == STATUS_INVALID_IMAGE_FORMAT) {
            SetLastError( ERROR_INVALID_DATA );
        } else {
            SetLastError( RtlNtStatusToDosError(Status) );
        }
        n = 0;
    }

    if (n < nSize) {
        lpReturnedString[ n ] = '\0';
    } else
        if (nSize > 0) {
        lpReturnedString[ nSize-1 ] = '\0';
    }

    return( n );
}


DWORD
WINAPI
GetProfileSectionA(
                  LPCSTR lpAppName,
                  LPSTR lpReturnedString,
                  DWORD nSize
                  )
{
    return( GetPrivateProfileSectionA( lpAppName,
                                       lpReturnedString,
                                       nSize,
                                       NULL
                                     )
          );
}


BOOL
WINAPI
WritePrivateProfileSectionA(
                           LPCSTR lpAppName,
                           LPCSTR lpString,
                           LPCSTR lpFileName
                           )
{
    NTSTATUS Status;

    Status = BaseDllReadWriteIniFile( FALSE,    // Unicode,
                                      TRUE,     // WriteOperation
                                      TRUE,     // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      NULL,
                                      (PVOID)lpString,
                                      NULL
                                    );
    if (NT_SUCCESS( Status )) {
        return( TRUE );
    } else {
        if (Status == STATUS_INVALID_IMAGE_FORMAT) {
            SetLastError( ERROR_INVALID_DATA );
        } else {
            SetLastError( RtlNtStatusToDosError(Status) );
        }
        return( FALSE );
    }
}


BOOL
WINAPI
WriteProfileSectionA(
                    LPCSTR lpAppName,
                    LPCSTR lpString
                    )
{
    return( WritePrivateProfileSectionA( lpAppName,
                                         lpString,
                                         NULL
                                       )
          );
}


UINT
WINAPI
GetPrivateProfileIntW(
                     LPCWSTR lpAppName,
                     LPCWSTR lpKeyName,
                     INT nDefault,
                     LPCWSTR lpFileName
                     )
{
    NTSTATUS Status;
    ULONG ReturnValue;
    WCHAR ValueBuffer[ 256 ];
    UNICODE_STRING Value;
    ANSI_STRING AnsiString;
    ULONG cb;

    ReturnValue = 0;
    cb = GetPrivateProfileStringW( lpAppName,
                                   lpKeyName,
                                   NULL,
                                   ValueBuffer,
                                   sizeof( ValueBuffer ) / sizeof( WCHAR ),
                                   lpFileName
                                 );
    if (cb == 0) {
        ReturnValue = nDefault;
    } else {
        Value.Buffer = ValueBuffer;
        Value.Length = (USHORT)(cb * sizeof( WCHAR ));
        Value.MaximumLength = (USHORT)((cb + 1) * sizeof( WCHAR ));
        Status = RtlUnicodeStringToAnsiString( &AnsiString,
                                               &Value,
                                               TRUE
                                             );
        if (NT_SUCCESS( Status )) {
            Status = RtlCharToInteger( AnsiString.Buffer, 0, &ReturnValue );
            RtlFreeAnsiString( &AnsiString );
        }

        if (!NT_SUCCESS( Status )) {
            SetLastError( RtlNtStatusToDosError(Status) );
        } else {
            SetLastError( NO_ERROR );
        }
    }

    return ReturnValue;
}


UINT
WINAPI
GetProfileIntW(
              LPCWSTR lpAppName,
              LPCWSTR lpKeyName,
              INT nDefault
              )
{
    return( GetPrivateProfileIntW( lpAppName,
                                   lpKeyName,
                                   nDefault,
                                   NULL
                                 )
          );
}

DWORD
WINAPI
GetProfileStringW(
                 LPCWSTR lpAppName,
                 LPCWSTR lpKeyName,
                 LPCWSTR lpDefault,
                 LPWSTR lpReturnedString,
                 DWORD nSize
                 )
{
    return( GetPrivateProfileStringW( lpAppName,
                                      lpKeyName,
                                      lpDefault,
                                      lpReturnedString,
                                      nSize,
                                      NULL
                                    )
          );
}

BOOL
WINAPI
WriteProfileStringW(
                   LPCWSTR lpAppName,
                   LPCWSTR lpKeyName,
                   LPCWSTR lpString
                   )
{
    return( WritePrivateProfileStringW( lpAppName,
                                        lpKeyName,
                                        lpString,
                                        NULL
                                      )
          );
}


DWORD
WINAPI
GetPrivateProfileSectionW(
                         LPCWSTR lpAppName,
                         LPWSTR lpReturnedString,
                         DWORD nSize,
                         LPCWSTR lpFileName
                         )
{
    NTSTATUS Status;
    ULONG n;

    n = nSize;
    Status = BaseDllReadWriteIniFile( TRUE,     // Unicode,
                                      FALSE,    // WriteOperation
                                      TRUE,     // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      NULL,
                                      (PVOID)lpReturnedString,
                                      &n
                                    );
    if (NT_SUCCESS( Status ) || Status == STATUS_BUFFER_OVERFLOW) {
        if (NT_SUCCESS( Status )) {
            SetLastError( NO_ERROR );
            n--;
        } else
            if (nSize >= 2) {
            n = nSize - 2;
            lpReturnedString[ n+1 ] = UNICODE_NULL;
        } else {
            n = 0;
        }
    } else {
        if (Status == STATUS_INVALID_IMAGE_FORMAT) {
            SetLastError( ERROR_INVALID_DATA );
        } else {
            SetLastError( RtlNtStatusToDosError(Status) );
        }
        n = 0;
    }

    if (n < nSize) {
        lpReturnedString[ n ] = UNICODE_NULL;
    } else
        if (nSize > 0) {
        lpReturnedString[ nSize-1 ] = UNICODE_NULL;
    }

    return( n );
}


DWORD
WINAPI
GetProfileSectionW(
                  LPCWSTR lpAppName,
                  LPWSTR lpReturnedString,
                  DWORD nSize
                  )
{
    return( GetPrivateProfileSectionW( lpAppName,
                                       lpReturnedString,
                                       nSize,
                                       NULL
                                     )
          );
}


BOOL
WINAPI
WritePrivateProfileSectionW(
                           LPCWSTR lpAppName,
                           LPCWSTR lpString,
                           LPCWSTR lpFileName
                           )
{
    NTSTATUS Status;

    Status = BaseDllReadWriteIniFile( TRUE,     // Unicode,
                                      TRUE,     // WriteOperation
                                      TRUE,     // SectionOperation
                                      (PVOID)lpFileName,
                                      (PVOID)lpAppName,
                                      NULL,
                                      (PVOID)lpString,
                                      NULL
                                    );
    if (NT_SUCCESS( Status )) {
        return( TRUE );
    } else {
        if (Status == STATUS_INVALID_IMAGE_FORMAT) {
            SetLastError( ERROR_INVALID_DATA );
        } else {
            SetLastError( RtlNtStatusToDosError(Status) );
        }
        return( FALSE );
    }
}


BOOL
WINAPI
WriteProfileSectionW(
                    LPCWSTR lpAppName,
                    LPCWSTR lpString
                    )
{
    return( WritePrivateProfileSectionW( lpAppName,
                                         lpString,
                                         NULL
                                       )
          );
}


BOOLEAN
NTAPI
IniRemoveStringCommentW(
    IN OUT PWSTR String
    )

/*++

Routine Description:

    This routine will remove comment (';') and tailing white space from
    string retrieved by GetProfileString, GetPrivateProfileString

Arguments:

    String - Pointer to null-terminated string, usually the result from
        GetProfileString, GetPrivateProfileString

Return Value:

    TRUE if comment has been removed, otherwise return FALSE

--*/

{
    PWSTR Ptr;
    BOOLEAN b = FALSE;

    ASSERT( String != NULL );

    try {

        Ptr = wcschr( String, L';' );
        if ( Ptr ) {
            *Ptr = 0;
        }

        Ptr = String;
        Ptr += wcslen(String) - 1;
        while ( Ptr > String && *Ptr <= L' ' ) {
            Ptr--;
        }

        Ptr++;
        if ( *Ptr && *Ptr <= L' ' ) {
            *Ptr = 0;
        }

        b = TRUE;

    } except( EXCEPTION_EXECUTE_HANDLER ) {
        ;
    }

    return b;
}


BOOLEAN
NTAPI
IniRemoveStringCommentA(
    IN OUT PSTR String
    )

/*++

Routine Description:

    This routine will remove comment (';') and tailing white space from
    string retrieved by GetProfileString, GetPrivateProfileString

Arguments:

    String - Pointer to null-terminated string, usually the result from
        GetProfileString, GetPrivateProfileString

Return Value:

    TRUE if comment has been removed, otherwise return FALSE

--*/

{
    PSTR Ptr;
    BOOLEAN b = FALSE;

    ASSERT( String != NULL );

    try {

        Ptr = strchr( String, ';' );
        if ( Ptr ) {
            *Ptr = 0;
        }

        Ptr = String;
        Ptr += strlen(String) - 1;
        while ( Ptr > String && *Ptr <= ' ' ) {
            Ptr--;
        }

        Ptr++;
        if ( *Ptr && *Ptr <= ' ' ) {
            *Ptr = 0;
        }

        b = TRUE;

    } except( EXCEPTION_EXECUTE_HANDLER ) {
        ;
    }

    return b;
}


BOOL
WINAPI
XTestLibIsStressTesting(
    VOID
    )
{
    return GetProfileIntA( "GeneralSettings", "StressTestMode", FALSE );
}
