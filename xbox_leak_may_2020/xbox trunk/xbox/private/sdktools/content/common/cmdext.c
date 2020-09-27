/*
 * cmdext.c
 */
#include "cabarc.h"
#include <conio.h>
#include "oldnames.h"

static cab_file_list_header *list_of_files_to_extract;
static bool success;


static bool extract_this_file(char *filename)
{
	if (list_of_files_to_extract->flh_head == NULL)
		return true;

	if (file_in_wildcard_list(filename, list_of_files_to_extract))
		return true;
	else
		return false;
}


static void clear_console_line(void)
{
	printf("\r                                                                               \r");
}


/*
 * Create a directory
 *
 * Name may or may not have trailing slash
 */
static bool create_dir(const byte *dirname)
{
   byte temp_dirname[MAX_PATHNAME_LENGTH];

   /*
    * Ensure strlen(DirName) > 0
    */
   if (*dirname)
   {
      /*
       * MakeDir routine doesn't want trailing slash for dir name
       */
      if (dirname[ strlen(dirname)-1 ] == SLASH_CHARACTER)
      {
         strncpy(temp_dirname, dirname, sizeof(temp_dirname));
         temp_dirname[ sizeof(temp_dirname)-1 ] = 0;

         /* Wipe out trailing slash */
         temp_dirname[ strlen(temp_dirname)-1 ] = 0;

         if (CreateDirectory(temp_dirname, NULL))
            return true;
         else
            return false;
      }

      /*
       * No trailing slash; ordinary makedir
       */
      if (CreateDirectory(dirname, NULL))
         return true;
      else
         return false;
   }
   else
   {
      return false;
   }
}


/*
 * Create the directories implied by this file name (with full path)
 *
 * Assumption:  We have already tried to write to this file name, and failed,
 *              so if we don't create any directories, report failure.
 *
 * This routine is also used to create subdirectories (e.g. "dir1/dir2/").
 */
bool create_directories_for_file(const byte *filename)
{
   char	pathname[MAX_PATHNAME_LENGTH];
   int	i;

   strcpy(pathname, filename);

   /*
    * Example: "newdir/dir2/dir3/filename"
    *
    * CreateDir(newdir/dir2/dir3)   fail
    * CreateDir(newdir/dir2)        fail
    * CreateDir(newdir)             success
    * CreateDir(newdir/dir2)        success
    * CreateDir(newdir/dir2/dir3)   success
    */

   i = strlen(pathname)-1;

   while (i > 0)
   {
      if (pathname[i] == SLASH_CHARACTER)
      {
         pathname[i] = 0;

         if (create_dir(pathname) == true)
         {
            /* Success */
            pathname[i] = SLASH_CHARACTER;

            while (1)
            {
               /* Now go backwards and create the subdirectories */
               do
               {
                  i++;
               } while (pathname[i] && pathname[i] != SLASH_CHARACTER);

               if (pathname[i] == SLASH_CHARACTER)
               {
                  pathname[i] = 0;

                  if (create_dir(pathname) == false)
                  {
                     /* Unexpected failure */
                     pathname[i] = SLASH_CHARACTER;
                     return false;
                  }

                  pathname[i] = SLASH_CHARACTER;
               }
               else
               {
                  /* Null terminator found; end of directory path */
                  return true;
               }
            }
         }

         pathname[i] = SLASH_CHARACTER;
      }

      i--;
   }

   /* Failure */
   return false;
}


