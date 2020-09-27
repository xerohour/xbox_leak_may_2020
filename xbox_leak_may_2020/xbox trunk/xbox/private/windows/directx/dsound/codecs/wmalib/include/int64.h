#ifndef _PRVINT64_
#define _PRVINT64_

typedef struct _INT64 { 
    unsigned long val[2];
} INT64;

typedef struct _UINT64 {   
    unsigned long val[2];
} UINT64;
#else
typedef struct _INT64 { 
    unsigned char val[8];
} INT64;

typedef struct _UINT64 {   
    unsigned char val[8];
} UINT64;
           
extern INT64 I64Add(const INT64 a, const INT64 b);
extern INT64 I64Sub(const INT64 a, const INT64 b);
extern INT64 I64Mul(const INT64 a, const INT64 b);
extern INT64 I64DIV(const INT64 a, const INT64 b);
extern INT64 I64MOD(const INT64 a, const INT64 b);
extern INT64 I64AND(const INT64 a, const INT64 b);
extern INT64 I64SHR(const INT64 a, const int unsigned b);
extern INT64 I64SHL(const INT64 a, const int unsigned b);
extern int   I64EQL(const INT64 a, const INT64 b);
extern int   I64LES(const INT64 a, const INT64 b);
extern INT64 I64(const long int b);
extern INT64 I64Asgn(const long int a, const long int b);
extern INT64 UI2I64(const UINT64 b);
extern unsigned long int I64toUI32(const INT64 b);

extern UINT64 UI64Add(const UINT64 a, const UINT64 b);
extern UINT64 UI64Sub(const UINT64 a, const UINT64 b);
extern UINT64 UI64Mul(const UINT64 a, const UINT64 b);
extern UINT64 UI64DIV(const UINT64 a, const UINT64 b);
extern UINT64 UI64MOD(const UINT64 a, const UINT64 b);
extern UINT64 UI64AND(const UINT64 a, const UINT64 b);
extern UINT64 UI64SHR(const UINT64 a, const unsigned int b);
extern UINT64 UI64SHL(const UINT64 a, const unsigned int b);
extern int    UI64EQL(const UINT64 a, const UINT64 b);
extern int    UI64LES(const UINT64 a, const UINT64 b);
extern UINT64 UI64(const unsigned long int b);
#endif
