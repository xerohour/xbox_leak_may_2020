
projbldps.dll: dlldata.obj projbld_p.obj projbld_i.obj
	link /dll /out:projbldps.dll /def:projbldps.def /entry:DllMain dlldata.obj projbld_p.obj projbld_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del projbldps.dll
	@del projbldps.lib
	@del projbldps.exp
	@del dlldata.obj
	@del projbld_p.obj
	@del projbld_i.obj
