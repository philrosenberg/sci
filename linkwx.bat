set WXVER=31

set INSTDIR=%LIB64S%\
set WXDIR=..\src\wxWidgets\lib\vc_x64_lib\
set SUF=
CALL :INSTALLWX

set INSTDIR=%LIB64SD%\
set WXDIR=..\src\wxWidgets\lib\vc_x64_lib\
set SUF=d
CALL :INSTALLWX

set INSTDIR=%LIB32S%\
set WXDIR=..\src\wxWidgets\lib\vc_lib\
set SUF=
CALL :INSTALLWX

set INSTDIR=%LIB32SD%\
set WXDIR=..\src\wxWidgets\lib\vc_lib\
set SUF=d
CALL :INSTALLWX

GOTO:EOF

:INSTALLWX
del %INSTDIR%wxbase.lib
del %INSTDIR%wxregex.lib
mklink %INSTDIR%wxbase.lib %WXDIR%wxbase%WXVER%u%SUF%.lib
mklink %INSTDIR%wxregex.lib %WXDIR%wxregexu%SUF%.lib
set BASENOTEXTLIBS=(expat jpeg png scintilla tiff zlib)
for %%i in %BASENOTEXTLIBS% do del %INSTDIR%wx%%i.lib
for %%i in %BASENOTEXTLIBS% do mklink %INSTDIR%wx%%i.lib %WXDIR%wx%%i%SUF%.lib
set MSWLIBS=(adv aui core gl html media propgrid qa ribbon richtext stc webview xrc)
for %%i in %MSWLIBS% do del %INSTDIR%wxmsw_%%i.lib
for %%i in %MSWLIBS% do mklink %INSTDIR%wxmsw_%%i.lib %WXDIR%wxmsw%WXVER%u%SUF%_%%i.lib
GOTO:EOF

echo %LIB64SD%