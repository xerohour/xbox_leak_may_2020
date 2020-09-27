/*

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    Cookie.cpp

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

*/
#include "stdafx.h"
#include "HttpClient.h"

int followDomainRules = 0;
int CompareDomain(char *full, char *gen);
int StrictCompareDomain(char *full, char *gen);

Cookie::Cookie()
    {
    name = NULL;
    value = NULL;
    domain = NULL;
    next = NULL;
    path = NULL;
    localvalue = NULL;
    localvalueLen = 0;
    }

Cookie::Cookie(char *n, char *v /*=NULL*/, char *d /*=NULL*/)
    {
    //__asm int 3;
    name = NULL;
    value = NULL;
    domain = NULL;
    path = NULL;
    localvalue = NULL;
    localvalueLen = 0;
    if(n)
        {
        name = new char[strlen(n)+1];
        if(name) strcpy(name, n);
        }
    if(v)
        {
        value = new char[strlen(v)+1];
        if(value) strcpy(value, v);
        }
    if(d)
        {
        domain = new char[strlen(d)+1];
        if(domain) strcpy(domain, d);
        }

    next = NULL;
    }

Cookie::~Cookie()
    {
    if(name)  delete[] name;
    if(value) delete[] value;
    if(domain) delete[] domain;
    if(localvalue) delete[] localvalue;
    if(next)  delete next;
    if(path) delete[] path;
    }

Cookie* Cookie::Find(char *n, char *d/*=NULL*/)
    {
    if(!n) return NULL;
    if(name && strcmp(name, n) == 0 && StrictCompareDomain(domain, d)) return this;
    if(!next) return NULL;
    else return next->Find(n);
    }

Cookie* Cookie::Add(char *n, char *v, char *d)
    {
    //__asm int 3;
    /*
        NOTES: We allow a server to set a cookie in a different domain
    */
    if(!n) return NULL;
    if(!v) v = "";

    if(!name)
        {
        if(next)
            {
            return next->Add(n, v, d);
            }
        next = new Cookie(n, v, d);
        return next;
        }
    else if(followDomainRules && strcmp(name, n)==0 && StrictCompareDomain(domain, d)>=0)
        {
        if(value) delete[] value;
        value = new char[strlen(v)+1];
        strcpy(value, v);
        return this;
        }
    else if(!followDomainRules && strcmp(name, n) == 0)
        {
        if(value) delete[] value;
        if(domain) delete[] domain;
        domain = NULL;

        value = new char[strlen(v)+1];
        strcpy(value, v);

        if(d)
            {
            domain = new char[strlen(d)+1];
            strcpy(domain, d);
            }
        return this;
        }
    else if(!next)
        {
        next = new Cookie(n, v, d);
        return next;
        }
    else return next->Add(n, v, d);
    }

Cookie* Cookie::Add(char *setcookie, char *d)
    {
    //__asm int 3;
    if(!setcookie) return NULL;

    size_t line = strcspn(setcookie, "\r\n");
    char end = setcookie[line];
    setcookie[line] = '\0';
    size_t len = strcspn(setcookie, "=");

    size_t localnameLen = 0;
    char *localname = NULL;
    char *cookiestart = setcookie;
    char *nameend = cookiestart+len;
    char *valueend = nameend + strcspn(nameend, "; ");

    if((unsigned)(nameend - cookiestart + 1) > localnameLen || !localname)
        {
        localnameLen = nameend - cookiestart + 1;
        if(localname) delete[] localname;
        localname = new char[localnameLen];
        if(!localname)
            {
            setcookie[line] = end;
            return NULL;
            }
        }
    if((unsigned)(valueend - (nameend+1) + 1) > localvalueLen || !localvalue)
        {
        localvalueLen = valueend - (nameend+1) + 1;
        if(localvalue) delete[] localvalue;
        localvalue = new char[localvalueLen];
        if(!localvalue)
            {
            setcookie[line] = end;
            return NULL;
            }
        }

    char *domainend = NULL;
    char *newdomain = strstr(setcookie, "; domain=");
    char domainterm = '\0';
    if(newdomain)
        {
        newdomain += 9;
        domainend = newdomain + strcspn(newdomain, "; ");
        if(domainend != newdomain)
            {
            domainterm = *domainend;
            *domainend = '\0';
            d = newdomain;
            }
        else domainend = NULL;
        }

    strncpy(localname, cookiestart, nameend - cookiestart);
    localname[nameend - cookiestart] = '\0';

    if(*nameend)
        {
        strncpy(localvalue, nameend+1, valueend - (nameend+1));
        localvalue[valueend - (nameend+1)] = '\0';
        }

    Cookie *c = Add(localname, localvalue, d);

    delete[] localname;

    setcookie[line] = end;
    if(domainend) *domainend = domainterm;

    return c;
    }

void Cookie::Eval(char *dest, char *d/*=NULL*/)
    {
    int bytes = 0;

    if(name && value)
        {
        if(followDomainRules && d && domain)
            {
            if(CompareDomain(d, domain) >= 0)
                {
                if(next) bytes = sprintf(dest, "%s=%s; ", name, value);
                else sprintf(dest, "%s=%s", name, value);
                }
            }
        else
            {
            if(next) bytes = sprintf(dest, "%s=%s; ", name, value);
            else sprintf(dest, "%s=%s", name, value);
            }
        }

    if(next) next->Eval(dest + bytes, d);
    }

BOOL Cookie::Remove(char *n, char *d/*=NULL*/)
    {
    //__asm int 3;
    if(!n) return FALSE;

    BOOL ret = FALSE;

    Cookie *p = NULL;

    for(Cookie*c = this; c; c=c->next)
        {
        if(c->name)
            {
            if(strcmp(c->name, n) == 0 && StrictCompareDomain(c->domain, d)>=0)
                {
                if(p)
                    {
                    p->next = c->next;
                    c->next = NULL;
                    delete c;
                    ret = TRUE;
                    c = p; // cause we dont change p in this case
                    }
                else
                    {
                    delete[] c->name;
                    c->name = NULL;
                    if(c->value) delete[] c->value;
                    if(c->domain) delete[] c->domain;
                    ret = TRUE;
                    }
                }
            }
        p = c;
        }

    return ret;
    }

/*
    returns
    - if they dont match
    0 if they are ==
    + if the generalized domain matches the full domain

    TODO: different algo needed if domain is actually an IP

*/
int CompareDomain(char *full, char *gen)
    {
    if(!gen) return 1;
    if(!full) return 1;

    size_t fullLen = strlen(full);
    size_t genLen = strlen(gen);

    if(genLen > fullLen) return -1;
    if(genLen == fullLen) return -(!!_stricmp(full, gen));

    full += fullLen-genLen;

    if(full[0] != '.' && full[-1] != '.') return -3;

    if(_stricmp(full, gen) == 0) return 1;
    return -4;
    }

int StrictCompareDomain(char *full, char *gen)
    {
    if(!gen) return 1;
    if(!full) return 1;

    if(_stricmp(full, gen) == 0) return 1;
    return -1;
    }