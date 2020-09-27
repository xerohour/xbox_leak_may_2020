#include "calls.h"

#define VAL1	8
#define VAL2	9	// Removable for testing
#define RG1	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }
#define RG2	{ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 }  // Removable
#define RG  { 65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90}
#define LENGTH 10000

int func1(void);
int func2(void);
int (*pfn1)() = func1;
__int64 func3(void);

#pragma data_seg("_CODE")
char rg_global_base_1[16] = RG1;	// only initialized data ends up here
char  rg_global_char[26] = RG;
int   rg_global_int[26] = RG;
__int64   rg_global_int64[26] = RG;
long  rg_global_long[26] = RG;
float rg_global_float[26] = RG;
double rg_global_double[26] = RG;
long double rg_global_ldouble[26] = RG;

int (*pfn)() = func1;
int (*pfn2)() = func2;
int (*pfn222)();
__int64 (*pfn64)();

long  rg_global_long2[LENGTH] = {0};
int   rg_global_int2[26];

void Based(void)
{
	char	*stack_base_ptr1;

	int i1, i2, i11, i22, ifn, ifn2;
	int i;

	char  *p_global_char;
	int   *p_global_int;
	__int64   *p_global_int64;
	long  *p_global_long;
	long  *p_global_long2;
	float *p_global_float;
	double *p_global_double;
	long double *p_global_ldouble;
	int *p_global_int2;

	int (*pfn11)();
	int (*pfn111)() = func1;
	int (*pfn22)();
	
	p_global_char = &rg_global_char[1];
	p_global_int = &rg_global_int[2];
	p_global_int64 = &rg_global_int64[3];
	p_global_long = &rg_global_long[4];
	p_global_float = &rg_global_float[5];
	p_global_double = &rg_global_double[6];
	p_global_ldouble = &rg_global_ldouble[7];
	p_global_long2 = &rg_global_long2[LENGTH-1];

	pfn11 = func1;
	ifn = pfn();
	i1 = func1(); // Based
	i11 = pfn11();
	i11 = pfn111();
	pfn111 = func2;
	i11 = pfn111();

	__int64 i64 = func3();

	for(i=0; i<26; i++) rg_global_int2[i] = 66;
	p_global_int2 = &rg_global_int2[5];

	pfn22 = func2;
	pfn222 = pfn22;
	ifn2 = pfn222();
	ifn2 = pfn2();
	i2 = func2();
	i22 = pfn22();

	for(i=0; i<LENGTH; i++)
		 rg_global_long2[i] = 50;

	stack_base_ptr1 = &rg_global_base_1[7];

	rg_global_base_1[7] = VAL1;

}


int func1(void)
{
	int i_1;
	i_1 = 5;

	int j_1 = 5;

	for (int i =0; i < 10; i++)	// func1
	{
		j_1 += i;
	}

	return 64;
}

__int64 func3(void)
{
	int j_3 = 5;
	for (int i =0; i < 10; i++)	// func3
	{
		j_3 += i;
	}

	return 3333;
}

int func2(void)
{
	static int (*pfn100)() = func1;

	int i1 = pfn100() + 1;

	int (*pfn101)() = func1;

	int i2 = pfn101() + 2;

	static int rg_static_int[26];
	int *p_2 = &rg_static_int[1];
	int *p_static = &rg_static_int[0];
	rg_static_int[2] = 32;
	i1 = func1(); // func2
	return rg_static_int[2];
}
