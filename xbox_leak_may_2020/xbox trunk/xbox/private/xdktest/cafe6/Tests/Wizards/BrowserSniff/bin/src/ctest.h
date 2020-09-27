/*
**	ctest.h
**		common "C" declarations
*/

#if defined( __cplusplus ) 
extern "C" {
#endif

typedef union holdtype_ {
	char c;
	short s;
	int i;
	long l;
	float f;
	double d;
} HOLDTYPE;

enum {
	TYPE_CHAR = 1,
	TYPE_SHORT,
	TYPE_INT,
	TYPE_LONG,
	TYPE_FLOAT,
	TYPE_DOUBLE,
};

typedef struct bitfield_ {
	int bit1 : 1;
	int bit2to4 : 3;
} BITFIELD;

typedef struct cstruct_ {
	BITFIELD type;
	HOLDTYPE data;
	struct cstruct_ *next;
} CSTRUCT;

#define MAX_ITEMS 10

extern int global_array[];

void ctest();
CSTRUCT* adddata( CSTRUCT* head, int type, void* data );

#if defined( __cplusplus ) 
};
#endif