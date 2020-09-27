//
// The following are stuctures for the category/patch lists...
//
typedef enum { LISTSTYLE_NONE, LISTSTYLE_EXCLUSIVE, LISTSTYLE_NON_EXCLUSIVE } LIST_STYLE ;
typedef enum { ENTRYTYPE_PATCH, ENTRYTYPE_PATCH_COLLECTION } ENTRY_TYPE ;

struct _PATCH_ENTRY ;
struct _PATCH_CATEGORY ;
typedef struct _PATCH_ENTRY PATCH_ENTRY, *PPATCH_ENTRY ;
typedef struct _PATCH_CATEGORY PATCH_CATEGORY, *PPATCH_CATEGORY ;

struct _PATCH_ENTRY {
   ENTRY_TYPE EntryType ;
   ULONG WalkFlags ;
   PVOID WalkNext ;
   char *Action ;
   char *ShortcutName ;
   char *MenuDescription ;
   char *LengthyDescription ;
   PPATCH_ENTRY Parent ;
   PPATCH_ENTRY Sibling ;
   PPATCH_ENTRY Child ;
   LIST_STYLE ChildStyle ;
} ;

struct _PATCH_CATEGORY {   
   char *CategoryName ;
   PPATCH_CATEGORY Sibling ;
   PPATCH_ENTRY Child ;
   LIST_STYLE ChildStyle ;
} ;

//
// This structure manages the a given patch tree
//
typedef struct {
   PPATCH_CATEGORY FirstCategory ;
   PPATCH_CATEGORY CurrentCategory ;
   BOOL InCategory ;
   PPATCH_ENTRY LastEntry ;
   PPATCH_ENTRY LastParent ;

} PATCH_TREE, *PPATCH_TREE ;

//
// The following functions build up an patch tree.
//
VOID
PatchCategoryBegin(
   PPATCH_TREE PatchTree,
   char *CategoryName
   ) ;

VOID
PatchCategoryEnd(
   PPATCH_TREE PatchTree
   );

VOID
PatchSiblingListBegin(
   PPATCH_TREE PatchTree,
   LIST_STYLE  ListStyle
   );

VOID
PatchSiblingListEnd(
   PPATCH_TREE PatchTree
   );

VOID
PatchDeclareEntry(
   PPATCH_TREE PatchTree,
   char *Action,
   char *ShortCut,
   char *MenuDescription,
   char *LengthyDescription
   );

VOID
PatchDeclareCollection(
   PPATCH_TREE PatchTree,
   char *Action,
   char *ShortCut,
   char *MenuDescription,
   char *LengthyDescription
   );

VOID
PatchBuildTree(
   PPATCH_TREE PatchTree
   );

extern PPATCH_TREE gpPatchTree ;

//
// This function is a printf to a malloc'd string
//
char *
PatchMallocSprintf(
   char *szFormat, ...
   ) ;

//
// The following macro's turn an "patch table" into a series of calls
// to the table building functions.
//
#define NO_SHORTCUT "###"

#define BEGIN_CATEGORY(CategoryName) \
   PatchCategoryBegin(gpPatchTree, CategoryName)

// And a hack so that Action and PatchList may be NULL
#define PatchMallocSprintfNULL NULL

#define DECLARE_ENTRY(Action, ShortCut, MenuDescription, LengthyDescription) \
   {\
      char *szOutputAction = PatchMallocSprintf##Action ; \
      PatchDeclareEntry(gpPatchTree, szOutputAction, \
                        " " ShortCut " ", MenuDescription, \
                        LengthyDescription) ;\
   }

#define DECLARE_COLLECTION(PatchList, ShortCut, MenuDescription, LengthyDescription) \
   {\
      char *szOutputList = PatchMallocSprintf##PatchList ; \
      PatchDeclareCollection(gpPatchTree, szOutputList, \
                             " " ShortCut " ", MenuDescription, \
                             LengthyDescription) ;\
   }

#define BEGIN_SIBLING_LIST(Style) \
   PatchSiblingListBegin(gpPatchTree, Style) 

#define END_SIBLING_LIST() \
   PatchSiblingListEnd(gpPatchTree) 

#define END_CATEGORY() \
   PatchCategoryEnd(gpPatchTree)

