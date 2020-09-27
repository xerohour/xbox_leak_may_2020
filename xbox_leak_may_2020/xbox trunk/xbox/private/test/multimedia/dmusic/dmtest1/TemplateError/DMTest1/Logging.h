#define DEBUG 1
#define FILE 2
#define CONSOLE 4
#define ALL 7


#define ABORTLOGLEVEL 1
#define FYILOGLEVEL 2


void Log
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags,
    LPSTR                   szFormat,
    ...
);

//Logs everywhere.
void Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
);


//So the test logging will still work.  Grrr.
void fnsLog
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags, 
    LPSTR                   szFormat,
    ...
);

void fnsLog
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
);



void LogSetDbgLevel(DWORD dwLevel);
void LogIncrementIndent();
void LogDecrementIndent();



