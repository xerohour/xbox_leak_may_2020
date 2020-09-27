// class defined in dll
class dll_class
{
private:
	int dll_class_var;
public:
	__declspec(dllexport) dll_class(void);
	__declspec(dllexport) int dll_class_func(int i);
};
