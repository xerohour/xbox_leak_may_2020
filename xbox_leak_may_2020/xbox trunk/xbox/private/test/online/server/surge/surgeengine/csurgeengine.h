// CSurgeEngine.h : Declaration of the CSurgeEngine

#ifndef __SURGEENGINE_H_
#define __SURGEENGINE_H_

#include "resource.h"       // main symbols
#include "reporting.h"
#include "stresstimer.h"
#include "../HttpClient/HttpClient.h"
#include "FastProfDB.h"
#include "digest.h"
#include "browser.h"
#include "../common/ParserTools.h"
#include "SurgeEngine.h"

#define MAXBUF 2048
#define BUFSIZE MAXBUF

/*
Logging Flags

ddhhhoooo

dd   bits 78    - dump raw or hex http communications
hhh  bits 654   - display http status message where http status >= hhh * 100d
oooo bits 3210  - error output location
*/
#define LOGGING_OUT_NONE        0x0000 // 0
#define LOGGING_OUT_FILE        0x0001 // 1
#define LOGGING_OUT_DEBUG       0x0002 // 2
#define LOGGING_OUT_MESSAGE     0x0004 // 4
#define LOGGING_HTTPALL         0x0000 // 0
#define LOGGING_HTTP100         0x0010 // 16
#define LOGGING_HTTP200         0x0020 // 32
#define LOGGING_HTTP300         0x0030 // 48
#define LOGGING_HTTP400         0x0040 // 64
#define LOGGING_HTTP500         0x0050 // 80
#define LOGGING_HTTPNONE        0x0070 // 112
#define LOGGING_HTTPMASK        0x0070 // 112
#define LOGGING_HTTPDUMPHEX     0x0080 // 128
#define LOGGING_HTTPDUMPRAW     0x0100 // 256
#define LOGGING_ALL             0xFFFF
#define LOGGING_CLEAN           LOGGING_OUT_NONE | LOGGING_HTTPNONE

class ServerStats
    {
    public:
        DWORD dwStatus;
        DWORD dwCount;
        Average avgConnect;
        Average avgTransaction;
        Average avgBytesSent;
        Average avgBytesReceived;

        // Blacklist state members (only used on the head of the list)
        DWORD dwInternalState;
        DWORD dwRetryCountdown;

    public:
        ServerStats *next;

    public:
        ServerStats();
        ~ServerStats();
    };

