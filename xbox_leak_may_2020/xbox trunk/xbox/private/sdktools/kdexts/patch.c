/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    assert.c

Abstract:

    WinDbg Extension Api

Author:

    Adrian J. Oney (adriao) 07-28-1998

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#include "patchp.h"
#pragma hdrstop

DECLARE_API( patch )

/*++

Routine Description:

    Implements !Patch, a handy way to set flags in binaries...

Arguments:

    None.

Return Value:

    None.

--*/

{
    PATCH_TREE patchTree ;
    PATCHSET PatchSet ;
    PDRIVERENTRY DriverPatches ;
    int i ;

    //
    // Build the patch tree
    //
    PatchBuildTree(&patchTree) ;

    PatchSet.InProgress = FALSE ;

    //
    // Look at the commandline, and either bring up help, the menu, or apply
    // any assertions passed in.
    //
    if (args[0]=='?') {

       dprintf("!Patch <patchname>[?] [<patchname>[?]*] [genreg]\n") ;
       dprintf("or !Patch with no parameters to bring up a menu of options.\n") ;

    } else if (args[0]=='!') {

       dprintf("*Commencing debug mode -\n") ;
       PatchTreePrint(&patchTree) ;
       dprintf("*  Building patch set\n") ;
       PatchSetBuild(&patchTree, ((char *) args)+1, &PatchSet) ;
       PatchSetPrint(&PatchSet) ;
       dprintf("*  Building mod set\n") ;
       DriverPatches = ModSetBuild(&PatchSet, TRUE) ;
       dprintf("*  Fixing up mod set\n") ;
       ModSetFixupAddresses(DriverPatches) ;
       ModSetPrint(DriverPatches) ;

    } else if (args[0]) {

       //
       // First, turn the options on the command-line into a link'd and sorted
       // set of asserts to process.
       //
       PatchSetBuild(&patchTree, (char *) args, &PatchSet) ;

       //
       // Now, compile a set of patches from that list.
       //
       DriverPatches = ModSetBuild(&PatchSet, FALSE) ;

       //
       // Now fix them up relative to the current symbol set
       //
       ModSetFixupAddresses(DriverPatches) ;

       //
       // Finally, slam them into memory.
       //
       if (ModSetApply(DriverPatches)) {

          dprintf("Patches applied.\n") ;
       }

    } else {

       PatchTreeMenuPrint(&patchTree) ;
    }

    PatchTreeFree(&patchTree) ;
    return;
}

//
// These functions walk an assertion tree.
//
VOID
PatchTreeWalk(
   PPATCH_TREE PatchTree,
   PPATCH_WALKCALLBACK PreCallback,
   PPATCH_WALKCALLBACK PostCallback,
   PVOID Context,
   BOOL ZeroWalkFlags
   )
{
   PPATCH_CATEGORY category, oldCategory ;

   if (ZeroWalkFlags) {

      PatchTreeWalk(
         PatchTree,
         PatchTreeWalkZeroFlags,
         NULL,
         NULL,
         FALSE
         ) ;
   }

   category = PatchTree->FirstCategory ;
   while(category) {

      if (PreCallback) {
         PreCallback(category, NULL, 0, Context) ;
      }
      if (category->Child) {

         PatchTreeWalkHelper(
            category->Child,
            PreCallback,
            PostCallback,
            1,
            Context
            ) ;
      }
      oldCategory = category ;
      category = category->Sibling ;
      if (PostCallback) {
         PostCallback(category, NULL, 0, Context) ;
      }
   }
}

VOID
PatchTreeWalkHelper(
   PPATCH_ENTRY Entry,
   PPATCH_WALKCALLBACK PreCallback,
   PPATCH_WALKCALLBACK PostCallback,
   ULONG Depth,
   PVOID Context
   )
{
   PPATCH_ENTRY nextEntry ;

   if (PreCallback) {
      PreCallback(NULL, Entry, Depth, Context) ;
   }

   if (Entry->Child) {
      PatchTreeWalkHelper(Entry->Child, PreCallback, PostCallback, Depth+1, Context) ;
   }

   nextEntry = Entry->Sibling ;
   if (PostCallback) {
      PostCallback(NULL, Entry, Depth, Context) ;
   }

   if (nextEntry) {
      PatchTreeWalkHelper(nextEntry, PreCallback, PostCallback, Depth, Context) ;
   }
}

