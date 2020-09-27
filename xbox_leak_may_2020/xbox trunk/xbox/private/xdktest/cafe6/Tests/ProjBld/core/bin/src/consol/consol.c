#include <assert.h>
#include "consol.h"
#define ArrayLength 100
#define StructString65 "String65String65String65String65String65String65String65String65"
#define LocalString65  "__local_String65__local_String65__local_String65__local_String65"
#define Flag 1

int TestG(void);
int func();
int recur_func();
int foo(int);
void nest_func();
int Global;
typedef int My_typedef;

struct Struct 
{
   int IntN;
   char *String65;
} Struct;

void nest_func()
{
 	if (func()) 
   	foo (func());
};

int func()
{ 
	return 33;
};

int foo(int Parameter)
{
   return Parameter;
};

int recur_func() 
{
   static x=1;
   x++;
   if (x<9) recur_func();
   else return x;
};

int main()
{
	int iGo = TestG();
	return 0;
}

int TestG()
{
 	int Local;
 	int Int;
 	long Long;
 	double Double;
 	long double Ldouble;
 	float Real;
 	char *String;
 	void *Void;
 	My_typedef Mine = 99;
 
	struct Struct Struct_local;
 	struct Struct Array_struct[ArrayLength];
 	int  Array1d[3];
 	long Array2d[3][3];
 	char Array3d[3][3][3];

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
     	Array3d[Int][Int][Int]=Int;
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
 	Global=77;
 	Long=99;
 	Int=func();

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

 	if (1 < Local) do { Double = 555.555; } while (Local == 0);

 	Int=foo(Local);
	Int = foo_Include(Int);
	Mine = foo_Include(Mine);
 	Int=recur_func();
	Void = (void*)Int;
  	nest_func();
if(func() == 33)goto LabeL;
	Int = 999;
LabeL: 	return 1;
}


