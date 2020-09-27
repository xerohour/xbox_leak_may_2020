#define DllExport __declspec(dllexport)
#define Thread __declspec(thread)

class base1
{
public:
	virtual void DllExport Func1() = 0;
};

class base2
{
public:
	virtual void DllExport Func2() = 0;
};

DllExport base2* func2();
