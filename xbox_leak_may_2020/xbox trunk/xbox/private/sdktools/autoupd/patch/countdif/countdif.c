/* countdif.c -- how many bytes are different between two files? */

#define PROGNAME                "CountDif"
#define PROGVERS                "0.11"


#define EXIT_NO_ERROR           ( 0 )
#define EXIT_COMMAND_ERROR      ( 1 )
#define EXIT_FILE_ERROR         ( 2 )
#define EXIT_MEMORY_ERROR       ( 3 )
#define EXIT_ASSERTION          ( 4 )


#include <stdio.h>
#include <windows.h>

#define JUMBO_HASH

#ifdef JUMBO_HASH
#define MAX_HASH_INDEX            ( 0x1000000 )   // 24-bit hash table = 64MB RAM
#else
#define MAX_HASH_INDEX            ( 0x10000 )     // 16-bit hash table = 256KB RAM
#endif

#define MAX_DWORD                 ( 0xFFFFFFFF )

#define BEST_MATCH_UNKNOWN        ( MAX_DWORD )
#define INVALID_OFFSET            ( MAX_DWORD )

#define DEFAULT_MIN_RUN_LENGTH    ( 3 )
#define DEFAULT_MAX_RUN_LENGTH    ( MAX_DWORD )
#define DEFAULT_MAX_CHAIN_LENGTH  ( 100 )

#define FEATURE_NO_HASHING        ( 0x00000001 )    // use exhaustive searching
#define FEATURE_NO_PRIORITY       ( 0x00000002 )    // walk new fragment list every time
#define FEATURE_SEQUENTIAL        ( 0x00000004 )    // work like a compressor
#define FEATURE_SHOW_FRAGMENTS    ( 0x00000010 )
#define FEATURE_SHOW_STATS        ( 0x00000020 )
#define FEATURE_SHOW_PRIORITY     ( 0x00000040 )
#define FEATURE_SHOW_DIFFERENCES  ( 0x00000100 )

#pragma warning( disable : 4102 )   // unreferenced labels


//
//  NEW_FRAGMENT nodes are contained within two separate bi-di linked lists.
//  One of the lists, rooted by Op->NewFragmentList, stays in sorted Offset
//  order.  The other list is rooted by Op->PriorityQueue, sorted in
//  descending BestMatch order (forming a priority queue.)
//

typedef struct _a_NewFragment NEW_FRAGMENT;

struct _a_NewFragment
{
    NEW_FRAGMENT * Previous;        // previous unmatched fragment
    NEW_FRAGMENT * Next;            // next unmatched fragment

    NEW_FRAGMENT * NextLarger;      // up the priority queue
    NEW_FRAGMENT * NextSmaller;     // down the priority queue

    DWORD          Offset;          // offset into file buffer
    DWORD          Length;          // length of this fragment

    DWORD          BestProspect;    // BestMatch if known, else Length

    DWORD          BestMatch;       // BEST_MATCH_UNKNOWN or length of the best match ever seen
    DWORD          BestNewOffset;   // new file buffer offset of BestMatch
    DWORD          BestOldOffset;   // old file buffer offset of BestMatch
};

typedef struct _a_NewSubFragment NEW_SUBFRAGMENT;

struct _a_NewSubFragment
{
    NEW_FRAGMENT * Fragment;        // subfrag is entirely within this
    DWORD          Offset;          // offset into file buffer
    DWORD          Length;          // length of this subfragment
};


typedef struct _a_OldFragment OLD_FRAGMENT;

struct _a_OldFragment
{
    OLD_FRAGMENT * Previous;        // previous unmatched fragment
    OLD_FRAGMENT * Next;            // next unmatched fragment
    DWORD          Offset;          // offset into file buffer
    DWORD          Length;          // length of this fragment
};

typedef struct _a_OldSubFragment OLD_SUBFRAGMENT;

struct _a_OldSubFragment
{
    OLD_FRAGMENT * Fragment;        // subfrag is entirely within this
    DWORD          Offset;          // offset into file buffer
    DWORD          Length;          // length of this subfragment
};


typedef struct _a_Operation OPERATION;

struct _a_Operation
{
    // fascinating statistical stuff

    DWORD            NumberOfFinds;
    DWORD            NumberOfSkips;
    DWORD            NumberOfRefinds;
    DWORD            NumberOfRefindsAvailable;
    DWORD            NumberOfSinkers;
    DWORD            NumberOfOldFragments;
    DWORD            NumberOfNewFragments;
    DWORD            NumberOfBytesResolvedOld;
    DWORD            NumberOfBytesResolvedNew;
    DWORD            UpdateTick;
    DWORD            SearchTick;

    // output (zero these before calling)

    DWORD            NumberOfBytesChangedOld;
    DWORD            NumberOfBytesChangedNew;
    DWORD            NumberOfBytesMatched;
    DWORD            NumberOfMatches;

    // input to CountFileDifferences()

    CHAR           * OldFileName;
    CHAR           * NewFileName;

    DWORD            MinimumRunLength;      // minimum length of a match
    DWORD            MaximumRunLength;      // maximum length of any match (max initial frag size)
    DWORD            MaximumChainLength;    // max # of hash chain hops
    DWORD            Feature;               // any of FEATURE_xxx

    // input to CountBufferDifferences()

    BYTE           * OldFileBuffer;         // min alloc = OldFileSize + MinimumRunLength
    DWORD            OldFileSize;
    BYTE           * NewFileBuffer;         // min alloc = NewFileSize + MinimumRunLength
    DWORD            NewFileSize;

    // input to CountHashDifferences()

    DWORD          * HashTable;             // [ MAX_HASH_INDEX ], offset of 1st occ. of this hash
    DWORD          * HashLinks;             // [ OldFileSize ], offset of next occ. of this hash

    // internal to CountDifferences()

    OLD_FRAGMENT   * OldFragmentList;       // link to old fragment with lowest offset
    NEW_FRAGMENT   * NewFragmentList;       // link to new fragment with lowest offset
    NEW_FRAGMENT   * PriorityQueue;         // link to largest available match
    OLD_FRAGMENT * * Container;             // [ OldFileSize ], ptr to containing fragment or NULL

    // internal to RemoveLargestMatch(), returned by GetLargestMatch()

    OLD_SUBFRAGMENT  OldBestSubFragment;
    NEW_SUBFRAGMENT  NewBestSubFragment;

    // internal to FindLargestMatchForFragment()

    OLD_SUBFRAGMENT  OldBestPairSubFragment;
    NEW_SUBFRAGMENT  NewBestPairSubFragment;
};


BOOL
StringToDword(
    CHAR * String,
    DWORD * ValuePtr
    );

int
CountFileDifferences(
    OPERATION * Operation
    );

int
CountBufferDifferences(
    OPERATION * Operation
    );

int
CountDifferences(
    OPERATION * Operation
    );

int
RemoveLargestMatch(
    OPERATION * Operation
    );

void
InsertPriority(
    OPERATION    * Operation,
    NEW_FRAGMENT * Fragment,
    NEW_FRAGMENT * NextLarger
    );

void
DeletePriority(
    OPERATION    * Operation,
    NEW_FRAGMENT * Fragment
    );

void
AdjustPriority(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    );

void
DeleteNewFragment(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    );

void
DeleteOldFragment(
    OPERATION    * Operation,
    OLD_FRAGMENT * OldFragment
    );

void
SetContainer(
    OPERATION    * Operation,
    OLD_FRAGMENT * Container,
    DWORD          Offset,
    DWORD          Length
    );

BOOL
GetLargestMatch(
    OPERATION    * Operation
    );

BOOL
GetLargestMatchUsingPriority(
    OPERATION    * Operation
    );

BOOL
GetLargestMatchForFragment(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    );

