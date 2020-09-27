#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <stdio.h>
#include <xtl.h>
extern "C" {
#include <ntos.h>
#include <xapip.h>  //private XAPI methods for low-level tests
}
#include "muprod.h"


// So that we do not need to include mu.h.
// That was causing some problems.  Obviously we need to keep these current.

#define MU_VSC_GET_BAD_BLOCK_TABLE   0
#define MU_VSC_MEMORY_TEST           1
#define MU_IOCTL_GET_BAD_BLOCK_TABLE\
    CTL_CODE(FILE_DEVICE_DISK, MU_VSC_GET_BAD_BLOCK_TABLE, METHOD_NEITHER, FILE_ANY_ACCESS)
#define MU_IOCTL_MEMORY_TEST\
    CTL_CODE(FILE_DEVICE_DISK, MU_VSC_MEMORY_TEST, METHOD_NEITHER, FILE_ANY_ACCESS)


BOXSTATUS LowLevelTest(int port, int slot, int &BB1, int &BB2, int &BB3)
{
    CHAR device[32]; // (mu port numbers are hex values, 1 based)
    NTSTATUS status;
    HANDLE hVolume;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK statusBlock;
    OCHAR         deviceNameBuffer[64];
    OBJECT_STRING deviceName;
    USHORT  BadBlockTable[16];
    USHORT  BadBlockCount;
    int i;

    //
    //  Assume the test failed, until it passes.
    //
    BOXSTATUS boxStatus = failed;

    //
    // Initialize the device name buffer
    //

    deviceName.Buffer = deviceNameBuffer;
    deviceName.Length = 0;
    deviceName.MaximumLength = 63;

    //
    //  Tell the MU driver to expose a device object
    //
    status = MU_CreateDeviceObject(port, slot, &deviceName);
    if(NT_ERROR(status))
    {
        DebugPrint("Couldn't create device object\n");
        goto close_nothing;
    }
    
    //
    //  Open the device object in raw mode.
    //
    InitializeObjectAttributes(
        &oa,
        &deviceName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    status = NtOpenFile(&hVolume,
                        SYNCHRONIZE | GENERIC_ALL,
                        &oa,
                        &statusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_ALERT);
    if (NT_ERROR(status))
    {
        DebugPrint("Couldn't open device object\n");
        goto close_mu_do;
    }


    //
    //  MU_IOCTL_GET_BAD_BLOCK_TABLE
    //
    DebugPrint("Getting bad block table\n");
    status = NtDeviceIoControlFile(hVolume,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   MU_IOCTL_GET_BAD_BLOCK_TABLE,
                                   NULL,
                                   0,
                                   &BadBlockTable,
                                   sizeof(BadBlockTable)
                                   );
    if (NT_ERROR(status))
    {
        DebugPrint("Couldn't get the bad block table\n");
        goto close_everything;
    }
    
    //
    //  Count the bad blocks if it exceeds ten fail
    //
    BadBlockCount = 0;
    for(i=0; i<16; i++)
    {
        DebugPrint("BadBlockTable[%d] = %d\n", i, (ULONG)BadBlockTable[i]);
        if(BadBlockTable[i]!=0xFFFF)
        {
            BadBlockCount++;
        }
    }
    if(BadBlockCount > 10)
    {
        DebugPrint("%d bad blocks found - test fails.\n", (ULONG)BadBlockCount);
        goto close_everything;
    }
    DebugPrint("%d bad blocks found - within specification.\n", (ULONG)BadBlockCount);
	BB1 = (ULONG)BadBlockCount;
	
	
    //
    //  MU_IOCTL_MEMORY_TEST
    //
    DebugPrint("Performing Memory Test - can take 30 seconds or more\n");
    status = NtDeviceIoControlFile(hVolume,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   MU_IOCTL_MEMORY_TEST,
                                   NULL,
                                   0,
                                   &BadBlockCount,
                                   sizeof(BadBlockCount)
                                   );
    if (NT_ERROR(status))
    {
        DebugPrint("Couldn't get the bad block table\n");
        goto close_everything;
    }
    //
    //  Check bad blocks
    //
    if(BadBlockCount > 10)
    {
        DebugPrint("Memory test found %d bad blocks - test fails.\n", (ULONG)BadBlockCount);
        goto close_everything;
    }
    DebugPrint("Memory Test found %d bad blocks - within specification.\n", (ULONG)BadBlockCount);
	BB2 = (ULONG)BadBlockCount;


    //
    //  MU_IOCTL_GET_BAD_BLOCK_TABLE
    //
    DebugPrint("Getting bad block table (second time)\n");
    status = NtDeviceIoControlFile(hVolume,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   MU_IOCTL_GET_BAD_BLOCK_TABLE,
                                   NULL,
                                   0,
                                   &BadBlockTable,
                                   sizeof(BadBlockTable)
                                   );
    if (NT_ERROR(status))
    {
        DebugPrint("Couldn't get the bad block table\n");
        goto close_everything;
    }
    //
    //  Count the bad blocks if it exceeds ten fail
    //
    BadBlockCount = 0;
    for(i=0; i<16; i++)
    {
        DebugPrint("BadBlockTable[%d] = %d\n", i, (ULONG)BadBlockTable[i]);
        if(BadBlockTable[i]!=0xFFFF)
        {
            BadBlockCount++;
        }
    }
    if(BadBlockCount > 10)
    {
        DebugPrint("%d bad blocks found - test fails.\n", (ULONG)BadBlockCount);
        goto close_everything;
    }
    DebugPrint("%d bad blocks found - within specification.\n", (ULONG)BadBlockCount);
	BB3 = (ULONG)BadBlockCount;

    //
    //  If we are here, the test passed.
    //
    boxStatus = passed;

close_everything:
    NtClose(hVolume);
close_mu_do:
    MU_CloseDeviceObject(port, slot);
close_nothing:

	return boxStatus;
}

BOXSTATUS FormatTest(int port, int slot)
{
    CHAR device[32]; // (mu port numbers are hex values, 1 based)
    CHAR deviceNameBuffer[64];
    STRING deviceName;

	//
    //  Assume the test failed, until it passes.
    //
    BOXSTATUS boxStatus = failed;
    
    //
    // Initialize the device name buffer
    //
    deviceName.Buffer = deviceNameBuffer;
    deviceName.Length = 0;
    deviceName.MaximumLength = 63;

    DebugPrint("Formatting Device\n");
    NTSTATUS status = MU_CreateDeviceObject(port, slot, &deviceName);
    if(NT_ERROR(status))
    {
        DebugPrint("Couldn't create device object\n");
        goto do_nothing;
    }

    if(XapiFormatFATVolume(&deviceName)) {
		DebugPrint("Format succeeded\n");
        boxStatus = passed;
	} else {
		DebugPrint("Format failed (ec: %lu)\n", GetLastError());
	}

	MU_CloseDeviceObject(port, slot);

do_nothing:
   
    return boxStatus;
}

BOXSTATUS VerifyTest(int port, int slot)
{
	return passed;
}

char* FindLastChar(char* buf, char match) {
	char*c;
	for(c = NULL; ; buf++) {
		if(*buf == match) c = buf;
		if(!*buf) return c;
	}
}

void CopyBadFile(CHAR* badmufile)
{
	if(!CopyFile(badmufile, "T:\\media\\muprod\\BadMuFile.txt", FALSE)) {
		DebugPrint("couldn't copy bad file %s back to t:\\media\\muprod\\badmufile.txt! Error %d\n", badmufile, GetLastError());
	} else {
		DebugPrint("Copied bad mu file %s to t:\\media\\murpod\\badmufile.txt\n");
	}
}

#ifdef GETMUWRITECOUNTS
LONG MU_GetWriteCount();
ULONG MU_GetWriteSize(LONG count);
ULONG MU_GetWriteBlock(LONG count);
#endif

void LogMuWrites(int start, int end, bool startplus1 = true)
{
#ifdef GETMUWRITECOUNTS
	int i;
	DebugPrint("\nTotal Writes: %d\n", end - start - startplus1 + 1);
	for(i = start+startplus1; i <= end; i++) {
		DebugPrint("Write: %d bytes, to block 0x%08x\n", MU_GetWriteSize(i), MU_GetWriteBlock(i));
	}
	DebugPrint("\n");
#endif
}

#define logmu(_title_, code) sc = MU_GetWriteCount(); DebugPrint _title_; code; Sleep(5000); LogMuWrites(sc, MU_GetWriteCount());
#define checkhandle(h) if(INVALID_HANDLE_VALUE == h) { DebugPrint("CreateFile failed, line %d, error %d\n", __LINE__, GetLastError()); goto nothing; }
#define check(r) if(0 == r) { DebugPrint("something failed, line %d, error %d\n", __LINE__, GetLastError()); goto closehandle; }

BOXSTATUS CountSectorWriteTest(CHAR drive)
{

	#ifdef _DO_MU_BLOCK_WRITE_COUNT_
		CHAR GameDir[80] = {drive, ":\\SaveGame"};
		CHAR GameDir2[80] = {drive, ":\\SaveGam2"};
		CHAR File1[80];
		CHAR File2[80];
		CHAR File3[80];
		CHAR File1a[80];
		int i;
		int sc;
		HANDLE h;
		int temp;
		ULONG temp2;
		char * tempbuf;
		
		DebugPrint("CountSectorWriteTest\n\n\n");
		sprintf(File1, "%s\\File1", GameDir);
		sprintf(File2, "%s\\File2", GameDir);
		sprintf(File3, "%s\\File3", GameDir);
		sprintf(File1a, "%s\\File1", GameDir2);

	//CreateDirectory \SaveGame
		logmu(("CreateDirectory(%S, 0): ", GameDir), 
			temp = CreateDirectory(GameDir, 0);
		);
		check(temp);
		
	//Create File \SaveGame\File1
		logmu(("CreateFile(%S, ...)", File1), 
			h = CreateFile(File1, GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
		);
		checkhandle(h);

	//Write 5 bytes to File1
		logmu(("WriteFile 5 to file1"), 
			temp = WriteFile(h, "Narf", 5, &temp2, 0);
		);
		check(temp);
		
	//Close File1
		logmu(("CloseHandle(h)"), 
			CloseHandle(h);
		);


		tempbuf = new char[17000];
		if(tempbuf == 0) { DebugPrint("couldn't allocate 16000 bytes!\n"); goto nothing;}

	//Create File \SaveGame\File2
		logmu(("CreateFile(%s, ...)", File2), 
			h = CreateFile(File2, GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
		);
		checkhandle(h);

	//Write 16000 bytes to File2
		logmu(("WriteFile 17000 to file2"), 
			temp = WriteFile(h, tempbuf, 17000, &temp2, 0);
		);
		delete[] tempbuf;
		check(temp);
		
	//Close File2
		logmu(("CloseHandle(h)"), 
			CloseHandle(h);
		);


		
	//Create File \SaveGame\File1

		logmu(("CreateFile(%S, ...)", File1), 
			h = CreateFile(File1, GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
		);
		checkhandle(h);

	//append 100 bytes to file1
		tempbuf = new char[100];
		if(tempbuf == 0) { DebugPrint("couldn't allocate 100 bytes!\n"); goto nothing; }

		SetFilePointer(h, 0, 0, FILE_END);
		
		logmu(("WriteFile 100 to file1"), 
			temp = WriteFile(h, tempbuf, 100, &temp2, 0);
		);
		delete[] tempbuf;
		check(temp);
		
	//close file 1
		logmu(("CloseHandle(h)"), 
			CloseHandle(h);
		);


		
	//Create File \SaveGame\File1

		logmu(("CreateFile(%S, ...)", File1), 
			h = CreateFile(File1, GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
		);
		checkhandle(h);

	//append 24000 bytes to file1
		tempbuf = new char[24000];
		if(tempbuf == 0) { DebugPrint("couldn't allocate 24000 bytes!\n"); goto nothing; }

		SetFilePointer(h, 0, 0, FILE_END);
		
		logmu(("WriteFile 24000 to file1"), 
			temp = WriteFile(h, tempbuf, 24000, &temp2, 0);
		);
		delete[] tempbuf;
		check(temp);
		
	//close file1
		logmu(("CloseHandle(h)"), 
			CloseHandle(h);
		);

		
	//delete file2
		logmu(("Deletefile(file2)"),
			temp = DeleteFile(File2);
		);
		check(temp);

	//create file3
		logmu(("CreateFile(%S, ...)", File3), 
			h = CreateFile(File3, GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
		);
		checkhandle(h);

	//write 24000 bytes to file3
		tempbuf = new char[7100000];
		if(tempbuf == 0) { DebugPrint("couldn't allocate 7100000 bytes!\n"); goto nothing; }

		SetFilePointer(h, 0, 0, FILE_END);
		
		logmu(("WriteFile 7100000 to file3"), 
			temp = WriteFile(h, tempbuf, 7100000, &temp2, 0);
		);
		delete[] tempbuf;
		check(temp);
		
	//close file 3
		logmu(("CloseHandle(h)"), 
			temp = CloseHandle(h);
		);
		check(temp);

	//CreateDirectory \SaveGam2
		logmu(("CreateDirectory(%S, 0): ", GameDir2),
			temp = CreateDirectory(GameDir2, 0);
		);
		check(temp);

	//CopyFile(savegame\file1 -> savegam2\file1)
		logmu(("CopyFile(savegame\\file1 -> savegam2\\file1"), 
			temp = CopyFile(File1, File1a, TRUE);
		);
		check(temp);

		logmu(("Delete File3"),
			temp = DeleteFile(File3);
		);
		check(temp);
		logmu(("Delete File1"),
			temp = DeleteFile(File1);
		);
		check(temp);
		logmu(("Delete File1a"),
			temp = DeleteFile(File1a);
		);
		check(temp);
		logmu(("Remove directory1"),
			RemoveDirectory(GameDir);
		);
		check(temp);
		logmu(("Remove directory2"),
			RemoveDirectory(GameDir2);
		);
		check(temp);

		goto nothing;
	closehandle:
		logmu(("CloseHandle(h)"), 
			if(h != INVALID_HANDLE_VALUE) CloseHandle(h);
		);

	nothing:

	#endif //defined(_DO_MU_BLOCK_WRITE_COUNT_)
	return passed;
}

 

/*char* FindLastChar(char* buf, char match) {
	char*c;
	for(c = NULL; ; buf++) {
		if(*buf == match) c = buf;
		if(!*buf) return c;
	}
}
*/

BOXSTATUS WriteReadTest(CHAR drive,int &readfailed, int &writefailed, int &comparefailed)
{
	CHAR   buf[1024];
	CHAR  buf2[1024];
	FILE* f;
	DWORD s1;
	DWORD s2;
	DWORD temp;
	int pass;

	char* b1;
	char* b2;

	HANDLE h1, h2;

	sprintf(buf, "%c:\\*.*", drive);
	DeleteFile(buf);

	f = fopen("t:\\media\\muprod\\muprod.ini", "r");
	if(f == NULL) {
		DebugPrint("couldn't open t:\\muprod.ini, == c:\\tdata\\{00000~1\\muprod.ini\n");
		return failed;
	}

	pass = 0;
	while(1 == fscanf(f, "%s", buf)) {
		pass++;

		DebugPrint("Filename I am copying FROM is: %s\n", buf);

		sprintf(buf2, "%c:\\%s", drive, FindLastChar(buf, '\\') + 1);
		DebugPrint("Filename I am copying TO is: %s\n", buf2);

		h1 = CreateFile(buf, GENERIC_READ,0,0,OPEN_EXISTING,0,0);
		if(h1)
		{
			s1 = GetFileSize(h1, NULL);
			DebugPrint("Before copy, source file (%s) is %d bytes\n", buf, s1);
			CloseHandle(h1);
		}

		if(!CopyFile(buf, buf2, FALSE)) {
			DebugPrint("CopyFile(%s, %s, FALSE) failed: %d (pass %d)\n", buf, buf2, GetLastError(), pass);
			writefailed++;
//			return failed;
		}
		
		h1 = CreateFile(buf, GENERIC_READ,0,0,OPEN_EXISTING,0,0);
		if(h1 == NULL) {
			DebugPrint("Couldn't open %s!!! error %d (pass %d)", buf, GetLastError(), pass);
			//return failed;
		}

		h2 = CreateFile(buf2,GENERIC_READ,0,0,OPEN_EXISTING,0,0);
		if(h2 == NULL) {
			DebugPrint("Couldn't open %s!!! error %d (pass %d)", buf2, GetLastError(), pass);
			CloseHandle(h1);
			readfailed++;
			//return failed;
		}

		s1 = GetFileSize(h1, NULL);
		DebugPrint("File I am copying FROM is %d bytes\n", s1);
		s2 = GetFileSize(h2, NULL);
		DebugPrint("File I am copying TO is %d bytes\n", s2);
		if(s1!= s2) {
			DebugPrint("GetFileSize(%s): %d, GetFileSize(%s): %d. BAD! (pass %d)\n", buf, s1, buf2, s2, pass);
			//CopyBadFile(buf2);
			if( fclose(f) )   DebugPrint("The file was not closed\n");
			CloseHandle(h1);
			CloseHandle(h2);
			comparefailed++;
			return failed;
		}

		b1 = new char[s1];
		if(b1 == NULL) {
			DebugPrint("new failed! (pass %d)\n", pass);
			if( fclose(f) )   DebugPrint("The file was not closed\n");
			if(h1) CloseHandle(h1);
			if(h2) CloseHandle(h2);
			return failed;
		}
		b2 = new char[s1];
		if(b2 == NULL) {
			DebugPrint("new failed! (pass %d)\n", pass);
			if( fclose(f) )   DebugPrint("The file was not closed\n");
			if(h1) CloseHandle(h1);
			if(h2) CloseHandle(h2);
			delete[] b1;
			return failed;
		}

		if(FALSE == ReadFile(h1, b1, s1, &temp, 0)) {
			DebugPrint("ReadFile(h1, .., %d, .., 0) failed! Error %d (pass %d)\n", s1, GetLastError(), pass);
			if( fclose(f) )   DebugPrint("The file was not closed\n");
			if(h1) CloseHandle(h1);
			if(h2) CloseHandle(h2);
			delete[] b1;
			delete[] b2;
			return failed;
		}
		if(FALSE == ReadFile(h2, b2, s1, &temp, 0)) {
			DebugPrint("ReadFile(h2, .., %d, .., 0) failed! Error %d (pass %d)\n", s1, GetLastError(), pass);

		
			readfailed++;
			//	CopyBadFile(buf2);
			if( fclose(f) )   DebugPrint("The file was not closed\n");
			if(h1) CloseHandle(h1);
			if(h2) CloseHandle(h2);
			delete[] b1;
			delete[] b2;
			return failed;
		}
		if(memcmp(b1, b2, s1)) {
			DebugPrint("%s is NOT the same on the mu! (pass %d)\n", buf, pass);
			/*CopyBadFile(buf2);
			HANDLE h3 = CreateFile("t:\\media\\muprod\\BadMuFile2.txt", GENERIC_READ | GENERIC_WRITE, 0,0,0,CREATE_ALWAYS,0);
			if(!h3) {
				DebugPrint("Couldn't open t:\\media\\muprod\\badmufile2.txt!!! Error %d\n", GetLastError());
			} else {
				if(FALSE == WriteFile(h3, b2, s1, &temp, 0)) {
					DebugPrint("Couldn't write to t:\\media\\muprod\\badmufile2.txt!!! Error %d\n", GetLastError());
				}
				CloseHandle(h3);
			}
			*/
			comparefailed++;
			if( fclose(f) )   DebugPrint("The file was not closed\n");
			CloseHandle(h1);
			CloseHandle(h2);
			delete[] b1;
			delete[] b2;

			return failed;
		}


		if( fclose(f) )   DebugPrint("The file was not closed\n");

		CloseHandle(h1);
		CloseHandle(h2);
//		delete[] b1;
//		delete[] b2;
		DebugPrint("WriteRead test pass %d, file %s -> %s successful!\n", pass, buf, buf2);
	}
	return passed;
}

