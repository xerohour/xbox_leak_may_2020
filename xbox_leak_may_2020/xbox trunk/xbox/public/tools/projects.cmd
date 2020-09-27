@echo off
if not "%_echo%" == "" echo on
if not "%verbose%"=="" echo on

REM ---------------------------------------------------------------------------
REM   PROJECTS.CMD
REM
REM By adding your project to this file, anyone will be able to do "ntenlist
REM <your project name>" to automatically enlist in the correct location in
REM the tree. "project" is the name of the project. "slm_root" is the server
REM where it exists. "proj_path" is where the project lives in the source tree.
REM
REM Don't forget to update the graphical tree at the bottom of this file.
REM
REM Finally, you might also want to create an alias so users will be able to
REM jump right to your project w/o typing the full path. See
REM \nt\private\developr\ntcue.pub. The only caveat, don't use an alias that
REM is also the name of some utility.
REM
REM ---------------------------------------------------------------------------

set project=
set slm_root=
set proj_path=
goto %1 2>nul

:ntos
           set project=ntos&      set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\ntos
goto end
:private
           set project=private&   set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private
goto end
:public
           set project=public&    set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\public
goto end
:sdk
           set project=SDK&       set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\Private\SDK
goto end
:sdktools
           set project=sdktools&  set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\sdktools
goto end
:usb
           set project=usb&       set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\ntos\dd\wdm\usb
goto end
:test
           set project=test&      set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\test
goto end
:ui
           set project=ui&        set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\ui
goto end
:directx
           set project=directx&   set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\windows\directx
goto end
:win2000
           set project=win2000&   set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\win2000
goto end
:atg
           set project=atg&       set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\atg
goto end
:xdktest
           set project=xdktest&   set slm_root=\\CPITGCFS01\XBOX&       set proj_path=%_ntroot%\private\xdktest
goto end