void
FindLargestMatchForFragment(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    );

void
FindLargestMatchForFragmentByHashing(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    );

BOOL
FindLargestMatchForFragmentPair(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment,
    OLD_FRAGMENT * OldFragment,
    DWORD          BestFoundSoFar
    );

DWORD
CountMatchingBytes(
    BYTE * Buffer1,
    BYTE * Buffer2,
    DWORD  MaximumMatch
    );

void
CountReport(
    OPERATION * Operation
    );

void
ResolvedReport(
    OPERATION * Operation
    );

void
SearchReport(
    OPERATION * Operation,
    DWORD       Status
    );

DWORD
ComputeHashIndex(
    BYTE * Buffer
    );

void
ShowAdded(
    OPERATION * Operation,
    DWORD       Offset,
    DWORD       Length
    );

void
ShowDeleted(
    OPERATION * Operation,
    DWORD       Offset,
    DWORD       Length
    );

//
//  Command line entry point.  Parse command line into operation and call CountFileDifferences().
//

int __cdecl main( int argc, char * argv[] )
{
    int Result = EXIT_COMMAND_ERROR;
	int arg;
    OPERATION Operation;
    char * Param;

    memset( &Operation, 0, sizeof( Operation ));

    Operation.MinimumRunLength = DEFAULT_MIN_RUN_LENGTH;
    Operation.MaximumRunLength = DEFAULT_MAX_RUN_LENGTH;
    Operation.MaximumChainLength = 100;

	for ( arg = 1; arg < argc; arg++ )
	{
		if (( argv[ arg ][ 0 ] == '-' ) ||
			( argv[ arg ][ 0 ] == '/' ))
        {
            switch ( argv[ arg ][ 1 ] )
            {

            case 'l':
            case 'L':

                Param = argv[ arg ] + 2;

                if ( *Param == ':' )
                {
                    Param++;
                }

                if (( *Param == '\0' ) && (( arg + 1 ) < argc ))
                {
                    Param = argv[ ++arg ];
                }

                if ( ! StringToDword( Param, &Operation.MinimumRunLength ) ||
                    ( Operation.MinimumRunLength < 3 ))
                {
                    goto unexpected;
                }

                break;

            case 'm':
            case 'M':

                Param = argv[ arg ] + 2;

                if ( *Param == ':' )
                {
                    Param++;
                }

                if (( *Param == '\0' ) && (( arg + 1 ) < argc ))
                {
                    Param = argv[ ++arg ];
                }

                if ( ! StringToDword( Param, &Operation.MaximumRunLength ) ||
                    ( Operation.MaximumRunLength < 3 ))
                {
                    goto unexpected;
                }

                break;

            case 'c':
            case 'C':

                Param = argv[ arg ] + 2;

                if ( *Param == ':' )
                {
                    Param++;
                }

                if (( *Param == '\0' ) && (( arg + 1 ) < argc ))
                {
                    Param = argv[ ++arg ];
                }

                if ( ! StringToDword( Param, &Operation.MaximumChainLength ))
                {
                    goto unexpected;
                }

                break;

            case '?':
            case 'h':
            case 'H':

                printf(
                    "\n"
                    PROGNAME " version " PROGVERS " (msliger)\n"
                    "\n"
                    "\"How many bytes are changed in this new file?\"\n"
                    "\n"
                    PROGNAME " determines how many of the bytes found in {newfile} do not exist\n"
                    "in {oldfile}, by excluding matching byte sequences of length N or larger\n"
                    "that exist in both {oldfile} and {newfile}.  The default value of N is %u.\n"
                    "\n"
                    "Usage: " PROGNAME " {oldfile} {newfile} [options]\n"
                    "\n"
                    "Options:\n"
                    "\n"
                    "  /L#  Sets the minimum run length.  Matching byte sequences of this length\n"
                    "       or larger will be excluded (considered \"unchanged\".)  Default=%u.\n"
                    "\n"
                    "  /M#  Sets the maximum run length.  No matches larger than this will be\n"
                    "       found.  (Internally, partitions the new file into separate regions,\n"
                    "       each no larger than this length.)  Default=%u.\n"
                    "\n"
                    "  /C#  Sets the maximum hash chain length.  After this many steps have been\n"
                    "       considered along the hash chain, searching will terminate (early-out).\n"
                    "       Default=%u.\n"
                    "\n"
                    "WARNING: This implementation uses exhaustive searches and performance may be\n"
                    "         unacceptable with large files.\n",
                    DEFAULT_MIN_RUN_LENGTH,
                    DEFAULT_MIN_RUN_LENGTH,
                    DEFAULT_MAX_RUN_LENGTH,
                    DEFAULT_MAX_CHAIN_LENGTH
                    );

                goto finished;

            case 'f':
            case 'F':

                Param = argv[ arg ] + 2;

                if ( *Param == ':' )
                {
                    Param++;
                }

                if (( *Param == '\0' ) && (( arg + 1 ) < argc ))
                {
                    Param = argv[ ++arg ];
                }

                if ( ! StringToDword( Param, &Operation.Feature ))
                {
                    goto unexpected;
                }

                break;

            default:

                goto unexpected;
            }
        }
        else if ( Operation.OldFileName == NULL )
        {
            Operation.OldFileName = argv[ arg ];
        }
        else if ( Operation.NewFileName == NULL )
        {
            Operation.NewFileName = argv[ arg ];
        }
		else
		{
            break;
		}
	}

    if ( arg < argc )
    {

unexpected:

        fprintf( stderr, PROGNAME ": Unexpected: '%s'\n", argv[ arg ] );

        goto finished;
    }

    if ( Operation.NewFileName == NULL )
    {
        fprintf( stderr, PROGNAME ": Incomplete command line.  Use \"" PROGNAME "\" /? to see usage.\n" );

        goto finished;
    }

    Result = CountFileDifferences( &Operation );

    if ( Result == EXIT_NO_ERROR )
    {
        CountReport( &Operation );
    }

finished:

    return( Result );
}


//
//  Given two filenames, count their differences.
//
//  Reads the two file's contents into buffers and call CountBufferDifferences().
//

int
CountFileDifferences(
    OPERATION * Operation
    )
{
    int Result = EXIT_FILE_ERROR;
    HANDLE FileHandle = INVALID_HANDLE_VALUE;
    DWORD BytesRead;

    Operation->OldFileBuffer = NULL;
    Operation->NewFileBuffer = NULL;

    FileHandle = CreateFile( Operation->OldFileName,
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL
                             );

    if ( FileHandle == INVALID_HANDLE_VALUE )
    {
        goto olderror;
    }

    Operation->OldFileSize = GetFileSize( FileHandle, NULL );

    if (( Operation->OldFileSize == INVALID_FILE_SIZE ) ||
        ( Operation->OldFileSize == 0 ))
    {
        goto olderror;
    }

    Operation->OldFileBuffer = malloc( Operation->OldFileSize + Operation->MinimumRunLength );

    if ( Operation->OldFileBuffer == NULL )
    {
        goto olderror;
    }

    memset( Operation->OldFileBuffer + Operation->OldFileSize, 0, Operation->MinimumRunLength );

    if ( ! ReadFile( FileHandle,
                     Operation->OldFileBuffer,
                     Operation->OldFileSize,
                     &BytesRead,
                     NULL ) ||
         ( BytesRead != Operation->OldFileSize ))
    {
        goto olderror;
    }

    CloseHandle( FileHandle );

    FileHandle = CreateFile( Operation->NewFileName,
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL
                             );

    if ( FileHandle == INVALID_HANDLE_VALUE )
    {
        goto newerror;
    }

    Operation->NewFileSize = GetFileSize( FileHandle, NULL );

    if (( Operation->NewFileSize == INVALID_FILE_SIZE ) ||
        ( Operation->NewFileSize == 0 ))
    {
        goto newerror;
    }

    Operation->NewFileBuffer = malloc( Operation->NewFileSize + Operation->MinimumRunLength );

    if ( Operation->NewFileBuffer == NULL )
    {
        goto newerror;
    }

    memset( Operation->NewFileBuffer + Operation->NewFileSize, 0, Operation->MinimumRunLength );

    if ( ! ReadFile( FileHandle,
                     Operation->NewFileBuffer,
                     Operation->NewFileSize,
                     &BytesRead,
                     NULL ) ||
         ( BytesRead != Operation->NewFileSize ))
    {
        goto newerror;
    }

    CloseHandle( FileHandle );
    FileHandle = INVALID_HANDLE_VALUE;

    Result = CountBufferDifferences( Operation );

    goto finished;


olderror:

    fprintf( stderr, PROGNAME ": Unable to load old file '%s'.\n", Operation->OldFileName );

    goto finished;


newerror:

    fprintf( stderr, PROGNAME ": Unable to load new file '%s'.\n", Operation->NewFileName );

    goto finished;


finished:

    if ( FileHandle != INVALID_HANDLE_VALUE )
    {
        CloseHandle( FileHandle );
    }

    if ( Operation->OldFileBuffer != NULL )
    {
        free( Operation->OldFileBuffer );

        Operation->OldFileBuffer = NULL;
    }

    if ( Operation->NewFileBuffer != NULL )
    {
        free( Operation->NewFileBuffer );

        Operation->NewFileBuffer = NULL;
    }

    return( Result );
}


