// CSurgeEngine.cpp : Implementation of CSurgeEngine

/*
TODO
    
*/

#include "stdafx.h"
#include "SurgeEngine.h"
#include "CSurgeEngine.h"
#include "../common/AnsiBstr.h"
#include "../common/utils.h"

extern int GetUserFromDB(FastProfDB *db, BOOL randomUsers, BOOL bad, char *username, char *domain, char *pw);

ServerStats::ServerStats()
{
	dwCount = 0;
	dwStatus = 0;
	next = NULL;
	dwInternalState = 0;
	dwRetryCountdown = 0;
}

ServerStats::~ServerStats()
{
	if(next)
    {
		delete next;
		next = NULL;
	}
}

// Called by the HashTable to clean up the stats
// As per "SetCleanupFunct();"
void DestroyServerStats(void *p)
{
	if(!p) 
	{
		return;
	}
	ServerStats *v = *(ServerStats**)p;
	delete v;
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// Uses Init() to initialize any variables that may need to be cleared when
// Reset() is called.
///////////////////////////////////////////////////////////////////////////////
CSurgeEngine::CSurgeEngine()
{
	Init();

	srvStats.SetCleanupFunct(DestroyServerStats);

    // socket/security intialization
    hSecuritydll = NULL; // loaded only if we need ssl
    WSADATA wsaData;
    unsigned short version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);
}


///////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////
CSurgeEngine::~CSurgeEngine()
{
    if(hSecuritydll) FreeLibrary(hSecuritydll);
    WSACleanup();
}


