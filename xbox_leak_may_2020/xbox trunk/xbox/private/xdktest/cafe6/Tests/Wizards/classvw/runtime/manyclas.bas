; Classes defined in manyclass.h


class empty
***

class pubclass
    base; base_class; public
***

class protclass
    base; base_class; protected
***

class privclass
    base; base_class; private
***

class easyclass
    mf; int; priv_func;  private
    mf; int; priv2_func;  private
    mf; int; prot_func;  protected
    mf; int; pub_func;  public
    md; int; priv_x;  private
    md; int; priv2_x;  private
    md; int; prot_x;  protected
    md; int; pub_x;  public
***

class newclass
    base; easyclass; public
    mf; int; priv_func;  private
    mf; int; priv2_func;  private
    mf; int; prot_func;  protected
    mf; int; pub_func;  public

    md; int; priv_x;  private
    md; int; priv2_x;  private
    md; int; prot_x;  protected
    md; int; pub_x;  public
***
class vbase
    mf; int; priv_vfunc; private
    mf; int; prot_vfunc; protected
    mf; int; pub_vfunc; public
	mf; none; ~vbase; public
***
class newvclass
    base; vbase; public
    mf; int; priv_vfunc; private
    mf; int; prot_vfunc; protected
    mf; int; pub_vfunc; public
***     
class virtclass
    base; base_class; public
***
class mclass
    base; base1; public 
	base; base2; private
***
class class1
	mf; none; class1; public
	mf; none; ~class1; public
	mf; int; vfunc; private
***
class class2
;This is blocked by VisualStudio bug #6561 
    mf; int; def_func; private
***
class class3
;This is blocked by VisualStudio bug #6561 
    mf; int; def_func; private
***
class class4
    mf; int; outer_func; private
    md; int; outer_x; private
***
; Nested classes not supported by test yet.
;class class4::nested_class
;    mf; int; nested_func; public
;    mf; int; nested_x; private
***
class class5
    md; int; x; private
***
; Nested classes not supported by test yet.
;class class5::nested_class
;    md; int; x; private
***
class class6
    md; int; m_x; private
    mf; int; inline_func; private
***
class class7
    mf; int; abstract_func; public
***
class class8
    mf; class8; operator =; public
	mf; class8&; operator +; public
	mf; class8&; operator -; public
	mf; class8&; operator >>; public
	mf; class8&; operator <<; public
	mf; int; operator ==; public
	mf; char; operator []; public
	mf; int; operator (); public
	mf; char*; operator char*; public
	mf; const char&; operator const char&; public
	mf; LPCTSTR; operator LPCTSTR; public
	mf; const LPSTR; operator const LPSTR; public
	mf; int; operator int; public

***
class class9
***
class class10
***
class class11
	mf; class11*; func; private
***
class class13
    mf; const class13* const; func; private
***
class class14
;This is blocked by VisualStudio bug #6561 
    mf; const class13* const; func; private
***
class class15
    mf; int; sfunc; private
	mf; int; sfunc2; private
	mf; int; func(int ( * pf )( const char * )); private
	mf; int; func(char *, int ( * pf )( int x ); private
***
class class16
    mf; int; func(int); private
	mf; int; func(BOOL); private
	mf; int; func(char); private
	mf; int; func(unsigned char); private
; The following won't work due to postponed bug #712.
	mf; int; func(unsigned); private
***
class class17
	mf; int; func; private
***
class class18
    md; int*; x; private
***
class class19
    mf; void; func; private
***
class class20
***
class class21
	md; int(* [ 10 ][ 20 ])( int x ); m_apfunc; private
	md; int(* )( int x , int y ); m_pfunc; private
***
class class22
;All these are blocked by VisualStudio98 bug #6560
	md; WORD; bOn; private
	md; WORD; bSys; private
	md; WORD; bSrc; private
	md; WORD; bRev; private
	md; WORD; bUpd; private
	md; WORD; index; private
***
class class23
	mf; int; func; private
	mf; int; func; private
***
globalfunc MAX
***
; We don't specify what's between the <> for a templated class because
; it's harder to determine if we've navigated to it.  It's possible to
; do, but at the time I didn't have the time.
; class Buffer1< class T , int size >
class Buffer1
	md; T; buffer; private
***
class Buffer2
	mf; T; GetItem; private
***
class class24
	md; CArray; m_BufArray; private
	md; Buffer; m_CharBuf; private
	md; Buffer; m_Class23Buf; private
	md; Buffer; m_pFuncBuf; private
	md; int; m_x; private
	md; Buffer; m_ArrayBuf; private
	md; Buffer; m_ArrayPtrBuf; private
	md; Buffer; m_boolBuf; private
***
class class25
; These 3 cases fail : we need to change the baseline structure in order to fix them. We need pass 2 strings - 1 for ClassView to search and other to compare with code
	mf; int; foo(myclass & c); private
	mf; int; foo(myunion* * pp); private
	mf; int; foo(mystruct* p); private
	md; struct mystruct; m_struct; private
	md; class myclass; m_class; private
	md; union myunion; m_union; private 
***
class invalid
	mf; int; func; private
***
globalfunc C_Func
***
globalfunc C_Func1
***
globalfunc C_Func2
***
globaldata instance1
***
globaldata instance2
***
class Globals
	md; int; m_x; private
	mf; int; foo; private
***
class Wierd$Name
	md; int; m_$x; private
***
; ClassView does not yet support namespaces.
class NamespacedClass
;	mf; int; foo; private
	md; int; m_x; private
***
class class26
***
class class27
	mf; int; func; private
***
class errorclass1
; The following is blocked by VisualStudio98 bug #7134.
;	md; int; x; private
	mf; int; func; private
***
class errorclass2
***
; This is blocked by VisualStudio bug #6895
class PT
	md; X; x; public
	md; Y; y; public
***
class RC
; The following data items do not appear in the class view due to the fact that
; they reside in a nested union with no name.  This is a known issue.
	md; X; xLeft; public
	md; Y; yTop; public
	md; X; xRight; public
	md; Y; yBot; public
	md; PT; ptTopLeft; public
	md; PT; ptBotRight; public

	md; PT; rgpt; public
	md; RECT; rect; public
	mf; DX; width; public
	mf; DY; height; public
***
class COLEClass
	base; CCmdTarget; public
	mf; HRESULT; AddComponent; public
	mf; none; COLEClass; private
	mf; none; ~COLEClass; protected
	md; LPCOMPONENTGALLERY; c_lpGallery; public
	md; UINT; c_nTypeID; public
***
; Nested classes not supported by test yet.
;class COLEClass::XType 
;	base; IComponentType; public
;	mf; ULONG; AddRef; public
;	mf; ULONG; Release; public
;	mf; int; QueryInterface; public
;	mf; HRESULT; BltTypeGlyph; public
;	mf; HRESULT; OnInitialRegister; public
;	mf; HRESULT; OnRegister; public
***