VOID
PatchTreeWalkZeroFlags(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   )
{
   if (Entry) {
      Entry->WalkFlags = 0 ;
   }
}


//
// These functions free a given tree...
//
VOID
PatchTreeFreeCallback(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   )
{
   if (Category) {
      free(Category) ;
   }
   if (Entry) {
      if (Entry->Action) {
         free(Entry->Action) ;
      }
      free(Entry) ;
   }
}

VOID
PatchTreeFree(
   PPATCH_TREE PatchTree
   )
{
   PatchTreeWalk(PatchTree, NULL, PatchTreeFreeCallback, NULL, FALSE) ;
}

//
// These functions print out a given assertion tree...
//
VOID
AssertPrintListStyle(
   LIST_STYLE ListStyle
   )
{
   switch(ListStyle)
   {
      case LISTSTYLE_NONE:          dprintf("None") ;         break;
      case LISTSTYLE_EXCLUSIVE:     dprintf("Exclusive") ;    break;
      case LISTSTYLE_NON_EXCLUSIVE: dprintf("NonExclusive") ; break;
      default:                      dprintf("????") ;         break;
   }
}

VOID
PatchTreePrintCallback(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   )
{
   char szIndent[1024] ;

   if (Category) {

      dprintf("\nCategory: %x\n", Category) ;
      dprintf("  Category Name: %s\n", Category->CategoryName) ;
      dprintf("  ListStyle: ") ;
      AssertPrintListStyle(Category->ChildStyle) ;
      dprintf("\n") ;
      dprintf("  Sibling: %x  - Child: %x\n",
         Category->Sibling,
         Category->Child
         ) ;
   }

   if (Entry) {

      memset(szIndent, ' ', Depth*2) ;
      szIndent[Depth*2]='\0' ;

      dprintf("%sEntry: %x", szIndent, Entry) ;
      switch(Entry->EntryType) {
         case ENTRYTYPE_PATCH:
            dprintf(" (Patch)") ;
            break;
         case ENTRYTYPE_PATCH_COLLECTION:
            dprintf(" (Collection)") ;
            break;
      }
      dprintf("\n") ;
      dprintf("%s  Action: %s\n", szIndent, Entry->Action) ;
      dprintf("%s  Shortcut: %s\n", szIndent, Entry->ShortcutName) ;
      dprintf("%s  MenuDescription: %s\n", szIndent, Entry->MenuDescription) ;
      dprintf("%s  LengthyDescription: %s\n", szIndent, Entry->LengthyDescription) ;
      dprintf("%s  ListStyle: ", szIndent) ;
      AssertPrintListStyle(Entry->ChildStyle) ;
      dprintf("\n") ;
      dprintf("%s  WalkNext: %x  - WalkFlags: %x\n", szIndent,
         Entry->WalkNext, Entry->WalkFlags) ;
      dprintf("%s  Parent: %x  - Sibling: %x  - Child: %x\n", szIndent,
         Entry->Parent, Entry->Sibling, Entry->Child) ;
   }
}

VOID
PatchTreePrint(
   PPATCH_TREE PatchTree
   )
{
   PatchTreeWalk(PatchTree, PatchTreePrintCallback, NULL, NULL, FALSE) ;
}

VOID
PatchSetPrint(
   PPATCHSET PatchSet
   )
{
   PPATCH_ENTRY entry ;

   entry = PatchSet->PatchEntry ;
   dprintf("GenReg: %s\n", (PatchSet->GenReg) ? "TRUE" : "FALSE" ) ;

   while(entry) {
      ASSERT(entry->WalkFlags&1) ;
      PatchTreePrintCallback(NULL, entry, 0, NULL) ;
      entry = entry->WalkNext ;
   }
}