///////////////////////////////////////////////////////////////////////////////
// CSurgeEngine::Init
// Set all resettable internal variables to a well-known state.
///////////////////////////////////////////////////////////////////////////////
void CSurgeEngine::Init()
{
    // network settings
    port = 80;
    sslProtocol = SP_PROT_SSL3;
    cookieJar = new Cookie;
    useHead = FALSE;
    httpHeader = new Browser;
	m_fIsHostHeaderOverridden = false;
    ssl = FALSE;
    cookie[0] = '\0';
    followRedirects = FALSE;
    foundCookies = 0;
    getFullPage = FALSE;
    numRedirects = 5;

    seed=0;
    lastError = 0;
    httpStatus = 0;
    server[0] = site[0] = '\0';

    // logging
    loggingFlags = LOGGING_CLEAN;
    messageWindow = NULL;
    resultsLinePos = 0;

    userdb = NULL;

    useServerBlacklist = TRUE;
	timers = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// CSurgeEngine::Shutdown
// Clear out any allocated resources on desturction or a call to Reset()
///////////////////////////////////////////////////////////////////////////////
void CSurgeEngine::Shutdown()
{
	if(httpHeader) 
	{
		delete httpHeader;
		httpHeader = NULL;
	}
    if(cookieJar)
	{
		delete cookieJar;
		cookieJar = NULL;
	}
    if(userdb) 
	{
		delete userdb;
		userdb = NULL;
	}
	if(timers) 
	{
		delete timers;
		timers = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// CSurgeEngine::Reset
// Called to put the component in a well-known "default" state.  Caller will
// have to re-add any custom headers by hand.
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSurgeEngine::Reset()
{	
	Shutdown();
	ResetResults();
	Init();
	return S_OK;
}




/*

Routine Description:



Arguments:



Return Value:



Notes:



*/
int GetUserFromDB(FastProfDB *db, BOOL randomUsers, BOOL bad, char *username, char *domain, char *pw)
    {
    LPSTR lpstrSearchResult;

    // get a user from a user profile db
    if(!db)
        {
        strcpy(username, "jpoley");
        strcpy(domain, "passport.com");
        strcpy(pw, "jpoley");
        return FALSE;
        }

    lpstrSearchResult = (randomUsers ? db->GetRandUser() : db->GetNextUser() );

    db->GetNamePW(lpstrSearchResult, username, domain, pw, BUFSIZE);
    if(bad)
        {
        username[0] = 'S'; // modify the username
        pw[0] = (char)(pw[0] + 1); // modify the password
        }

    return TRUE;
    }

/*

Routine Description:



Arguments:



Return Value:



Notes:



*/
void CSurgeEngine::ReportError(long error, char* lpszFormat, ...)
    {
    if(loggingFlags == LOGGING_OUT_NONE) return;

    va_list args;
    va_start(args, lpszFormat);

    char dbuffer[10];
    char tbuffer[10];
    _strdate(dbuffer);
    _strtime(tbuffer);

    char *szBuffer = new char[65536];
    if(!szBuffer) return;
    char *szFormat = new char[strlen(lpszFormat) + 128]; // + 128 for timestamp and error values
    if(!szFormat)
        {
        delete[] szBuffer;
        return;
        }

    sprintf(szFormat, "%s, %s, %Ld, 0x%LX, %s\n", tbuffer, dbuffer, error, error, lpszFormat);
    vsprintf(szBuffer, szFormat, args);

    if(loggingFlags & LOGGING_OUT_FILE)
        {
        FILE *f = fopen(SURGE_ERROR_FILENAME, "a+");
        if(f)
            {
            fprintf(f, "%s", szBuffer);
            fclose(f);
            }
        }

    if(loggingFlags & LOGGING_OUT_DEBUG)
        {
        OutputDebugString(szBuffer);
        }

    if((loggingFlags & LOGGING_OUT_MESSAGE) && messageWindow)
        {
        COPYDATASTRUCT cpdata;
        cpdata.cbData = MAXBUF;
        cpdata.lpData = szBuffer;
        cpdata.dwData = error;
        SendMessage(messageWindow, WM_COPYDATA, error, (LPARAM)&cpdata);
        }

    va_end(args);
    delete[] szBuffer;
    delete[] szFormat;
    }

/*

Routine Description:



Arguments:



Return Value:



Notes:



*/
void CSurgeEngine::ReportDump(const char *title, int isRaw, const char * buffer, DWORD length)
    {
    if(loggingFlags == LOGGING_OUT_NONE) return;

    char dbuffer[10];
    char tbuffer[10];
    _strdate(dbuffer);
    _strtime(tbuffer);

    FILE *f = fopen(SURGE_ERROR_FILENAME, "a+b");
    if(f) fprintf(f, "%s, %s, %s\r\n", tbuffer, dbuffer, title);

    //
    // Raw Dump
    //
    if(isRaw)
        {
        if((loggingFlags & LOGGING_OUT_FILE) && f)
            {
            fwrite(buffer, length, 1, f);
            fprintf(f, "\r\n");
            }

        if(f) fclose(f);
        return;
        }

    //
    // Hex Dump
    //
    DWORD i,count,index;
    CHAR rgbDigits[]="0123456789ABCDEF";
    CHAR rgbLine[100];
    char cbLine;

    for(index = 0; length; length -= count, buffer += count, index += count)
        {
        count = (length > 16) ? 16:length;

        sprintf(rgbLine, "%4.4x  ",index);
        cbLine = 6;

        for(i=0;i<count;i++)
            {
            rgbLine[cbLine++] = rgbDigits[buffer[i] >> 4];
            rgbLine[cbLine++] = rgbDigits[buffer[i] & 0x0f];
            if(i == 7)
                {
                rgbLine[cbLine++] = ' ';
                }
            else
                {
                rgbLine[cbLine++] = ' ';
                }
            }
        for(; i < 16; i++)
            {
            rgbLine[cbLine++] = ' ';
            rgbLine[cbLine++] = ' ';
            rgbLine[cbLine++] = ' ';
            }

        rgbLine[cbLine++] = ' ';

        for(i = 0; i < count; i++)
            {
            if(buffer[i] < 32 || buffer[i] > 126)
                {
                rgbLine[cbLine++] = '.';
                }
            else
                {
                rgbLine[cbLine++] = buffer[i];
                }
            }

        rgbLine[cbLine++] = 0;
        if(f) fprintf(f, "%s\r\n", rgbLine);
        else printf("%s\r\n", rgbLine);
        }

    if(f) fclose(f);
    }

/*

Routine Description:

    CSurgeThread::ConnectBatch
    This function just does the common CHttpClient connection calls
    
Arguments:

    CHttpClient &client - 
    ReportStat &stats   -
    char *server        -
    BOOL ssl            -
    int action          -

Return Value:

    long - 

Notes:

*/
long CSurgeEngine::ConnectBatch(const char *server, BOOL ssl, DWORD &latency)
    {
    if(ssl && !hSecuritydll) hSecuritydll = LoadLibrary("security.dll");

    DWORD time = GetTickCount();

    lastError = client.Open();
    client.SetDefaultPort(port);

    // client.SetProxy("itgproxy", 80);

    if(ssl)
        {
        lastError = client.SetSSL(NULL, sslProtocol);
        if(lastError)
            {
            latency = GetTickCount() - time;
            ReportError(lastError, "CHttpClient.SetSSL, %s%s", server, site);
            client.Disconnect();
            return lastError;
            }
        }

    lastError = client.Connect(server);

    latency = GetTickCount() - time;

    if(lastError && lastError != SEC_E_UNTRUSTED_ROOT)
        {
        ReportError(lastError, "CHttpClient.Connect, %s%s", server, site);
        client.Disconnect();
        return lastError;
        }

    return lastError = 0;
    }

/*

Routine Description:

    CSurgeThread::SendReceiveBatch
    This function just does the common CHttpClient communication calls
    
Arguments:



Return Value:



Notes:



*/
long CSurgeEngine::SendReceiveBatch(DWORD &latency, char *server)
    {
    DWORD time = GetTickCount();

    foundCookies = 0;

    // client.data and client.dataLen MUST be prepopulated with the data to send

    if(loggingFlags & LOGGING_HTTPDUMPHEX)
        {
        ReportDump("Sent:", false, client.GetData(), client.GetDataLen());
        }
    else if(loggingFlags & LOGGING_HTTPDUMPRAW)
        {
        ReportDump("Sent:", true, client.GetData(), client.GetDataLen());
        }

    lastError = client.Send();
    if(lastError)
        {
        latency = GetTickCount() - time;
        ReportError(lastError, "CHttpClient.Send, %s%s", server, site);
        client.Disconnect();
        return lastError;
        }

    do
        {
        lastError = client.Receive(getFullPage);
        if(loggingFlags & LOGGING_HTTPDUMPHEX)
            {
            ReportDump("Received:", false, client.GetData(), client.GetDataLen());
            }
        else if(loggingFlags & LOGGING_HTTPDUMPRAW)
            {
            ReportDump("Received:", true, client.GetData(), client.GetDataLen());
            }

        httpStatus = client.GetHTTPStatus();

        if(lastError != 0)
            {
            ReportError(lastError, "CHttpClient.Receive, %s%s", server, site);
            break;
            }

        lastError = httpStatus;

        } while(httpStatus == HTTP_STATUS_CONTINUE && !getFullPage);
        
    latency = GetTickCount() - time;

    if(httpStatus == HTTP_STATUS_CONTINUE && (strncmp(client.GetBody(), "HTTP", 4)==0))
        {
        size_t size = client.dataLen - (client.GetData()-client.GetBody());
        memmove(client.GetData(), client.GetBody(), size);
        lastError = httpStatus = client.GetHTTPStatus();
        }

    if(httpStatus && (httpStatus >= ((loggingFlags&LOGGING_HTTPMASK)>>4)*100))
        {
        ReportError(httpStatus, "%s%s, %s", server, site, ErrorNo(ERRCLASS_HTTP, httpStatus));
        }

    foundCookies = client.GrabCookies(cookieJar, client.GetData(), server);

    client.Disconnect();
    return (long)httpStatus;
    }

/*

Routine Description:

    CSurgeThread::GetPage
    
Arguments:



Return Value:



Notes:



*/
long CSurgeEngine::GetPage(char *url, char *postdata/*=NULL*/)
    {
    DWORD sentSize = 0;
    DWORD connectLatency = 0;
    DWORD transactionLatency = 0;
    long error;
    char datasize[32];
    size_t len;
    size_t postLen=0;
    ServerStats *s;
    char keyUrl[4096];
    httpStatus = 0;
    char placeholder;
    int redirects = 0;

    // strip off the query string for the key
    len = strcspn(url, "?");
    strncpy(keyUrl, url, len);
    keyUrl[len] = '\0';

    if(followRedirects) redirects = numRedirects;

    client.ParseURL(url, server, site, &ssl);
    do
        {
        if(postdata)
            {
            postLen = strlen(postdata);
            sprintf(datasize, "%u", postLen);
            // NOTE: The below line was commented out to provide necessary POST custimization
            //httpHeader->AddHeader("Content-Type", "application/x-www-form-urlencoded");
            httpHeader->AddHeader("Content-Length", datasize);
            }
        else
            {
            httpHeader->AddHeader("Content-Type", NULL);
            httpHeader->AddHeader("Content-Length", NULL);
            }

        s = NULL;
        srvStats.Get(keyUrl, s);
        if(s)
            {
            if(useServerBlacklist && s->dwInternalState)
                {
                if(--s->dwRetryCountdown == 0)
                    {
                    // retry, and if failed continue the countdown
                    s->dwRetryCountdown = s->dwInternalState *= 2;
                    }
                else
                    {
                    // skip this request cause the server isnt responding
                    // TODO return a blacklist error code
                    return lastError=WSAETIMEDOUT;
                    }
                }
            }

        cookieJar->Eval(cookie, server);
        httpHeader->AddHeader("Cookie", cookie);

		// if the user has not set the Host header variable, make
		// sure we have something in there
        if ( !httpHeader->IsHeaderSet("Host") || !m_fIsHostHeaderOverridden)
		{
			httpHeader->AddHeader("Host", server);
		}
        client.dataLen = httpHeader->EvalHeader((useHead ? "HEAD" : (postdata ? "POST" : "GET")), site, client.GetData());
        if(postdata)
            {
            strcpy(&client.data[client.dataLen], postdata);
            client.dataLen += postLen;
            }
        sentSize = client.dataLen;

        if((error=ConnectBatch(server, ssl, connectLatency)) != 0)
            {
            s = InsertResult(s, keyUrl, error, connectLatency);
            // TODO blacklist on ANY connect error?
            if(error == WSAETIMEDOUT)
                {
                if(!s->dwInternalState) s->dwInternalState = 4;
                s->dwRetryCountdown = s->dwInternalState;
                ReportError(WSAETIMEDOUT, "Connect Error, %s will be skipped (retry in %Lu attempts)", keyUrl, s->dwRetryCountdown);
                }
            break;
            }

        // Send & Read the response
        error=SendReceiveBatch(transactionLatency, server);

        // Update stats
        s = InsertResult(s, keyUrl, lastError, connectLatency, transactionLatency, sentSize, client.dataLen);
        s->dwInternalState = 0; // reset blacklist

        // check for redirects
        if(followRedirects && (error == 300 || error == 301 || error == 302 || error == 303))
            {
            postdata = NULL;
            char *l = strstr(client.data, "Location: ");
            if(l)
                {
                //__asm int 3;
                --redirects;

                // get rid of any query string
                len = strcspn(site, "?");
                if(len) site[len] = '\0';

                // find where we need to go
                l += strlen("Location: ");
                len = strcspn(l, "\r\n");
                placeholder = l[len];
                l[len] = '\0';

                // munge it into the site and server variables
                if(_strnicmp(l, "http", 4) == 0) client.ParseURL(l, server, site, &ssl);
                else if(l[0] == '\\' || l[0] == '/') strcpy(site, l);
                else
                    {
                    char *t = strrchr(site, '\\');
                    if(!t) t = strrchr(site, '/');
                    if(!t) strcpy(site+1, l);
                    else strcpy(t+1, l);
                    }
                l[len] = placeholder;

                // strip off any new query string for the key
                len = strcspn(site, "?");
                placeholder = site[len];
                site[len] = '\0';
                sprintf(keyUrl, "http%s://%s%s", (ssl?"s":""), server, site);
                site[len] = placeholder;
                continue;
                }
            }
        else break;

        } while(redirects);

    if(postdata)
        {
        httpHeader->AddHeader("Content-Type", NULL);
        httpHeader->AddHeader("Content-Length", NULL);
        }

    return error;
    }

ServerStats* CSurgeEngine::InsertResult(ServerStats *base, char *key, long status, DWORD clat /*=(DWORD)~0*/, DWORD tlat /*= (DWORD)~0*/, DWORD bytesSent /*=(DWORD)0*/, DWORD bytesReceived /*= (DWORD)0*/)
    {
    ServerStats *s;
    if(!base)
        {
        base = s = new ServerStats;
        if(!s) return NULL;
        srvStats.Add(key, s);
        s->dwStatus = status;
        }
    else
        {
        for(s = base; s; s=s->next)
            {
            if(s->dwStatus == (DWORD)status) break;
            if(!s->next)
                {
                s = s->next = new ServerStats;
                if(!s) return NULL;
                s->dwStatus = status;
                break;
                }
            }
        }

    ++s->dwCount;
    if(clat != (DWORD)~0) s->avgConnect.Insert(clat);
    if(tlat != (DWORD)~0) s->avgTransaction.Insert(tlat);
    if(bytesSent != (DWORD)0) s->avgBytesSent.Insert(bytesSent);
    if(bytesReceived != (DWORD)0) s->avgBytesReceived.Insert(bytesReceived);

    return base;
    }

/////////////////////////////////////////////////////////////////////////////
// CSurgeEngine

STDMETHODIMP CSurgeEngine::About()
    {
    MessageBox(NULL, "Surge Core Engine\n\nCopyright (C) 1999-2000 Microsoft", "Surge.Engine About", MB_OK);
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_messageWindow(long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

	*pVal = (long)messageWindow;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_messageWindow(long newVal)
    {
	messageWindow = (HWND)newVal;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GET(BSTR url)
    {
    USES_CONVERSION;
    if(!url) return E_INVALIDARG;

    char *aurl = W2A(url);
    if(strncmp(aurl, "http", 4) != 0) return E_INVALIDARG;

    GetPage(aurl);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::POST(BSTR url, BSTR data)
    {
    USES_CONVERSION;
    if(!url || !data) return E_INVALIDARG;

    char *aurl = W2A(url);
    if(strncmp(aurl, "http", 4) != 0) return E_INVALIDARG;

    GetPage(aurl, W2A(data));

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_httpStatus(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (short)httpStatus;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetUser(VARIANT_BOOL sequential, BSTR *username, BSTR *domain, BSTR *password)
    {
    // TODO watch for overflows
    if(!username || !domain || !password) return E_INVALIDARG;
    char u[MAXBUF];
    char d[MAXBUF];
    char p[MAXBUF];

    GetUserFromDB(userdb, !sequential, FALSE, u, d, p);

    *username = CreateBstrFromAnsi(u);
    *domain = CreateBstrFromAnsi(d);
    *password = CreateBstrFromAnsi(p);
    
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_usersFile(BSTR newVal)
    {
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;
    if(newVal[0] == '\0') return E_INVALIDARG;

    if(userdb) delete userdb;
    userdb = new FastProfDB(W2A(newVal));

    return S_OK;
    }


// ASSUMPTION: Host header set by browser definition file is not considered
// user over-ridden.
STDMETHODIMP CSurgeEngine::AddHeader(BSTR header, BSTR value)
{
    USES_CONVERSION;
    if( NULL == header ) 
	{
		return E_INVALIDARG;
	}

    char *aHeader = W2A(header);

	// If user overrides the Host header, we will not default to the
	// requested server name
    if ( (NULL == value) || ('\0' == value[0]) )
    {
		if( 0 == strcmp(aHeader, "Host") )
		{
			m_fIsHostHeaderOverridden = false;
		}
		httpHeader->AddHeader(aHeader, NULL);
    }
    else
    {
		if( 0 == strcmp(aHeader, "Host") )
		{
			m_fIsHostHeaderOverridden = true;
		}
		char *aValue = W2A(value);
		httpHeader->AddHeader(aHeader, aValue);
    }

    return S_OK;
}

STDMETHODIMP CSurgeEngine::put_browser(BSTR newVal)
    {
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;
    if(newVal[0] == '\0') return E_INVALIDARG;

    char *filename = W2A(newVal);

    FILE *f=fopen(filename, "r");
    if(!f)
        {
        ReportError(0, "Could not open Browser file: %s", filename);
        return S_OK;
        }
    fclose(f);

    if(httpHeader) delete httpHeader;
    httpHeader = new Browser(filename);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_sslProtocol(long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (long)sslProtocol;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_sslProtocol(long newVal)
    {
    sslProtocol = (DWORD)newVal;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_tcpPort(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (short)port;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_tcpPort(short newVal)
    {
    port = (WORD)newVal;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_followRedirects(VARIANT_BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (VARIANT_BOOL)followRedirects;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_followRedirects(VARIANT_BOOL newVal)
    {
    followRedirects = (BOOL)newVal;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::MD5Hash(BSTR input, BSTR *hash)
    {
    USES_CONVERSION;
    if(!hash) return E_INVALIDARG;
    if(!input)
        {
        *hash = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char h[36];
    ::MD5Hash(W2A(input), h);

    *hash = CreateBstrFromAnsi(h);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::DigestHash(BSTR algo, BSTR user, BSTR realm, BSTR password, BSTR nonce, BSTR noncecount, BSTR cnonce, BSTR qop, BSTR method, BSTR uri, BSTR entity, BSTR *hash)
    {
    USES_CONVERSION;
	if(!algo || !user || !realm || !password || !nonce || !method || !uri || !hash)
        {
        if(hash) *hash = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char session[129]; session[0] = '\0';
    char pwHash[129]; pwHash[0] = '\0';

    DigestFromCreds(W2A(algo), W2A(user), W2A(realm), W2A(password), W2A(nonce), W2A(noncecount), W2A(cnonce), W2A(qop), W2A(method), W2A(uri), W2A(entity), session, pwHash);

    *hash = CreateBstrFromAnsi(pwHash);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::URLEncode(BSTR url, BSTR *encoded)
    {
    USES_CONVERSION;
    if(!encoded) return E_INVALIDARG;
	if(!url)
        {
        *encoded = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char *p = W2A(url);
    char *result = new char[(strlen(p)+1)*3];
    if(!result)
        {
        *encoded = CreateBstrFromAnsi("");
        return E_OUTOFMEMORY;
        }

    client.URLEncode(result, p);

    *encoded = CreateBstrFromAnsi(result);
    delete[] result;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::URLDecode(BSTR url, BSTR *decoded)
    {
    USES_CONVERSION;
    if(!decoded) return E_INVALIDARG;
	if(!url)
        {
        *decoded = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char *p = W2A(url);
    char *result = new char[(strlen(p)+1)];
    if(!result)
        {
        *decoded = CreateBstrFromAnsi("");
        return E_OUTOFMEMORY;
        }

    client.URLDecode(result, p);

    *decoded = CreateBstrFromAnsi(result);
    delete[] result;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::POSTEncode(BSTR post, BSTR *encoded)
    {
    USES_CONVERSION;
    if(!encoded) return E_INVALIDARG;
	if(!post)
        {
        *encoded = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char *p = W2A(post);    
    char *result = new char[(strlen(p)+1)*3];
    if(!result)
        {
        *encoded = CreateBstrFromAnsi("");
        return E_OUTOFMEMORY;
        }

    client.POSTEncode(result, p);

    *encoded = CreateBstrFromAnsi(result);
    delete[] result;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_useHead(VARIANT_BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (VARIANT_BOOL)useHead;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_useHead(VARIANT_BOOL newVal)
    {
    useHead = (BOOL)newVal;
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetBody(BSTR *body)
    {
    if(!body)
        {
        return E_INVALIDARG;
        }

    *body = CreateBstrFromAnsi(client.GetBody());

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetHeader(BSTR *header)
    {
    if(!header)
        {
        return E_INVALIDARG;
        }

    char t='\0';
    char *temp = client.GetBody();
    if(temp)
        {
        t = temp[0];
        temp[0] = '\0';
        }

    *header = CreateBstrFromAnsi(client.GetData());

    if(temp) temp[0] = t;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::Log(long dwData, BSTR message)
    {
    USES_CONVERSION;

    ReportError(dwData, "User Message, %s", W2A(message));

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_loggingFlags(long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = loggingFlags;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_loggingFlags(long newVal)
    {
    loggingFlags = newVal;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GenerateRandom(short maxLength, short contents, BSTR *random)
    {
    if(!random)
        {
        return E_INVALIDARG;
        }

    size_t len=maxLength+1;
    if(len < 2) len = 2;

    char *str = NewRandomBuffer(2, len, (unsigned char)contents, len, seed);

    *random = CreateBstrFromAnsi(str);
    if(str) delete[] str;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetResultsHeader(BSTR *output)
    {
    if(!output)
        {
        return E_INVALIDARG;
        }

    *output = CreateBstrFromAnsi("URL, Error, Count, Connect Average, Connect Max, Connect Min, Transaction Avg, Transaction Max, Transaction Min, Bytes Out Avg, Bytes Out Max, Bytes Out Min, Bytes In Avg, Bytes In Max, Bytes In Min\r\n");
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetResults(BSTR *output)
    {
    if(!output)
        {
        return E_INVALIDARG;
        }

	char *buffer = new char[(srvStats.m_size+1) * 4096];
    char *ptr = buffer;

    if(!buffer)
        {
        return E_OUTOFMEMORY;
        }

    // Place header information in to the results, and advance the point by the number of bytes added (minus the NULL)
    ptr += sprintf(ptr, "URL, Error, Count, Connect Average, Connect Max, Connect Min, Transaction Avg, Transaction Max, Transaction Min, Bytes Out Avg, Bytes Out Max, Bytes Out Min, Bytes In Avg, Bytes In Max, Bytes In Min\r\n");

    for(size_t i=0; i<srvStats.m_size; i++)
        {
        if(srvStats.table[i])
            {
            for(HashEntry<ServerStats*> *h = srvStats.table[i]; h; h=h->next)
                {
                for(ServerStats *s = h->obj; s; s=s->next)
                    {
                    ptr += sprintf(ptr, "%s", h->key);
                    ptr += sprintf(ptr, ", %lu, %lu", s->dwStatus, s->dwCount);
                    if(s->avgConnect.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgConnect.Eval(), s->avgConnect.dwMax, s->avgConnect.dwMin);
                    else ptr += sprintf(ptr, ", NA, NA, NA");
                    if(s->avgTransaction.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgTransaction.Eval(), s->avgTransaction.dwMax, s->avgTransaction.dwMin);
                    else ptr += sprintf(ptr, ", NA, NA, NA");
                    if(s->avgBytesSent.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgBytesSent.Eval(), s->avgBytesSent.dwMax, s->avgBytesSent.dwMin);
                    else ptr += sprintf(ptr, ", 0, 0, 0");
                    if(s->avgBytesReceived.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgBytesReceived.Eval(), s->avgBytesReceived.dwMax, s->avgBytesReceived.dwMin);
                    else ptr += sprintf(ptr, ", 0, 0, 0");
                    ptr += sprintf(ptr, "\r\n");
                    }
                }
            }
        }

    if(i == 0) sprintf(ptr, "(no results yet)\r\n");

    *output = CreateBstrFromAnsi(buffer);
    delete[] buffer;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetResultsFor(BSTR key, BSTR *output)
    {
    USES_CONVERSION;
    if(!output) return E_INVALIDARG;
    if(!key)
        {
        *output = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char* aKey = W2A(key);

	char *buffer = new char[2 * 4096];
    char *ptr = buffer;
    if(!buffer)
        {
        return E_OUTOFMEMORY;
        }

    ServerStats *h=NULL;
    srvStats.Get(aKey, h);

    ptr += sprintf(ptr, "URL, Error, Count, Connect Average, Connect Max, Connect Min, Transaction Avg, Transaction Max, Transaction Min, Bytes Out Avg, Bytes Out Max, Bytes Out Min, Bytes In Avg, Bytes In Max, Bytes In Min\r\n");
    if(h)
        {
        for(ServerStats *s = h; s; s=s->next)
            {
            ptr += sprintf(ptr, "%s", aKey);
            ptr += sprintf(ptr, ", %lu, %lu", s->dwStatus, s->dwCount);
            if(s->avgConnect.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgConnect.Eval(), s->avgConnect.dwMax, s->avgConnect.dwMin);
            else ptr += sprintf(ptr, ", NA, NA, NA");
            if(s->avgTransaction.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgTransaction.Eval(), s->avgTransaction.dwMax, s->avgTransaction.dwMin);
            else ptr += sprintf(ptr, ", NA, NA, NA");
            if(s->avgBytesSent.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgBytesSent.Eval(), s->avgBytesSent.dwMax, s->avgBytesSent.dwMin);
            else ptr += sprintf(ptr, ", 0, 0, 0");
            if(s->avgBytesReceived.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgBytesReceived.Eval(), s->avgBytesReceived.dwMax, s->avgBytesReceived.dwMin);
            else ptr += sprintf(ptr, ", 0, 0, 0");
            ptr += sprintf(ptr, "\r\n");
            }
        }
    else sprintf(ptr, "(no results yet)\r\n");

    *output = CreateBstrFromAnsi(buffer);
    delete[] buffer;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetResultsLine(BSTR *output)
    {
    *output = NULL;
    if(!output)
        {
        return E_INVALIDARG;
        }

	char *buffer = new char[4096];
    char *ptr = buffer;
    if(!buffer)
        {
        return E_OUTOFMEMORY;
        }


    ++resultsLinePos;
    unsigned count = 0;

    for(size_t i=0; i<srvStats.m_size; i++)
        {
        if(srvStats.table[i])
            {
            for(HashEntry<ServerStats*> *h = srvStats.table[i]; h; h=h->next)
                {
                for(ServerStats *s = h->obj; s; s=s->next)
                    {
                    ++count;
                    if(count == resultsLinePos)
                        {
                        ptr += sprintf(ptr, "%s", h->key);
                        ptr += sprintf(ptr, ", %lu, %lu", s->dwStatus, s->dwCount);
                        if(s->avgConnect.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgConnect.Eval(), s->avgConnect.dwMax, s->avgConnect.dwMin);
                        else ptr += sprintf(ptr, ", NA, NA, NA");
                        if(s->avgTransaction.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgTransaction.Eval(), s->avgTransaction.dwMax, s->avgTransaction.dwMin);
                        else ptr += sprintf(ptr, ", NA, NA, NA");
                        if(s->avgBytesSent.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgBytesSent.Eval(), s->avgBytesSent.dwMax, s->avgBytesSent.dwMin);
                        else ptr += sprintf(ptr, ", 0, 0, 0");
                        if(s->avgBytesReceived.dwCount) ptr += sprintf(ptr, ", %lu, %lu, %lu", s->avgBytesReceived.Eval(), s->avgBytesReceived.dwMax, s->avgBytesReceived.dwMin);
                        else ptr += sprintf(ptr, ", 0, 0, 0");
                        ptr += sprintf(ptr, "\r\n");

                        *output = CreateBstrFromAnsi(buffer);
                        delete[] buffer;
                        return S_OK;
                        }
                    }
                }
            }
        }

    *output = NULL;

    delete[] buffer;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::ResetResults(void)
    {
    resultsLinePos = 0;

    for(size_t i=0; i<srvStats.m_size; i++)
        {
        if(srvStats.table[i])
            {
            for(HashEntry<ServerStats*> *h = srvStats.table[i]; h; h=h->next)
                {
                ServerStats *s = h->obj;
                delete s;
                h->obj = NULL;
                }
            delete srvStats.table[i];
            srvStats.table[i] = NULL;
            }
        }
	
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_cookieString(BSTR *pVal)
    {
    if(!pVal)
        {
        return E_INVALIDARG;
        }

    *pVal = CreateBstrFromAnsi(cookie);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_cookieString(BSTR newVal)
    {
    newVal = newVal; // just to stop the compiler from whining

    /*
    This function is obsolete since the cookie string will be overwritten 
    on the next call to POST or GET.
    */

    return E_NOTIMPL;

    /*
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;

    char *in = W2A(newVal);
    
    int followDomainRules = cookieJar->followDomainRules;
    if(cookieJar) delete cookieJar;
    cookieJar = new Cookie;
    cookieJar->followDomainRules = followDomainRules;
    cookieJar->Add(in, NULL);
    strcpy(cookie, in);
    httpHeader->AddHeader("Cookie", cookie);

    return S_OK;
    */
    }

STDMETHODIMP CSurgeEngine::SetReceiveTimeout(SHORT sec, SHORT msec)
    {
    client.SetRecvTimeout((UINT)sec, (UINT)msec);
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_randomSeed(long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (long)seed;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_randomSeed(long newVal)
    {
	seed = (unsigned) newVal;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_useServerBlacklist(VARIANT_BOOL *pVal)
    {
	if(!pVal) return E_INVALIDARG;

    *pVal = (VARIANT_BOOL)useServerBlacklist;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_useServerBlacklist(VARIANT_BOOL newVal)
    {
	useServerBlacklist = (BOOL)newVal;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::StartTimer(BSTR name)
    {
    USES_CONVERSION;
    if(!name) return E_INVALIDARG;

    char *aName = W2A(name);

    StressTimer *start;
    if(!timers) start = timers = new StressTimer(aName);
    else start = timers->Add(aName);

    if(start) start->Start();
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::StopTimer(BSTR name, long *ms)
    {
    USES_CONVERSION;
    if(!name || !ms) return E_INVALIDARG;

    *ms = 0;

    if(!timers) return S_OK;

    StressTimer *stop = timers->Find(W2A(name));
    if(stop)
        {
        *ms = (long)stop->Stop();
        }
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetTimer(BSTR name, long *ms)
    {
    USES_CONVERSION;
    if(!name || !ms) return E_INVALIDARG;

    *ms = 0;

    if(!timers) return S_OK;

    StressTimer *time = timers->Find(W2A(name));
    if(time)
        {
        *ms = (long)time->GetTime();
        }
    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetTimerAvg(BSTR name, long *ms)
    {
    USES_CONVERSION;
    if(!name || !ms) return E_INVALIDARG;

    *ms = 0;

    if(!timers) return S_OK;

    StressTimer *t = timers->Find(W2A(name));
    if(t && t->avg.dwCount)
        {
        *ms = (long)t->avg.dwSum/t->avg.dwCount;
        }

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetTimerCount(BSTR name, long *ms)
    {
    USES_CONVERSION;
    if(!name || !ms) return E_INVALIDARG;

    *ms = 0;

    if(!timers) return S_OK;

    StressTimer *t = timers->Find(W2A(name));
    if(t && t->avg.dwCount)
        {
        *ms = (long)t->avg.dwCount;
        }

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetTimerMax(BSTR name, long *ms)
    {
    USES_CONVERSION;
    if(!name || !ms) return E_INVALIDARG;

    *ms = 0;

    if(!timers) return S_OK;

    StressTimer *t = timers->Find(W2A(name));
    if(t && t->avg.dwCount)
        {
        *ms = (long)t->avg.dwMax;
        }

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetTimerMin(BSTR name, long *ms)
    {
    USES_CONVERSION;
    if(!name || !ms) return E_INVALIDARG;

    *ms = 0;

    if(!timers) return S_OK;

    StressTimer *t = timers->Find(W2A(name));
    if(t && t->avg.dwCount)
        {
        *ms = (long)t->avg.dwMin;
        }

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetLastError(long *pError)
    {
    if(!pError) return E_INVALIDARG;

    *pError = lastError;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_Cookies(BSTR name, /*[in, optional]*/ BSTR domain, BSTR *pVal)
    {
    USES_CONVERSION;
    if(!pVal) return E_INVALIDARG;
    if(!name || !cookieJar)
        {
        *pVal = CreateBstrFromAnsi("");
        return E_INVALIDARG;
        }

    char *aName = W2A(name);
    char *aDomain = W2A(domain);
    if(!*aDomain) aDomain = NULL;

    Cookie *c = cookieJar->Find(aName, aDomain);

    if(!c || !c->value)
        {
        *pVal = CreateBstrFromAnsi("");
        return S_OK;
        }

    *pVal = CreateBstrFromAnsi(c->value);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_Cookies(BSTR name, /*[in, optional]*/ BSTR domain, BSTR newVal)
    {
    USES_CONVERSION;
    if(!name) return E_INVALIDARG;

    char *aName = W2A(name);
    char *aValue = W2A(newVal);
    char *aDomain = W2A(domain);
    if(!*aDomain) aDomain = NULL;

    if(!cookieJar) cookieJar = new Cookie;
    if(!cookieJar) return E_OUTOFMEMORY;

    if(!newVal || aValue[0] == '\0')
        {
        cookieJar->Remove(aName, aDomain);
        }
    else
        {
        cookieJar->Add(aName, aValue, aDomain);
        }

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_cookiesSet(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (short)foundCookies;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_getFullPage(VARIANT_BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (VARIANT_BOOL)getFullPage;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_getFullPage(VARIANT_BOOL newVal)
    {
    getFullPage = (BOOL)newVal;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::GetCTime(long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = time(NULL);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::DNSLookup(BSTR address, BSTR *pVal)
    {
    USES_CONVERSION;
    if(!address || !pVal) return E_INVALIDARG;

    char output[1024];

    BOOL err = client.DNSLookup(W2A(address), output, 1024);

    if(err == FALSE)
        {
        *pVal = NULL;
        return S_OK;
        }

    *pVal = CreateBstrFromAnsi(output);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_cookieRules(long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = client.GetCookieRules();

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_cookieRules(long newVal)
    {
    client.SetCookieRules(newVal);

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::get_numRedirects(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = (short)numRedirects;

    return S_OK;
    }

STDMETHODIMP CSurgeEngine::put_numRedirects(short newVal)
    {
    if(newVal <= 0) numRedirects = 1;
    else numRedirects = newVal;

    return S_OK;
    }


// "ttfb = Time to First Byte"
// "ttlb = Time to Last Byte"
STDMETHODIMP CSurgeEngine::GetStats(BSTR strStatName, long *pVal)
    {
    // __asm int 3;
    USES_CONVERSION;
    if ((!strStatName) || (!pVal)) return E_INVALIDARG;

    char *aStatName = W2A(strStatName);
    
    if (lstrcmpi("ttfb", aStatName) == 0)
        {
        *pVal = client.GetTime(0);
        }
    else if (lstrcmpi("ttlb", aStatName) == 0)
        {
        *pVal = client.GetTime(1);
        }
    else
        {
        *pVal = 0;
        return E_INVALIDARG;
        }
    
    return S_OK;
    }
