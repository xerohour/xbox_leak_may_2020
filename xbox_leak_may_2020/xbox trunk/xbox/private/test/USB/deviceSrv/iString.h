#ifndef _ISTRING_H_
#define _ISTRING_H_

class iString
    {
    public:
        char *str;
        char *end;
        unsigned bufferSize;
        bool aggressive;
        bool allocated;

    public:
        iString();
        iString(unsigned initialSize);
        ~iString();

    public:
        void* Grow(unsigned count = 0);
        unsigned length(void) { return end-str; }
        operator char*() { return str; }
        void Attach(char *src);
        unsigned Hash(void);

    public:
        size_t strlen(void) { return end-str; }
        char* sprintf(char *format, ...);
        char* scatf(char *format, ...);
        char* strcpy(const char *src);
        char* strcat(const char *src);
        char* strncpy(const char *src, unsigned len);
        char* strncat(const char *src, unsigned len);
        char* strrchr(int ch);

    public:
        void DebugPrint(void);
    };

#endif // _ISTRING_H_