//
// These functions select a given set of items from an assertion tree.
//
VOID
PatchSetBuild(
   PPATCH_TREE PatchTree,
   char *szCommandLine,
   PPATCHSET PatchSet
   )
{
   char szTokenList[1024] ;
   char *szOffset ;
   char szGenReg[]=" GENREG " ; // Must be caps
   int i, j ;
   BOOL bOuterPass = FALSE ;

   //
   // Build a token list with ' ' on either side
   //
   i=strlen(szCommandLine) ;
   memcpy(szTokenList+1, szCommandLine, i) ;
   szTokenList[0]=' ' ;
   strcpy(szTokenList+1+i, " ") ;
   _strupr(szTokenList+1) ;

   PatchSet->TokenList = szTokenList ;
   if (PatchSet->InProgress == FALSE) {
      PatchSet->InProgress = TRUE ;
      PatchSet->PatchEntry = NULL ;
      PatchSet->PatchTree = PatchTree ;
      bOuterPass = TRUE ;
   }

   PatchTreeWalk(PatchTree, PatchSetBuildTreeCallback, NULL, PatchSet, bOuterPass) ;

   //
   // Now the list is built, look for any remaining tokens...
   //
   szOffset = strstr(PatchSet->TokenList, szGenReg) ;
   PatchSet->GenReg = (szOffset != NULL) ;
   while(szOffset) {
      memset(szOffset, ' ', sizeof(szGenReg)) ;
      szOffset = strstr(PatchSet->TokenList, szGenReg) ;
   }

   //
   // Any tokens remaining?
   //
   szOffset = strtok(PatchSet->TokenList, " ") ;
   while(szOffset&&bOuterPass) {
      //
      // This token was *not* found...
      //
      dprintf("Could not find !Patch %s, mispelling perhaps?\n", szOffset) ;
      szOffset = strtok(NULL, " ") ;
   }

   PatchSet->TokenList = NULL ;
}

VOID
PatchSetBuildTreeCallback(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   )
{
   PPATCHSET patchSet = (PPATCHSET) Context ;
   char *szOffset ;
   char szToken[1024] ;
   int i ;

   if ((!Entry)||
       ((Entry->EntryType!=ENTRYTYPE_PATCH)&&
        (Entry->EntryType!=ENTRYTYPE_PATCH_COLLECTION))||
       (!Entry->ShortcutName)||
       (Entry->WalkFlags&1)) {

      //
      // If not an assert, no shortcut, or previously marked, bail
      //
      return ;
   }

   //
   // Our token list is guarenteed to have two spaces on either edge, as is
   // each shortcut name...
   //
   strcpy(szToken, Entry->ShortcutName) ;
   i=strlen(szToken) ;
   _strupr(szToken) ;
   ASSERT(szToken[0]==' ') ;
   ASSERT(szToken[i-1]==' ') ;
   ASSERT(patchSet->TokenList[0]==' ') ;
   ASSERT(patchSet->TokenList[strlen(patchSet->TokenList)-1]==' ') ;

   szOffset = strstr(patchSet->TokenList, szToken) ;

   if (!szOffset) {

      //
      // This token wasn't in the list. Quickly see this is a request for
      // more information.
      //
      szToken[i-1]='?' ;
      szToken[i+0]=' ' ;
      szToken[i+1]='\0' ;

      //dprintf("> [ %s %s ]\n", patchSet->TokenList, szToken) ;
      szOffset = strstr(patchSet->TokenList, szToken) ;
      if (szOffset) {

         do {
            memset(szOffset, ' ', i) ; // doesn't need to be i+1
            szOffset = strstr(patchSet->TokenList, szToken) ;
         } while(szOffset) ;

         PatchSetBuildTreeCallback2(Entry, patchSet, Entry->EntryType, TRUE) ;
      }

      return ;
   }

   //
   // Token was found. Wipe out that entry from the token list...
   //
   while(szOffset) {
      memset(szOffset, ' ', i) ;
      szOffset = strstr(patchSet->TokenList, szToken) ;
   }

   PatchSetBuildTreeCallback2(Entry, patchSet, Entry->EntryType, FALSE) ;
}

