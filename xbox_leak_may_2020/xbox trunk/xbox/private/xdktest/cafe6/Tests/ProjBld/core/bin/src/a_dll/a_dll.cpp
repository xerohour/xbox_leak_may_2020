#include "a_dll.h"

class Inherited : public base1, public base2 {
public:
	void DllExport Func2(void); // Implementation of above
	void DllExport Func1(void); // Implementation of above

private:
	int m_int; 
	void Helper(int); // Helper function for implementation
};

void DllExport Inherited::Func2 (void)
{
	int i = 2;
	m_int =3;
	Helper(i);
}

void DllExport Inherited::Func1 (void)
{
	int i = 0;
}

void Inherited::Helper(int par)
{
	par++;
}

Thread static int g_tls;
/*
void DllExport __stdcall func()
{
  Thread static int l_tls;

  l_tls = 10; // OK: Local tls is correct
  l_tls++;    // OK
  g_tls = 1;  // NG: Debugger don't show correct value in watch window, but executed result is correct. 
  g_tls++;    // NG: For expamle, OutputDebugString(); can show correct value.
}
*/
Inherited inh2;

DllExport base2* func2()
{
	return (base2*)(&inh2);
}

