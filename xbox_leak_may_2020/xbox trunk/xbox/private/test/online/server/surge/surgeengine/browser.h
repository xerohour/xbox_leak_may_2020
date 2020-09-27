
#ifndef _BROWSER_H_
#define _BROWSER_H_

#define MAX_HEADERS 50 // approx# based from HTTP 1.1 rfc

typedef struct
    {
    char *header;
    char *value;
    size_t headerLen;
    size_t valueLen;
    } HTTP_HEADER;

class Browser
    {
    public:
        HTTP_HEADER headers[MAX_HEADERS];
        char version[32];
        size_t versionLen;

        int length;

    public:
        Browser();
        Browser(char *filename);
        ~Browser();

        size_t EvalHeader(char *action, char *site, char *output);
        void AddHeader(char *header, char *value, size_t len=0);
        int Translate(char *h);
		bool IsHeaderSet(char *szHeaderName);
    };

#endif //_BROWSER_H_