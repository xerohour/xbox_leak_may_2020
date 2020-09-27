/*
**	ctest.c
**		c code
*/

#include "browse.h"
#include "ctest.h"
#include "malloc.h"

int global_array[MAX_ITEMS];

static CSTRUCT static_array[MAX_ITEMS];

static CSTRUCT* addtail();
static void setdata( CSTRUCT* ptr, int type, void* data );

void ctest() {
	CSTRUCT c;
	c.type.bit2to4 = TYPE_FLOAT;
	c.data.f = 5.123f;

	static_array[1] = c;
}

CSTRUCT* adddata( CSTRUCT* head, int type, void* data )
{
	CSTRUCT *p;
	p = malloc( sizeof( CSTRUCT ) );
	if( p )
	{
		setdata( p, type, data );
		return addtail( head, p );
	}
	else
		return NULL;
}

static CSTRUCT* addtail( head, data )

CSTRUCT *head, *data;

{
	if( head == NULL )
	{
		data->next = NULL;
		return data;
	}
	else if( head->next == NULL )
	{
		data->next = NULL;
		head->next = data;
		return data;
	}
	else
	{
		return addtail( head->next, data );
	}
}

static void setdata( CSTRUCT* ptr, int type, void* data )
{
	switch( type )
	{
		case TYPE_CHAR:
			ptr->data.c = *((char*)data);
			ptr->type.bit2to4 = TYPE_CHAR;
			break;

		case TYPE_SHORT:
			ptr->data.s = *((short*)data);
			ptr->type.bit2to4 = TYPE_SHORT;
			break;

		case TYPE_INT:
			ptr->data.i = *((int*)data);
			ptr->type.bit2to4 = TYPE_INT;
			break;

		case TYPE_LONG:
			ptr->data.l = *((long*)data);
			ptr->type.bit2to4 = TYPE_LONG;
			break;

		case TYPE_FLOAT:
			ptr->data.f = *((float*)data);
			ptr->type.bit2to4 = TYPE_FLOAT;
			break;

		case TYPE_DOUBLE:
			ptr->data.d = *((double*)data);
			ptr->type.bit2to4 = TYPE_DOUBLE;
			break;

	}
}

// Add another definition for m_i to spice up ambiguity testing
static int m_i = 0;

void c_refto_mi() { int i = m_i; }