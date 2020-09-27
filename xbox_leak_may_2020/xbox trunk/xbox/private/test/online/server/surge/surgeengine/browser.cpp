#include "stdafx.h"

#include "browser.h"

Browser::Browser()
{
    length = 0;
    for(int i=0; i<MAX_HEADERS; i++)
	{
        headers[i].header = NULL;
        headers[i].value = NULL;
	}
    versionLen = sprintf(version, "HTTP/1.0");
	
    char *surgeheader[] = 
	{
        "Accept",
		"User-Agent",
		"Connection",
		"Content-Length",
		"Content-Type",
		"Authorization",
		"Cookie"
	};
    char *surgevalues[] =
	{
        "*/*",
		"Surge (Default Browser)",
		"Close",
		NULL,
		NULL,
		NULL,
		NULL
	};
	
    length = sizeof(surgeheader)/sizeof(surgeheader[0]);
    for(i=0; i<length; i++)
	{
        headers[i].headerLen = strlen(surgeheader[i]) + 1;
        headers[i].header = new char[headers[i].headerLen];
        memcpy(headers[i].header, surgeheader[i], headers[i].headerLen);
        if(surgevalues[i])
		{
            headers[i].valueLen = strlen(surgevalues[i]) + 1;
            headers[i].value = new char[headers[i].valueLen];
            memcpy(headers[i].value, surgevalues[i], headers[i].valueLen);
		}
		else
		{
			headers[i].valueLen = 0;
		}
	}
}

Browser::Browser(char *filename)
{
    length = 0;
    for(int i=0; i<MAX_HEADERS; i++)
	{
        headers[i].header = NULL;
        headers[i].value = NULL;
		headers[i].headerLen = 0;
		headers[i].valueLen = 0;
		
	}
    versionLen = sprintf(version, "HTTP/1.0");
	
    char h[1024];
    char v[4096];
    char blank[1024];
    FILE *f = fopen(filename, "r");
    if(!f) 
	{
		return;
	}
	
    // get the version
    fscanf(f, "%s\n", version);
    versionLen = strlen(version);
	
    char string[4096];
	
    // get the rest of the header
    for(int line=0; line<MAX_HEADERS; line++)
	{
        h[0] = v[0] = '\0';
        if(!fgets(string, 2048, f)) 
		{
			break;
		}
        sscanf(string, "%[^:\0\r\n\t]%[: \t]%[^\0\r\n]\n", h, blank, v);
        if(h[0])
		{
            headers[line].headerLen = strlen(h) + 1;
            headers[line].header = new char[headers[line].headerLen];
            memcpy(headers[line].header, h, headers[line].headerLen);
            if(v[0])
			{
                headers[line].valueLen = strlen(v) + 1;
                headers[line].value = new char[headers[line].valueLen];
                memcpy(headers[line].value, v, headers[line].valueLen);
			}
		}
	}
	
    // size of the header
    length = line;
	
    fclose(f);
}

Browser::~Browser()
{
    for(int i=0; i<length; i++)
	{
        if(headers[i].header) 
		{
			delete[] headers[i].header;
		}
        if(headers[i].value) 
		{
			delete[] headers[i].value;
		}
	}
}

size_t Browser::EvalHeader(char *action, char *site, char *output)
{
    size_t offset=0;
    size_t l;
    l = sprintf(output, "%s %s %s\r\n", action, site, version);
    output += l;
    offset += l;
	
    for(int i=0; i<length; i++)
	{
        if(headers[i].value)
		{
            memcpy(output, headers[i].header, headers[i].headerLen-1);
            output += headers[i].headerLen-1;
            offset += headers[i].headerLen-1;
			
            output[0] = ':';
            output[1] = ' ';
            output += 2;
            offset += 2;
			
            memcpy(output, headers[i].value, headers[i].valueLen-1);
            output += headers[i].valueLen-1;
            offset += headers[i].valueLen-1;
			
            output[0] = '\r';
            output[1] = '\n';
            output[2] = '\0';
            output += 2;
            offset += 2;
		}
	}
	
    output[0] = '\r';
    output[1] = '\n';
    output[2] = '\0';
    offset += 2;
	
    return offset;
}

int Browser::Translate(char *h)
{
    for(int i=0; i<length; i++)
	{
        if(_stricmp(h, headers[i].header)==0)
		{
            return i;
		}
	}
	
    if(length==MAX_HEADERS) 
	{
		return -1;
	}
	
    headers[length].headerLen = strlen(h) + 1;
    headers[length].header = new char[headers[length].headerLen];
    strcpy(headers[length].header, h);
	
    return length++;
}

void Browser::AddHeader(char *h, char *v, size_t len/*=0*/)
{
    if(!h) return;
	
    int index = Translate(h);
    if(index == -1) return;
	
    if(!v)
	{
        if(headers[index].value) 
		{
			delete[] headers[index].value;
		}
        headers[index].value = NULL;
		headers[index].valueLen = 0;
        return;
	}
	
    if(!len) 
	{
		len = strlen(v);
	}
    ++len; // include terminating null char
    if(len <= 1) 
	{
		return;
	}
	
    if(len > headers[index].valueLen)
	{
        if(headers[index].value) 
		{
			delete[] headers[index].value;
		}
        headers[index].value = NULL;
	}
	
    headers[index].valueLen = len;
    if(!headers[index].value) 
	{
		headers[index].value = new char[len];
	}
	
    memcpy(headers[index].value, v, len);
}


///////////////////////////////////////////////////////////////////////////////
// Browser::IsHeaderSet
//
// Returns whether or not the header named in szHeaderName has already been
// set to some value.
///////////////////////////////////////////////////////////////////////////////
bool Browser::IsHeaderSet(char *szHeaderName)
{
    if(!szHeaderName) 
	{
		return false;
	}
	
    for(int index = 0; index < length; ++index)
    {
		if ( 0 == _stricmp(szHeaderName, headers[index].header) )
        {
			if ( 0 == headers[index].valueLen )
			{
				return false;
			}

			return true;
        }
    }

	return false;
}