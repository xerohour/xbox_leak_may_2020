
atlservips.dll: dlldata.obj atlservi_p.obj atlservi_i.obj
	link /dll /out:atlservips.dll /def:atlservips.def /entry:DllMain dlldata.obj atlservi_p.obj atlservi_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del atlservips.dll
	@del atlservips.lib
	@del atlservips.exp
	@del dlldata.obj
	@del atlservi_p.obj
	@del atlservi_i.obj
