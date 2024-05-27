:: simple build script for now
:: should probably setup for cmake later
 
@setlocal

:: common options
@set CLOPTS=/nologo /c /IC:\apps\include /W4
@set LINKOPTS=/nologo /DLL /INCREMENTAL:NO /LIBPATH:C:\apps\lib /MANIFEST:EMBED
@set LINKLIBS=zlib.lib bz2.lib liblzma.lib zstd.lib

:: debug opts
@set CLOPTS=%CLOPTS% /Zi /Od /MDd
@set LINKOPTS=%LINKOPTS% /DEBUG

:: release opts
::@set CLOPTS=%CLOPTS% /O2 /MD

:: build
cl %CLOPTS% *.c
link %LINKOPTS% *.obj %LINKLIBS% 