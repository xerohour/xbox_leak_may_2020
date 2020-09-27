#include "cpp.h"

int Result=0;

extern "C"
{
void Cxx_Tests();
}

void Cxx_Tests()
{
  CALL_CONSTRUCTOR:
  fooclass foo1;

  foo1.vf1();
  Result = foo1.base::vf2();
  Result = foo1.vf2(7);
  Result = foo1.vf3(9);

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
};