//
// When walking entries in an patch tree, we will use callback functions
// of the following type. And the walking functions follow afterwards...
//
typedef VOID (*PPATCH_WALKCALLBACK)(PPATCH_CATEGORY, PPATCH_ENTRY, ULONG Depth, PVOID Context) ;

VOID
PatchTreeWalk(
   PPATCH_TREE PatchTree,
   PPATCH_WALKCALLBACK PreCallback,
   PPATCH_WALKCALLBACK PostCallback,
   PVOID Context,
   BOOL ZeroWalkFlags
   );

VOID
PatchTreeWalkHelper(
   PPATCH_ENTRY Entry,
   PPATCH_WALKCALLBACK PreCallback,
   PPATCH_WALKCALLBACK PostCallback,
   ULONG Depth,
   PVOID Context
   );

VOID 
PatchTreeWalkZeroFlags(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   );

//
// Useful tree functions
//
VOID
PatchTreeFree(
   PPATCH_TREE PatchTree
   );

VOID
PatchTreePrint(
   PPATCH_TREE PatchTree
   );

//
// The following definitions control a selected "set" of patchs.
// 
typedef struct {
   char          *TokenList ;
   PPATCH_ENTRY  PatchEntry ;
   BOOL          GenReg ;
   BOOL          InProgress ;
   PPATCH_TREE   PatchTree ;
} PATCHSET, *PPATCHSET ;

VOID 
PatchSetBuild(
   PPATCH_TREE PatchTree,
   char *szCommandLine,
   PPATCHSET PatchSet
   );

VOID 
PatchSetBuildTreeCallback(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   );

VOID 
PatchSetBuildTreeCallback2(
   PPATCH_ENTRY     Entry,
   PPATCHSET        PatchSet,
   ENTRY_TYPE       EntryType,
   BOOL             IsQuestion
   );

VOID
PatchSetPrint(
   PPATCHSET PatchSet
   );

//
// This group of definitions describe patches to binaries. Note that setting
// a variable to a given variable is represented by AND'ing the variable with
// zero and following up with an or of the appropriate value.
//
typedef struct _DRIVERENTRY DRIVERENTRY, *PDRIVERENTRY ;
typedef struct _DRIVERMOD DRIVERMOD, *PDRIVERMOD ;

struct _DRIVERENTRY {
   char szModuleName[256] ;
   ULONG BaseAddress ;
   ULONG Checksum ;
   PDRIVERMOD FirstPatch ;
   PDRIVERENTRY Sibling ;
} ;

#define PATCHFLAG_BYTE_PATCH        0x00000000
#define PATCHFLAG_WORD_PATCH        0x00000001
#define PATCHFLAG_DWORD_PATCH       0x00000003
#define PATCHFLAG_FIXUPS_COMPLETE   0x00000010
#define PATCHVALUE_INVALID          0xFFFFFFFF

#define APPLIED_PATCH_SIZE(x) (((x)->PatchFlags)&0x7)

struct _DRIVERMOD {
   char      szSymbolicName[512] ;
   ULONG_PTR ulModuleOffset ;
   ULONG     PatchFlags ;
   ULONG     AndPatch ;
   ULONG     OrPatch ;
   PDRIVERMOD Sibling ;
} ;

PDRIVERENTRY
ModSetBuild(
   PPATCHSET PatchSet,
   BOOL      DebugMode
   );

BOOL
ModEntryParse(
   IN     char *Action, 
   IN     BOOL DebugMode,
   IN OUT char *szModuleDestination, 
   IN OUT PDRIVERMOD DriverPatchDest,
   IN OUT PULONG pulMore
   );

VOID
ModEntryMerge(
   PDRIVERMOD DriverPatchDest,
   PDRIVERMOD DriverPatchNew
   );

VOID
ModSetPrint(
   PDRIVERENTRY DriverPatchDest
   );

BOOL
ModSetGetDriverInfo(
   char *szModuleName,
   PVOID *ModuleBaseAddress,
   PULONG Checksum
   );

VOID
ModSetFixupAddresses(
   PDRIVERENTRY DriverPatchHead
   );

BOOL
ModSetApply(
   PDRIVERENTRY DriverPatchHead
   );

VOID 
PatchTreeMenuPrintCallback(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   );

VOID
PatchTreeMenuPrint(
   PPATCH_TREE PatchTree
   );

    
