#define PREPEND_MODULE(Suffix) mirror##Suffix
#define MODULE_STRING "mirror"
#pragma comment(linker, "/include:_mirror_ExhibitScene@8")
#pragma comment(linker, "/include:_mirror_texture")
#pragma comment(linker, "/include:_mirror_shade")
