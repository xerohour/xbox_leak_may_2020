pushdef(_prediv,divnum)divert(-1)dnl Redirect output to /dev/null for now
changecom(`|')
|
|  M4 macros for munging dinput.w
|
| begindoc / enddoc
|
| everything between begindoc and enddoc is ignored.  They are in the .w
| file so that autodoc can extract the documentation.
|
define(`begindoc',`divert(-1)dnl')
define(`enddoc',`divert('_prediv`)dnl')
|
|   Internal macros which persist outside a begin_interface / end_interface
|   begin with _ so that they won't collide with words that happen to
|   appear in dinput.w
|
|   Yeah, it isn't foolproof, but it's close enough for now.
|

|-----------------------------------------------------------------------------
|
| _Quote quotes its argument
|
define(`_Quote', `$@')

|-----------------------------------------------------------------------------
|
| strsubst performs a search-and-replace on its argument
|
| First, the helper functions...
|
|   strsubst__ -- Performs one found substitution, or stops
|
|       $1 = processed source string (don't rescan)
|       $2 = unprocessed source string
|       $3 = old
|       $4 = new
|       $5 = location where `old' was found in $1, or -1 if stop
|

define(`strsubst__',
    `ifelse($5,-1,`$1$2',
            `strsubst_(`$1'_Quote(substr(`$2',0,$5))`$4',
                       _Quote(substr(`$2',eval($5+len(`$3')))),
                       `$3',`$4')')')