//
//  Given two buffers containing "old" and "new" data, count their differences.
//
//  Setup a hash table for lookup of old file data, and call CountDifferences().
//

int
CountBufferDifferences(
    OPERATION * Operation
    )
{
    int Result = EXIT_MEMORY_ERROR;
    BYTE * Lookup;
    DWORD Offset;
    DWORD HashIndex;

    Operation->HashLinks = NULL;
    Operation->HashTable = NULL;

    // If either file is smaller than MinimumRunLength, stop here.  They're different.

    if (( Operation->OldFileSize < Operation->MinimumRunLength ) ||
        ( Operation->NewFileSize < Operation->MinimumRunLength ))
    {
        Operation->NumberOfBytesChangedNew = Operation->NewFileSize;
        Operation->NumberOfBytesChangedOld = Operation->OldFileSize;

        if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
        {
            ShowDeleted( Operation, 0, Operation->OldFileSize );
            ShowAdded( Operation, 0, Operation->NewFileSize );
        }

        Operation->NumberOfBytesResolvedNew = Operation->NewFileSize;
        Operation->NumberOfBytesResolvedOld = Operation->OldFileSize;

        Result = EXIT_NO_ERROR;

        goto finished;
    }

    // allocate hash table & links array

    Operation->HashTable = malloc( MAX_HASH_INDEX * sizeof( Operation->HashTable[ 0 ] ));

    if ( Operation->HashTable == NULL )
    {
        goto finished;
    }

    for ( HashIndex = 0; HashIndex < MAX_HASH_INDEX; HashIndex++ )
    {
        Operation->HashTable[ HashIndex ] = INVALID_OFFSET;
    }

    Operation->HashLinks = malloc( Operation->OldFileSize * sizeof( Operation->HashLinks[ 0 ] ));

    if ( Operation->HashLinks == NULL )
    {
        goto finished;
    }

    for ( Offset = 0; Offset < Operation->OldFileSize; Offset++ )
    {
        Operation->HashLinks[ Offset ] = INVALID_OFFSET;
    }

    // populate hashing structures from old file contents

    Offset = Operation->OldFileSize - Operation->MinimumRunLength;

    for ( Lookup = Operation->OldFileBuffer + Offset;
          Lookup >= Operation->OldFileBuffer;
          Lookup--, Offset-- )
    {
        HashIndex = ComputeHashIndex( Lookup );

        Operation->HashLinks[ Offset ] = Operation->HashTable[ HashIndex ];
        Operation->HashTable[ HashIndex ] = Offset;
    }

    // do it

    Result = CountDifferences( Operation );

finished:

    if ( Operation->HashTable != NULL )
    {
        free( Operation->HashTable );

        Operation->HashTable = NULL;
    }

    if ( Operation->HashLinks != NULL )
    {
        free( Operation->HashLinks );

        Operation->HashLinks = NULL;
    }

    return( Result );
}


//
//  Count the number of changed bytes of two files.
//
//  On entry, the files' data has been loaded into buffers, and the old file's data has been
//  indexed into a hashing structure.  Think of these buffers as fragments.  Remove the largest
//  match, possibly splitting fragments, until all fragments for either file have been exhausted.
//  Any remaining fragments must then contain changed bytes.
//
//  As fragments split, any residual sequences smaller than the minimum run length are discarded.
//

int
CountDifferences(
    OPERATION * Operation
    )
{
    int Result;
    DWORD Remaining;
    DWORD FragmentOffset;
    DWORD FragmentLength;
    NEW_FRAGMENT * Fragment;
    NEW_FRAGMENT * LastFragment;
    NEW_FRAGMENT * * BackLink;

    Operation->Container = NULL;
    Operation->OldFragmentList = NULL;
    Operation->NewFragmentList = NULL;
    Operation->NumberOfNewFragments = 0;
    Operation->PriorityQueue = NULL;

    // coordinate features

    if ( Operation->Feature & FEATURE_SEQUENTIAL )
    {
        Operation->Feature &= ~FEATURE_NO_HASHING;
    }

    // allocate container array

    Operation->Container = malloc( Operation->OldFileSize * sizeof( Operation->Container[ 0 ] ));

    if ( Operation->Container == NULL )
    {
        goto finished;
    }

    memset( Operation->Container, 0, Operation->OldFileSize * sizeof( Operation->Container[ 0 ] ));

    // allocate initial old fragment consisting of entire old buffer

    Operation->NumberOfOldFragments = 1;

    Operation->OldFragmentList = malloc( sizeof( OLD_FRAGMENT ));
    if ( Operation->OldFragmentList == NULL )
    {
        goto finished;
    }

    memset( Operation->OldFragmentList, 0, sizeof( OLD_FRAGMENT ));

    Operation->OldFragmentList->Length = Operation->OldFileSize;

    SetContainer( Operation,
                  Operation->OldFragmentList,
                  0,
                  Operation->OldFileSize
                  );

    // allocate initial new fragment(s) consisting of entire new buffer,
    // each MaximumRunLength or smaller, but none smaller than MinimumRunLength.

    Remaining = Operation->NewFileSize;
    FragmentOffset = 0;
    FragmentLength = Operation->MaximumRunLength;
    BackLink = &Operation->NewFragmentList;
    LastFragment = NULL;

    while ( Remaining > 0 )
    {
        if ( FragmentLength > Remaining )
        {
            FragmentLength = Remaining;
        }

        if ( FragmentLength < Operation->MinimumRunLength )
        {
            Operation->NumberOfBytesChangedNew  += FragmentLength;
            Operation->NumberOfBytesResolvedNew += FragmentLength;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowAdded( Operation, FragmentOffset, FragmentLength );
            }

            break;
        }

        Operation->NumberOfNewFragments++;

        Fragment = malloc( sizeof( NEW_FRAGMENT ));
        if ( Fragment == NULL )
        {
            goto finished;
        }

        memset( Fragment, 0, sizeof( NEW_FRAGMENT ));

        Fragment->BestProspect = FragmentLength;

        Fragment->Offset = FragmentOffset;
        Fragment->Length = FragmentLength;
        Fragment->BestMatch = BEST_MATCH_UNKNOWN;

        Fragment->Previous = LastFragment;
        LastFragment = Fragment;

        *BackLink = Fragment;
        BackLink = &Fragment->Next;

        Remaining      -= FragmentLength;
        FragmentOffset += FragmentLength;

        AdjustPriority( Operation, Fragment );
    }

    ResolvedReport( Operation );

    //  until no fragments remain, remove the largest match

    while (( Operation->NewFragmentList != NULL ) &&
           ( Operation->OldFragmentList != NULL ))
    {
        Result = RemoveLargestMatch( Operation );

        if ( Result != EXIT_NO_ERROR )
        {
            goto finished;
        }

        ResolvedReport( Operation );
    }

    //  any remaining fragments contain changed bytes; tabulate during cleanup

    Result = EXIT_NO_ERROR;

