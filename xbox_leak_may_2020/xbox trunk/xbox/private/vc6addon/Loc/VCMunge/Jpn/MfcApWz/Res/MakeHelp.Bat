@echo off
$$IF(MULTIPLE_LANGS)

if "%1" == "?" goto :Error
if "%1" == "/?" goto :Error
if "%1" == "-?" goto :Error
if "%1" == "help" goto :Error
if "%1" == "-help" goto :Error
if "%1" == "/help" goto :Error
if "%1" == "" goto :Error

$$ENDIF //MULTIPLE_LANGS
$$IF(HM_NOTE)

REM -- PLEASE NOTE: OEM VS. ANSI CHARACTER SET DIFFERENCES
REM -- Many of the file names below look wierd in most editors because they need to be 
REM -- in the OEM character set, not the ANSI character set, in order to make batch  
REM -- files work right. The output and editor windows work with the ANSI character set.  
REM -- Where names are seen only in the output window, they have been left in the 
REM -- ANSI character set.

$$ENDIF	//HM_NOTE
REM -- 最初に Microsoft Visual C++ で作成した resource.h からマップ ファイルを作成します
echo // MAKEHELP.BAT generated Help Map file.  Used by $$ROOT$$.HPJ. >"hlp\$$HM_FILE_OEM$$.hm"
echo. >>"hlp\$$HM_FILE_OEM$$.hm"
echo // コマンド (ID_* , IDM_*) >>"hlp\$$HM_FILE_OEM$$.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$$HM_FILE_OEM$$.hm"
echo. >>"hlp\$$HM_FILE_OEM$$.hm"
echo // プロンプト (IDP_*) >>"hlp\$$HM_FILE_OEM$$.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$$HM_FILE_OEM$$.hm"
echo. >>"hlp\$$HM_FILE_OEM$$.hm"
echo // リソース (IDR_*) >>"hlp\$$HM_FILE_OEM$$.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$$HM_FILE_OEM$$.hm"
echo. >>"hlp\$$HM_FILE_OEM$$.hm"
echo // ダイアログ (IDD_*) >>"hlp\$$HM_FILE_OEM$$.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$$HM_FILE_OEM$$.hm"
echo. >>"hlp\$$HM_FILE_OEM$$.hm"
echo // フレーム コントロール (IDW_*) >>"hlp\$$HM_FILE_OEM$$.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$$HM_FILE_OEM$$.hm"
REM -- プロジェクト $$ROOT$$ のヘルプを作成

$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)

echo Win32 ﾍﾙﾌﾟ ﾌｧｲﾙのﾋﾞﾙﾄﾞ中
$$IF(MULTIPLE_LANGS)
start /wait hcw /C /E /M "hlp\%1\$$root_oem$$.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\%1\$$root_oem$$.hlp" goto :Error
if not exist "hlp\%1\$$root_oem$$.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\%1\$$root_oem$$.hlp" Debug
if exist Debug\nul copy "hlp\%1\$$root_oem$$.cnt" Debug
if exist Release\nul copy "hlp\%1\$$root_oem$$.hlp" Release
if exist Release\nul copy "hlp\%1\$$root_oem$$.cnt" Release
$$ELSE //!MULTIPLE_LANGS
start /wait hcw /C /E /M "hlp\$$root_oem$$.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\$$root_oem$$.hlp" goto :Error
if not exist "hlp\$$root_oem$$.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\$$root_oem$$.hlp" Debug
if exist Debug\nul copy "hlp\$$root_oem$$.cnt" Debug
if exist Release\nul copy "hlp\$$root_oem$$.hlp" Release
if exist Release\nul copy "hlp\$$root_oem$$.cnt" Release
$$ENDIF //MULTIPLE_LANGS
echo.
$$ENDIF //INTEL/MIPS/ALPHA
goto :done

$$// 複数言語の設定に応じて makehelp は異なった引数をとります。
$$IF(MULTIPLE_LANGS)
:Error
echo Usage MAKEHELP LanguageAbbreviation
echo       LanguageAbbreviation は $$LANG_LIST_SUFFIXES$$ のどれかです。
$$ELSE //!MULTIPLE_LANGS
:Error
echo hlp\$$root$$.hpj(1) : error: ヘルプファイルを作成する時に問題が発生しました。
$$ENDIF //MULTIPLE_LANGS

:done
echo.
