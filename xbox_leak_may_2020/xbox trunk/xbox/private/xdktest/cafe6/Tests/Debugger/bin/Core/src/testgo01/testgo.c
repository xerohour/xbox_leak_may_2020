/*********************************************/
/* testgo.c                                  */
/*********************************************/
/* CVM : g : Go command                      */
/*********************************************/
#include <stdio.h>
#include "testg.h"
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

void nest_func()
{
 	if (func()) 
   	foo (func());
};
int func() { 
			return 33;
		   };
int foo(int Parameter) {
    return Parameter;
 };

int recur_func() 
{
   static x=1;
   x++;
   if (x<9) recur_func();
   else return x;
};
#include <windows.h>
int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
//	char c;
//	unsigned int ui;
	int iGo = TestG();
// 	c = getchar(&ui);
	return 0;
}

int TestG()
{
 	int Local;
 	char Char;
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
 	Real=1.303;
 	Double=1.303;
 	Ldouble=1.303;
 	String="this is a string";
 	Global=77;
 	Long=99;
 	Int=func();

 	if (Local > 0) 
	{
 		while (Local == 1)  
			Real = 9.9999;    // dead code
		if (Local > 1)
   	{							// not executable code line
	 		while (Local == 17)
			{                 // not executable code line
	 			Real = 8.88888;
				break;
			}
		}
 		else
 		 Real = 7.77;		// dead code
	}
 	else						
   {							// dead code
      Real = 6.1;	      // dead code
	}							// dead code

 	if (1 < Local) do { Double = 555.555; } while (Local == 0);

DA_FUNC:	                        // label
 	Int=foo(Local);
	Int = foo_Include(Int);
 	Int=recur_func();
 	Int=0;  Int=check_func(Int,90);
	Void = (void*)Int;
  	nest_func();
if(func() == 33)goto LabeL;
	Int = 999;
LabeL: 	return 1;
}


int check_func( int a, int l )
{
	char test[] = "TestName";
	
   if( !a )
   {
//      printf("%s: Passed , line: %d --- %d != 0\n",test, l, a );
   }
   else
   {
//      printf("%s: Failure, line: %d --- %d = 0\n",test, l, a );
   }
   return a;
}