finished:

    while ( Operation->NewFragmentList != NULL )
    {
        Operation->NumberOfBytesChangedNew  += Operation->NewFragmentList->Length;
        Operation->NumberOfBytesResolvedNew += Operation->NewFragmentList->Length;

        if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
        {
            ShowAdded( Operation, Operation->NewFragmentList->Offset, Operation->NewFragmentList->Length );
        }

        DeleteNewFragment( Operation, Operation->NewFragmentList );
    }

    while ( Operation->OldFragmentList != NULL )
    {
        Operation->NumberOfBytesChangedOld  += Operation->OldFragmentList->Length;
        Operation->NumberOfBytesResolvedOld += Operation->OldFragmentList->Length;

        if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
        {
            ShowDeleted( Operation, Operation->OldFragmentList->Offset, Operation->OldFragmentList->Length );
        }

        DeleteOldFragment( Operation, Operation->OldFragmentList );
    }

    Operation->UpdateTick = MAX_DWORD;  // force report
    ResolvedReport( Operation );

    if ( Operation->Container != NULL )
    {
        free( Operation->Container );

        Operation->Container = NULL;
    }

    return( Result );
}


//
//  Find and remove the largest match.  This may cause the old and/or new fragments to be split. 
//
//  For each new fragment, find the largest match which might be in any of the remaining old fragments.
//  Choose the new fragment with the largest match found, and remove the old and new subfragments that
//  describe that match.
//
//  To remove a subfragment, split the containing fragment as needed so that the subfragment represents
//  the entire containing fragment.  Regions before or after this fragment may become new, adjacent
//  fragments, or they are discarded if the resulting fragment would be too small (and therefor
//  counted as changed bytes.)  Once the subfragment represents the entire containing fragment, the
//  containing fragment is removed.
//

int
RemoveLargestMatch(
    OPERATION * Operation
    )
{
    int               Result;
    NEW_FRAGMENT    * SplitNewFragment;
    OLD_FRAGMENT    * SplitOldFragment;
    NEW_SUBFRAGMENT   NewBestSubFragment;
    OLD_SUBFRAGMENT   OldBestSubFragment;
    DWORD             ResidualSize;

    if (( Operation->Feature & FEATURE_NO_PRIORITY ) ||
        ( Operation->Feature & FEATURE_SEQUENTIAL ))
    {
        Result = GetLargestMatch( Operation );
    }
    else
    {
        Result = GetLargestMatchUsingPriority( Operation );
    }

    if ( Result == FALSE )
    {
        if ( Operation->NewFragmentList != NULL )
        {
            fprintf( stderr, PROGNAME ": assertion failed at __FILE__(__LINE__)\n" );

            Result = EXIT_ASSERTION;

            goto finished;
        }

        Result = EXIT_NO_ERROR;

        goto finished;
    }

    NewBestSubFragment = Operation->NewBestSubFragment;
    OldBestSubFragment = Operation->OldBestSubFragment;

    Operation->NumberOfMatches++;
    Operation->NumberOfBytesMatched += NewBestSubFragment.Length;
    Operation->NumberOfBytesResolvedNew += NewBestSubFragment.Length;
    Operation->NumberOfBytesResolvedOld += NewBestSubFragment.Length;

    //  Isolate NewBestSubFragment

    ResidualSize = NewBestSubFragment.Offset - NewBestSubFragment.Fragment->Offset;

    if ( ResidualSize > 0 )
    {
        if (( ResidualSize < Operation->MinimumRunLength ) ||
            ( Operation->Feature & FEATURE_SEQUENTIAL ))
        {
            // Residual at beginning of this fragment is too small.
            // Shrink fragment at head & record residual as unmatched.

            Operation->NumberOfBytesChangedNew += ResidualSize;
            Operation->NumberOfBytesResolvedNew += ResidualSize;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowAdded( Operation, NewBestSubFragment.Fragment->Offset, ResidualSize );
            }
        }
        else
        {
            // Create a new fragment containing the leading residual

            Operation->NumberOfNewFragments++;

            SplitNewFragment = malloc( sizeof( NEW_FRAGMENT ));
            if ( SplitNewFragment == NULL )
            {
                Result = EXIT_MEMORY_ERROR;

                goto finished;
            }

            memset( SplitNewFragment, 0, sizeof( NEW_FRAGMENT ));

            SplitNewFragment->Previous = NewBestSubFragment.Fragment->Previous;
            SplitNewFragment->Next = NewBestSubFragment.Fragment;
            NewBestSubFragment.Fragment->Previous = SplitNewFragment;

            if ( SplitNewFragment->Previous != NULL )
            {
                SplitNewFragment->Previous->Next = SplitNewFragment;
            }
            else
            {
                Operation->NewFragmentList = SplitNewFragment;
            }

            SplitNewFragment->BestProspect = min( ResidualSize, NewBestSubFragment.Length );

            SplitNewFragment->Offset = NewBestSubFragment.Fragment->Offset;
            SplitNewFragment->Length = ResidualSize;
            SplitNewFragment->BestMatch = BEST_MATCH_UNKNOWN;

            AdjustPriority( Operation, SplitNewFragment );
        }

        NewBestSubFragment.Fragment->Offset += ResidualSize;
        NewBestSubFragment.Fragment->Length -= ResidualSize;
    }

    ResidualSize = NewBestSubFragment.Fragment->Length - NewBestSubFragment.Length;

    if ( ResidualSize > 0 )
    {
        if ( ResidualSize < Operation->MinimumRunLength )
        {
            // Residual at end of this fragment is too small.
            // Shrink fragment at end & record residual as unmatched.

            Operation->NumberOfBytesChangedNew += ResidualSize;
            Operation->NumberOfBytesResolvedNew += ResidualSize;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowAdded( Operation, NewBestSubFragment.Fragment->Offset + NewBestSubFragment.Length, ResidualSize );
            }
        }
        else
        {
            // Create a new fragment containing the trailing residual

            Operation->NumberOfNewFragments++;

            SplitNewFragment = malloc( sizeof( NEW_FRAGMENT ));
            if ( SplitNewFragment == NULL )
            {
                Result = EXIT_MEMORY_ERROR;

                goto finished;
            }

            memset( SplitNewFragment, 0, sizeof( NEW_FRAGMENT ));

            SplitNewFragment->Previous = NewBestSubFragment.Fragment;
            SplitNewFragment->Next = NewBestSubFragment.Fragment->Next;
            NewBestSubFragment.Fragment->Next = SplitNewFragment;

            if ( SplitNewFragment->Next != NULL )
            {
                SplitNewFragment->Next->Previous = SplitNewFragment;
            }

            SplitNewFragment->BestProspect = min( ResidualSize, NewBestSubFragment.Length );

            SplitNewFragment->Offset = NewBestSubFragment.Fragment->Offset + NewBestSubFragment.Length;
            SplitNewFragment->Length = ResidualSize;
            SplitNewFragment->BestMatch = BEST_MATCH_UNKNOWN;

            AdjustPriority( Operation, SplitNewFragment );
        }

        NewBestSubFragment.Fragment->Length -= ResidualSize;
    }

    //  Isolate OldBestSubFragment

    ResidualSize = OldBestSubFragment.Offset - OldBestSubFragment.Fragment->Offset;

    if ( ResidualSize > 0 )
    {
        if ( ResidualSize < Operation->MinimumRunLength )
        {
            // Residual at beginning of this fragment is too small.
            // Shrink fragment at head & record residual as unmatched.

            Operation->NumberOfBytesChangedOld += ResidualSize;
            Operation->NumberOfBytesResolvedOld += ResidualSize;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowDeleted( Operation, OldBestSubFragment.Fragment->Offset, ResidualSize );
            }

            SplitOldFragment = NULL;
        }
        else
        {
            // Create a new fragment containing the leading residual

            Operation->NumberOfOldFragments++;

            SplitOldFragment = malloc( sizeof( OLD_FRAGMENT ));
            if ( SplitOldFragment == NULL )
            {
                Result = EXIT_MEMORY_ERROR;

                goto finished;
            }

            memset( SplitOldFragment, 0, sizeof( OLD_FRAGMENT ));

            SplitOldFragment->Previous = OldBestSubFragment.Fragment->Previous;
            SplitOldFragment->Next = OldBestSubFragment.Fragment;
            SplitOldFragment->Next->Previous = SplitOldFragment;

            if ( SplitOldFragment->Previous != NULL )
            {
                SplitOldFragment->Previous->Next = SplitOldFragment;
            }
            else
            {
                Operation->OldFragmentList = SplitOldFragment;
            }

            SplitOldFragment->Offset = OldBestSubFragment.Fragment->Offset;
            SplitOldFragment->Length = ResidualSize;
        }

        SetContainer( Operation,
                      SplitOldFragment,
                      OldBestSubFragment.Fragment->Offset,
                      ResidualSize
                      );

        OldBestSubFragment.Fragment->Offset += ResidualSize;
        OldBestSubFragment.Fragment->Length -= ResidualSize;
    }

    ResidualSize = OldBestSubFragment.Fragment->Length - OldBestSubFragment.Length;

    if ( ResidualSize > 0 )
    {
        if ( ResidualSize < Operation->MinimumRunLength )
        {
            // Residual at end of this fragment is too small.
            // Shrink fragment at end & record residual as unmatched.

            Operation->NumberOfBytesChangedOld += ResidualSize;
            Operation->NumberOfBytesResolvedOld += ResidualSize;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowDeleted( Operation, OldBestSubFragment.Fragment->Offset + OldBestSubFragment.Length, ResidualSize );
            }

            SplitOldFragment = NULL;
        }
        else
        {
            // Create a new fragment containing the trailing residual

            Operation->NumberOfOldFragments++;

            SplitOldFragment = malloc( sizeof( OLD_FRAGMENT ));
            if ( SplitOldFragment == NULL )
            {
                Result = EXIT_MEMORY_ERROR;

                goto finished;
            }

            memset( SplitOldFragment, 0, sizeof( OLD_FRAGMENT ));

            SplitOldFragment->Previous = OldBestSubFragment.Fragment;
            SplitOldFragment->Next = OldBestSubFragment.Fragment->Next;
            OldBestSubFragment.Fragment->Next = SplitOldFragment;

            if ( SplitOldFragment->Next != NULL )
            {
                SplitOldFragment->Next->Previous = SplitOldFragment;
            }

            SplitOldFragment->Offset = OldBestSubFragment.Fragment->Offset + OldBestSubFragment.Length;
            SplitOldFragment->Length = ResidualSize;
        }

        SetContainer( Operation,
                      SplitOldFragment,
                      OldBestSubFragment.Fragment->Offset + OldBestSubFragment.Length,
                      ResidualSize
                      );

        OldBestSubFragment.Fragment->Length -= ResidualSize;
    }

    if ( Operation->Feature & FEATURE_SHOW_FRAGMENTS )
    {
        printf( "Matched %u at new %u old %u.\n",
                NewBestSubFragment.Length,
                NewBestSubFragment.Offset,
                OldBestSubFragment.Offset
                );
    }

