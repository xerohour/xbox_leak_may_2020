/*
;++
;
;   Module Name:
;
;       apic.h
;
;   Abstract:
;
;       include file for apic.c.
;
;
;   Author:
;
;
;--
;
*/


//
// The next structures are defined so that ntapic.h is 
// satisfied.  They are dummied up here so that halacpi
// can share code with halmps more easily.
//

typedef PVOID PPCMPPROCESSOR;
typedef PVOID PPCMPBUS;
typedef PVOID PPCMPIOAPIC;
typedef PVOID PPCMPINTI;
typedef PVOID PPCMPLINTI;
typedef PVOID PMPS_EXTENTRY;
typedef PVOID ADDRESS_USAGE;
