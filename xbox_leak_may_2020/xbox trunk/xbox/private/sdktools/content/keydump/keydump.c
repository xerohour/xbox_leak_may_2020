#include <windows.h>
#include <stdio.h>

void _cdecl main(int argc, char **argv)
{
	HANDLE	hFile;
	BYTE	rgbBuffer[4096];
	PBYTE	pBuffer;
	DWORD	cbData;
	DWORD	i = 0;

	hFile = CreateFile(argv[1], 
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL,
				OPEN_EXISTING, 
				0, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Cannot open file %s\n", argv[1]);
		return;
	}

	while (ReadFile(hFile, rgbBuffer, sizeof(rgbBuffer), &cbData, NULL))
	{
		if (!cbData)
			break;

		pBuffer = rgbBuffer;
		while (cbData--)
		{
			if (!(i++ & 0x7))
				fprintf(stdout, "\n\t");
			fprintf(stdout, "0x%02x, ", *pBuffer++);
		}
	}
	fprintf(stdout, "\n");
}

/*
void main(int argc, char **argv)
{
	int				n, i = 0;
	unsigned char	szBuffer[4096];
	unsigned char	*pBuffer;

	while ((n = fread(szBuffer, 1, sizeof(szBuffer), stdin)) != 0)
	{
		pBuffer = szBuffer;
		while (n--)
		{
			if (!(i++ & 0x7))
				fprintf(stdout, "\n\t");
			fprintf(stdout, "0x%02x, ", *pBuffer++);
		}
	}
	fprintf(stdout, "\n");
}
*/

/*
void main(int argc, char **argv)
{
	int	i, j = 0;

	while ((i = fgetc(stdin)) != EOF)
	{
		if (!(j++ & 0x7))
			fprintf(stdout, "\n\t");
		fprintf(stdout, "0x%02x, ", i & 0xff);
	}
	fprintf(stdout, "\n");
}
*/