VOID
PatchSetBuildTreeCallback2(
   PPATCH_ENTRY     Entry,
   PPATCHSET        PatchSet,
   ENTRY_TYPE       EntryType,
   BOOL             IsQuestion
   )
{
   PPATCH_ENTRY childEntry, parentEntry, *nextEntry ;
   char szToken[1024] ;
   char *szLastTokenList ;
   int i ;

   if (IsQuestion) {

      //
      // The user typed in !Patch foo? Help tell our user about this assert.
      //
      dprintf("!Patch%s- %s\n",
         Entry->ShortcutName,
         Entry->MenuDescription
         ) ;

      if (Entry->LengthyDescription) {
         dprintf("\n%s\n", Entry->LengthyDescription) ;
      }
      if (Entry->EntryType==ENTRYTYPE_PATCH_COLLECTION) {
         dprintf("\nThe following are enabled by !Patch%s- %s\n",
            Entry->ShortcutName,
            Entry->Action
            ) ;
      } else {
         dprintf("\nPatch: %s\n", Entry->Action) ;
      }

      parentEntry = Entry->Parent ;
      if (parentEntry) {
         dprintf("The following asserts are also automatically applied:") ;
         //
         // There could be a collection in the list, but we won't bother
         // expanding those...
         //
         do {

            if (parentEntry->ShortcutName) {
               strcpy(szToken, parentEntry->ShortcutName) ;
               i=strlen(szToken) ;
               szToken[i-1]='\0' ;
               dprintf("%s", szToken) ;
            }
            parentEntry = parentEntry->Parent ;
         } while(parentEntry) ;
         dprintf("\n") ;
      }
      return ;
   }

   //
   // Mark it found.
   //
   Entry->WalkFlags|=1 ;
   Entry->WalkNext = NULL ;
   //dprintf("[ Marking %s ]\n", Entry->ShortcutName) ;

   //
   // Process any collections...
   //
   if (Entry->EntryType==ENTRYTYPE_PATCH_COLLECTION) {

      szLastTokenList = PatchSet->TokenList ;
      PatchSetBuild(PatchSet->PatchTree, Entry->Action, PatchSet) ;
      PatchSet->TokenList = szLastTokenList ;
   }

   //
   // Link it and its' ancestors in if that is not already done...
   //
   parentEntry = Entry->Parent ;
   childEntry = Entry ;
   while(parentEntry&&(!(parentEntry->WalkFlags&1))) {

      parentEntry->WalkNext = childEntry ;
      parentEntry->WalkFlags |= 1 ;
      //dprintf("[ Marking %s .]\n", parentEntry->ShortcutName) ;
      childEntry = parentEntry ;
      parentEntry = parentEntry->Parent ;
   }

   //
   // Add this to the list...
   //
   nextEntry = &PatchSet->PatchEntry ;
   while(*nextEntry) {
      nextEntry = (PPATCH_ENTRY *) (&(*nextEntry)->WalkNext) ;
   }
   *nextEntry = childEntry ;
}


PDRIVERENTRY
ModSetBuild(
   PPATCHSET PatchSet,
   BOOL DebugMode
   )
{
   PPATCH_ENTRY patchEntry ;
   PDRIVERENTRY driverPatchList = NULL, driver ;
   PDRIVERENTRY *driverTail ;
   char szModuleName[512] ;
   DRIVERMOD driverPatch ;
   PDRIVERMOD patch, *patchTail ;
   ULONG ulMore ;

   patchEntry = PatchSet->PatchEntry ;
   ulMore = 0 ;
   while(patchEntry) {

      if (DebugMode) {
         dprintf("Trying patch entry %x delta %x\n", patchEntry, ulMore) ;
      }
      if ((patchEntry->EntryType == ENTRYTYPE_PATCH)&&
          ModEntryParse(patchEntry->Action, DebugMode, szModuleName, &driverPatch, &ulMore)) {

         if (DebugMode) {
            dprintf("Parsed: %s\n", szModuleName) ;
            dprintf("  Symbol:  %s\n", driverPatch.szSymbolicName) ;
            dprintf("  Flags:   %x\n", driverPatch.PatchFlags) ;
            dprintf("  AndMask: %x\n", driverPatch.AndPatch) ;
            dprintf("  OrMask:  %x\n", driverPatch.OrPatch) ;
         }

         driver = driverPatchList;
         driverTail = &driverPatchList ;
         while(driver) {
            driverTail = &driver->Sibling ;
            if (!_stricmp(szModuleName, driver->szModuleName)) {
               break;
            }
            driver = driver->Sibling ;
         }
         if (!driver) {
            driver = (PDRIVERENTRY) malloc(sizeof(DRIVERENTRY)) ;
            strcpy(driver->szModuleName, szModuleName) ;
            driver->FirstPatch = NULL ;
            driver->Sibling = NULL ;
            driver->Checksum = 0 ;
            *driverTail = driver ;
            driverTail = &driver->Sibling ;
         }
         patch = driver->FirstPatch ;
         patchTail = &driver->FirstPatch ;
         while(patch) {
            patchTail = &patch->Sibling ;
            if (!_stricmp(driverPatch.szSymbolicName, patch->szSymbolicName)) {
               break;
            }
            patch = patch->Sibling ;
         }
         if (!patch) {
            patch = (PDRIVERMOD) malloc(sizeof(DRIVERMOD)) ;
            *patch = driverPatch ;
            patch->Sibling = NULL ;
            *patchTail = patch ;
            patchTail = &patch->Sibling ;
         } else {
            ModEntryMerge(patch, &driverPatch) ;
         }
         if (DebugMode) {
            ModSetPrint(driverPatchList) ;
         }
      }
      if (!ulMore) {
         patchEntry = (PPATCH_ENTRY) patchEntry->WalkNext ;
      }
   }
   return driverPatchList ;
}

