#define VERSION_NUMERIC 40
#define VERSION_STRING ".40"


#if PSS_STYLE==2

#define PSS "\\"
#define PS '\\'

#elif PSS_STYLE==1

#define PSS "/"
#define PS '/'

#elif PSS_STYLE==3

#define PSS ":"
#define PS ':'

#endif
