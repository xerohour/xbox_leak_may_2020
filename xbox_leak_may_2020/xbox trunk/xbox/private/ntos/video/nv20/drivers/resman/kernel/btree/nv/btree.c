 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** Balanced Tree *******************************\
*                                                                           *
* Module: BTREE.C                                                           *
*   A generic library to balanced tree management is provided.              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nvrm.h>
#include <class.h>
#include <btree.h>
#include <os.h>
//
// Debugging support.
//
#ifdef DEBUG
//
// Validate node.
//
#define VALIDATE_NODE(pn)                   \
{                                           \
    if (btreeNodeValidate(pn) != RM_OK)     \
        return (RM_ERROR);                  \
}
#define VALIDATE_TREE(pt)                   \
{                                           \
    if (btreeTreeValidate(pt) != RM_OK)     \
        return (RM_ERROR);                  \
}
//
// Prototype validation routines.
//
RM_STATUS btreeNodeValidate(PNODE);
RM_STATUS btreeTreeValidate(PNODE);
RM_STATUS btreeDumpBranch(PNODE, U032);
RM_STATUS btreeDumpTree(PNODE);
//
// Validate a nodes branch and count values.
//
RM_STATUS btreeNodeValidate
(
    PNODE Node
)
{
    RM_STATUS status;
    
    status = RM_OK;
    if (Node == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR validating NULL NODE.\n\r");
        DBG_BREAKPOINT();
        return (RM_ERROR);
    }
    if (((Node->LeftCount != 0) && (Node->LeftBranch == NULL))
    ||  ((Node->LeftCount == 0) && (Node->LeftBranch != NULL)))
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR inconsistent left branch, Value = ", Node->Value);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                       Left count = ", Node->LeftCount);
        DBG_PRINT_STRING_PTR  (DEBUGLEVEL_ERRORS, "                                      Left branch = ", Node->LeftBranch);
        DBG_BREAKPOINT();
        status = RM_ERROR;
    }
    if (((Node->RightCount != 0) && (Node->RightBranch == NULL))
    ||  ((Node->RightCount == 0) && (Node->RightBranch != NULL)))
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR inconsistent right branch, Value = ", Node->Value);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                       Right count = ", Node->RightCount);
        DBG_PRINT_STRING_PTR  (DEBUGLEVEL_ERRORS, "                                      Right branch = ", Node->RightBranch);
        DBG_BREAKPOINT();
        status = RM_ERROR;
    }
    return (status);
}
//
// Validate entire BTREE.
//
RM_STATUS btreeBranchValidate
(
    PNODE Node,
    U032 *NodeCount
)
{
    RM_STATUS status;
    U032 LeftCount;
    U032 RightCount;
    
    status = RM_OK;
    if (Node)
    {
        btreeNodeValidate(Node);
        if (Node->LeftBranch)
            status = btreeBranchValidate(Node->LeftBranch, &LeftCount);
        else
            LeftCount = 0;
        if (Node->RightBranch)
            status = btreeBranchValidate(Node->RightBranch, &RightCount);
        else
            RightCount = 0;
        if (Node->LeftCount != LeftCount)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR inconsistent left count, Value = ", Node->Value);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                      Left count = ", Node->LeftCount);
            DBG_PRINT_STRING_PTR  (DEBUGLEVEL_ERRORS, "                                     Left branch = ", Node->LeftBranch);
            DBG_BREAKPOINT();
            status = RM_ERROR;
        }
        if (Node->RightCount != RightCount)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR inconsistent right count, Value = ", Node->Value);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                      Right count = ", Node->RightCount);
            DBG_PRINT_STRING_PTR  (DEBUGLEVEL_ERRORS, "                                     Right branch = ", Node->RightBranch);
            DBG_BREAKPOINT();
            status = RM_ERROR;
        }
        *NodeCount = LeftCount + RightCount + 1;
    }
    return (status);
}
RM_STATUS btreeTreeValidate
(
    PNODE Trunk
)
{
    RM_STATUS status;
    U032      LeftCount;
    U032      RightCount;
    
    status = RM_OK;
    if (Trunk)
    {
        status = btreeNodeValidate(Trunk);
        if (Trunk->LeftBranch)
            status |= btreeBranchValidate(Trunk->LeftBranch, &LeftCount);
        else
            LeftCount = 0;
        if (Trunk->RightBranch)
            status |= btreeBranchValidate(Trunk->RightBranch, &RightCount);
        else
            RightCount = 0;
        if (Trunk->LeftCount != LeftCount)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR inconsistent left count, Value = ", Trunk->Value);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                      Left count = ", Trunk->LeftCount);
            DBG_PRINT_STRING_PTR  (DEBUGLEVEL_ERRORS, "                                     Left branch = ", Trunk->LeftBranch);
            DBG_BREAKPOINT();
            status = RM_ERROR;
        }
        if (Trunk->RightCount != RightCount)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ERROR inconsistent right count, Value = ", Trunk->Value);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                      Right count = ", Trunk->RightCount);
            DBG_PRINT_STRING_PTR  (DEBUGLEVEL_ERRORS, "                                     Right branch = ", Trunk->RightBranch);
            DBG_BREAKPOINT();
            status = RM_ERROR;
        }
    }
    if (status)
        btreeDumpTree(Trunk);
    return (status);
}
//
// Dump current tree to debug port.
//
RM_STATUS btreeDumpBranch
(
    PNODE Node,
    U032  Level
)
{
    U032 i;

    if (Node)
    {
        VALIDATE_NODE(Node);
        btreeDumpBranch(Node->RightBranch, Level + 1);
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM_BTREE: ");
        for (i = 0; i < Level; i++)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, ".");
        }
        DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "Node         = ", Node);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "Value        = ", Node->Value);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "Left count   = ", Node->LeftCount);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "Right count  = ", Node->RightCount);
        DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "Left branch  = ", Node->LeftBranch);
        DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "Right branch = ", Node->RightBranch);
        btreeDumpBranch(Node->LeftBranch, Level + 1);
    }
    return (RM_OK);
}
RM_STATUS btreeDumpTree
(
    PNODE Trunk
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM_BTREE: ======================== Tree Dump ==========================\n\r");
    if (Trunk == NULL)
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM_BTREE: NULL\n\r");
    else
        btreeDumpBranch(Trunk, 0);
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM_BTREE: =============================================================\n\r");
    return (RM_OK);
}
#else
//
// Validate nothing.
//
#define VALIDATE_NODE(pn)
#define VALIDATE_TREE(pt)
#endif // DEBUG
//
// Insert new node into tree.
//
RM_STATUS btreeInsertBranch
(
    PNODE Node, 
    PNODE BranchNode
)
{
    RM_STATUS status;
    
    //
    // Check for duplicate names.
    //
    VALIDATE_NODE(BranchNode);
    if (Node->Value == BranchNode->Value)
        return (RM_ERR_INSERT_DUPLICATE_NAME);
    status = RM_OK;    
    if (Node->Value < BranchNode->Value)
    {
        //
        // Insert into right branch.
        //
        if (BranchNode->RightBranch)
        {
            status = btreeInsertBranch(Node, BranchNode->RightBranch);
        }
        else
        {
            BranchNode->RightBranch = Node;
            Node->RightCount        = 0;
            Node->LeftCount         = 0;
            Node->RightBranch       = NULL;
            Node->LeftBranch        = NULL;
        }
        if (status == RM_OK)    
            BranchNode->RightCount++;
    }
    else
    {
        //
        // Insert into left branch.
        //
        if (BranchNode->LeftBranch)
        {
            status = btreeInsertBranch(Node, BranchNode->LeftBranch);
        }
        else
        {
            BranchNode->LeftBranch = Node;
            Node->RightCount       = 0;
            Node->LeftCount        = 0;
            Node->RightBranch      = NULL;
            Node->LeftBranch       = NULL;
        }
        if (status == RM_OK)    
            BranchNode->LeftCount++;
    }
    return (status);
}
RM_STATUS btreeInsert
(
    PNODE  Node,
    PNODE *Trunk
)
{
    RM_STATUS status;

#ifdef DEBUG
    if (Node == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM_BTREE: Inserting NULL into BTREE!\n\r");
        DBG_BREAKPOINT();
    }
#endif // DEBUG    
    status = RM_OK;
    if (*Trunk == NULL)
    {
        *Trunk            = Node;
        Node->RightCount  = 0;
        Node->LeftCount   = 0;
        Node->RightBranch = NULL;
        Node->LeftBranch  = NULL;
    }
    else
    {
        status = btreeInsertBranch(Node, *Trunk);
    }
    VALIDATE_TREE(*Trunk);
    return (status);
}
//
// Remove node from tree.
//
RM_STATUS btreeDelete
(
    U032   Value,
    PNODE *Trunk
)
{
    PNODE Node;
    PNODE ParentNode;
    PNODE PrevNode;
    PNODE SearchNode;

    if (*Trunk == NULL)
        return (RM_ERROR);
    Node       = *Trunk;
    SearchNode = NULL;
    ParentNode = NULL;
    VALIDATE_TREE(*Trunk);
    //
    // Search tree for node being deleted.
    //
    while (Value != Node->Value)
    {
        ParentNode = Node;
        if (Value > Node->Value)
            Node = Node->LeftBranch;
        else
            Node = Node->RightBranch;
        if (Node == NULL)
            //
            // Not found.
            //
            return (RM_ERROR);
    }
    //
    // Check for branches of node to be deleted.
    //
    if (Node->RightCount + Node->LeftCount > 0)
    {
        PrevNode = NULL;
        //
        // Find best candidate to fill in place of deleted node.
        //
        if (Node->RightCount > Node->LeftCount)
        {
            //
            // Get next higher node.
            //
            SearchNode = Node->RightBranch;
            Node->RightCount--;
            while (SearchNode->LeftBranch)
            {
                SearchNode->LeftCount--;
                PrevNode   = SearchNode;
                SearchNode = SearchNode->LeftBranch;
            }
            if (PrevNode)
                PrevNode->LeftBranch = SearchNode->RightBranch;
            else
                Node->RightBranch = SearchNode->RightBranch;
        }
        else
        {
            //
            // Get next lower node.
            //
            SearchNode = Node->LeftBranch;
            Node->LeftCount--;
            while (SearchNode->RightBranch)
            {
                SearchNode->RightCount--;
                PrevNode   = SearchNode;
                SearchNode = SearchNode->RightBranch;
            }
            if (PrevNode)
                PrevNode->RightBranch  = SearchNode->LeftBranch;
            else
                Node->LeftBranch = SearchNode->LeftBranch;
        }
        //
        // Replace deleted node with best candidate.
        //
        SearchNode->LeftBranch  = Node->LeftBranch;
        SearchNode->LeftCount   = Node->LeftCount;
        SearchNode->RightBranch = Node->RightBranch;
        SearchNode->RightCount  = Node->RightCount;
    }
    //
    // Update parent branch pointers.
    //
    if (ParentNode)
    {
        if (ParentNode->LeftBranch == Node)
            ParentNode->LeftBranch  = SearchNode;
        else
            ParentNode->RightBranch = SearchNode;
    }
    else
    {
       *Trunk = SearchNode;
    }
    //
    // Update branch counts up to the deleted node.
    //    
    PrevNode = *Trunk;
    while (PrevNode && (PrevNode != SearchNode))
    {
        if (Value > PrevNode->Value)
        {
            PrevNode->LeftCount--;
            PrevNode = PrevNode->LeftBranch;
        }
        else
        {
            PrevNode->RightCount--;
            PrevNode = PrevNode->RightBranch;
        }
    }
    Node->RightCount  = 0;
    Node->LeftCount   = 0;
    Node->RightBranch = NULL;
    Node->LeftBranch  = NULL;
    VALIDATE_TREE(*Trunk);
    return (RM_OK);
}
//
// Search for node in tree.
//
RM_STATUS btreeSearch
(
    U032   Value,
    PNODE *Node,
    PNODE  Trunk
)
{
    VALIDATE_TREE(Trunk);
    *Node = Trunk;
    //
    // Search for Node Value.
    //
    while (*Node)
    {
        if (Value == (*Node)->Value)
            return (RM_OK);
        if (Value > (*Node)->Value)
            *Node = (*Node)->LeftBranch;
        else
            *Node = (*Node)->RightBranch;
    }
    return (RM_ERR_OBJECT_NOT_FOUND);
}
//
// Enumerate tree.
//
RM_STATUS btreeEnumStart
(
    U032   Value,
    PNODE *Node,
    PNODE  Trunk
)
{
    PNODE EnumNode;
    
    *Node    = Trunk;
    EnumNode = NULL;
    //
    // Search for Node Value.
    //
    while (*Node)
    {
        VALIDATE_NODE(*Node);
        if (Value == (*Node)->Value)
            return (RM_OK);
        if (Value > (*Node)->Value)
            *Node = (*Node)->LeftBranch;
        else
        {
            //
            // Save next nearest node.
            //
            EnumNode = *Node;
            *Node    = (*Node)->RightBranch;
        }
    }
    *Node = EnumNode;
    return (RM_OK);
}
RM_STATUS btreeEnumNext
(
    PNODE *Node,
    PNODE  Trunk
)
{
    PNODE EnumNode;
    U032  Value;
    
    VALIDATE_NODE(*Node);
    if ((*Node)->LeftBranch)
    {
        //
        // Start where we left off.
        //
        Value     = (*Node)->Value + 1;
        *Node    = (*Node)->LeftBranch;
        EnumNode = *Node;
        while (*Node)
        {
            VALIDATE_NODE(*Node);
            if (Value == (*Node)->Value)
                return (RM_OK);
            if (Value > (*Node)->Value)
                *Node = (*Node)->LeftBranch;
            else
            {
                //
                // Save next nearest node.
                //
                EnumNode = *Node;
                *Node    = (*Node)->RightBranch;
            }
        }
        *Node = EnumNode;
        return (RM_OK);
    }
    else
        return (btreeEnumStart((*Node)->Value + 1, Node, Trunk));
}
//
// Balance branch.
//
RM_STATUS btreeBalanceBranch
(
    PNODE *Node
)
{
    PNODE SearchNode;
    PNODE ParentNode;

    VALIDATE_NODE(*Node);
    while ((((*Node)->RightCount > (*Node)->LeftCount)
          ? ((*Node)->RightCount - (*Node)->LeftCount)
          : ((*Node)->LeftCount  - (*Node)->RightCount)) > 1)
    {
        ParentNode = NULL;
        if ((*Node)->RightCount > (*Node)->LeftCount)
        {
            SearchNode = (*Node)->RightBranch;
            while (SearchNode->LeftBranch)
            {
                ParentNode = SearchNode;
                SearchNode->LeftCount--;
                SearchNode = SearchNode->LeftBranch;
            }
            if (ParentNode)
            {
                ParentNode->LeftBranch  = SearchNode->RightBranch;
                SearchNode->RightBranch = (*Node)->RightBranch;
                SearchNode->RightCount  = (*Node)->RightCount - 1;
            }
            else
            {
                (*Node)->RightBranch = NULL;
                (*Node)->RightCount  = 0;
            }
            SearchNode->LeftBranch     = *Node;
            SearchNode->LeftCount      = (*Node)->LeftCount + 1;
            (*Node)->RightBranch = NULL;
            (*Node)->RightCount  = 0;
        }
        else
        {
            SearchNode = (*Node)->LeftBranch;
            while (SearchNode->RightBranch)
            {
                ParentNode = SearchNode;
                SearchNode->RightCount--;
                SearchNode = SearchNode->RightBranch;
            }
            if (ParentNode)
            {
                ParentNode->RightBranch = SearchNode->LeftBranch;
                SearchNode->LeftBranch  = (*Node)->LeftBranch;
                SearchNode->LeftCount   = (*Node)->LeftCount - 1;
            }
            else
            {
                (*Node)->LeftBranch = NULL;
                (*Node)->LeftCount  = 0;
            }
            SearchNode->RightBranch   = *Node;
            SearchNode->RightCount    = (*Node)->RightCount + 1;
            (*Node)->LeftBranch = NULL;
            (*Node)->LeftCount  = 0;
        }
        *Node = SearchNode;
    }
    if ((*Node)->RightBranch)
        btreeBalanceBranch(&((*Node)->RightBranch));
    if ((*Node)->LeftBranch)
        btreeBalanceBranch(&((*Node)->LeftBranch));
    return (RM_OK);
}
//
// Balance tree.
//
RM_STATUS btreeBalance
(
    PNODE *Trunk
)
{
    RM_STATUS status;
    
    status = RM_OK;
    if (*Trunk)
    {
        VALIDATE_TREE(*Trunk);
        status = btreeBalanceBranch(Trunk);
        VALIDATE_TREE(*Trunk);
    }
    return (status);    
}