|   strsubst_ -- Try to perform one substitution, or stop
|
|       $1 = processed source string (don't rescan)
|       $2 = unprocessed source string
|       $3 = old
|       $4 = new
|

define(`strsubst_', `strsubst__(`$1',`$2',`$3',`$4',index(`$2',`$3'))')

|   strsubst -- Perform a search-and-replace
|
|       $1 = source string
|       $2 = old
|       $3 = new

define(`strsubst', `strsubst_(`',`$1',`$2',`$3')')

|-----------------------------------------------------------------------------
|
| uppercase converts a string to uppercase
|
define(`uppercase',
    `translit(`$1', `abcdefghijklmnopqrstuvwxyz',
                    `ABCDEFGHIJKLMNOPQRSTUVWXYZ')')

|-----------------------------------------------------------------------------
| Interface definition macros.  It goes like this:
|
| begin_interface(IMumble%, IUnknown)
| begin_methods
| declare_method(DeleteFeedback,DWORD)
| declare_method2(DWORD,DeleteFeedback,DWORD)
| declare_method(EnumFeedback,LPDIENUMFEEDBACKPROC%, LPVOID)
| end_methods
| end_interface
|
| (Actually, the second parameter to begin_interface is assumed
|  to be IUnknown if omitted.)
|
|
| While an interface is being built:
|
| _itf contains the name of the interface
| _itfP contains the name of the interface, sans % sign
| _itfBase contains the name of the base interface
| _itfBase{ contains the name of the base interface, sans % sign
| _ucP contains an uppercase version of _itfP, sans leading `I'
| _cset contains the current character set (W or A)
|
| _methods contains a list of the methods, in the form
|
|	_invoke_(Method1,arg1,arg2)_invoke_(Method2,arg1,arg2)...
|
| except that you might find an _invoke2_() inserted when the
| base class methods disappear and the extended methods emerge.
| and you might find an _invoke3_() inserted when the
| method returns a type other than HRESULT.
|
| declare_method takes a variable number of parameters.  The first
| is the method name.  The rest are method argument types.
|
define(`begin_interface',`pushdef(`_itf',`$1')'dnl
`pushdef(`_itfBase', ifelse(`$2',`', `IUnknown', `$2'))dnl')

define(`begin_methods',
        `pushdef(`_methods',
            `_invoke2_(ifelse(_itfBase, `IUnknown', `_itf', `_itfBase'))')dnl')
define(`declare_method',
       `define(`_methods',defn(`_methods')`_invoke_($*)')dnl')
define(`end_base_class_methods',
       `define(`_methods',defn(`_methods')`_invoke2_(_itf)')dnl')
define(`declare_method2',
       `define(`_methods',defn(`_methods')`_invoke3_($*)')dnl')
define(`end_methods',`_emit_methods`'popdef(`_methods')dnl')
define(`end_interface',`popdef(`_itf')dnl')

|-----------------------------------------------------------------------------
|
| _invoke_per_method calls its argument macro once for each method.
|
| Optional second argument handles _invoke2_'s.
|
define(`_invoke_per_method',
`pushdef(`_invoke_', defn(`$1'))'dnl
`pushdef(`_invoke3_', defn(`$3'))'dnl
`pushdef(`_invoke2_', ifelse(`$2', `', `', defn(`$2')))'dnl
`_methods`'popdef(`_invoke_')popdef(`_invoke2_')popdef(_invoke3_)')

|-----------------------------------------------------------------------------
| 
|  _DePercent
| 
|   Remove percent signs by turning TSTR% into WSTR/STR and other %'s
|   into W or A.  Turning TSTR% into WSTR/STR is a hack:  We have more
|   macros that map symbols like LPTSTRW -> LPWSTR.
|
|   $1 - W or A
|   $2... - the thing to change
| 
define(`_DePercent', `translit(`$2', `%', `$1')')

define(`LPTSTRW',  `LPWSTR')
define(`LPTSTRA',  `LPSTR')
define(`LPCTSTRW', `LPCWSTR')
define(`LPCTSTRA', `LPCSTR')

|-----------------------------------------------------------------------------
|
|  _DoItf emits a single interface definition in the appropriate
|  character set.
|
|  $1 - character set (A or W)
|  _itf - temporarily redefined to the de-percentified name
|  _TF - interface name uppercased, without the leading "I"
| 
define(`_DoItf',
`pushdef(`_cset', `$1')'dnl
`pushdef(`_itf', _DePercent(`$1', _itf))'dnl
`pushdef(`_itfBase', _DePercent(`$1', _itfBase))'dnl
`pushdef(`_TF', uppercase(substr(_itf, 1)))'dnl
`
HRESULT WINAPI _itf`'_`'QueryInterface(_itf * p, REFIID riid, LPVOID * ppvObj);
ULONG WINAPI _itf`'_`'AddRef(_itf * p);
ULONG WINAPI _itf`'_`'Release(_itf * p);

_invoke_per_method(`_emit_method_definition', `_emit_class_header', `_emit_method2_definition')

typedef struct _itf *LP`'_TF;

popdef(`_TF')popdef(`_itf')popdef(`_itfBase')popdef(`_cset')')

|-----------------------------------------------------------------------------
|
| *emit_method_definition                           
|
|   Called once for each method.  Generates the HRESULT function prototype.
|
define(`_emit_method_definition',
    `_DePercent(_cset, `HRESULT WINAPI _itf`'_`'$1(_itf * p`'ifelse(`$2',,, `,shift($*)'))');
')')


|-----------------------------------------------------------------------------
|
| *emit_method2_definition
|
|   Called once for each method.  Generates the arbitrary type function prototype.
|
define(`_emit_method2_definition',
    `_DePercent(_cset, `$1 WINAPI _itf`'_`'$2(_itf * p`'ifelse(`$3',,, `,shift(shift($*))'))');
')')


|-----------------------------------------------------------------------------
|
| _emit_class_header
|
|   Called once for each change of class.  Generates a happy comment block.
|
define(`_emit_class_header',
    `')

|-----------------------------------------------------------------------------
|
|   _DoAfterItfVariation
|
|   Emit $1, first as W, then as A.
|
define(`_DoAfterItfVariation',
`#ifdef UNICODE
_DePercent(`W', `$*')
#else
_DePercent(`A', `$*')
#endif')

|-----------------------------------------------------------------------------
|
|  _DoAfterItf emits the follow-up stuff that comes after an interface
|  definition.  If the interface name contains a percent sign, emit
|  the appropriate mix.
|
define(`_DoAfterItf',
`ifelse(_itf, _itfP,, `_DoAfterItfVariation(
`#'define IID_`'_itfP IID_`'_itf
`#'define _itfP _itf
`#'define _itfP`'Vtbl _itf`'Vtbl)
typedef struct _itfP *LP`'_ucP;

')#if defined(__cplusplus) 

struct _itf`'ifelse(_itfBase,`IUnknown',,` : public '_itfBase)
{
_EmitCPPWrapper(`QueryInterface', REFIID riid, LPVOID * ppvObj)'dnl
`_EmitCPPWrapper2(`ULONG',`AddRef')'dnl
`_EmitCPPWrapper2(`ULONG',`Release')'dnl
`_invoke_per_method( `_EmitCPPWrapper', `', `_EmitCPPWrapper2')
};

#endif
')

|-----------------------------------------------------------------------------
|
|   _Arity - generate fake arguments based on arity
|
|   What comes out is "Method(p,a,b,c)" if arity is 4
|
|   Note that we skip `p' in the alphabetical list.  Duh.
|
|   $1 = method
|   $2 = number of arguments (including method itself)
|
define(`_Arity',
`$1(substr(`p,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,q,r,s,t,u,v,w,x,y,z',
0, eval($2 * 2 - 1)))')

|-----------------------------------------------------------------------------
|
|   _ArityCPP - generate fake arguments based on arity
|
|   What comes out is "Method(a,b,c)" if arity is 4 (we ate the `p')
|
|   Note that we skip `p' in the alphabetical list.  Duh.
|
|   Note also that we need to special-case the situation where
|   the arity is exactly zero, so we don't underflow.
|
|   $1 = method
|   $2 = number of arguments (including method itself)
|
define(`_ArityCPP',
`$1(ifelse($2,1,,`substr(`a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,q,r,s,t,u,v,w,x,y,z',
0, eval($2 * 2 - 3))'))')

|-----------------------------------------------------------------------------
|
|   _EmitWrapper - generate a wrapper macro
|
|   _itfP - interface name without % sign
|   $1    - method name
|   $2... - other arguments
|
define(`_EmitWrapper',
``#'define _itfP`'_`'_Arity($1,$#) (p)->lpVtbl->_Arity($1,$#)
')

|-----------------------------------------------------------------------------
|
|   _EmitWrapper2 - generate a wrapper macro
|
|   _itfP - interface name without % sign
|   $2    - method name
|   $3... - other arguments
|
define(`_EmitWrapper2',
``#'define _itfP`'_`'_Arity($2,eval($#-1)) (p)->lpVtbl->_Arity($2,eval($#-1))
')

|-----------------------------------------------------------------------------
|
|   _NoAsterisk - removes any '*' from a string
|

define(`_NoAsterisk',
    `translit(`$1', `*')')

|-----------------------------------------------------------------------------
|
|   _LastWord - returns the last word of an argument
|
|   $1    - argument
|

define(`_LastWord',
    `ifelse(index(`$1',` '),-1,$1,`_LastWord(substr(`$1',eval(index(`$1',` ') + 1)))')')

|-----------------------------------------------------------------------------
|
|   _ArgumentName - 
|
|   $1    - argument
|
define(`_ArgumentName',
    `_NoAsterisk(`_LastWord(`$1')')')

|-----------------------------------------------------------------------------
|
|   _ArgumentNames - returns the last words of an argument list
|
|   For example, given the argument list:
|
|       BYTE Byte, BLAH* blah, FOO *pFoo,
|
|   it would return:
|
|       Byte, blah, pFoo
|
|   $*    - argument list
|

define(`_ArgumentNames',
    `_ArgumentName($1)`'ifelse(`$2',,,``,'_ArgumentNames(shift($*))')')

|-----------------------------------------------------------------------------
|
|   _EmitCPPWrapper - generate a C++ thunk
|
|   _itfP - interface name without % sign
|   $1    - method name
|   $2... - other arguments
|

define(`_EmitCPPWrapper',
    `_DePercent(_cset, `    HRESULT $1(shift($*)) { return(_itf`'_`'$1(this`'ifelse(`$2',,,`,_ArgumentNames(shift($*))'))')); }
')

|-----------------------------------------------------------------------------
|
|   _EmitCPPWrapper2 - generate a C++ thunk
|
|   _itfP - interface name without % sign
|   $1    - type
|   $2    - method name
|   $3... - other arguments
|

define(`_EmitCPPWrapperType',
    `_DePercent(_cset, `    $1 $2(shift(shift($*))) { return(_itf`'_`'$2(this`'ifelse(`$3',,,`,_ArgumentNames(shift(shift($*)))'))')); }
')

|-----------------------------------------------------------------------------
|
|   _EmitCPPWrapper2 - generate a C++ thunk for a method that is VOID
|
|   _itfP - interface name without % sign
|   $2    - method name
|   $3... - other arguments
|

define(`_EmitCPPWrapperVoid',
    `_DePercent(_cset, `    $1 $2(shift(shift($*))) { _itf`'_`'$2(this`'ifelse(`$3',,,`,_ArgumentNames(shift(shift($*)))'))'); }
')

|-----------------------------------------------------------------------------
|
|   _EmitCPPWrapper2 - generate a C++ thunk for a method that returns
|                      a type other than HRESULT
|
|   _itfP - interface name without % sign
|   $1    - type
|   $2    - method name
|   $3... - other arguments
|

define(`_EmitCPPWrapper2',
    `ifelse(uppercase(`$1'),`VOID',`_EmitCPPWrapperVoid($*)',`_EmitCPPWrapperType($*)')')

|-----------------------------------------------------------------------------
|
| The long-awaited _emit_methods, which drives all the real work.
|
|
|define(`emit_methods', `invoke_per_method(`_DoItf')')
define(`_emit_methods',
`pushdef(`_itfP', translit(_itf, `%'))'dnl
`pushdef(`_ucP', uppercase(substr(_itfP, 1)))'dnl
`ifelse(index(_itf, `%'), -1, `_DoItf(`W')',
`_DoItf(`W')_DoItf(`A')')'dnl
`_DoAfterItf()popdef(`_ucP')popdef(`_itfP')')

|-----------------------------------------------------------------------------
|
| dik_define - remember the definition as an m4 macro before spitting it out

define(`dik_def', `define(strsubst(`_$1',` '),$2)`#'define `$1' $2')

|-----------------------------------------------------------------------------
|
| Message cracker definition macros.  It goes like this:
|
| begin_message_cracker(DIDM_,IDirectInputDeviceCallback *,this,CDIDev)
|   message_cracker(Acquire,HANDLE,hevt)
|   message_cracker(GetDataFormat,,,LPDIDATAFORMAT,pdidf)
| end_message_cracker()
|

|-----------------------------------------------------------------------------
|
| _ifnb   - macro which emits its second argument if the first is nonblank

define(`_ifnb', `ifelse(`$1',,,`$2')')

|-----------------------------------------------------------------------------
|
| begin_message_cracker - Begin a set of message crackers
|
|   $1 - prefix for messages
|   $2 - type of first parameter
|   $3 - name of first parameter
|   $4 - type of parent object
|
| For example, Windows messages would be
|
|   begin_message_cracker(WM_,HWND,hwnd,?)
|
| IDirectInputDeviceCallback would be
|
|   begin_message_cracker(DIDM_,IDirectInputDeviceCallback *,this,CDIDev)
|
| Variables used during message cracker macro generation:
|
| _prefix - the prefix for the message
| _type1  - type of the first argument
| _name1  - name of the first argument
| _ptype1 - type of parent object
|
define(`begin_message_cracker',
`pushdef(`_prefix',`$1')'dnl
`pushdef(`_type1',`$2')'dnl
`pushdef(`_name1',`$3')'dnl
`pushdef(`_ptype',`$4')'dnl
`HRESULT NTAPI;internal
_ptype`'_CallDevice(struct _ptype *this, UINT didm, WPARAM wp, LPARAM lp);;internal
')

define(`end_message_cracker',
`popdef(`_prefix')popdef(`_type1')popdef(`_name1')popdef(`_ptype')')

|-----------------------------------------------------------------------------
|
| message_cracker
|
|   $1 - name of message, mixed-case.
|   $2 - type of wParam, or null if wParam is ignored
|   $3 - name of wParam, or null if wParam is ignored
|   $4 - type of lParam, or null if lParam is ignored
|   $5 - name of lParam, or null if lParam is ignored
|
|   Local variable:
|
|   _uc    - uppercase version of $1 with _prefix attached

define(`message_cracker',
`pushdef(`_uc', _prefix`'uppercase(`$1'))'dnl
`/*
 *  HRESULT Cls_On$1('dnl
`_ifnb(`$2$4',`
 *      ')_type1 _name1`'_ifnb(`$2',`, $2 $3`'')_ifnb(`$4',`, $4 $5'))
 */
`#'define HANDLE_`'_uc`'(_name1, wParam, lParam, fn) \
        ((fn)((_name1)'dnl
`_ifnb(`$2',`, ($2)(wParam)')'dnl
`_ifnb(`$4',`, ($4)(lParam)')'dnl
`))

`#'define FORWARD_`'_uc`'(_name1`''dnl
`_ifnb(`$2',`, $3`'')'dnl
`_ifnb(`$4',`, $5'), fn) \
        ((fn)(_name1, _uc, 'dnl
`ifelse(`$2',,0,`(WPARAM)($3)'), 'dnl
`ifelse(`$4',,0,`(LPARAM)($5)')))

HRESULT static __inline
dcb$1(struct _ptype *this`'_ifnb(`$2',`, $2 $3`'')_ifnb(`$4',`, $4 $5'))
{
    return FORWARD_`'_uc`'(this`'_ifnb(`$2',`, $3`'')_ifnb(`$4',`, $5'),'dnl
` _ptype`'_CallDevice);
}
')

|-----------------------------------------------------------------------------
| Restore comments and the previous diversion
changecom()
divert(_prediv)popdef(`_prediv')dnl End of macro header file
