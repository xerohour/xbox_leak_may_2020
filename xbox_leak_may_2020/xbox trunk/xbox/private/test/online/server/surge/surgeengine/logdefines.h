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