#ifdef _DEBUG
    if ( NewBestSubFragment.Fragment->Length < Operation->PriorityQueue->BestProspect )
    {
        printf( "Match selected doesn't agree with PriorityQueue\n" );
    }
#endif

    if ( Operation->Feature & FEATURE_SHOW_PRIORITY )
    {
        NEW_FRAGMENT * Priority;

        for ( Priority = Operation->PriorityQueue;
              Priority != NULL;
              Priority = Priority->NextSmaller )
        {
            printf( "   PQ: Offset: %u  Length: %u  Prospect: %u  Best: %u  (New: %u  Old: %u)\n",
                    Priority->Offset,
                    Priority->Length,
                    Priority->BestProspect,
                    Priority->BestMatch,
                    Priority->BestNewOffset,
                    Priority->BestOldOffset
                    );
        }
    }

    //  Remove NewBestSubFragment.Fragment

    DeleteNewFragment( Operation, NewBestSubFragment.Fragment );

    //  Remove OldBestSubFragment.Fragment

    DeleteOldFragment( Operation, OldBestSubFragment.Fragment );

    Result = EXIT_NO_ERROR;

finished:

    return( Result );
}


//
//  Locate the largest match between the given new fragment and any old fragment.  If this new
//  fragment was previously searched, the results of the previous search may be returned after
//  re-verification.
//
//  Sets Operation->NewBestSubFragment and Operation->OldBestSubFragment.  If no match is found,
//  Operation->NewBestSubFragment.Length will be 0 on return.
//
//  Returns TRUE if any match is found, FALSE if there are no old fragments which match any part
//  of the given new fragment.  If FALSE is returned, the caller may wish to prune the fragment.
//

BOOL
GetLargestMatchForFragment(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    )
{
    OLD_FRAGMENT * OldFragment;
    DWORD          BestOldEnding;
    DWORD          ThisOldEnding;

    // if this isn't the first find on this fragment, the previous find's results
    // might still be available, in which case searching again isn't necessary.

    if ( NewFragment->BestMatch != BEST_MATCH_UNKNOWN )
    {
        if ( NewFragment->BestNewOffset < NewFragment->Offset )
        {
            goto search;
        }

        if (( NewFragment->BestNewOffset + NewFragment->BestMatch ) >
            ( NewFragment->Offset + NewFragment->Length ))
        {
            goto search;
        }

        // see if an old fragment containing this former subfragment still exists

        BestOldEnding = NewFragment->BestOldOffset + NewFragment->BestMatch;

        for ( OldFragment = Operation->OldFragmentList;
              OldFragment != NULL;
              OldFragment = OldFragment->Next )
        {
            if ( OldFragment->Offset > NewFragment->BestOldOffset )
            {
                goto search;
            }

            ThisOldEnding = OldFragment->Offset + OldFragment->Length;

            if ( ThisOldEnding > NewFragment->BestOldOffset )
            {
                if ( ThisOldEnding < BestOldEnding )
                {
                    goto search;
                }

                Operation->NumberOfRefindsAvailable++;

                Operation->NewBestSubFragment.Fragment = NewFragment;
                Operation->NewBestSubFragment.Offset   = NewFragment->BestNewOffset;
                Operation->NewBestSubFragment.Length   = NewFragment->BestMatch;

                Operation->OldBestSubFragment.Fragment = OldFragment;
                Operation->OldBestSubFragment.Offset   = NewFragment->BestOldOffset;
                Operation->OldBestSubFragment.Length   = NewFragment->BestMatch;

                goto foundit;
            }
        }
    }

search:

    // search every old fragment for the best match

    if ( Operation->Feature & FEATURE_NO_HASHING )
    {
        FindLargestMatchForFragment( Operation, NewFragment );
    }
    else
    {
        FindLargestMatchForFragmentByHashing( Operation, NewFragment );
    }

    // save this best match in case we're called again and it hasn't been used 

    NewFragment->BestMatch     = Operation->NewBestSubFragment.Length;
    NewFragment->BestNewOffset = Operation->NewBestSubFragment.Offset;
    NewFragment->BestOldOffset = Operation->OldBestSubFragment.Offset;

    NewFragment->BestProspect  = Operation->NewBestSubFragment.Length;

    AdjustPriority( Operation, NewFragment );

foundit:

    if ( Operation->NewBestSubFragment.Length > Operation->MinimumRunLength )
    {
        return( TRUE );
    }

    return( FALSE );
}