BOOL
ModEntryParse(
   char *Action,
   BOOL DebugMode,
   char *szModuleDestination,
   PDRIVERMOD DriverPatchDest,
   PULONG pulMore
   )
{
   char *szOffset, *szOffset2 ;
   ULONG patchFlags, patchValue ;
   char szAction[1024], c ;
   int i, j ;

   if (!Action) {
      *pulMore = 0 ;
      return FALSE ;
   }
   strcpy(szAction, Action+*pulMore) ;
   if (DebugMode) {
      dprintf("Parsing %s\n", szAction) ;
   }
   szOffset = strtok(szAction, " !") ;
   if (!szOffset) {
      dprintf("Parse fail (1)\n") ;
      *pulMore = 0 ;
      return FALSE ;
   }

   //dprintf(":%s\n", szOffset) ;

   if      (!_stricmp(szOffset, "eb")) patchFlags = PATCHFLAG_BYTE_PATCH ;
   else if (!_stricmp(szOffset, "ew")) patchFlags = PATCHFLAG_WORD_PATCH ;
   else if (!_stricmp(szOffset, "ed")) patchFlags = PATCHFLAG_DWORD_PATCH ;
   else                                patchFlags = PATCHVALUE_INVALID ;

   if (patchFlags!=PATCHVALUE_INVALID) {
      szOffset = strtok(NULL, "!") ;
      //dprintf(":%s\n", szOffset) ;
   } else {
      patchFlags = PATCHFLAG_DWORD_PATCH ;
   }

   strcpy(szModuleDestination, szOffset) ;
   szOffset+=strlen(szOffset)+1 ;
   i = strcspn(szOffset, "|=& ") ;
   if (!szOffset[i]) {
      dprintf("Parse fail (2)\n") ;
      *pulMore = 0 ;
      return FALSE ;
   }

   strncpy(DriverPatchDest->szSymbolicName, szOffset, i) ;
   DriverPatchDest->szSymbolicName[i]='\0' ;
   //dprintf(":%s\n", DriverPatchDest->szSymbolicName) ;

   szOffset+=i+1 ;
   i = strcspn(szOffset, " 1234567890ABCDEFabcdef") ;

   c=szOffset[i] ;
   szOffset[i]='\0' ;
   //dprintf(":%s\n", szOffset) ;

   if (!c) {
      dprintf("Parse fail (3)\n") ;
      *pulMore = 0 ;
      return FALSE ;
   }

   szOffset2 = strtok(szOffset, " ") ;
   if (!szOffset2) {
      dprintf("Parse fail (4)\n") ;
      *pulMore = 0 ;
      return FALSE ;
   }

   if      (!strcmp(szOffset2, "|=")) j=0 ;
   else if (!strcmp(szOffset2, "&=")) j=1 ;
   else if (!strcmp(szOffset2, "=" )) j=2 ;
   else {
      dprintf("Parse fail (5)\n") ;
      *pulMore = 0 ;
      return FALSE ;
   }

   szOffset[i]=c ;
   if (sscanf(szOffset+i, "%x", &patchValue)!=1) {
      dprintf("Parse fail (6)\n") ;
      *pulMore = 0 ;
      return FALSE ;
   }
   szOffset+=i ;
   i = strcspn(szOffset, ";") ;
   if (szOffset[i]&&szOffset[i+1]) {
      *pulMore = *pulMore + szOffset - szAction + i+1 ;
   } else {
      *pulMore = 0 ;
   }

   switch(j) {

      case 0:
         DriverPatchDest->AndPatch = 0xFFFFFFFF ;
         DriverPatchDest->OrPatch = patchValue ;
         break;

      case 1:
         DriverPatchDest->AndPatch = patchValue ;
         DriverPatchDest->OrPatch =  0 ;
         break;

      case 2:
         DriverPatchDest->AndPatch = 0 ;
         DriverPatchDest->OrPatch =  patchValue ;
         break;
   }
   DriverPatchDest->ulModuleOffset = 0 ;
   DriverPatchDest->PatchFlags = patchFlags ;
   return TRUE ;
}

