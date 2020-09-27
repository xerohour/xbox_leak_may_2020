// GhProfiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int GetSymbol(char *map, DWORD address, char *symbol)
    {
    char hex[32];
    char sym[1024];
    sprintf(hex, "%08x", address);
    char *addr = strstr(map, hex);

    if(!addr) return 0;

    do { --addr; } while(*addr == ' ');
    do { --addr; } while(*addr != ' ');

    sscanf(addr, "%s", sym);

    UnDecorateSymbolName(sym, symbol, 1024, UNDNAME_COMPLETE);

    return 1;
    }

int __cdecl main(int argc, char* argv[])
    {
    DWORD address, count, exit, error, avgTime, maxTime, minTime, penterTime, childTime, freq=0;
    const unsigned MAX_BUFF = 1024;
    char buffer[MAX_BUFF];
    char *symbolMap = new char[4*1024*1024];
    char symbol[MAX_BUFF];
    unsigned columns;

    // read in the map file
    FILE *map = fopen("harness.map", "r");
    if(!map)
        {
        delete[] symbolMap;
        printf("ERROR: harness.map must exist in the current directory.\n");
        return 1;
        }
    DWORD length = _filelength(_fileno(map));
    fread(symbolMap, 1, length, map);
    fclose(map);
    symbolMap[length] = '\0';

    // parse the log and generate the report
    FILE *log = fopen("GhProf.log", "r");
    if(!log)
        {
        delete[] symbolMap;
        printf("ERROR: ghprof.log must exist in the current directory.\n");
        return 1;
        }

    FILE *out = fopen("GhReport.txt", "w+");
    FILE *xml = fopen("GhReport.xml", "w+");
    fprintf(xml, "<GhProfiler>\n");
    fprintf(xml, "    <Report>\n");
    while(fgets(buffer, MAX_BUFF, log))
        {
        if(9 == sscanf(buffer, "%x, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu", &address, &count, &exit, &error, &avgTime, &maxTime, &minTime, &penterTime, &childTime))
            {
            if(!GetSymbol(symbolMap, address, symbol)) strcpy(symbol, "(no symbol)");
            fprintf(out, "%08x, %5lu, %5lu, %5lu, %8lu, %8lu, %8lu, %8lu, %8lu,  %s\n", address, count, exit, error, avgTime, maxTime, minTime, penterTime, childTime, symbol);
            fprintf(xml, "        <Call address=\"%08x\"><Name><![CDATA[%s]]></Name><HitCount>%lu</HitCount><AvgTime>%lu</AvgTime><ChildTime>%lu</ChildTime></Call>\n", address, symbol, count, avgTime, childTime);
            }
        else if(1 == sscanf(buffer, "Freq=%lu", &freq))
            {
            fprintf(out, "%s", buffer);
            }
        else
            {
            fprintf(out, "%s", buffer);
            }
        }
    fprintf(xml, "    </Report>\n");
    
    if(freq)
        {
        fprintf(xml, "<Resolution Note='ticks per second'>%lu</Resolution>\n", freq);
        }

    fprintf(xml, "</GhProfiler>\n");
    fclose(xml);
    fclose(out);
    fclose(log);

    delete[] symbolMap;

    printf("Done. GhReport.txt created.\n");
    return 0;
    }

