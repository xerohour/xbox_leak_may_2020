/*
 * callback.c
 */
#include "cabarc.h"
#include <errno.h>


/*
 * ----- FCI -----
 */
FNFCIALLOC(callback_fci_alloc)
{
    return callback_alloc(cb);
}

FNFCIFREE(callback_fci_free)
{
    callback_free(memory);
}

FNFCIOPEN(callback_fci_file_open)
{
    INT_PTR result;

    result = callback_file_open(pszFile, oflag, pmode);

    if (result == -1)
        *err = GetLastError();

    return result;
}

FNFCIREAD(callback_fci_file_read)
{
    unsigned int result;

    result = callback_file_read(hf, memory, cb);

    if (result != cb)
        *err = GetLastError();

    return result;
}

FNFCIWRITE(callback_fci_file_write)
{
    unsigned int result;

    result = callback_file_write(hf, memory, cb);

    if (result != cb)
        *err = GetLastError();

    return result;
}

FNFCICLOSE(callback_fci_file_close)
{
    int result;

    result = callback_file_close(hf);

    if (result == -1)
        *err = GetLastError();

    return result;
}

FNFCISEEK(callback_fci_file_seek)
{
    long result;

    result = callback_file_seek(hf, dist, seektype);

    if (result == -1)
        *err = GetLastError();

    return result;
}

FNFCIDELETE(callback_fci_file_delete)
{
    if (DeleteFile(pszFile) == FALSE)
    {
        *err = GetLastError();
        return -1;
    }
    else
    {
        return 0;
    }
}


/*
 * ----- FDI -----
 */

/*
 * Memory allocation function
 */
FNALLOC(callback_alloc)
{
	return LocalAlloc(LMEM_FIXED, cb);
}


/*
 * Memory free function
 */
FNFREE(callback_free)
{
	LocalFree(pv);
}


FNOPEN(callback_file_open)
{
	DWORD openmode, flags, readwrite_mode, fileshare;
	HANDLE handle;

	if (oflag & (_O_APPEND | _O_TEXT))
	{
		printf("Error, oflag not supported\n");
		return -1;
	}

	if (oflag & _O_RDWR)
	{
		readwrite_mode = GENERIC_READ | GENERIC_WRITE;
		fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else if (oflag & _O_WRONLY)
	{
		readwrite_mode = GENERIC_WRITE;
		fileshare = 0;
	}
	else /* oflag & _O_RDONLY, but _O_RDONLY is actually == 0 */
	{
		readwrite_mode = GENERIC_READ;
		fileshare = FILE_SHARE_READ;
	}

	flags = FILE_ATTRIBUTE_NORMAL;

	if (oflag & _O_RANDOM)
		flags |= FILE_FLAG_RANDOM_ACCESS;
	else if (oflag & _O_SEQUENTIAL)
		flags |= FILE_FLAG_SEQUENTIAL_SCAN;

	if (oflag & _O_CREAT)
	{
		if (oflag & _O_EXCL)
			openmode = CREATE_NEW;
		else
			openmode = CREATE_ALWAYS; /* yes, even if _O_TRUNC */
	}
	else if (oflag & _O_TRUNC)
	{
		openmode = TRUNCATE_EXISTING;
	}
	else
	{
		openmode = OPEN_EXISTING;
	}

	if (oflag & _O_TEMPORARY)
		flags |= FILE_FLAG_DELETE_ON_CLOSE;

	handle = CreateFile(
		pszFile,
		readwrite_mode,
		fileshare,
		NULL,
		openmode,
		flags,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	else
		return (INT_PTR)handle;
}  


FNREAD(callback_file_read)
{
	DWORD bytes_read;

	(void) ReadFile((HANDLE) hf, pv, cb, &bytes_read, NULL);

	return bytes_read;
}


FNWRITE(callback_file_write)
{
	DWORD bytes_written;

	(void) WriteFile((HANDLE) hf, pv, cb, &bytes_written, NULL);

	return bytes_written;
}


FNCLOSE(callback_file_close)
{
	if (CloseHandle((HANDLE) hf) == TRUE)
		return 0;
	else
		return -1;
}


FNSEEK(callback_file_seek)
{
	switch (seektype)
	{
		case SEEK_SET:
			return SetFilePointer((HANDLE) hf, dist, NULL, FILE_BEGIN);

		case SEEK_CUR:
			return SetFilePointer((HANDLE) hf, dist, NULL, FILE_CURRENT);

		case SEEK_END:
			return SetFilePointer((HANDLE) hf, dist, NULL, FILE_END);
	}

	return -1;
}