VOID
ModEntryMerge(
   PDRIVERMOD DriverPatchDest,
   PDRIVERMOD DriverPatchNew
   )
{
   ASSERT(PATCHFLAG_BYTE_PATCH == sizeof(BYTE)-1) ;
   ASSERT(PATCHFLAG_WORD_PATCH == sizeof(WORD)-1) ;
   ASSERT(PATCHFLAG_DWORD_PATCH == sizeof(DWORD)-1) ;

   while(APPLIED_PATCH_SIZE(DriverPatchDest) < APPLIED_PATCH_SIZE(DriverPatchNew)) {

      //
      // Promote the destination
      //
      DriverPatchDest->PatchFlags++ ;
      DriverPatchDest->AndPatch <<= 8 ;
      DriverPatchDest->AndPatch |= 0xFF ;
      DriverPatchDest->OrPatch <<= 8 ;
   }

   if (!DriverPatchNew->AndPatch) {
      //
      // It's a set, just obliterate the previous OR mask...
      //
      DriverPatchDest->OrPatch = 0 ;
   }

   DriverPatchDest->AndPatch &= DriverPatchNew->AndPatch ;
   DriverPatchDest->OrPatch |= DriverPatchNew->OrPatch ;
}

VOID
ModSetPrint(
   PDRIVERENTRY DriverPatchHead
   )
{
   PDRIVERENTRY driver = DriverPatchHead ;
   PDRIVERMOD patch ;

   while(driver) {
      dprintf("Driver: %s  Checksum: %x  Base: %x\n", driver->szModuleName, driver->Checksum, driver->BaseAddress) ;
      patch = driver->FirstPatch ;
      while(patch) {
         dprintf("  Symbol:  %s\n", patch->szSymbolicName) ;
         dprintf("  Offset:  %x\n", patch->ulModuleOffset) ;
         dprintf("  Flags:   %x\n", patch->PatchFlags) ;
         dprintf("  AndMask: %x\n", patch->AndPatch) ;
         dprintf("  OrMask:  %x\n", patch->OrPatch) ;
         patch=patch->Sibling ;
      }
      driver=driver->Sibling ;
   }
}

VOID
ModSetFixupAddresses(
   PDRIVERENTRY DriverPatchHead
   )
{
   PDRIVERENTRY driver = DriverPatchHead ;
   PDRIVERMOD patch ;
   ULONG displacement, checksum ;
   PVOID address, baseAddress ;
   char szVariableName[512] ;

   while(driver) {
      patch = driver->FirstPatch ;
      while(patch) {

         //
         // First get the virtual address. We do this because we can't simply
         // get an offset.
         //
         sprintf(szVariableName, "%s!%s", driver->szModuleName, patch->szSymbolicName) ;
         address = (PVOID) GetExpression( szVariableName );

         if (address) {
            if (ModSetGetDriverInfo(driver->szModuleName, &baseAddress, &checksum)) {
               driver->Checksum = checksum ;
               driver->BaseAddress = (ULONG) baseAddress ;
               patch->ulModuleOffset = (PUCHAR) address - (PUCHAR) baseAddress ;
               patch->PatchFlags |= PATCHFLAG_FIXUPS_COMPLETE ;
            }
         } else {
            dprintf("Couldn't resolve symbol %s, try !reload maybe?\n", szVariableName) ;
         }
         patch=patch->Sibling ;
      }
      driver=driver->Sibling ;
   }
}

