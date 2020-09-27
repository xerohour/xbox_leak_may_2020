#include <xtl.h>

// This compiles  fine with VC6

template <class T = float, class Tr = VectorTraits2<T> >
class BaseVector2
{
	BaseVector2();
	BaseVector2& operator += (const BaseVector2& v);
	BaseVector2<T,Tr>& set(T n);

private:
  T mN[3];
};

template <class T, class Tr>
inline BaseVector2<T,Tr>&
BaseVector2<T,Tr>::operator += (const BaseVector2<T,Tr>& v)
{ 
	mN[0] += v.mN[0];
	mN[1] += v.mN[1];
	return *this;
}

template <class T, class Tr>
inline BaseVector2<T,Tr>&
BaseVector2<T,Tr>::set(T n)
{ 
	mN[0] = n;
	mN[1] = n;
	return *this;
}

void __cdecl main()
{
}