#define PREPEND_MODULE(Suffix) d3dsnow##Suffix
#define MODULE_STRING "d3dsnow"
#pragma comment(linker, "/include:_d3dsnow_ExhibitScene@8")
#pragma comment(linker, "/include:_d3dsnow_backgrnd")
#pragma comment(linker, "/include:_d3dsnow_snow")
#pragma comment(linker, "/include:_d3dsnow_sheet")
