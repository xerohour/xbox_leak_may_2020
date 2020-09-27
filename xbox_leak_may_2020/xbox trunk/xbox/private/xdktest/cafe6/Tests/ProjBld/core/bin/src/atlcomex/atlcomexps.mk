
atlcomexps.dll: dlldata.obj atlcomex_p.obj atlcomex_i.obj
	link /dll /out:atlcomexps.dll /def:atlcomexps.def /entry:DllMain dlldata.obj atlcomex_p.obj atlcomex_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del atlcomexps.dll
	@del atlcomexps.lib
	@del atlcomexps.exp
	@del dlldata.obj
	@del atlcomex_p.obj
	@del atlcomex_i.obj
