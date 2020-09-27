#ifndef _BTREE_H_
#define _BTREE_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/*********************** Balanced Tree data structure **********************\
*                                                                           *
* Module: BTREE.H                                                           *
*       API to BTREE routines.                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    01/21/95 - broke into seperate file.    *
*                                                                           *
\***************************************************************************/

//
// BTREE structure.
//
typedef struct _def_node
{
    U032              Value;
    VOID             *Data;
    U032              LeftCount;
    U032              RightCount;
    struct _def_node *LeftBranch;
    struct _def_node *RightBranch;
} NODE, *PNODE;

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

RM_STATUS btreeInsertBranch(PNODE, PNODE);
RM_STATUS btreeInsert(PNODE, PNODE *);
RM_STATUS btreeDelete(U032, PNODE *);
RM_STATUS btreeSearch(U032, PNODE *, PNODE);
RM_STATUS btreeEnumStart(U032, PNODE *, PNODE);
RM_STATUS btreeEnumNext(PNODE *, PNODE);
RM_STATUS btreeBalanceBranch(PNODE *);
RM_STATUS btreeBalance(PNODE *);

#endif // _BTREE_H_
