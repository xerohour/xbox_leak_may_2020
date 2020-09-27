#pragma once

#include "globals.h"

void DbgPrintGUID (REFIID riid);
void _stdcall LogDMUS_OBJECTDESCStruct(DMUS_OBJECTDESC Desc);
LPWSTR PathFindFileNameW(LPWSTR pPath);
//LPSTR PathFindFileName(LPSTR pPath);
LPCSTR PathFindFileNameA(LPCSTR pPath);
