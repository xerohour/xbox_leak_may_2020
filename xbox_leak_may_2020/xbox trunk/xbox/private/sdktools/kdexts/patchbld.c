#include "precomp.h"
#include "patchp.h"
#pragma hdrstop

#define PATCHASSERT(x) { if (!(x)) { dprintf("Assertion on file %s line %d\n", __FILE__, __LINE__); ASSERT(x); } }

//
// These functions build the assertion tree.
//

PPATCH_TREE gpPatchTree ;

VOID
PatchCategoryBegin(
   PPATCH_TREE PatchTree,
   char *CategoryName
   )
{
   PPATCH_CATEGORY newCategory ;

   PATCHASSERT(!PatchTree->InCategory) ;
   PatchTree->InCategory = TRUE ;
   newCategory = (PPATCH_CATEGORY) malloc(sizeof(PATCH_CATEGORY)) ;
   if (newCategory) {
      newCategory->CategoryName = CategoryName ;
      newCategory->ChildStyle = LISTSTYLE_NONE ;
      newCategory->Child = NULL ;
      newCategory->Sibling = NULL ;
      if (PatchTree->CurrentCategory) {
         PatchTree->CurrentCategory->Sibling = newCategory ;
      } else {
         PatchTree->FirstCategory = newCategory ;
      }
      PatchTree->CurrentCategory = newCategory ;
   }
}

VOID
PatchCategoryEnd(
   PPATCH_TREE PatchTree
   )
{
   PATCHASSERT(PatchTree->InCategory) ;
   PatchTree->InCategory = FALSE ;

   //
   // We don't zero out the CurrentCategory as the next category will be
   // linked into the list through it.
   //
}

VOID
PatchSiblingListBegin(
   PPATCH_TREE PatchTree,
   LIST_STYLE ListStyle
   )
{
   PATCHASSERT(PatchTree->InCategory) ;
   //dprintf("[ %x %x\n", PatchTree, PatchTree->LastEntry);
   if (PatchTree->LastEntry == NULL) {
       PATCHASSERT(PatchTree->CurrentCategory->ChildStyle == LISTSTYLE_NONE) ;
       PatchTree->CurrentCategory->ChildStyle = ListStyle ;
   } else {
       PATCHASSERT(PatchTree->LastEntry->ChildStyle == LISTSTYLE_NONE) ;
       PatchTree->LastEntry->ChildStyle = ListStyle ;
   }
   PatchTree->LastParent = PatchTree->LastEntry ;
   PatchTree->LastEntry = NULL ;
}

VOID
PatchSiblingListEnd(
   PPATCH_TREE PatchTree
   )
{
   PATCHASSERT(PatchTree->InCategory) ;
   PATCHASSERT(PatchTree->CurrentCategory->Child) ;

   //dprintf("] %x of %x\n", PatchTree, PatchTree->LastParent);
   if (PatchTree->LastParent == NULL) {
       PATCHASSERT(PatchTree->CurrentCategory->ChildStyle != LISTSTYLE_NONE) ;
   } else {
       PATCHASSERT(PatchTree->LastParent->ChildStyle != LISTSTYLE_NONE) ;
   }

   PATCHASSERT(PatchTree->LastEntry) ;
   PatchTree->LastEntry = PatchTree->LastParent ;
   if (PatchTree->LastParent) {
      PatchTree->LastParent = PatchTree->LastParent->Parent ;
   }
}

char *
PatchMallocSprintf(
   char *szFormat, ...
   )
{
   va_list arglist;
   char Buffer[1024], *szReturn ;
   int cb;

   va_start(arglist, szFormat);

   cb = _vsnprintf(Buffer, sizeof(Buffer), szFormat, arglist);
   if (cb == -1) {             // detect buffer overflow
      Buffer[sizeof(Buffer) - 1] = '\0';
      cb = sizeof(Buffer)-1 ;
   }
   szReturn = malloc(cb+1) ;
   memcpy(szReturn, Buffer, cb+1) ;
   return szReturn ;
}

VOID
PatchInsertEntry(
   PPATCH_TREE PatchTree,
   PPATCH_ENTRY NewEntry
   )
{
   PATCHASSERT(PatchTree->InCategory) ;

   NewEntry->Sibling = NULL ;
   NewEntry->Child = NULL ;
   if (PatchTree->LastEntry) {
      PatchTree->LastEntry->Sibling = NewEntry ;
      NewEntry->Parent = PatchTree->LastEntry->Parent ;
   } else {
      NewEntry->Parent = PatchTree->LastParent ;
      if (PatchTree->LastParent) {
         PatchTree->LastParent->Child = NewEntry ;
      } else {
         PatchTree->CurrentCategory->Child = NewEntry ;
      }
   }
   PatchTree->LastEntry = NewEntry ;
}

VOID
PatchDeclareEntry(
   PPATCH_TREE PatchTree,
   char *Action,
   char *ShortCut,
   char *MenuDescription,
   char *LengthyDescription
   )
{
   PPATCH_ENTRY newEntry ;

   newEntry = (PPATCH_ENTRY) malloc(sizeof(PATCH_ENTRY)) ;
   if (newEntry) {
      newEntry->EntryType = ENTRYTYPE_PATCH ;
      newEntry->Action = Action ;
      if (!strcmp(ShortCut, " " NO_SHORTCUT " ")) {
         newEntry->ShortcutName = NULL ;
      } else {
         newEntry->ShortcutName = ShortCut ;
      }
      newEntry->MenuDescription = MenuDescription ;
      newEntry->LengthyDescription = LengthyDescription ;
      newEntry->ChildStyle = LISTSTYLE_NONE ;
      PatchInsertEntry(PatchTree, newEntry) ;
   } else {
      free(Action) ;
   }
}

VOID
PatchDeclareCollection(
   PPATCH_TREE PatchTree,
   char *Action,
   char *ShortCut,
   char *MenuDescription,
   char *LengthyDescription
   )
{
   PPATCH_ENTRY newEntry ;

   newEntry = (PPATCH_ENTRY) malloc(sizeof(PATCH_ENTRY)) ;
   if (newEntry) {
      newEntry->EntryType = ENTRYTYPE_PATCH_COLLECTION ;
      newEntry->Action = Action ;
      if (!strcmp(ShortCut, " " NO_SHORTCUT " ")) {
         newEntry->ShortcutName = NULL ;
      } else {
         newEntry->ShortcutName = ShortCut ;
      }
      newEntry->MenuDescription = MenuDescription ;
      newEntry->LengthyDescription = LengthyDescription ;
      newEntry->ChildStyle = LISTSTYLE_NONE ;
      PatchInsertEntry(PatchTree, newEntry) ;
   } else {
      free(Action) ;
   }
}

VOID
PatchBuildTree(
   PPATCH_TREE pTree
   )
{
   //
   // Initialize the Tree
   //
   memset(pTree, 0, sizeof(PATCH_TREE)) ;

   //
   // This is the global to include into
   //
   gpPatchTree = pTree ;

   //
   // Generate the code to read the table...
   //
   #include "patch.h"
}