BOOL
ModSetGetDriverInfo(
   char *szModuleName,
   PVOID *ModuleBaseAddress,
   PULONG Checksum
   )
{
   LIST_ENTRY List;
   PLIST_ENTRY Next;
   ULONG ListHead;
   ULONG Result;
   PLDR_DATA_TABLE_ENTRY DataTable;
   LDR_DATA_TABLE_ENTRY DataTableBuffer;
   WCHAR UnicodeBuffer[128];
   char szNtos[]="ntoskrnl" ;
   char szFilename[260] ;
   char *pszDot ;
   IMAGE_DOS_HEADER DosHeader;
   ULONG cb;
   ULONG Offset;
   IMAGE_NT_HEADERS imageNtHeaders;
   PIMAGE_FILE_HEADER imageFileHdr;
   PIMAGE_OPTIONAL_HEADER imageOptionalHdr;

   //
   // Adjust for "nt" passed in by our caller, who means ntoskrnl
   //
   if (!_strcmpi(szModuleName, "nt")) {
      szModuleName = szNtos ;
   }

   ListHead = GetNtDebuggerData( PsLoadedModuleList );

   if (!ListHead) {
      dprintf("Couldn't get offset of PsLoadedModuleListHead\n");
      return FALSE;
   } else {
      if ((!ReadMemory((DWORD)ListHead,
                       &List,
                       sizeof(LIST_ENTRY),
                       &Result)) || (Result < sizeof(LIST_ENTRY))) {
         dprintf("Unable to get value of PsLoadedModuleListHead\n");
         return FALSE;
      }
   }

   Next = List.Flink;
   if (Next == NULL) {
      dprintf("PsLoadedModuleList is NULL!\n");
      return FALSE;
   }

   while ((ULONG)Next != ListHead) {

      DataTable = CONTAINING_RECORD(Next,
                                    LDR_DATA_TABLE_ENTRY,
                                    InLoadOrderLinks);
      if ((!ReadMemory((DWORD)DataTable,
                       &DataTableBuffer,
                       sizeof(LDR_DATA_TABLE_ENTRY),
                       &Result)) || (Result < sizeof(LDR_DATA_TABLE_ENTRY))) {
         dprintf("Unable to read LDR_DATA_TABLE_ENTRY at %08lx\n",
                 DataTable);
         return FALSE;
      }

      //
      // Get the base DLL name.
      //
      if ((!ReadMemory((DWORD)DataTableBuffer.BaseDllName.Buffer,
                       UnicodeBuffer,
                       DataTableBuffer.BaseDllName.Length,
                       &Result)) || (Result < DataTableBuffer.BaseDllName.Length)) {
         dprintf("Unable to read name string at %08lx\n",
                 DataTable);
         return FALSE;
      }

      UnicodeBuffer[Result] = UNICODE_NULL ;

      //
      // Is this the driver we passed in?
      //
      sprintf(szFilename, "%S", UnicodeBuffer) ;
      pszDot = strchr(szFilename, '.') ;
      *pszDot = '\0' ;
      if (!_strcmpi(szFilename, szModuleName)) {

         //
         // We have a match, record the base and find the Checksum.
         //
         *ModuleBaseAddress = DataTableBuffer.DllBase ;

         if (!ReadMemory((DWORD) DataTableBuffer.DllBase, &DosHeader, sizeof(DosHeader), &cb)) {
            dprintf("Can't read file header: error == %d\n", GetLastError());
            return FALSE ;
         }

         if (cb != sizeof(DosHeader) || DosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
            dprintf("No file header.\n");
            return FALSE;
         }

         Offset = ((ULONG) DataTableBuffer.DllBase) + DosHeader.e_lfanew;

         if (!ReadMemory((DWORD) Offset, &imageNtHeaders, sizeof(imageNtHeaders), &cb)) {
            dprintf("Can't read optional header: error == %d\n", GetLastError());
            return FALSE;
         }

         if (cb != sizeof(imageNtHeaders)) {
             dprintf("Bad file header.\n");
             return FALSE;
         }

         imageFileHdr = &imageNtHeaders.FileHeader;
         imageOptionalHdr = &imageNtHeaders.OptionalHeader;

         if (imageFileHdr->SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER)) {
            dprintf("Cannot get Checksum, invalid optional header.\n");
            return FALSE;
         }

         *Checksum = imageOptionalHdr->CheckSum ;
         return TRUE ;
      }

      Next = DataTableBuffer.InLoadOrderLinks.Flink;
   }
   dprintf("Unable to locate module %s\n", szModuleName) ;
   return FALSE ;
}

