#include <xtl.h>

extern "C" unsigned int DbgPrint(const char *, ...);


struct struct_x
{
    enum enum_x
    {
        struct_normal_enum_zero = 0,
        struct_normal_enum_one = 1,
        struct_normal_enum_two = 2,
        struct_normal_enum_three = 3
    };

    struct
	{
		enum enum_y
		{
			struct_anon_enum_zero = 0,
			struct_anon_enum_one = 1,
			struct_anon_enum_two = 2,
			struct_anon_enum_three = 3
		};
		// data member prevents C2625 error
		int foo;
	};
};


void __cdecl main()
{
	// demonstrate the enum-within-anonymous-struct bug
    DbgPrint("x: %d %d %d %d\n",
        struct_x::struct_normal_enum_zero,
        struct_x::struct_normal_enum_one,
        struct_x::struct_normal_enum_two,
        struct_x::struct_normal_enum_three
    );

    DbgPrint("anon: %d %d %d %d\n",
        struct_x::struct_anon_enum_zero,
        struct_x::struct_anon_enum_one,
        struct_x::struct_anon_enum_two,
        struct_x::struct_anon_enum_three
    );

    DbgPrint("both lines above should contain 0 1 2 3\n");

	while (1)
		Sleep(500);
    return;
}