/////////////////////////////////////////////////////////////////////////////
// CSurgeEngine
class ATL_NO_VTABLE CSurgeEngine : public CComObjectRootEx<CComSingleThreadModel>, public CComCoClass<CSurgeEngine, &CLSID_SurgeEngine>, public IDispatchImpl<ISurgeEngine, &IID_ISurgeEngine, &LIBID_SURGEENGINELib>
    {
    private:
        HINSTANCE hSecuritydll;

        // network items
        WORD port;
        DWORD sslProtocol;
        Cookie *cookieJar;
        char cookie[4096];
        char server[MAXBUF];
        char site[MAXBUF];
        BOOL ssl;
        CHttpClient client;
        Browser *httpHeader;
        int foundCookies;
        BOOL useHead;
        BOOL followRedirects;
        BOOL getFullPage;
        int numRedirects;
		BOOL m_fIsHostHeaderOverridden;

        // stats
        HashTable<ServerStats*, 13> srvStats;
        unsigned resultsLinePos;
        int httpStatus;
        long lastError;

        // settings
        unsigned seed;
        BOOL useServerBlacklist;

        // user stuff
        FastProfDB *userdb;

        // logging
        long loggingFlags;
        HWND messageWindow;
        StressTimer *timers;

        // optimizations

	private:
		void Init();
		void Shutdown();

    public:
	    CSurgeEngine();
        ~CSurgeEngine();

    public:
        long ConnectBatch(const char *server, BOOL ssl, DWORD &latency);
        long SendReceiveBatch(DWORD &latency, char *server);
        long GetPage(char *url, char *postdata=NULL);

        void ReportError(long error, char* lpszFormat, ...);
        void ReportDump(const char *title, int isRaw, const char * buffer, DWORD length);
        ServerStats* InsertResult(ServerStats *base, char *key, long status, DWORD clat = (DWORD)~0, DWORD tlat = (DWORD)~0, DWORD bytesSent =(DWORD)0, DWORD bytesReceived = (DWORD)0);

    public:
        DECLARE_REGISTRY_RESOURCEID(IDR_SURGEENGINE)
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        BEGIN_COM_MAP(CSurgeEngine)
	        COM_INTERFACE_ENTRY(ISurgeEngine)
	        COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

    // ISurgeEngine
    public:
	    STDMETHOD(GetStats)(BSTR strStatName, /*[out, retval]*/ long *pVal);
	    STDMETHOD(GetResultsHeader)(/*[out, retval]*/ BSTR *output);
	    STDMETHOD(GetResultsLine)(/*[out, retval]*/ BSTR *output);
	    STDMETHOD(get_numRedirects)(/*[out, retval]*/ short *pVal);
	    STDMETHOD(put_numRedirects)(/*[in]*/ short newVal);
	    STDMETHOD(get_cookieRules)(/*[out, retval]*/ long *pVal);
	    STDMETHOD(put_cookieRules)(/*[in]*/ long newVal);
	    STDMETHOD(GetTimerMin)(/*[in]*/ BSTR name, /*[out, retval]*/ long *ms);
	    STDMETHOD(GetTimerMax)(/*[in]*/ BSTR name, /*[out, retval]*/ long *ms);
	    STDMETHOD(GetTimerCount)(/*[in]*/ BSTR name, /*[out, retval]*/ long *ms);
	    STDMETHOD(GetTimer)(/*[in]*/ BSTR name, /*[out, retval]*/ long *ms);
	    STDMETHOD(DNSLookup)(/*[in]*/ BSTR address, /*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(GetCTime)(/*[out, retval]*/ long *pVal);
	    STDMETHOD(About)();
	    STDMETHOD(AddHeader)(BSTR header, BSTR value);
	    STDMETHOD(DigestHash)(BSTR algo, BSTR user, BSTR realm, BSTR password, BSTR nonce, BSTR noncecount, BSTR cnonce, BSTR qop, BSTR method, BSTR uri, BSTR entity, /*[out]*/ BSTR *hash);
	    STDMETHOD(GenerateRandom)(short maxLength, short contents, /*[out, retval]*/ BSTR *random);
	    STDMETHOD(GET)(BSTR url);
	    STDMETHOD(get_Cookies)(BSTR name, /*[in, optional]*/ BSTR domain, /*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(get_cookiesSet)(/*[out, retval]*/ short *pVal);
	    STDMETHOD(get_cookieString)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(get_followRedirects)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	    STDMETHOD(get_getFullPage)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	    STDMETHOD(get_httpStatus)(/*[out, retval]*/ short *pVal);
	    STDMETHOD(get_loggingFlags)(/*[out, retval]*/ long *pVal);
	    STDMETHOD(get_messageWindow)(/*[out, retval]*/ long *pVal);
	    STDMETHOD(get_randomSeed)(/*[out, retval]*/ long *pVal);
	    STDMETHOD(get_sslProtocol)(/*[out, retval]*/ long *pVal);
	    STDMETHOD(get_tcpPort)(/*[out, retval]*/ short *pVal);
	    STDMETHOD(get_useHead)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	    STDMETHOD(get_useServerBlacklist)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	    STDMETHOD(GetBody)(/*[out, retval]*/ BSTR *body);
	    STDMETHOD(GetHeader)(/*[out, retval]*/ BSTR *header);
	    STDMETHOD(GetLastError)(/*[out, retval]*/ long *pError);
	    STDMETHOD(GetResults)(/*[out, retval]*/ BSTR *output);
	    STDMETHOD(GetResultsFor)(BSTR key, /*[out, retval]*/ BSTR *output);
	    STDMETHOD(GetTimerAvg)(BSTR name, /*[out, retval]*/ long *ms);
	    STDMETHOD(GetUser)(/*[in]*/ VARIANT_BOOL sequential, /*[out]*/ BSTR *username, /*[out]*/ BSTR *domain, /*[out]*/ BSTR *password);
	    STDMETHOD(Log)(long dwData, BSTR message);
	    STDMETHOD(MD5Hash)(BSTR input, /*[out, retval]*/ BSTR *hash);
	    STDMETHOD(POST)(BSTR url, BSTR data);
	    STDMETHOD(POSTEncode)(BSTR post, /*[out, retval]*/ BSTR *encoded);
	    STDMETHOD(put_browser)(/*[in]*/ BSTR newVal);
	    STDMETHOD(put_Cookies)(BSTR name, /*[in, optional]*/ BSTR domain, /*[in]*/ BSTR newVal);
	    STDMETHOD(put_cookieString)(/*[in]*/ BSTR newVal);
	    STDMETHOD(put_followRedirects)(/*[in]*/ VARIANT_BOOL newVal);
	    STDMETHOD(put_getFullPage)(/*[in]*/ VARIANT_BOOL newVal);
	    STDMETHOD(put_loggingFlags)(/*[in]*/ long newVal);
	    STDMETHOD(put_messageWindow)(/*[in]*/ long newVal);
	    STDMETHOD(put_randomSeed)(/*[in]*/ long newVal);
	    STDMETHOD(put_sslProtocol)(/*[in]*/ long newVal);
	    STDMETHOD(put_tcpPort)(/*[in]*/ short newVal);
	    STDMETHOD(put_useHead)(/*[in]*/ VARIANT_BOOL newVal);
	    STDMETHOD(put_usersFile)(/*[in]*/ BSTR newVal);
	    STDMETHOD(put_useServerBlacklist)(/*[in]*/ VARIANT_BOOL newVal);
	    STDMETHOD(ResetResults)(void);
	    STDMETHOD(SetReceiveTimeout)(short sec, short msec);
	    STDMETHOD(StartTimer)(BSTR name);
	    STDMETHOD(StopTimer)(BSTR name, /*[out, retval]*/ long *ms);
	    STDMETHOD(URLEncode)(BSTR url, /*[out, retval]*/ BSTR *encoded);
	    STDMETHOD(URLDecode)(/*[in]*/ BSTR url, /*[out, retval]*/ BSTR *decoded);
		STDMETHOD(Reset)();
    };

#endif //__SURGEENGINE_H_
