#include <stdio.h>
#include "testg.h"
#define ArrayLength 100
#define StructString65 "String65String65String65String65String65String65String65String65"
#define LocalString65  "__local_String65__local_String65__local_String65__local_String65"
#define Flag 1

int func();
int recur_func();
int func_1param(int);
long func_2param(int, long);
double func_3param(int, long, float);
void nest_func();
int GlobalG;
typedef int My_typedef;

struct Struct 
{
   int IntN;
   char *String65;
} Struct;

union Union 
{
   int Int;
   long Long;
} Union;

enum Enum 
{
  one,
  two,
  three
} Enum;

void nest_func() {
 	if (func()) 
   	func_1param (func());
};

int func() { 
			return 33;
		   };

int func_1param(int Parameter) {
    return Parameter;
 };

long func_2param(int intParam, long longParam) 
{
    return intParam + longParam;
};

double func_3param(int intParam, long longParam, float floatParam)
{
	int i_3p = 1;
	long l_3p = 999;
	float f_3p = (float)2.6;
	double d_3p = intParam + longParam + floatParam;

	i_3p = func();
	i_3p++;
	l_3p += i_3p;
	f_3p = l_3p - i_3p;

	do { //func_3param
		l_3p++;
	}while(l_3p <1050);
 
    return d_3p;
};

int recur_func() {
   static x=1;
   x++;
   if (x<9)
	   recur_func(); // call line
   return x;
};

int TestG()
{
 	int Local;
 	char Char = 'f';
 	int Int;
 	long Long;
 	double Double;
 	long double Ldouble;
 	float Real;
 	char *String;
 	void *Void;
 	My_typedef Mine;
 
	struct Struct Struct_local;
	union  Union Union_local;
	enum   Enum Enum_local;

 	struct Struct Array_struct[ArrayLength];
 	int  Array1d[3];
 	long Array2d[3][3];
 	char Array3d[3][3][3];

	Union_local.Int = 5;
	Union_local.Long = 999;
	Enum_local = one;

 	if (Flag) 
	{
  		for (Int=0; Int < ArrayLength; Int++)
   	{						                      // not executable code line
			Array_struct[Int].IntN = Int;
			Array_struct[Int].String65 = StructString65;
		};	  
 	};

 	for (Int=0; Int<3; Int++) 
	{
     	Array1d[Int]=Int;
     	Array2d[Int][Int]=Int;
     	Array3d[Int][Int][Int]=(char)Int;
  	};

 	Local=17;
 	Array1d[2]=9;
 	Struct.IntN = 1;
 	Struct.String65 = StructString65;
 	Struct_local.IntN = 11;
 	Struct_local.String65 = LocalString65;
 	Int=2;Int=6;Int=9;
 	Real=(float)1.303;
 	Double=1.303;
 	Ldouble=1.303;
 	String="this is a string";
 	GlobalG=77;
 	Long=99;

 	Mine=func();

 	if (Local > 0) 
	{
 		while (Local == 1)  
			Real = (float)9.9999;    // dead code
		if (Local > 1)
   		{							// not executable code line
	 		while (Local == 17)
			{                 // not executable code line
	 			Real = (float)8.88888;
				break;
			}
		}
 		else
 			Real = (float)7.77;		// dead code
	}
 	else						
    {							// dead code
		Real = (float)6.1;	      // dead code
	}							// dead code

 	if (1 < Local) do { GlobalG++; } while (Local == 0);

 	for (int i=0; i<15; i++) 
	{
	 	Int = func_1param(Local);
	 	Long = func_2param(Int, Long);
	 	Double = func_3param(Int, Long, Real);
	 	Ldouble = func_3param(func_1param(Local), func_2param(Int, Long), Real);
		Int = foo_Include(i);
	 	Int = recur_func(); // initial call
  		nest_func(); // nest_func call line
	}

	Int=0;  Int=check_func(Int,90);
	Void = (void*)Int;

	if(func() == 33)goto LabeL;
	Int = 999;
LabeL: 	return 1;

}


int check_func( int a, int l )
{
	char test[] = "TestName";
	
   if( !a )
   {
      printf("%s: Passed , line: %d --- %d != 0\n",test, l, a );
   }
   else
   {
      printf("%s: Failure, line: %d --- %d = 0\n",test, l, a );
   }
   return a;
}
