#define PREPEND_MODULE(Suffix) clear_test##Suffix
#define MODULE_STRING "clear_test"
#pragma comment(linker, "/include:_clear_test_ExhibitScene@8")
