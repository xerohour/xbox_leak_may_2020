#include <xtl.h>
#include <map>


template<class T> void foo(std::map<int, T> &) {}
template<class T> void foo(std::map<double, T> &) {}


void __cdecl main()
{
}
