#include <stdio.h>
#pragma hdrstop
#include "\BLib\inc\BLib.h"

void main (void)
{
    //
    // create linked list of integers
    //
    BLinkedList<int,int> list1;

    //
    // add some data (0,1,2,3,4)
    //
    list1.addTail (2);
    list1.addHead (1);
    list1.addTail (3);
    list1.addTail (4);
    list1.addHead (0);

    //
    // remove at endpoints
    //
    list1.removeHead();
    list1.removeTail();

    //
    // front to back traversal
    //
    BPosition pos;
    pos = list1.getHeadPosition();
    while (pos)
    {
        int i = list1.getNextPosition(pos);
        printf ("%d ",i);
    }
    printf ("\n");

    //
    // back to front traversal
    //
    pos = list1.getTailPosition();
    while (pos)
    {
        int i = list1.getPreviousPosition(pos);
        printf ("%d ",i);
    }
    printf ("\n");

    //
    // add more data
    //
    list1.addTail (2);
    list1.addHead (1);

    //
    // find entries from front of list
    //
    pos = list1.findForward(2,list1.getHeadPosition());
    while (pos)
    {
        // have to extract data so 'pos' can advance to next element
        int i = list1.getNextPosition(pos);
        printf ("%d ",i);
        // find next match
        pos = list1.findForward(2,pos);
    }
    printf ("\n");

    //
    // find entries from back of list
    //
    pos = list1.findBackward(1,list1.getTailPosition());
    while (pos)
    {
        // have to extract data so 'pos' can advance to next element
        int i = list1.getPreviousPosition(pos);
        printf ("%d ",i);
        // find next match
        pos = list1.findBackward(1,pos);
    }
    printf ("\n");

    //
    // remove all '2's
    //
    pos = list1.findForward(2,list1.getHeadPosition());
    while (pos)
    {
        list1.removeAt (pos);
        printf ("* ");
        // find next match
        pos = list1.findForward(2,list1.getHeadPosition());
    }
    printf ("\n");
}