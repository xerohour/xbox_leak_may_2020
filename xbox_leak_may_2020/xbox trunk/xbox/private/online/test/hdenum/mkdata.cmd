delnode /q testdata
md testdata

md testdata\00000001.00000001
md testdata\00000002.00000002
md testdata\00000003.00000003
md testdata\00000004.00000004
md testdata\00000005.00000005
md testdata\00000006.00000006
md testdata\00000007.00000007
md testdata\00000008.00000008
md testdata\00000009.00000009
md testdata\0000000a.0000000a
md testdata\0000000b.0000000b
md testdata\0000000c.0000000c
md testdata\0000000d.0000000d
md testdata\0000000e.0000000e
md testdata\0000000f.0000000f
md testdata\00000010.00000000
md testdata\00000011.00000001
md testdata\00000012.00000002
md testdata\00000013.00000003
md testdata\00000014.00000004
md testdata\00000015.00000005

xbmkdir xt:\$c
xbcp /r /s /t testdata\* xt:\$c