//
//  Given a specific new fragment and a specific old fragment, find the largest sequence
//  of bytes the two fragments have in common.  Don't bother if the length of that largest
//  sequence isn't at least BestFoundSoFar, which indicates the length of the largest
//  sequence already found for this new fragment from some other old fragment.
//
//  Returns subfragments describing the common sequence in Operation->NewBestPairSubFragment
//  and Operation->OldBestPairSubFragment, or FALSE if no better common sequence was found.
//

BOOL
FindLargestMatchForFragmentPair(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment,
    OLD_FRAGMENT * OldFragment,
    DWORD          BestFoundSoFar
    )
{
    BYTE * NewStart = Operation->NewFileBuffer + NewFragment->Offset;
    BYTE * NewSearch;
    DWORD  NewMaxMatch;
    BYTE * NewLimit = NewStart + NewFragment->Length;

    BYTE * OldStart = Operation->OldFileBuffer + OldFragment->Offset;
    BYTE * OldSearch;
    DWORD  OldMaxMatch;
    BYTE * OldLimit = OldStart + OldFragment->Length;

    DWORD  BestMatch = 0;
    BYTE * NewBest;
    BYTE * OldBest;

    DWORD  LengthThreshold = max( BestFoundSoFar, Operation->MinimumRunLength );
    DWORD  Length;

    for ( NewSearch = NewStart, NewMaxMatch = NewFragment->Length;
          NewSearch < NewLimit;
          NewSearch++, NewMaxMatch-- )
    {
        if ( NewMaxMatch <= LengthThreshold )
        {
            break;
        }

        for ( OldSearch = OldStart, OldMaxMatch = OldFragment->Length;
              OldSearch < OldLimit;
              OldSearch++, OldMaxMatch-- )
        {
            if ( OldMaxMatch <= LengthThreshold )
            {
                break;
            }

            Length = CountMatchingBytes( NewSearch, OldSearch, min( NewMaxMatch, OldMaxMatch ));

            if ( Length > LengthThreshold )
            {
                LengthThreshold = Length;
                BestMatch = Length;
                NewBest = NewSearch;
                OldBest = OldSearch;
            }
        }
    }

    if ( BestMatch == 0 )
    {
        return( FALSE );
    }

    Operation->NewBestPairSubFragment.Fragment = NewFragment;
    Operation->NewBestPairSubFragment.Offset = NewBest - Operation->NewFileBuffer;
    Operation->NewBestPairSubFragment.Length = BestMatch;

    Operation->OldBestPairSubFragment.Fragment = OldFragment;
    Operation->OldBestPairSubFragment.Offset = OldBest - Operation->OldFileBuffer;
    Operation->OldBestPairSubFragment.Length = BestMatch;

    return( TRUE );
}


//
//  Locate the largest match between the given new fragment and any old fragment.
//
//  Sets Operation->NewBestSubFragment and Operation->OldBestSubFragment.  If no match is found,
//  Operation->NewBestSubFragment.Length will be 0 on return.
//

void
FindLargestMatchForFragment(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    )
{
    OLD_FRAGMENT * OldFragment;

    Operation->NewBestSubFragment.Length = 0;

    for ( OldFragment = Operation->OldFragmentList;
          OldFragment != NULL;
          OldFragment = OldFragment->Next )
    {
        if ( Operation->NewBestSubFragment.Length >= OldFragment->Length )
        {
            continue;
        }

        if ( FindLargestMatchForFragmentPair( Operation,
                                              NewFragment,
                                              OldFragment,
                                              Operation->NewBestSubFragment.Length
                                              ))           
        {
            if ( Operation->NewBestSubFragment.Length < Operation->NewBestPairSubFragment.Length )
            {
                Operation->NewBestSubFragment = Operation->NewBestPairSubFragment;
                Operation->OldBestSubFragment = Operation->OldBestPairSubFragment;
            }
        }
    }
}


//
//  Locate the largest match between the given new fragment and any old fragment.
//
//  Sets Operation->NewBestSubFragment and Operation->OldBestSubFragment.  If no match is found,
//  Operation->NewBestSubFragment.Length will be 0 on return.
//

void
FindLargestMatchForFragmentByHashing(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    )
{
    DWORD          HashIndex;

    BYTE         * NewStart = Operation->NewFileBuffer + NewFragment->Offset;
    BYTE         * NewSearch;
    DWORD          NewMaxMatch;
    BYTE         * NewLimit = NewStart + NewFragment->Length - Operation->MinimumRunLength + 1;

    DWORD          OldOffset;

    OLD_FRAGMENT * OldFragment;
    BYTE         * OldSearch;
    DWORD          OldMaxMatch;

    DWORD          BestMatch = 0;
    BYTE         * NewBest;
    BYTE         * OldBest;
    OLD_FRAGMENT * OldBestFragment;

    DWORD          LengthThreshold = Operation->MinimumRunLength;
    DWORD          Length;

    DWORD        * Referrer;   // for self-pruning hash chain
    DWORD          NumberOfHops;

    for ( NewSearch = NewStart, NewMaxMatch = NewFragment->Length;
          NewSearch < NewLimit;
          NewSearch++, NewMaxMatch-- )
    {
        SearchReport( Operation, NewSearch - Operation->NewFileBuffer );

        if ( NewMaxMatch <= LengthThreshold )
        {
            break;
        }

        HashIndex = ComputeHashIndex( NewSearch );

        NumberOfHops = 0;

        Referrer = &Operation->HashTable[ HashIndex ];
        
        while ( OldOffset = *Referrer, OldOffset != INVALID_OFFSET )
        {
            OldFragment = Operation->Container[ OldOffset ];

            if ( OldFragment == NULL )
            {
                *Referrer = Operation->HashLinks[ OldOffset ];  // prune hash chain

                continue;
            }

            Referrer = &Operation->HashLinks[ OldOffset ];  // ptr to next offset

            NumberOfHops++;

            OldSearch = Operation->OldFileBuffer + OldOffset;

            if (( NewSearch[ 0 ] != OldSearch[ 0 ] ) ||
                ( NewSearch[ 1 ] != OldSearch[ 1 ] ) ||
                ( NewSearch[ 2 ] != OldSearch[ 2 ] ))
            {
                continue;
            }

            OldMaxMatch = OldFragment->Offset + OldFragment->Length - OldOffset;

            if ( OldMaxMatch <= LengthThreshold )
            {
                continue;
            }

            Length = CountMatchingBytes( NewSearch, OldSearch, min( NewMaxMatch, OldMaxMatch ));

            if ( Length > LengthThreshold )
            {
                LengthThreshold = Length;
                BestMatch = Length;
                NewBest = NewSearch;
                OldBest = OldSearch;
                OldBestFragment = OldFragment;

                NumberOfHops = 0;
            }

            if ( NumberOfHops > Operation->MaximumChainLength )
            {
                break;
            }
        }

        if (( BestMatch > 0 ) &&
            ( Operation->Feature & FEATURE_SEQUENTIAL ))
        {
            break;
        }
    }

    Operation->NewBestSubFragment.Fragment = NewFragment;
    Operation->NewBestSubFragment.Offset = NewBest - Operation->NewFileBuffer;
    Operation->NewBestSubFragment.Length = BestMatch;

    Operation->OldBestSubFragment.Fragment = OldBestFragment;
    Operation->OldBestSubFragment.Offset = OldBest - Operation->OldFileBuffer;
    Operation->OldBestSubFragment.Length = BestMatch;
}


