; Classes defined in afxtempl.h
;
; Unfortunately the ClassView snap is currently not smart enough to 
; deal with correct verification of navigation to templated classes.
; Therefore, this baseline will only check for existence and navigation
; based on the template name.  The full template spec will be kept for
; future use.

; class CArray< class TYPE , class ARG_TYPE >
class CArray
***
; class CList< class TYPE , class ARG_TYPE >
class CList
***
; class CMap< class KEY , class ARG_KEY , class VALUE , class ARG_VALUE >
class CMap
***
; class CTypedPtrArray< class BASE_CLASS , class TYPE >
class CTypedPtrArray
***
; class CTypedPtrList< class BASE_CLASS , class TYPE >
class CTypedPtrList
***
; class CTypedPtrMap< class BASE_CLASS , class KEY , class VALUE >
class CTypedPtrMap
***
globalfunc CompareElements
***
globalfunc ConstructElements
***
globalfunc CopyElements
***
globalfunc DestructElements
***
globalfunc DumpElements
***
globalfunc HashKey
***
globalfunc SerializeElements
