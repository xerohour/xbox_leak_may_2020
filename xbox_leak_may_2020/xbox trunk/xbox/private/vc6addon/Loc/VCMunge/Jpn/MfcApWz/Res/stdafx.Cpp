// stdafx.cpp : 標準インクルードファイルを含むソース ファイル
//              $$root$$.pch : 生成されるプリコンパイル済ヘッダー
//              stdafx.obj : 生成されるプリコンパイル済タイプ情報

#include "stdafx.h"

$$IF(ATL_SUPPORT)
#include <atlimpl.cpp>
$$ENDIF

$$IF(OLEDB)
CComModule _Module;
#include <atlimpl.cpp>
$$ENDIF