//
//  Convert a string of digits into a DWORD, return FALSE if a non-digit is seen or overflow.
//

BOOL
StringToDword(
    CHAR * String,
    DWORD * ValuePtr
    )
{
    unsigned Value = 0;
    unsigned Digit;

    if (( String[ 0 ] == '0' ) && ( String[ 1 ] == 'x' ))
    {
        String += 2;

        while ( *String != '\0' )
        {
            if (( *String >= '0' ) && ( *String <= '9' ))
            {
                Digit = *String - '0';
            }
            else if (( *String >= 'A' ) && ( *String <= 'F' ))
            {
                Digit = *String - 'A' + 10;
            }
            else if (( *String >= 'a' ) && ( *String <= 'a' ))
            {
                Digit = *String - 'a' + 10;
            }
            else
            {
                break;
            }

            if ( Value > 0x0FFFFFFF)
            {
                break;
            }

            Value *= 16;
            Value += Digit;

            String++;
        }
    }
    else
    {
        while (( *String >= '0' ) && ( *String <= '9' ))
        {
            Digit = *String - '0';

            if ( Value > 0x0FFFFFFF )
            {
                break;
            }

            Value *= 10;
            Value += Digit;

            String++;
        }
    }

    *ValuePtr = Value;

    if ( *String != '\0' )
    {
        return( FALSE );
    }

    return( TRUE );
}


//
//  Display count statistics
//

void
CountReport(
    OPERATION * Operation
    )
{
    printf( "%u matches totaling %u bytes, %u deleted bytes, %u new bytes\n",
            Operation->NumberOfMatches,
            Operation->NumberOfBytesMatched,
            Operation->NumberOfBytesChangedOld,
            Operation->NumberOfBytesChangedNew
            );

    if ( Operation->Feature & FEATURE_SHOW_STATS )
    {
        printf( "new filesize=%u, old filesize=%u, new resolved=%u, old resolved=%u\n",
                Operation->NewFileSize,
                Operation->OldFileSize,
                Operation->NumberOfBytesResolvedNew,
                Operation->NumberOfBytesResolvedOld
                );

        printf( "  Finds: %u  Skips: %u  Refinds: %u  Available: %u  Sinkers: %u\n",
                Operation->NumberOfFinds,
                Operation->NumberOfSkips,
                Operation->NumberOfRefinds,
                Operation->NumberOfRefindsAvailable,
                Operation->NumberOfSinkers
                );

        printf( "Fragments remaining: %u/%u\n",
                Operation->NumberOfNewFragments,
                Operation->NumberOfOldFragments
                );
    }
}


//
//  Display intermediate resolution statistics
//

void
ResolvedReport(
    OPERATION * Operation
    )
{
    DWORD Now = GetTickCount();

    if (( Now > ( Operation->UpdateTick + 2500 )) ||
        ( Now < ( Operation->UpdateTick )))
    {
        fprintf( stderr, "                 %u/%u new, %u/%u old (%u/%u)    \r",
                Operation->NumberOfBytesResolvedNew,
                Operation->NewFileSize,
                Operation->NumberOfBytesResolvedOld,
                Operation->OldFileSize,
                Operation->NumberOfNewFragments,
                Operation->NumberOfOldFragments
                );

        Operation->UpdateTick = Now;
    }
}


//
//  Display intermediate resolution statistics
//

void
SearchReport(
    OPERATION * Operation,
    DWORD       Status
    )
{
    DWORD Now = GetTickCount();

    if (( Now > ( Operation->SearchTick + 2500 )) ||
        ( Now < ( Operation->SearchTick )))
    {
        fprintf( stderr, " %u \r", Status );

        Operation->SearchTick = Now;
    }
}


//
//  Determine how many bytes match at the given locations, bounded by MaximumMatch
//

DWORD
CountMatchingBytes(
    BYTE * Buffer1,
    BYTE * Buffer2,
    DWORD  MaximumMatch
    )
{
    DWORD Result = 0;

    while ( *Buffer1++ == *Buffer2++ )
    {
        Result++;

        if ( Result >= MaximumMatch )
        {
            break;
        }
    }

    return( Result );
}


//
//  Return the hash function of the three bytes indicated
//

DWORD
ComputeHashIndex(
    BYTE * Buffer
    )
{
    DWORD HashIndex;

#ifdef JUMBO_HASH
    HashIndex = Buffer[ 0 ] + ( Buffer[ 1 ] << 8 ) + ( Buffer[ 2 ] << 16 );
#else
    HashIndex = Buffer[ 0 ] ^ ( Buffer[ 1 ] << 4 ) + ( Buffer[ 2 ] << 8 );
#endif

    return( HashIndex );
}


void
InsertPriority(
    OPERATION    * Operation,
    NEW_FRAGMENT * Fragment,
    NEW_FRAGMENT * NextLarger
    )
{
    Fragment->NextLarger = NextLarger;

    if ( NextLarger == NULL )
    {
        Fragment->NextSmaller = Operation->PriorityQueue;

        Operation->PriorityQueue = Fragment;
    }
    else
    {
        Fragment->NextSmaller = NextLarger->NextSmaller;

        NextLarger->NextSmaller = Fragment;
    }

    if ( Fragment->NextSmaller != NULL )
    {
        Fragment->NextSmaller->NextLarger = Fragment;
    }
}


void
DeletePriority(
    OPERATION    * Operation,
    NEW_FRAGMENT * Fragment
    )
{
    if ( Fragment->NextLarger == NULL )
    {
        Operation->PriorityQueue = Fragment->NextSmaller;
    }
    else
    {
        Fragment->NextLarger->NextSmaller = Fragment->NextSmaller;
    }

    if ( Fragment->NextSmaller != NULL )
    {
        Fragment->NextSmaller->NextLarger = Fragment->NextLarger;
    }
}


//
//  Move a fragment to the correct position within the priority queue
//
//  Note that the fragment might not be in the priority queue yet.
//

void
AdjustPriority(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    )
{
    NEW_FRAGMENT * Reference;
    NEW_FRAGMENT * ReferenceNext;
    BOOL IsLinked = FALSE;

    Reference = NewFragment->NextSmaller;

    if ( Reference != NULL )
    {
        IsLinked = TRUE;

        if ( Reference->BestProspect > NewFragment->BestProspect)
        {
            //  downqueue

            DeletePriority( Operation, NewFragment );

            while (( ReferenceNext = Reference->NextSmaller, ReferenceNext != NULL ) &&
                   ( ReferenceNext->BestProspect > NewFragment->BestProspect ))
            {
                Reference = ReferenceNext;
            }

            goto insert;
        }
    }

    Reference = NewFragment->NextLarger;

    if ( Reference != NULL )
    {
        IsLinked = TRUE;

        if ( Reference->BestProspect < NewFragment->BestProspect )
        {
            //  upqueue

            DeletePriority( Operation, NewFragment );

            do
            {
                Reference = Reference->NextLarger;

            } while (( Reference != NULL ) &&
                     ( Reference->BestProspect < NewFragment->BestProspect ));

            goto insert;
        }
    }

    if ( IsLinked )
    {
        goto finished;
    }

    if ( Operation->PriorityQueue == NewFragment )
    {
        IsLinked = TRUE;

        goto finished;
    }

    Reference = Operation->PriorityQueue;

    if ( Reference != NULL )
    {
        while (( ReferenceNext = Reference->NextSmaller, ReferenceNext != NULL ) &&
               ( ReferenceNext->BestProspect > NewFragment->BestProspect ))
        {
            Reference = ReferenceNext;
        }
    }

insert:

    InsertPriority( Operation, NewFragment, Reference );

finished:

    return;
}


