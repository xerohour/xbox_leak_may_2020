#include "stdafx.h"
#include "ParserTools.h"

Variable::Variable()
    {
    value = NULL;
    nameLen = valueLen = 0;
    }

Variable::Variable(char *n, char *v)
    {
    nameLen = strlen(n) + 1;
    valueLen = strlen(v) + 1;
    value = new char[valueLen];
    if(value) strcpy(value, v);
    }

Variable::~Variable()
    {
    if(value) delete[] value;
    value = NULL;
    }

// Called by the HashTable to clean up the variables
// As per "variables.SetCleanupFunct(DestroyVariable);"
void DestroyVariable(void *p)
    {
    if(!p) return;
    Variable *v = *(Variable**)p;
    delete v;
    }

