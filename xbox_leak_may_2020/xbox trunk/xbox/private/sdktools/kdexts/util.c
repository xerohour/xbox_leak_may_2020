/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    util.c

Abstract:

    WinDbg Extension Api

Author:

    Wesley Witt (wesw) 15-Aug-1993

Environment:

    User Mode.

Revision History:

--*/


#include "precomp.h"
#pragma hdrstop


ULONG
GetUlongFromAddress (
    ULONG Location
    )
{
    ULONG Value;
    ULONG result;

    if ((!ReadMemory((DWORD)Location,&Value,sizeof(ULONG),&result)) ||
        (result < sizeof(ULONG))) {
        dprintf("unable to read from %08x\n",Location);
        return 0;
    }

    return Value;
}

ULONG
GetUlongValue (
    PCHAR String
    )
{
    ULONG Location;
    ULONG Value;
    ULONG result;


    Location = GetExpression( String );
    if (!Location) {
        dprintf("unable to get %s\n",String);
        return 0;
    }

    return GetUlongFromAddress(Location);
}


VOID
DumpImageName(
    IN PKPROCESS ProcessContents
    )
{
    dprintf("%ws",L"System Process");
}

ULONG
DumpSplayTree(
    IN PVOID pSplayLinks,
    IN PDUMP_SPLAY_NODE_FN DumpNodeFn
    )
/*++
    Purpose:

        Perform an in-order iteration across a splay tree, calling a
        user supplied function with a pointer to each RTL_SPLAY_LINKS
        structure encountered in the tree, and the level in the tree
        at which it was encountered (zero based).

    Arguments:

        pSplayLinks     - pointer to root of a splay tree

        DumpNodeFn      - user supplied dumping function

   Returns:

        Count of nodes encountered in the tree.

   Notes:

        Errors reading memory do not terminate the iteration if more
        work is possible.

        Consumes the Control-C flag to terminate possible loops in
        corrupt structures.

--*/
{
    RTL_SPLAY_LINKS SplayLinks, Parent;
    ULONG Level = 0;
    ULONG NodeCount = 0;

    if (pSplayLinks) {

        //
        //  Retrieve the root links, find the leftmost node in the tree
        //

        if (!ReadAtAddress(pSplayLinks,
                            &SplayLinks,
                            sizeof(RTL_SPLAY_LINKS),
                            &pSplayLinks)) {

            return NodeCount;
        }

        while (DbgRtlLeftChild(SplayLinks) != NULL) {

            if ( CheckControlC() ) {

                return NodeCount;
            }

            if (!ReadAtAddress(DbgRtlLeftChild(SplayLinks),
                                &SplayLinks,
                                sizeof(RTL_SPLAY_LINKS),
                                &pSplayLinks)) {

                //
                //  We can try to continue from this
                //

                break;
            }

            Level++;
        }

        while (TRUE) {

            if ( CheckControlC() ) {

                return NodeCount;
            }

            NodeCount++;
            (*DumpNodeFn)(pSplayLinks, Level);

            /*
                first check to see if there is a right subtree to the input link
                if there is then the real successor is the left most node in
                the right subtree.  That is find and return P in the following diagram

                      Links
                         \
                          .
                         .
                        .
                       /
                      P
                       \
            */

            if (DbgRtlRightChild(SplayLinks) != NULL) {

                if (!ReadAtAddress(DbgRtlRightChild(SplayLinks),
                                    &SplayLinks,
                                    sizeof(RTL_SPLAY_LINKS),
                                    &pSplayLinks)) {

                    //
                    //  We've failed to step through to a successor, so
                    //  there is no more to do
                    //

                    return NodeCount;
                }

                Level++;

                while (DbgRtlLeftChild(SplayLinks) != NULL) {

                    if ( CheckControlC() ) {

                        return NodeCount;
                    }

                    if (!ReadAtAddress(DbgRtlLeftChild(SplayLinks),
                                        &SplayLinks,
                                        sizeof(RTL_SPLAY_LINKS),
                                        &pSplayLinks)) {

                        //
                        //  We can continue from this
                        //

                        break;
                    }

                    Level++;
                }

            } else {

                /*
                    we do not have a right child so check to see if have a parent and if
                    so find the first ancestor that we are a left decendent of. That
                    is find and return P in the following diagram

                               P
                              /
                             .
                              .
                               .
                              Links
                */

                //
                //  If the IsLeft or IsRight functions fail to read through a parent
                //  pointer, then we will quickly exit through the break below
                //

                while (DbgRtlIsRightChild(SplayLinks, pSplayLinks, &Parent)) {

                    if ( CheckControlC() ) {

                        return NodeCount;
                    }

                    Level--;
                    pSplayLinks = DbgRtlParent(SplayLinks);
                    SplayLinks = Parent;
                }

                if (!DbgRtlIsLeftChild(SplayLinks, pSplayLinks, &Parent)) {

                    //
                    //  we do not have a real successor so we break out
                    //

                    break;

                } else {

                    Level--;
                    pSplayLinks = DbgRtlParent(SplayLinks);
                    SplayLinks = Parent;
                }
            }
        }
    }

    return NodeCount;
}


VOID
DumpUnicode(
    UNICODE_STRING u
    )
{
    UNICODE_STRING v;
    DWORD BytesRead;

    if ((u.Length <= u.MaximumLength) &&
        (u.Buffer) &&
        (u.Length > 0)) {

        v.Buffer = LocalAlloc(LPTR, u.MaximumLength);
        if (v.Buffer != NULL) {
            v.MaximumLength = u.MaximumLength;
            v.Length = u.Length;
            if (ReadAtAddress(u.Buffer,
                              v.Buffer,
                              u.Length,
                              &u.Buffer)) {
                dprintf("%wZ", &v);
            } else {
                dprintf("<???>");
            }
            LocalFree(v.Buffer);

            return;
        }
    }
}

BOOLEAN
IsHexNumber(
   const char *szExpression
   )
{
   if (!szExpression[0]) {
      return FALSE ;
   }

   for(;*szExpression; szExpression++) {
      
      if      ((*szExpression)< '0') { return FALSE ; } 
      else if ((*szExpression)> 'f') { return FALSE ; }
      else if ((*szExpression)>='a') { continue ;     }
      else if ((*szExpression)> 'F') { return FALSE ; }
      else if ((*szExpression)<='9') { continue ;     }
      else if ((*szExpression)>='A') { continue ;     }
      else                           { return FALSE ; }
   }
   return TRUE ;
}


BOOLEAN
IsDecNumber(
   const char *szExpression
   )
{
   if (!szExpression[0]) {
      return FALSE ;
   }

   while(*szExpression) {
      
      if      ((*szExpression)<'0') { return FALSE ; } 
      else if ((*szExpression)>'9') { return FALSE ; }
      szExpression ++ ;
   }
   return TRUE ;
}

