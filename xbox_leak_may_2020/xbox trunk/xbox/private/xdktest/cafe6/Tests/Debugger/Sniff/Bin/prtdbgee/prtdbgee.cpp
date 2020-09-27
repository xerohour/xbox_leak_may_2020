/********************************************************************/
/*                        testx.cxx, C++ features                   */
/*                                                                  */
/*  designed to explore most c++ specific features                  */
/********************************************************************/
//  MAKE MAIN => LOCAL all               *** NYI in c700
//  #define make_local 1

#include <windows.h>

#define stream_io  1                     // comment out to use printf's

#ifdef stream_io
#include <iostream.h>
#else
#include <stdio.h>
#endif

#ifdef _M_MPPC
#include <macos/types.h>
#endif

/******************* function prototypes & global functions *************/
int foo();
//int func(int INT, int & rint, const int CINT, fooclass & fc_obj,int defualt=111);
int buddy();

int foo() {
   return 5;
};



/******************************************************************/
/*********************  main here makes rest local  ***************/
/******************************************************************/
#ifdef make_local
int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow) {
#endif

/*********************** FOOCLASS **********************/
class fooclass {
private:
  long private_val;
protected:
  long protect_val;
public:
  static int static_val;
  long public_val;
  int id;

  friend int buddy();                 // friend function

  fooclass();
  fooclass(fooclass &);
  ~fooclass();
  void print(int x);
  void print(char *str);
  void print(int x, char *str);
  void hi();
  static int static_func() {
     return 9;
  };
  fooclass  operator+(fooclass arg){  // overloaded operator
       return arg;                    // return second operand (so a+b=b)
  };
  void operator=(fooclass& arg){  // overloaded operator =
       if (this == &arg) return;
#ifdef stream_io
    cout<<"** Overload Assignment(=) id="<<id<<"  private_val "<<private_val<<" -> "<<arg.private_val<<"\n";
#else
   printf("** Overload Assignment(=) id=%d  private_val %ld -> %ld \n",id,private_val,arg.private_val);
#endif
       private_val=arg.private_val;   // return second operand (so a=b)
  };
  operator int() {                    // return private value
      return (int)private_val;        // conversion operator
  };
  inline void inlinefunc() {          // inline function
      private_val++;
      print("inline func called: incr private val");
   };
};

/******************** define function for fooclass ************/
 int fooclass::static_val=1;

  fooclass::fooclass() {              // constructor for fooclass
    id=static_val;
#ifdef stream_io
    cout<<"** Constructor called  id="<<id<<"  ref_count= "<<id<<"\n";
#else
   printf("** Constructor called  id=%d  ref_count= %d\n",id,id);
#endif
    static_val++;
    private_val=-2*id;
    public_val=0;
    protect_val=1;
  };

  fooclass::fooclass(fooclass & a) {     // Copy constructor for fooclass
    id=static_val;
#ifdef stream_io
    cout<<"** Copy Constructor called  id="<<id<<"  ref_count= "<<id<<"\n";
#else
   printf("** Copy Constructor called  id=%d  ref_count= %d\n",id,id);
#endif
    static_val++;
    private_val=a.private_val;
  };

  fooclass::~fooclass() {             // destructor for fooclass
    static_val--;
#ifdef stream_io
    cout<<"** Destructor called  id="<<id<<"  ref_count= "<<static_val<<"\n";
#else
   printf("** Destructor called  id=%d  ref_count= %d\n",id,static_val);
#endif
  };

  void fooclass::print(int x) {       // overloaded member functions

#ifdef stream_io
   cout<<"int: "<<x<<"\n";
#else
   printf("int: %d\n",x);
#endif

  };

  void fooclass::print(char *str) {   // use c++ lib stream functions

#ifdef stream_io
   cout<<"string: "<<str<<"\n";
#else
   printf("string: %s\n",str);
#endif

};

  void fooclass::print(int x, char *str) {   // use c++ lib stream functions

#ifdef stream_io
    cout<<"Result: "<<x<<" \t "<<str<<"\n";
#else
   printf("Result: %d \t %s\n",x,str);
#endif

  };

  void fooclass::hi() {               //  simple member function
    print("hello world");
    private_val=-20;
  };



/**********************  Object A,B,AB,VAB classes *****************/
class obj_a {
public:
   int a_num;
   int common;
   virtual int common_func() {return 1;};
   obj_a() {
       a_num=1;
       common=1;
   };
};
class obj_b {
public:
   int b_num;
   int common;
   virtual int common_func() {return 2;};
   obj_b() {
       b_num=2;
       common=2;
   };
};
class obj_aa: public obj_a {
public:
   int aa_num;
   int common;
   int common_func() {return 3;};
   obj_aa() {
       aa_num=3;
       common=3;
   };
};
class obj_ab: public obj_a, public obj_b {
public:
   int ab_num;
   int common;
   int common_func() {return 3;};
   obj_ab() {
       ab_num=3;
       common=3;
   };
};
class obj_v_ab: virtual public obj_a, virtual public obj_b {
public:
   int v_ab_num;
   int common_func() {return 4;};
   obj_v_ab() {
       v_ab_num=4;
   };
};


/***************       function to check parameter features   *********/
int func(int INT, int & rint, const int CINT, fooclass & fc_obj,int defualt=111) {
//   CINT=100;                // CONST can't be on Left Hand Side
   INT=99;
   rint=101;
   fc_obj.public_val=98;
   return defualt;
};


/******************** Global Object *******************************/
fooclass fclass3;


/********************          friend function    **************************/
int buddy() {
     return (int)fclass3.private_val;
};

namespace n {int i = 1;};

/******************************************************************/
/************    main starts, here (unless above main used)  ******/
/******************************************************************/
#ifndef make_local
int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow) {
#endif

obj_a    obj_a1;                      // OBJECT A,B,AB,VAB stuff
obj_b    obj_b1;
obj_aa   obj_aa1;
obj_ab   obj_ab1;
obj_v_ab obj_v_ab1;

fooclass f,fclass, fclass2;
fooclass * foo_obj_ptr=&f;

int h=0;
int i=0;
int j=0;
int k=0;
int number=0;
int &ref=number;
const int Const=8;
const int Const_Arr[3]={0,1,2};

f.print("=========== C900 C++ Functionality tests =============");

/************** assigning tests *****************/
f.print("=========== Assignment tests ====================");
k=0;
obj_ab1.ab_num=12;
f.print(obj_ab1.ab_num,"Expect 12 \t[Public Var Mult Inherit]");
obj_v_ab1.v_ab_num=55;
f.print(obj_v_ab1.v_ab_num,"Expect 55 \t[Public Var Mult Inherit]");
obj_a1.a_num=33;
BEFORE:
for (int Int=0; Int < Const; Int++)
DURING:
   obj_a1.a_num++;

AFTER:
f.print(obj_a1.a_num,"Expect 41 \t[Public Var No Inherit] \t(33+8)");



/************** function call tests ***************/

f.print("=========== Function call tests =================");
f.print(1);              // check overloaded print()
f.print("ok, [ '1' then 'ok' expected]");   // check overloaded print()
f.print(0,"Expect 0  \t[testing overloaded print() with two parms]");

f.print(foo(), "Expect 5 \t[ foo() returns 5 ]"); // simple reg (non-oop) func
h=buddy();               // friend function
f.print(h, "Expect -2 \t[Friend function]");
if (h != (-2)) f.print("Error: friend function");
fclass3.inlinefunc();          // inline function
f.print(buddy(), "Expect -1 \t[Friend function]");
f.hi();                  // set private_val=-20, print hello world
f.print(     f.static_val, "Expect 5 \t[Static class data (f.static_val)]");
f.print(fclass.static_val, "Expect 5 \t[Static class data (fclass.static_val)]");
h=50+f;                  // implict call to int() conversion operator
f.print(h, "Expect 30 \t[conversion operator]");
fclass3=f+fclass2;       // overloaded operator
f.print(buddy(), "Expect -8 \t[prv_val from fclass2]");
h=10+fclass3;
f.print(h, "Expect 2 \t[overloaded operator (conv opr too)] \t(10-8)");
f.print(f.static_func(), "Expect 9 \t[Static function]");



/*************** other stuff  *************/
f.print("=========== other tests ===========================");
// add new, dispose (destructor) stuff here


/****** OBJECT A,B,AB,VAB tests ****************/
f.print("=========== A,B,AB,VAB [inheritence] tests ========");
f.print(obj_a1.common_func(),   "Expect 1  \t[  NO   Inherit]");
f.print(obj_b1.common_func(),   "Expect 2  \t[  NO   Inherit]");
f.print(obj_aa1.common_func(),  "Expect 3  \t[Single Inherit]");
f.print(obj_ab1.common_func(),  "Expect 3  \t[Multiple Inherit]");
f.print(obj_v_ab1.common_func(),"Expect 4  \t[Virtual Multiple]");



/******   func() tests ******************/
DA_FUNC:
f.print("=========== func() call tests ===================");
i=func(number, number, number, fclass, 88);
  if (i != 88)               f.print(i,"Error: \tdefualt args [expected 88]");
  if (fclass.public_val!=98) f.print(fclass.public_val,"Error \tref parms [expected 98]");
  if (number==101)           f.print(number,"Error ? \tconst parm func(&x,const x) x changes");
j=func(k, number, number, fclass);
  if (j != 111)              f.print(j,"Error:  \tdefualt args [expected 111]");
  if (fclass.public_val!=98) f.print(fclass.public_val,"Error:  \tref parms [expected 98]");
  f.print(k,"Expect 0 \tsince call by value");


f.print("=========== Testing completed  ==================");
f.print("=========== Implict destruction now  ============");

return 0;
}
