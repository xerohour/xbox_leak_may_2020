
atlcomawps.dll: dlldata.obj atlcomaw_p.obj atlcomaw_i.obj
	link /dll /out:atlcomawps.dll /def:atlcomawps.def /entry:DllMain dlldata.obj atlcomaw_p.obj atlcomaw_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del atlcomawps.dll
	@del atlcomawps.lib
	@del atlcomawps.exp
	@del dlldata.obj
	@del atlcomaw_p.obj
	@del atlcomaw_i.obj
