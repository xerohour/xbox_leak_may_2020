// global variable.
extern int global_var;

// enum (defined in header).
enum enum1{one = 1, two, three};

// non-overloaded global function.
int global_func(int i);
// overloaded global function.
int overloaded_global_func(int i);
char overloaded_global_func(char ch);
// inline function (defined in header).
inline int inline_func(int i){return global_var + i;};
