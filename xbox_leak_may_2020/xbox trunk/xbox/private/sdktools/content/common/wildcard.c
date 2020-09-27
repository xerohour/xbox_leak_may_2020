/*
 * wildcard.c
 *
 * Wildcard related routines.
 */
#include "cabarc.h"


bool wildcards_in_string(const byte *wildcard)
{
	while (*wildcard)
	{
		if (*wildcard == '*' || *wildcard == '?')
			return true;

		wildcard++;
	}

	return false;
}


/*
 * Returns whether <string> is matched by <wildcard>
 */
bool wildcard_match(const byte *string, const byte *wildcard)
{
	if (strrchr(string, '\\') != NULL)
	{
		string = strrchr(string, '\\')+1;
	}

	while (*string)
	{
		if (*wildcard != '*')
		{
			if (toupper(*wildcard) != toupper(*string) && *wildcard != '?')
			{
				return false;
			}

			wildcard++;
		}
		else if (*string == '.')
		{
			wildcard++;

			if (*wildcard == 0)
				return true;

			wildcard++;
		}

		string++;
	}

	return true;
}


bool expand_wildcard(
   cab_file_list_header *list_header,
   const byte           *wildcard,
   bool                 recurse_subdirs,
   bool                 (*qualifier_function)(const byte *filename)
)
{
	WIN32_FIND_DATA   fd;
	HANDLE            handle;
	byte              wild_str[MAX_FILENAME_LENGTH];
	byte              search_path[MAX_FILENAME_LENGTH];
	byte              search_str[MAX_FILENAME_LENGTH];
	byte              temp_name[MAX_FILENAME_LENGTH];
	byte              *name;
	bool              done;
	bool              found_files;
	cab_file_list_header recurse_subdir_list;
	struct cab_file_link *recurse_link;

	if (wildcards_in_string(wildcard) == false)
	{
		if (qualifier_function)
		{
			if ((*qualifier_function)(wildcard) == false)
				return true;
		}

        return (add_to_list(list_header, wildcard, NULL));
   }

	/*
	 * Get the path component of the search
	 *
	 * e.g. if wildcard == "\elroy\diamond\layout\*.c"
	 *      then search_path should equal "\elroy\diamond\layout\"
	 *      and wild_str should equal "*.c"
	 */
	strcpy(search_path, wildcard);

	if ((name = strrchr(search_path, SLASH_CHARACTER)) != NULL)
	{
		/* There are slashes in the name */
		strcpy(wild_str, name+1);
		*(name+1) = 0;
	}
	else
	{
		strcpy(wild_str, search_path);
		*search_path = 0;
	}

	found_files = false;

	init_list(&recurse_subdir_list);

	if (recurse_subdirs)
	{
		recurse_link = NULL;

		sprintf(search_str, "%s*.*", search_path); 

		/* Recurse into subdirectories and get all subdir names */
		do
		{
			handle = FindFirstFile(search_str, &fd);

			if (handle == INVALID_HANDLE_VALUE)
				done = true;
			else
				done = false;

			while (done == false)
			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char	new_dir[MAX_PATHNAME_LENGTH];

					if (fd.cFileName[0] != '.') /* . and .. */
					{
						sprintf(new_dir, "%s%s", search_path, fd.cFileName);
                        (void) add_to_list(&recurse_subdir_list, new_dir, NULL);
					}
				}

				if (FindNextFile(handle, &fd) == FALSE)
					done = TRUE;
				else
					done = FALSE;
			}

			if (handle != INVALID_HANDLE_VALUE)
				FindClose(handle);

			if (recurse_link == NULL)
				recurse_link = recurse_subdir_list.flh_head;
			else
				recurse_link = recurse_link->fl_next;

			if (recurse_link)
			{
				sprintf(search_path, "%s\\", recurse_link->fl_filename);
				sprintf(search_str, "%s\\*.*", recurse_link->fl_filename);
			}
		} while (recurse_link);
	}

	strcpy(search_str, wildcard);
	recurse_link = NULL;


	strcpy(search_path, wildcard);

	if ((name = strrchr(search_path, SLASH_CHARACTER)) != NULL)
	{
		/* There are slashes in the name */
		*(name+1) = 0;
	}
	else
	{
		*search_path = 0;
	}

	do
	{
		handle = FindFirstFile(search_str, &fd);

		if (handle == INVALID_HANDLE_VALUE)
			done = true;
		else
			done = false;

		while (done == false)
		{
			if (fd.cFileName[0] != '.') /* . and .. */
			{
				/*
				 * Don't add directories to the file list
				 */
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					/* Full name */
					sprintf(temp_name, "%s%s", search_path, fd.cFileName);
         
					found_files = true;

					/*
					 * If a qualifier function was supplied, this allows the
					 * caller to exclude certain files at the last minute.
					 *
					 * For example, if testing archives, and *.* is supplied
					 * for the archive name, the qualifier function can be
					 * used to ensure that only files ending in .cab are
					 * returned.
					 */
					if (qualifier_function)
					{
						if ((*qualifier_function)(temp_name) == false)
							continue;
					}

                    (void) add_to_list(list_header, temp_name, NULL);
				}
			}

			if (FindNextFile(handle, &fd) == FALSE)
				done = TRUE;
			else
				done = FALSE;
		}

		if (handle != INVALID_HANDLE_VALUE)
		{
			FindClose(handle);
		}

		if (recurse_link == NULL)
			recurse_link = recurse_subdir_list.flh_head;
		else
			recurse_link = recurse_link->fl_next;

		if (recurse_link)
		{
			sprintf(search_str, "%s\\%s", recurse_link->fl_filename, wild_str);
			sprintf(search_path, "%s\\", recurse_link->fl_filename);
		}

	} while (recurse_link);

	free_list(&recurse_subdir_list);

	return true;
}
