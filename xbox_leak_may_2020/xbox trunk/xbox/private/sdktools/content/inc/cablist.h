/*
 * cablist.h
 *
 * List handling routines
 */

#ifndef _CAB_LIST_H

#define _CAB_LIST_H

/*
 * List element
 */
struct cab_file_link
{
   struct cab_file_link *fl_next;

   /*
    * Logical filename to use (may be NULL).
    * Used if reading a file list from a file (-l option).
    */
   byte *                fl_logical_filename;

   /* physical filename, variable length... */
   byte                  fl_filename[1];
};


/*
 * List header
 */
typedef struct
{
   struct cab_file_link *flh_head;
   struct cab_file_link *flh_tail;
} cab_file_list_header;

#endif /* _CAB_LIST_H */
