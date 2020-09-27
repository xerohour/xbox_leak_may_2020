/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* Component:
*
* File: vulcandlist.h
*
* File Comments:
*
*
***********************************************************************/

#ifndef __DList_H__
#define __DList_H__

#include "vulcanpa.h"   

#define VIRTUALPAD
///////////////////////Normal DList////////////////////////

#define PADDING(x) 
#include "vulcandlisttemplate.h"
#undef PADDING

///////////////////////Padded DList////////////////////////
#define CDList2     CDList_PA
#define CDList2Iter CDList_PA_Iter
#define CDList2Elem CDList_PA_Elem

#define PADDING(x) CPAD(x);
#include "vulcandlisttemplate.h"
#undef PADDING

#undef CDList2
#undef CDList2Iter
#undef CDList2Elem

///////////////////////Padded DList (VTable complient)/////
#define CDList2     CDList_VPA
#define CDList2Iter CDList_VPA_Iter
#define CDList2Elem CDList_VPA_Elem

#undef VIRTUALPAD
#define VIRTUALPAD   CPAD(v);
#define PADDING(x)   CPAD(x);
#include "vulcandlisttemplate.h"
#undef PADDING

#undef CDList2
#undef CDList2Iter
#undef CDList2Elem

///////////////////////DList TYPEDEFS /////////////////////
class CBBlock;
typedef CDList_PA_Elem<CBBlock*> CDListElem_CBBlock;
typedef CDList_PA<CBBlock*> CDList_CBBlock;
typedef CDList_PA_Iter<CBBlock*> CDListIter_CBBlock;
class VImport;
typedef CDList2Elem<VImport*> CDListElem_VImport;
typedef CDList2<VImport*> CDList_VImport;
typedef CDList2Iter<VImport*> CDListIter_VImport;
class VBlock;
typedef CDList_PA_Elem<VBlock*> CDListElem_VBlock;
typedef CDList_PA<VBlock*> CDList_VBlock;
typedef CDList_PA_Iter<VBlock*> CDListIter_VBlock;

typedef CDList_PA_Elem<VBlock*> CDListElem_VBBlock;
typedef CDList_PA<VBlock*> CDList_VBBlock;
typedef CDList_PA_Iter<VBlock*> CDListIter_VBBlock;
class CProtoList;
typedef CDList2Elem<CProtoList*> CDListElem_CProtoList;
typedef CDList2<CProtoList*> CDList_CProtoList;
typedef CDList2Iter<CProtoList*> CDListIter_CProtoList;
class IDFFile;
typedef CDList2Elem<IDFFile*> CDListElem_IDFFile;
typedef CDList2<IDFFile*> CDList_IDFFile;
typedef CDList2Iter<IDFFile*> CDListIter_IDFFile;
class INLINE_EDGE;
typedef CDList2Elem<INLINE_EDGE*> CDListElem_INLINE_EDGE;
typedef CDList2<INLINE_EDGE*> CDList_INLINE_EDGE;
typedef CDList2Iter<INLINE_EDGE*> CDListIter_INLINE_EDGE;
struct HISTORY_ListNode;
typedef CDList2Elem<HISTORY_ListNode*> CDListElem_HISTORY_ListNode;
typedef CDList2<HISTORY_ListNode*> CDList_HISTORY_ListNode;
typedef CDList2Iter<HISTORY_ListNode*> CDListIter_HISTORY_ListNode;
class VInst;
typedef CDList_PA_Elem<VInst*> CDListElem_VInst;
typedef CDList_PA<VInst*> CDList_VInst;
typedef CDList_PA_Iter<VInst*> CDListIter_VInst;
class VSect;
typedef CDList2Elem<VSect*> CDListElem_VSect;
typedef CDList2<VSect*> CDList_VSect;
typedef CDList2Iter<VSect*> CDListIter_VSect;
class VComp;
typedef CDList2Elem<VComp*> CDListElem_VComp;
typedef CDList2<VComp*> CDList_VComp;
typedef CDList2Iter<VComp*> CDListIter_VComp;
class VProg;
typedef CDList2Elem<VProg*> CDListElem_VProg;
typedef CDList2<VProg*> CDList_VProg;
typedef CDList2Iter<VProg*> CDListIter_VProg;
class VExport;
typedef CDList2Elem<VExport*> CDListElem_VExport;
typedef CDList2<VExport*> CDList_VExport;
typedef CDList2Iter<VExport*> CDListIter_VExport;
class VProc;
typedef CDList_VPA_Elem<VProc*> CDListElem_VProc;
typedef CDList_VPA<VProc*> CDList_VProc;
typedef CDList_VPA_Iter<VProc*> CDListIter_VProc;
class VSymbol;
typedef CDList2Elem<VSymbol*> CDListElem_VSymbol;
typedef CDList2<VSymbol*> CDList_VSymbol;
typedef CDList2Iter<VSymbol*> CDListIter_VSymbol;


#undef INLINE_DLIST
#endif