static FNFDINOTIFY(fdi_ext_notification_function)
{
	switch (fdint)
	{
		case fdintCABINET_INFO: // general information about the cabinet
			return 0;

		case fdintPARTIAL_FILE: // first file in cabinet is continuation
			return 0;

		case fdintCOPY_FILE:	// file to be copied
		{
			char	destination[MAX_PATHNAME_LENGTH];
			char	*filename;
			INT_PTR	handle;
			int		c;

			/*
			 * Ignore files which have a colon in them (e.g. C:\CONFIG.SYS)
			 */
			if (strchr(pfdin->psz1, ':'))
			{
				printf("   File '%s' contains illegal character (colon) -- skipping\n", pfdin->psz1);
				return 0;
			}

			filename = pfdin->psz1;

			/*
			 * Strip out path names if we're not preserving them on extract
			 */
			if (g_preserve_path_names == false)
			{
				char	*p;

				p = strrchr(pfdin->psz1, '\\');

				if (p)
					filename = p+1;
			}

			if (extract_this_file(filename))
			{
				sprintf(
					destination, 
					"%s%s",
					g_dest_dir,
					filename
				);

				if (g_confirm_files)
				{
					printf("   -- Extract '%s'? (Yes/No/All/Quit): ", filename);

					do
					{
						c = getch();
						c = toupper(c);
					} while (c != 'Y' && c != 'N' && c != 'Q' && c != 'A');

					if (c == 'N')
					{
						clear_console_line();
						return 0;
					}
					else if (c == 'Q')
					{
						printf("q\n");
						return -1;
					}
					else if (c == 'A')
					{
						g_confirm_files = false;
					}

					clear_console_line();
				}

				/*
				 * If we're not overwriting files automatically and
				 * the file exists, ask the user
				 */
				if (g_overwrite_files == false)
				{
					if (GetFileAttributes(destination) != 0xFFFFFFFF)
					{
						/*
						 * File exists
						 */
						printf(
							"   -- File '%s' already exists; overwrite? (Yes/No/All/Quit): ", 
							destination
						);
						
						do
						{
							c = getch();
							c = toupper(c);
						} while (c != 'Y' && c != 'N' && c != 'Q' && c != 'A');
						
						if (c == 'N')
						{
							printf("n\n");
							return 0;
						}
						else if (c == 'Q')
						{
							printf("q\n");
							return -1;
						}
						else if (c == 'A')
						{
							printf("a\n");
							g_overwrite_files = true;
						}

						clear_console_line();
					}
				}

				handle = callback_file_open(
					destination,
					_O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL,
					_S_IREAD | _S_IWRITE 
				);

				if (handle == -1)
				{
					/*
					 * It might have failed because it contained a path name for
					 * a directory (or subdirectory) which didn't exist
					 */
					if (g_preserve_path_names)
					{
						(void) create_directories_for_file(destination);

						handle = callback_file_open(
							destination,
							_O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL,
							_S_IREAD | _S_IWRITE 
						);
					}
				}

				if (handle == -1)
				{
					printf("   -- Error opening '%s' for output\n", destination);
					success = false;
				}
				else 
				{
					printf("   extracting: %s\n", destination);
				}

				return handle;
			}
			else
			{
				return 0;
			}
		}

		case fdintCLOSE_FILE_INFO:	// close the file, set relevant info
		{
			DWORD		attrs;
			FILETIME	datetime;
			char		destination[MAX_PATHNAME_LENGTH];
			char		*filename;

			filename = pfdin->psz1;

			/*
			 * Strip out path names if we're not preserving them on extract
			 */
			if (g_preserve_path_names == false)
			{
				char *p;

				p = strrchr(pfdin->psz1, '\\');

				if (p)
					filename = p+1;
			}

			sprintf(
				destination, 
				"%s%s",
				g_dest_dir,
				filename
			);

            attrs = 0;

			if (pfdin->attribs & _A_RDONLY)
                attrs |= FILE_ATTRIBUTE_READONLY;

            if (pfdin->attribs & _A_ARCH)
                attrs |= FILE_ATTRIBUTE_ARCHIVE;

            if (pfdin->attribs & _A_HIDDEN)
                attrs |= FILE_ATTRIBUTE_HIDDEN;

            if (pfdin->attribs & _A_SYSTEM)
                attrs |= FILE_ATTRIBUTE_SYSTEM;

            if (attrs == 0)
				attrs = FILE_ATTRIBUTE_NORMAL;

			/*
			 * Set file attributes and date/time
			 */
			if (TRUE == DosDateTimeToFileTime(
				pfdin->date,
				pfdin->time,
				&datetime))
			{
				FILETIME	local_filetime;

				if (TRUE == LocalFileTimeToFileTime(
					&datetime, 
					&local_filetime))
				{
					(void) SetFileTime(
						(HANDLE) pfdin->hf,
						&local_filetime,
						NULL,
						&local_filetime
					);
				}
			}

			(void) callback_file_close(pfdin->hf);
			(void) SetFileAttributes(destination, attrs);

			return TRUE;
		}

		case fdintNEXT_CABINET:	// file continued to next cabinet

			return -1;
	}

	return 0;
}


bool extract_from_cab(char *cabname, cab_file_list_header *list)
{
	HFDI			hfdi;
	ERF				erf;
	FDICABINETINFO	fdici;
	INT_PTR			hf;
	char			*p;
	char			cabinet_name[MAX_PATHNAME_LENGTH];
	char			cabinet_path[MAX_PATHNAME_LENGTH];

	success = true;

	list_of_files_to_extract = list;

	hfdi = FDICreate(
		callback_alloc,
		callback_free,
		callback_file_open,
		callback_file_read,
		callback_file_write,
		callback_file_close,
		callback_file_seek,
		cpu80386,
		&erf
	);

	if (hfdi == NULL)
	{
		printf("FDICreate() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper)
		);

		return false;
	}

	/*
	 * Is this file really a cabinet?
	 */
	hf = callback_file_open(
		cabname,
		_O_BINARY | _O_RDONLY,
		0
	);

	if (hf == -1)
	{
		(void) FDIDestroy(hfdi);

		printf("Unable to open '%s' for input\n", cabname);
		return false;
	}

	if (FALSE == FDIIsCabinet(
			hfdi,
			hf,
			&fdici))
	{
		/*
		 * No, it's not a cabinet!
		 */
		callback_file_close(hf);

		printf(
			"FDIIsCabinet() failed: '%s' is not a cabinet\n",
			cabname
		);

		(void) FDIDestroy(hfdi);
		return false;
	}

	callback_file_close(hf);

	printf(
		"Extracting file(s) from cabinet '%s':\n",
		cabname
	);

	p = strrchr(cabname, '\\');

	if (p == NULL)
	{
		strcpy(cabinet_name, cabname);
		strcpy(cabinet_path, "");
	}
	else
	{
		strcpy(cabinet_name, p+1);

		strncpy(cabinet_path, cabname, (int) (p-cabname)+1);
		cabinet_path[ (int) (p-cabname)+1 ] = 0;
	}

	if (TRUE != FDICopy(
		hfdi,
		cabinet_name,
		cabinet_path,
		0,
		fdi_ext_notification_function,
		NULL,
		NULL))
	{
		printf(
			"FDICopy() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper)
		);

		(void) FDIDestroy(hfdi);
		return false;
	}

	printf("\n");

	if (FDIDestroy(hfdi) != TRUE)
	{
		printf(
			"FDIDestroy() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper)
		);

		return false;
	}

	if (success)
		printf("Operation successful\n");
	else
		printf("Operation not entirely successful\n");

	return success;
}
