//#include <afx.h>
#include <iostream.h>
#include <assert.h>

#include "spcfc01.h"
#define ArrayLength 100
#define StructString65 "String65String65String65String65String65String65String65String65"
#define LocalString65  "__local_String65__local_String65__local_String65__local_String65"
#define Flag 1

int Global;

struct Struct 
{
   int IntN;
   char *String65;
} StructG;

union Union 
{
   int Int;
   long Long;
} UnionG;

enum Enum 
{
  zero,
  one,
  two,
  three
} EnumG;


int myAssert(int exp)
{
	__asm int 3
	return 1;
}

static int foo(int Parameter);
int foo(int Parameter)
{
	int i = foo_Include(Parameter);
    return (i + Parameter);
};

int recur_func() 
{
   static x=1;
   x++;
   if (x<9) return recur_func();
   else return x;
};

int TestG()
{
 	int Local;
 	int Int;
 	long Long;
 	double Double;
 	long double Ldouble;
 	float Real;
 	char *String;
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
 	StructG.IntN = 1;
 	StructG.String65 = StructString65;
 	Struct_local.IntN = 11;
 	Struct_local.String65 = LocalString65;
 	Int=2;Int=6;Int=9;
 	Real=(float)1.303;
 	Double=1.303;
 	Ldouble=1.303;
 	String="this is a string";
 	Global=77;
 	Long=99;
 	Int=func(3);
 	Int=foo(Local);
	Int = foo_Include(Int);
	Mine = foo_Include(Mine);
 	Int=recur_func();
  	nest_func(foo_Include(foo(Global)));

	Int = foo( foo_Include( foo( func(2) ) ) );
	Int = foo( foo_Include( foo( recur_func() ) ) );
	Int = foo( foo_Include(foo(foo_Include(func(5)))));
	Mine = foo_Include( foo_Include( foo( func(7) ) ) );

if(func(3) == 33)goto LabeL;
	Int = 999;
LabeL: 	return 1;
}



template <class Type> class Array;
template <class Type> ostream& 
	operator<<(ostream&, Array<Type>&);

const int ArraySize = 12;

template <class Type>
class Array 
{
public:
	Array(int sz=ArraySize) { init(0,sz); }
	Array(const Type *ar, int sz) { init(ar,sz); }
	Array(const Array &iA) { init(iA.ia, iA.size); }
	~Array() { delete [] ia; }

	Array& operator=(const Array&);
	int getSize() {return size; }
	void grow();
	Array& ngrow(int = 1);
	void print(ostream& = cout);

	Type& operator[](int ix) { return ia[ix]; }
	int find(Type);
	Type minar();
	Type maxar();
	static void static_func(int);

private:
	void swap(int,int);
	void init(const Type*, int);

	int size;
	Type *ia;

	static int static_int;
};

int Array<int>::static_int = 5;
int Array<double>::static_int = 15;

template <class Type> ostream&
	operator<<(ostream& os, Array<Type>& ar)
{
	ar.print(os);
	return os;
}

template <class Type> 
void Array<Type>::print(ostream& os)
{
	const lineLength = 12;
	
	os << " { " << size << " )< " ;
	for (int ix = 0; ix < size; ++ix)
	{
		if (ix % lineLength == 0 && ix) os << "\n\t";
		os << ia[ix];

		if (ix % lineLength != lineLength - 1 && ix != size - 1)
			os << ", ";
	}
	os << " >\n";
}


template <class Type> Array<Type>&
Array<Type>::operator=(const Array<Type> &iA)
{
	if (this == &iA) return *this;
	delete [] ia;
	init( iA.ia, iA.size );
	return *this;
}

template <class Type> void
Array<Type>::init(const Type *array, int sz)
{
	ia = new Type[size = sz];
	assert( ia != 0 );
					   
	for (int ix = 0; ix < size; ++ix)
		ia[ix] = (array!=0) ?  array[ix] : (Type)0;

	static_int = size;
}

template <class Type> Array<Type>&
Array<Type>::ngrow(int igrow)
{
	for (int i = 0; i < igrow; i++)
	    this->grow();
	return *this;
}

template <class Type> void
Array<Type>::grow()
{
	Type *oldia = ia;
	int oldSize = size;
	int newSize = oldSize + oldSize/2 + 1;

	ia = new Type[size = newSize];
	assert( ia != 0 );
	
	for (int i=0; i<oldSize; ++i) ia[i] = oldia[i];
	for (; i<size; ++i) ia[i] = (Type)0;

	delete oldia;
}


template <class Type> Type 
Array<Type>::minar()
{
	assert( ia != 0 );
	Type min_val = ia[0];
	
	for (int ix=1; ix<size; ++ix)
		if(min_val > ia[ix]) min_val = ia[ix];

	return min_val;
}


template <class Type> 
Type Array<Type>::maxar()
{
	assert( ia != 0 );
	Type max_val = ia[0];
	
	for (int ix=1; ix<size; ++ix)
		if(max_val < ia[ix]) max_val = ia[ix];

	return max_val;
}


template <class Type> 
int Array<Type>::find(Type val)
{
	for (int ix=1; ix<size; ++ix)
		if(val == ia[ix]) return ix;

	return -1;
}


template <class Type> 
void Array<Type>::swap(int i, int j)
{
	Type tmp = ia[i];
	ia[i] = ia[j];
	ia[j] = tmp;
}

template <class Type> void 
Array<Type>::static_func(int par)
{
	int ix = static_int + par;
}

template <class Type>
void try_array( Array<Type> &iA )
{
	Type find_val = iA[2];
	iA[iA.getSize()-1] = iA.minar();

	int mid = iA.getSize()/2;
	iA.static_func(mid);
	iA[0] = iA.maxar();
	iA[mid] = iA[0];

	Array<Type> iA2 =iA;
	iA[mid/2] = iA[mid];

	iA = iA2;

	iA.grow();

	int index = iA.find(find_val);

	if(index < 0)
	;
  	else
	{
		Type value = iA[index];
	}

}

int main(void)
{
	static int ia[] = {12, 7, 14, 9, 128, 17, 6, 3, 27, 5};
	static double da[] = {12.3, 7.9, 14.6, 9.8, 128.0};

	Array<int> iA(ia, sizeof(ia)/sizeof(int));
	Array<double> dA(da, sizeof(da)/sizeof(double));

	cout << "template Array<int> class\n" << endl;
	try_array(iA);
	try_array(iA.ngrow(2));

	cout << "template Array<double> class\n" << endl;
	try_array(dA);
	iA.static_func(iA.getSize());
	dA.static_func(iA.getSize());

	int iGo = TestG();

	return 0;
}


