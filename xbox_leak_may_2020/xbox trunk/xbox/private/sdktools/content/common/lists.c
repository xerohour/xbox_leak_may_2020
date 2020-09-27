/*
 * lists.c
 *
 * List handling routines
 */
#include "cabarc.h"
#include "oldnames.h"


void init_list(cab_file_list_header *header)
{
	memset(header, 0, sizeof(*header));
}


bool add_to_list(
   cab_file_list_header *header,
   const byte *filename,
   const byte *logical_filename // may be NULL
)
{
   struct cab_file_link *new_link;
   long                  alloc_size;

   alloc_size = sizeof(struct cab_file_link) + strlen(filename) + 1;

   // allocate space for the logical filename if it is present
   if (logical_filename != NULL)
      alloc_size += (strlen(logical_filename)+1);

   new_link = (struct cab_file_link *) mem_malloc(alloc_size);

   if (new_link == NULL)
      return false;

   memset(new_link, 0, sizeof(struct cab_file_link));

   strcpy(new_link->fl_filename, filename);

   if (logical_filename != NULL)
   {
      // logical filename points to memory right after filename
      new_link->fl_logical_filename = &new_link->fl_filename[strlen(filename)+1];
      strcpy(new_link->fl_logical_filename, logical_filename);
   }

   new_link->fl_next = NULL;

   if (header->flh_tail)
   {
      header->flh_tail->fl_next = new_link;
      header->flh_tail = new_link;
   }
   else
   {
      header->flh_head = header->flh_tail = new_link;
   }

   return true;
}


void free_list(cab_file_list_header *header)
{
   struct cab_file_link *search;
   struct cab_file_link *next;

   for (search = header->flh_head; search; search = next)
   {
      next = search->fl_next;

      mem_free((byte *) search);
   }

   header->flh_head = NULL;
   header->flh_tail = NULL;
}


/*
 * If require_extract_match is true, then the whole path and filename
 * must match; otherwise, only the filename part needs to match
 */
struct cab_file_link *filename_in_list(
   const byte *filename,
   const cab_file_list_header *header,
   bool require_exact_match
)
{
   struct cab_file_link *search;
   const byte           *p;

   if (require_exact_match == false)
   {
      if ((p = strrchr(filename, SLASH_CHARACTER)) == NULL)
      {
         p = filename;
      }
      else
      {
         p++;
      }
   }

   for (search = header->flh_head; search; search = search->fl_next)
   {
      if (require_exact_match)
      {
         if (!stricmp(search->fl_filename, filename))
            return (search);
      }
      else
      {
         const byte *q;

         if ((q = strrchr(search->fl_filename, SLASH_CHARACTER)) == NULL)
         {
            q = search->fl_filename;
         }
         else
         {
            q++;
         }

         if (!stricmp(p, q))
         {
            return (search);
         }
      }
   }

   return NULL;
}


bool file_in_wildcard_list(
   const byte *filename,
   const cab_file_list_header *header
)
{
   struct cab_file_link *search;

   for (search = header->flh_head; search; search = search->fl_next)
   {
      if (wildcard_match(filename, search->fl_filename))
         return (true);
   }

   return false;
}


static void strip_trailing_whitespace(byte *line)
{
    int i;

    for (i = strlen(line)-1; i >= 0; i--)
    {
        if (!isspace(line[i]))
            break;

        line[i] = 0;
    }
}


/*
 * Read a list of files from a text file, and add to list.
 * Expand any wildcards
 */
bool input_file_list_from_file(
    const byte *filename,
    cab_file_list_header *header
)
{
    FILE *  handle;
    byte    line[MAX_PATHNAME_LENGTH+8];

    handle = fopen(filename, "r");

    if (handle == NULL)
        return false;

    while (1)
    {
        byte *logical_filename;
        byte *physical_filename;
        byte *p, *q;

        if (fgets(line, sizeof(line), handle) == NULL)
            break;

        // remove trailing newline and any trailing spaces
        strip_trailing_whitespace(line);

        // ignore blank lines
        if (strlen(line) == 0)
            continue;

        // no logical filename yet
        logical_filename = NULL;

        // handle quotes around the physical filename
        if (line[0] == '\"')
        {
            // find matching quote
            p = strchr(&line[1], '\"');

            if (p == NULL)
            {
                // no matching quote; error!
                printf(
                    "No matching quote in file '%s' line:\n%s\n",
                    filename, line
                );

                fclose(handle);
                return false;
            }

            physical_filename = &line[1];
            *p++ = 0; // null terminate physical filename

            // skip over space
            while (isspace(*p))
                p++;
        }
        else
        {
            // no quote for physical filename
            physical_filename = &line[0];

            p = strchr(line, ' '); // find a space

            if (p != NULL)
            {
                // there's a space, which means a logical filename follows
                *p++ = 0; // null terminate string

                // skip over space
                while (isspace(*p))
                    p++;
            }
        }

        if (p != NULL)
        {
            // logical filename follows?
            if (*p != 0)
            {
                // not end of string

                // is logical filename in quotes?
                if (*p == '\"')
                {
                    q = strchr(p+1, '\"');

                    if (q == NULL)
                    {
                        // no matching quote; error!
                        printf(
                            "No matching quote in file '%s' line:\n%s\n",
                            filename, line
                        );

                        fclose(handle);
                        return false;
                    }

                    logical_filename = p+1;
                    *q = 0; // null terminate string
                }
                else
                {
                    // logical filename is not in quotes
                    logical_filename = p;
                }
            }
        }

        if ((strlen(physical_filename) >= MAX_PATHNAME_LENGTH) ||
             ((logical_filename != NULL) &&
             (strlen(logical_filename) >= MAX_PATHNAME_LENGTH)))
        {
            printf("Filenames cannot be more than %d characters in length, line:\n", MAX_PATHNAME_LENGTH);
            printf("%s\n", line);
            fclose(handle);
            return false;
        }

        // if the filename contains wildcards, expand them
        if (wildcards_in_string(physical_filename))
        {
            // logical filenames are not allowed if there are wildcards
            if (logical_filename != NULL)
            {
                printf(
                    "Logical filenames are not allowed if wildcards are used, line:\n%s\n",
                    line
                );

                fclose(handle);
                return false;
            }

            if (false == expand_wildcard(
                header,
                physical_filename,
                g_recurse_subdirs,
                NULL
            ))
            {
                printf("Error expanding wildcard '%s'\n", physical_filename);
                fclose(handle);
                return false;
            }
        }
        else
        {
            if (add_to_list(header, physical_filename, logical_filename) == false)
                return false;
        }
    }

    fclose(handle);
    return true;
}