BOOL
ModSetApply(
   PDRIVERENTRY DriverPatchHead
   )
{
   PDRIVERENTRY driver = DriverPatchHead ;
   PDRIVERMOD patch ;
   ULONG displacement, checksum ;
   PVOID address, baseAddress ;
   ULONG Results, patchAddress ;
   BOOL bSuccess ;
   BYTE bytePatch ;
   WORD wordPatch ;
   DWORD dwordPatch ;
   BOOL appliedAtLeastOnePatch = FALSE ;

   while(driver) {
      patch = driver->FirstPatch ;
      while(patch) {

         if (patch->PatchFlags&PATCHFLAG_FIXUPS_COMPLETE) {

            patchAddress = (driver->BaseAddress + patch->ulModuleOffset) ;

            if (patch->AndPatch) {

               switch(APPLIED_PATCH_SIZE(patch)) {

                  case PATCHFLAG_BYTE_PATCH:
                     bSuccess = (ReadMemory((DWORD) patchAddress, &bytePatch, sizeof(BYTE), &Results)&&
                                 (Results == sizeof(BYTE))) ;
                     bytePatch &= (BYTE) (patch->AndPatch&0xFF) ;
                     break;

                  case PATCHFLAG_WORD_PATCH:
                     bSuccess = (ReadMemory((DWORD) patchAddress, &wordPatch, sizeof(WORD), &Results)&&
                                 (Results == sizeof(WORD))) ;
                     wordPatch &= (WORD) (patch->AndPatch&0xFFFF) ;
                     break;

                  case PATCHFLAG_DWORD_PATCH:
                     bSuccess = (ReadMemory((DWORD) patchAddress, &dwordPatch, sizeof(DWORD), &Results)&&
                                 (Results == sizeof(DWORD))) ;
                     dwordPatch &= (DWORD) patch->AndPatch ;
                     break;

                  default:
                     bSuccess = FALSE ;
               }

            } else {

               bytePatch = 0 ;
               wordPatch = 0 ;
               dwordPatch = 0 ;
               bSuccess = TRUE ;
            }

            if (!bSuccess) {
               dprintf("Cannot patch %s!%s, read failed\n", driver->szModuleName, patch->szSymbolicName);

            } else {

               switch(APPLIED_PATCH_SIZE(patch)) {

                  case PATCHFLAG_BYTE_PATCH:
                     bytePatch |= (BYTE) (patch->OrPatch&0xFF) ;
                     bSuccess = (WriteMemory( (ULONG)patchAddress, &bytePatch, sizeof(BYTE), &Results ) &&
                        (Results == sizeof(BYTE))) ;
                     break;

                  case PATCHFLAG_WORD_PATCH:
                     wordPatch |= (WORD) (patch->OrPatch&0xFFFF) ;
                     bSuccess = (WriteMemory( (ULONG)patchAddress, &wordPatch, sizeof(WORD), &Results ) &&
                        (Results == sizeof(WORD))) ;
                     break;

                  case PATCHFLAG_DWORD_PATCH:
                     dwordPatch |= (DWORD) patch->OrPatch ;
                     bSuccess = (WriteMemory( (ULONG)patchAddress, &dwordPatch, sizeof(DWORD), &Results ) &&
                        (Results == sizeof(DWORD))) ;
                     break;

                  default:
                     bSuccess = FALSE ;
               }

               if (!bSuccess) {
                  dprintf("Cannot patch %s!%s, write failed\n", driver->szModuleName, patch->szSymbolicName);
               }
            }

            appliedAtLeastOnePatch |= bSuccess ;
         }

         patch=patch->Sibling ;
      }
      driver=driver->Sibling ;
   }
   return appliedAtLeastOnePatch ;
}

VOID
PatchTreeMenuPrintCallback(
   PPATCH_CATEGORY  Category,
   PPATCH_ENTRY     Entry,
   ULONG            Depth,
   PVOID            Context
   )
{
   char szIndent[1024] ;
   int i ;

   if (Category) {

      dprintf("\nCategory: %s\n", Category->CategoryName) ;
      i=strlen(Category->CategoryName) ;
      i+=sizeof("Category: ") ;
      while(--i) dprintf("-") ;
      dprintf("\n") ;
   }

   if (Entry) {

      memset(szIndent, ' ', Depth*2) ;
      szIndent[Depth*2]='\0' ;

      if (Entry->ShortcutName) {

         dprintf("%s!Patch%s- %s\n",
            szIndent,
            Entry->ShortcutName,
            Entry->MenuDescription
            ) ;
      } else {

         dprintf("%s%s\n",
            szIndent,
            Entry->MenuDescription
            ) ;
      }

      if (Entry->ChildStyle == LISTSTYLE_EXCLUSIVE) {
         dprintf("%s  And up to one of:\n", szIndent) ;
      }
   }
}

VOID
PatchTreeMenuPrint(
   PPATCH_TREE PatchTree
   )
{
   PatchTreeWalk(PatchTree, PatchTreeMenuPrintCallback, NULL, NULL, FALSE) ;
   dprintf("\n") ;
   dprintf("Selecting an indented assert automatically enables all higher asserts\n") ;
   dprintf("Type !Patch <assertname>? to get more information on an assert.\n") ;
}


