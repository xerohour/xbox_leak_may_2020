#include <afx.h>

void main(int argc, char *argv[])

{
	/*
	FILE *pfIn = fopen("StringsJapanese.rc", "r");
	FILE *pfOut = fopen("StringsJapanese2.rc", "w");
	char szLine[1024];
	CString cstrLine;
	CString cstrPostNumber;
	int intNumberIndex;
	int intNumber;

	while(!feof(pfIn))

	{
		fgets(szLine, 1023, pfIn);
		cstrLine = szLine;
		intNumberIndex = cstrLine.FindOneOf("0123456789");
		
		if(intNumberIndex != -1)
		
		{
			intNumber = atoi(cstrLine.Mid(intNumberIndex, 5));
			intNumber -= 5000;
			fprintf(pfOut, "    %d %s", intNumber, cstrLine.Mid(intNumberIndex + 5));
		}

		else
			fprintf(pfOut, "%s", szLine);
	}

	fclose(pfIn);
	fclose(pfOut);
	*/

	FILE *pfIn = fopen("c:\\cafe6\\include\\Strings.h", "r");
	char szLine[1024];
	CString cstrLine;
	CString cstrSymbolName;
	CString cstrSymbolNames[551];
	CString cstrSymbolIDs[551];
	int intSpaceIndex;

	for(int i = 0; i < 551; i++)	
	{
		fgets(szLine, 1023, pfIn);
		cstrLine = szLine;
		cstrLine.TrimRight();
		cstrSymbolName = cstrLine.Mid(8);
		intSpaceIndex = cstrSymbolName.FindOneOf(" \t");
		cstrSymbolNames[i] = cstrLine.Mid(8, intSpaceIndex);
		cstrSymbolIDs[i] = cstrLine.Right(4);
	}

	fclose(pfIn);
	pfIn = fopen("StringsJapanese2.rc", "r");
	FILE *pfOut = fopen("StringsJapanese.rc", "w");
	int intNumberIndex;
	int intTabs;
	CString cstrNumber;
	CString cstrPostNumber;

	while(!feof(pfIn))

	{
		fgets(szLine, 1023, pfIn);
		cstrLine = szLine;
		intNumberIndex = cstrLine.FindOneOf("0123456789");
		
		if(intNumberIndex != -1)
		
		{
			cstrNumber = cstrLine.Mid(intNumberIndex, 4);
			cstrPostNumber = cstrLine.Mid(intNumberIndex + 4);
			cstrPostNumber.TrimLeft();

			for(i = 0; i < 551; i++)
			{
				if(cstrNumber == cstrSymbolIDs[i])
				{
					intTabs = 11 - (cstrSymbolNames[i].GetLength() / 4);
					fprintf(pfOut, "    %s", cstrSymbolNames[i]);
					
					for(int j = 0; j < intTabs; j++)
						fprintf(pfOut, "\t");
					
					fprintf(pfOut, " %s", cstrPostNumber);
				}
			}
		}

		else
			fprintf(pfOut, "%s", szLine);
	}

	fclose(pfIn);
	fclose(pfOut);
}