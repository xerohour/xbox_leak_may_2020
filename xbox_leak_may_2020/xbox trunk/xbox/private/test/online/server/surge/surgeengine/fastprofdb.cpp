#include "stdafx.h"
#include "FastProfdb.h"

FastProfDB::FastProfDB(char *filename)
    {
    f = fopen(filename, "r");
    if(f) len = _filelength(_fileno(f));
    else len = 1;
    if(!len) len = 1;
    }

FastProfDB::~FastProfDB()
    {
    if(!f) return;
    fclose(f);
    }

char* FastProfDB::GetRandUser(void)
    {
    if(!f) return NULL;
    QueryPerformanceCounter(&liPerf);

    DWORD dwNewLowPart =  liPerf.LowPart & 0xFF000000;
    dwNewLowPart =  dwNewLowPart | ( (liPerf.LowPart & 0x00FF0000) >> 16);
    dwNewLowPart =  dwNewLowPart | ( (liPerf.LowPart & 0x0000FF00) << 8 );
    dwNewLowPart =  dwNewLowPart | ( (liPerf.LowPart & 0x000000FF) << 8);

    fseek(f, dwNewLowPart%len, SEEK_SET);

    fgets(buff, 1024, f);
    if(fgets(buff, 1024, f) == NULL)
        {
        fseek(f, 0, SEEK_SET);
        fgets(buff, 1024, f);
        }
    return buff;
    }

char* FastProfDB::GetNextUser(void)
    {
    if(!f) return NULL;
    if(fgets(buff, 1024, f) == NULL)
        {
        fseek(f, 0, SEEK_SET);
        fgets(buff, 1024, f);
        }
    return buff;
    }

// Parse out the username and password from a profDB line 
void FastProfDB::GetNamePW(char *dbline, char *username, char *domain, char *pw, int size)
    {
    if(!dbline)
        {
        strcpy(username, "jpoley");
        strcpy(domain, "moonport.com");
        strcpy(pw, "jpoley");
        return;
        }

    int i, j;

    // grab username
    for(i=0; i<size; i++)
        {
        if((username[i] = dbline[i]) == '@')
            {
            username[i] = '\0';
            break;
            }
        else if(username[i] == '|')
            {
            username[i] = '\0';
            break;
            }
        }

    ++i;
    // grab domain
    for(j=0; j<size; j++, i++)
        {
        if((domain[j] = dbline[i]) == '|')
            {
            domain[j] = '\0';
            break;
            }
        }

    // skip a column
    while(dbline[++i] != '|')
        {
        if(i > size*2)
            {
            strcpy(pw, "profDBerror");
            return;
            }
        }

    ++i;
    // grab password
    for(j=0; j<size; j++, i++)
        {
        if((pw[j] = dbline[i]) == '|')
            {
            pw[j] = '\0';
            break;
            }
        }
    }