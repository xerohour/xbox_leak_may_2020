/*DOC*
**
** Suite : 
** File  : cxx.cxx
**
** Revision History :
** M001 2-Feb-1994
** - created from cxx.cxx from old callstack sources.
**
**DOC*/


#include "foo.h"
#include "cxx.h"

int Result=0;

extern "C"
{
void Cxx_Tests();
}

void Cxx_Tests()
{
  CALL_CONSTRUCTOR:
  fooclass foo1;


  CALL_MEMBER_FUNC:
  Result=foo1.getID();
  Result=foo1.getCount();


  CALL_CONVERSION:
  Result=foo1;


  CALL_STATIC:
  Result=fooclass::static_func();
  foo1.clear();  // clear private value

  CALL_INLINE:
  foo1.inline_func();
  Result=foo1;
  foo1.chain1_func();
};

void fooclass::chain1_func() {					// chain1 function
  START:
      unsigned char local_cpp_func='A';
      private_value++;                // increment private value
	  int chain2_return = chain2_func();
      private_value--;                // decrement private value
  END:;
};
