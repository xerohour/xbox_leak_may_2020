/*
 * fciutil.c
 *
 * Support functions for the demo programs
 */
#include "cabarc.h"


void strip_path(char *filename, char *stripped_name)
{
	char	*p;

	p = strrchr(filename, '\\');

	if (p == NULL)
		strcpy(stripped_name, filename);
	else
		strcpy(stripped_name, p+1);
}


int get_percentage(unsigned long a, unsigned long b)
{
	while (a > 10000000)
	{
		a >>= 3;
		b >>= 3;
	}

	if (b == 0)
		return 0;

	return ((a*100)/b);
}


char *return_fci_error_string(FCIERROR err)
{
	switch (err)
	{
		case FCIERR_NONE:
			return "No error";

		case FCIERR_OPEN_SRC:
			return "Failure opening file to be stored in cabinet";
		
		case FCIERR_READ_SRC:
			return "Failure reading file to be stored in cabinet";
		
		case FCIERR_ALLOC_FAIL:
			return "Insufficient memory in FCI";

		case FCIERR_TEMP_FILE:
			return "Could not create a temporary file";

		case FCIERR_BAD_COMPR_TYPE:
			return "Unknown compression type";

		case FCIERR_CAB_FILE:
			return "Could not create cabinet file";

		case FCIERR_USER_ABORT:
			return "Client requested abort";

		case FCIERR_MCI_FAIL:
			return "Failure compressing data";

		default:
			return "Unknown error";
	}
}
