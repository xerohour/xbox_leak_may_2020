if %JazzDir%xx==xx set JazzDir=..\..\..
autodoc /t4 /f IDMUSProd.fmt /o %JazzDir%\3rdParty\doc\word\IDMUSProd.doc %JazzDir%\Framework\*.h %JazzDir%\Framework\*.cpp %JazzDir%\Framework\*.d %JazzDir%\Timeline\*.h %JazzDir%\Timeline\*.cpp %JazzDir%\Timeline\*.d %JazzDir%\Conductor\*.h %JazzDir%\Conductor\*.cpp %JazzDir%\Conductor\*.d %JazzDir%\SegmentDesigner\*.h %JazzDir%\SegmentDesigner\*.cpp
autodoc /rh /t4 /f IDMUSProd.fmt /o %JazzDir%\3rdParty\doc\IDMUSProd.rtf %JazzDir%\Framework\*.h %JazzDir%\Framework\*.cpp %JazzDir%\Framework\*.d %JazzDir%\Timeline\*.h %JazzDir%\Timeline\*.cpp %JazzDir%\Timeline\*.d %JazzDir%\Conductor\*.h %JazzDir%\Conductor\*.cpp %JazzDir%\Conductor\*.d %JazzDir%\SegmentDesigner\*.h %JazzDir%\SegmentDesigner\*.cpp
hcw /e /c %JazzDir%\3rdParty\doc\IDMUSProd.hpj