//
//  Unlink a fragment from the NewFragmentList and PriorityQueue and delete it.
//

void
DeleteNewFragment(
    OPERATION    * Operation,
    NEW_FRAGMENT * NewFragment
    )
{
    if ( NewFragment->Previous != NULL )
    {
        NewFragment->Previous->Next = NewFragment->Next;
    }
    else
    {
        Operation->NewFragmentList = NewFragment->Next;
    }

    if ( NewFragment->Next != NULL )
    {
        NewFragment->Next->Previous = NewFragment->Previous;
    }

    DeletePriority( Operation, NewFragment );

    Operation->NumberOfNewFragments--;

    free( NewFragment );
}


//
//  Unlink a fragment from the OldFragmentList and delete it.
//

void
DeleteOldFragment(
    OPERATION    * Operation,
    OLD_FRAGMENT * OldFragment
    )
{
    SetContainer( Operation,
                  NULL,
                  OldFragment->Offset,
                  OldFragment->Length
                  );

    if ( OldFragment->Previous != NULL )
    {
        OldFragment->Previous->Next = OldFragment->Next;
    }
    else
    {
        Operation->OldFragmentList = OldFragment->Next;
    }

    if ( OldFragment->Next != NULL )
    {
        OldFragment->Next->Previous = OldFragment->Previous;
    }

    Operation->NumberOfOldFragments--;

    free( OldFragment );
}


//
//  Set all Container pointers starting at Offset for the given Length to the specified fragment.
//

void
SetContainer(
    OPERATION    * Operation,
    OLD_FRAGMENT * Container,
    DWORD          Offset,
    DWORD          Length
    )
{
    OLD_FRAGMENT * * Link;

    Link = Operation->Container + Offset;

    while ( Length-- )
    {
        *Link++ = Container;
    }
}


BOOL
GetLargestMatch(
    OPERATION * Operation
    )
{
    NEW_FRAGMENT    * NewFragment;
    NEW_FRAGMENT    * NextNewFragment;
    NEW_SUBFRAGMENT   NewBestSubFragment;
    OLD_SUBFRAGMENT   OldBestSubFragment;

    NewBestSubFragment.Length = 0;

    for ( NewFragment = Operation->NewFragmentList;
          NewFragment != NULL;
          NewFragment = NextNewFragment )
    {
        if ( Operation->Feature & FEATURE_SHOW_FRAGMENTS )
        {
            printf( "       Offset: %u  Length: %u  Prospect: %u  Best: %u  (New: %u  Old: %u)\n",
                    NewFragment->Offset,
                    NewFragment->Length,
                    NewFragment->BestProspect,
                    NewFragment->BestMatch,
                    NewFragment->BestNewOffset,
                    NewFragment->BestOldOffset
                    );
        }

        NextNewFragment = NewFragment->Next;

        if ( NewBestSubFragment.Length > NewFragment->Length )
        {
            continue;   // best is larger than this entire fragment
        }

        if ( NewBestSubFragment.Length >= NewFragment->BestMatch )
        {
            Operation->NumberOfSkips++;

            continue;   // best is larger than ever seen in this fragment
        }

        if ( NewFragment->BestMatch != BEST_MATCH_UNKNOWN )
        {
            Operation->NumberOfRefinds++;
        }
        else
        {
            Operation->NumberOfFinds++;
        }

        if ( ! GetLargestMatchForFragment( Operation,
                                           NewFragment
                                           ))
        {
            // No matches were found for this fragment.  Prune it (new changed bytes).

            Operation->NumberOfBytesChangedNew += NewFragment->Length;
            Operation->NumberOfBytesResolvedNew += NewFragment->Length;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowAdded( Operation, NewFragment->Offset, NewFragment->Length );
            }

            DeleteNewFragment( Operation, NewFragment );

            if ( Operation->Feature & FEATURE_SHOW_FRAGMENTS )
            {
                printf( "   (pruned)\n" );
            }

            continue;
        }

        if ( Operation->NewBestSubFragment.Length > NewBestSubFragment.Length )
        {
            NewBestSubFragment = Operation->NewBestSubFragment;
            OldBestSubFragment = Operation->OldBestSubFragment;
        }
        else if ( Operation->NewBestSubFragment.Length == NewBestSubFragment.Length )
        {
            // Do nothing for greedy matches.  Ideally, we'd optimize to prefer edges, leave no fragments
            // smaller than MinimumRunLength, prefer entire fragments over subfragments, etc.
        }

        if ( Operation->Feature & FEATURE_SEQUENTIAL )
        {
            break;
        }
    }

    if ( NewBestSubFragment.Length == 0 )
    {
        return( FALSE );
    }

    Operation->NewBestSubFragment = NewBestSubFragment;
    Operation->OldBestSubFragment = OldBestSubFragment;

    return( TRUE );
}


BOOL
GetLargestMatchUsingPriority(
    OPERATION * Operation
    )
{
    NEW_FRAGMENT    * NewFragment;

    while ( NewFragment = Operation->PriorityQueue, NewFragment != NULL )
    {
        if ( Operation->Feature & FEATURE_SHOW_FRAGMENTS )
        {
            printf( "       Offset: %u  Length: %u  Prospect: %u  Best: %u  (New: %u  Old: %u)\n",
                    NewFragment->Offset,
                    NewFragment->Length,
                    NewFragment->BestProspect,
                    NewFragment->BestMatch,
                    NewFragment->BestNewOffset,
                    NewFragment->BestOldOffset
                    );
        }

        if ( NewFragment->BestMatch != BEST_MATCH_UNKNOWN )
        {
            Operation->NumberOfRefinds++;
        }
        else
        {
            Operation->NumberOfFinds++;
        }

        if ( ! GetLargestMatchForFragment( Operation,
                                           NewFragment
                                           ))
        {
            // No matches were found for this fragment.  Prune it (new changed bytes).

            Operation->NumberOfBytesChangedNew += NewFragment->Length;
            Operation->NumberOfBytesResolvedNew += NewFragment->Length;

            if ( Operation->Feature & FEATURE_SHOW_DIFFERENCES )
            {
                ShowAdded( Operation, NewFragment->Offset, NewFragment->Length );
            }

            DeleteNewFragment( Operation, NewFragment );

            if ( Operation->Feature & FEATURE_SHOW_FRAGMENTS )
            {
                printf( "   (pruned)\n" );
            }

            continue;
        }

        //  Calling GetLargestMatchForFragment() may have caused this fragment to
        //  sink in the priority queue.  If this isn't still the top, start over.

        if ( NewFragment != Operation->PriorityQueue )
        {
            Operation->NumberOfSinkers++;

            continue;
        }

        return( TRUE );
    }

    return( FALSE );
}


void
ShowAdded(
    OPERATION * Operation,
    DWORD       Offset,
    DWORD       Length
    )
{
    while ( Length-- )
    {
        printf( "Add: %08X  %02X\n", Offset, Operation->NewFileBuffer[ Offset ] );
        Offset++;
    }
}

void
ShowDeleted(
    OPERATION * Operation,
    DWORD       Offset,
    DWORD       Length
    )
{
    while ( Length-- )
    {
        printf( "Del: %08X  %02X\n", Offset, Operation->OldFileBuffer[ Offset ] );
        Offset++;
    }
}
