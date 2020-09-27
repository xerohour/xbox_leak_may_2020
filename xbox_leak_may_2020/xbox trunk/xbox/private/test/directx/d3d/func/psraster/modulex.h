#define PREPEND_MODULE(Suffix) psraster##Suffix
#define MODULE_STRING "psraster"
#pragma comment(linker, "/include:_psraster_ExhibitScene@8")
#pragma comment(linker, "/include:_psraster_zealot")
