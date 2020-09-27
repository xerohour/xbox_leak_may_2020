#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <netsync.h>

void PrintUsage()
{
	_tprintf(TEXT("Usage: syncserv <no arguments>\n"));
	return;
}

int __cdecl main(int argc, TCHAR *argv[])
{
	CHAR	tcInput = 0;

    if(argc > 1)
		PrintUsage();

	if(!NetsyncCreateServer())
	{
		_tprintf(TEXT("Couldn't create Netsync server: %u\n"), GetLastError());
		goto END;
	}
	
	_tprintf(TEXT("Netsync server started, hit \'q\' to exit\n"), GetLastError());

	tcInput = (CHAR) _getch();
	while((tcInput != 'q') && (tcInput != 'Q'))
	{
		tcInput = (TCHAR) _getch();
	}

	_tprintf(TEXT("Shutting down server.\n"), GetLastError());

	NetsyncCloseServer();

	_tprintf(TEXT("Exiting test...\n"), GetLastError());

END:
	return -1;
}
