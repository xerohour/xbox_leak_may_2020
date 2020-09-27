// classed declared in .pch file.
class class_pch
	{
	private:
		int class_pch_var;
	public:
		class_pch(void);
		int class_pch_func(int i);
	};

// struct declared in .pch file.
struct struct_pch
	{
	private:
		int struct_pch_var;
	public:
		struct_pch(void);
		int struct_pch_func(int i);
	};

// union declared in .pch file.
union union_pch
	{
	private:
		int union_pch_var;
	public:
		union_pch(void);
		int union_pch_func(int i);
	};

// enum decalred in a .pch file.
enum enum_pch {one_pch = 1, two_pch, three_pch};
