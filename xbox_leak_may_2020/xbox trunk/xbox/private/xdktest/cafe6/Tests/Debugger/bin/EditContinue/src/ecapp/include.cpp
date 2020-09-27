#include "include.h"



void baseInclude::virtual_func(int i)
{
	m_private = i;
	m_public = i;
}
int global2 = 0;
int iGlobal2;
int *piGlobal2 = &iGlobal2;

int Test_func_Include ( )
{
	derived1_Include drv1;
	drv1.virtual_func1();


	iGlobal2 = 0;
	*piGlobal2 = 1;
	int *pint = &iGlobal2;

	baseInclude b;
	b.virtual_func(0);

	baseInclude *pb2;
	baseInclude *pb1 = new baseInclude;
	pb2 = pb1;
	*pb1 = *pb1;

	::global2 = 1;

	return 0;
};





