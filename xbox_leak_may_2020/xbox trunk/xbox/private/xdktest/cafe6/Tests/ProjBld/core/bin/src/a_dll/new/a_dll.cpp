#include "a_dll.h"

class Inherited : public base1, public base2
{
public:
	void DllExport Func2(void); // Implementation of above
	void DllExport Func1(void); // Implementation of above

private:
	int m_int; 
	void Helper(int); // Helper function for implementation
};

void DllExport Inherited::Func2 (void)
{
	int i = 22;
	m_int = 3;
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

Inherited inh2;

DllExport base2* func2()
{
	return (base2*)(&inh2);
}