ÃÄÄÄbldrules
ÃÄÄÄbldrules
ÃÄÄÄprivate
³   ÃÄÄÄamovie
³   ³   ÀÄÄÄfilters.ks (ksuser)
³   ÃÄÄÄasn1
³   ÃÄÄÄatg
³   ÃÄÄÄbasestor
³   ÃÄÄÄbpc
³   ³   ÃÄÄÄbda
³   ³   ÃÄÄÄbpccomm
³   ³   ÃÄÄÄbpcui
³   ³   ÃÄÄÄca
³   ³   ÃÄÄÄcommon
³   ³   ÃÄÄÄds
³   ³   ÃÄÄÄepgldr
³   ³   ÃÄÄÄfilters
³   ³   ÃÄÄÄquartz
³   ³   ÃÄÄÄvbi
³   ³   ÀÄÄÄvidsvr
³   ÃÄÄÄca
³   ÃÄÄÄcluster
³   ÃÄÄÄddk
³   ÃÄÄÄdfs.ds
³   ÃÄÄÄds
³   ÃÄÄÄdsmigrat
³   ÃÄÄÄdsui
³   ÃÄÄÄeventlog
³   ÃÄÄÄeventsystem
³   ÃÄÄÄfax
³   ÃÄÄÄimaging2
³   ÃÄÄÄinet
³   ³   ÃÄÄÄathena
³   ³   ÃÄÄÄcontrols
³   ³   ÃÄÄÄdigest
³   ³   ÃÄÄÄicw
³   ³   ÃÄÄÄiis
³   ³   ÃÄÄÄiisrearc
³   ³   ÃÄÄÄmshtml
³   ³   ÃÄÄÄnetmtng
³   ³   ÃÄÄÄsetup (iesetup)
³   ³   ÃÄÄÄurlmon
³   ³   ÃÄÄÄvml
³   ³   ÃÄÄÄwininet
³   ³   ÀÄÄÄxml
³   ÃÄÄÄinfosoft
³   ÃÄÄÄintlwb
³   ÃÄÄÄispu
³   ÃÄÄÄmapistub
³   ÃÄÄÄmobile
³   ÃÄÄÄmvdm
³   ÃÄÄÄmsagent
³   ÃÄÄÄnet
³   ³   ÃÄÄÄrouting
³   ³   ÃÄÄÄsfm
³   ³   ÃÄÄÄsockets
³   ³   ÀÄÄÄui
³   ³       ÀÄÄÄnwc
³   ÃÄÄÄnlgwb
³   ÃÄÄÄntos
³   ³   ÃÄÄÄarcinst
³   ³   ÃÄÄÄdd
³   ³   ³   ÃÄÄÄsound
³   ³   ³   ³   ÀÄÄÄcreative
³   ³   ³   ÀÄÄÄwdm
³   ³   ³       ÃÄÄÄcapture
³   ³   ³       ³   ÀÄÄÄatiwdm
³   ³   ³       ÃÄÄÄdvd
³   ³   ³       ÃÄÄÄinput
³   ³   ³       ÃÄÄÄusb
³   ³   ³       ÀÄÄÄ1394
³   ³   ÃÄÄÄks
³   ³   ÃÄÄÄminiport
³   ³   ³   ÃÄÄÄadaptec
³   ³   ³   ÃÄÄÄflashpnt
³   ³   ³   ÃÄÄÄslcd
³   ³   ³   ÀÄÄÄsymbios
³   ³   ÃÄÄÄndis
³   ³   ³   ÃÄÄÄnetoem
³   ³   ³   ÃÄÄÄnpfddi
³   ³   ³   ÀÄÄÄtestprot
³   ³   ÃÄÄÄrdr2
³   ³   ³   ÀÄÄÄmfiomi
³   ³   ÃÄÄÄstreams
³   ³   ÃÄÄÄw32
³   ³   ³   ÃÄÄÄcslpk
³   ³   ³   ÃÄÄÄntcon
³   ³   ³   ÃÄÄÄntgdi
³   ³   ³   ³   ÃÄÄÄmstypeo
³   ³   ³   ³   ÀÄÄÄprinters
³   ³   ³   ³       ÀÄÄÄrasprint
³   ³   ³   ³           ÀÄÄÄmini
³   ³   ³   ÀÄÄÄntuser
³   ³   ÀÄÄÄwow64
³   ÃÄÄÄntrk
³   ÃÄÄÄnw
³   ÃÄÄÄoleds
³   ÃÄÄÄole2ui32
³   ÃÄÄÄole32
³   ÃÄÄÄoleutest
³   ÃÄÄÄos2
³   ÃÄÄÄpnpu
³   ÃÄÄÄposix
³   ÃÄÄÄprinter5
³   ³   ÀÄÄÄoemdrv
³   ³       ÃÄÄÄbroth
³   ³       ³   ÃÄÄÄhbp
³   ³       ÃÄÄÄcanon
³   ³       ÃÄÄÄepson
³   ³       ÃÄÄÄhp
³   ³       ³   ÃÄÄÄhpclj
³   ³       ³   ÃÄÄÄhplj5si
³   ³       ³   ÀÄÄÄhpmini
³   ³       ÃÄÄÄlexmk
³   ³       ÀÄÄÄxerox
³   ³           ÀÄÄÄnseries
³   ÃÄÄÄquery
³   ÃÄÄÄredist
³   ³   ÀÄÄÄmsagent
³   ÃÄÄÄremoteboot
³   ÃÄÄÄrpc
³   ÃÄÄÄsamples
³   ÃÄÄÄsdktools
³   ³   ÀÄÄÄvctools
³   ÃÄÄÄsecurity
³   ÃÄÄÄtapi
³   ÃÄÄÄunimodem
³   ÃÄÄÄutils
³   ÃÄÄÄwin2000
³   ÃÄÄÄwindbg64
³   ÃÄÄÄwindows
³   ³    ÃÄÄÄbase
³   ³    ÃÄÄÄcmd
³   ³    ÃÄÄÄdanim
³   ³    ÃÄÄÄdirectx
³   ³    ÃÄÄÄep
³   ³    ÃÄÄÄmedia
³   ³    ³   ÀÄÄÄdrivers
³   ³    ³       ÀÄÄÄcreatdll
³   ³    ÃÄÄÄole
³   ³    ÃÄÄÄopengl
³   ³    ÃÄÄÄreality
³   ³    ÃÄÄÄsam
³   ³    ÃÄÄÄsetup
³   ³    ÃÄÄÄshell
³   ³    ÃÄÄÄspooler
³   ³    ³   ÀÄÄÄmonitors
³   ³    ³       ÀÄÄÄhpmon
³   ³    ÃÄÄÄtshoot
³   ³    ÃÄÄÄtts
³   ³    ÃÄÄÄwin4help
³   ³    ÃÄÄÄwinnls
³   ³    ÀÄÄÄwmi
³   ÃÄÄÄwx86
³   ÀÄÄÄxdktest
ÃÄÄÄpublic
³   ÀÄÄÄspec
ÃÄÄÄtest
ÃÄÄÄui
ÀÄÄÄnttest
    ÃÄÄÄbasetest
    ÃÄÄÄbasetool
    ÀÄÄÄguitest
    
:end
