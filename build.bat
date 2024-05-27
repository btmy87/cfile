:: simple build script for now
:: should probably setup for cmake later
 
@setlocal

:: common options
@set CLOPTS=/nologo /c /IC:\apps\include /W4 /D CFILE_EXPORTS
@set LINKOPTS=/NOLOGO /INCREMENTAL:NO /LIBPATH:C:\apps\lib /MANIFEST:EMBED
@set LIBOPTS=/NOLOGO /LIBPATH:C:\apps\lib
@set LINKLIBS=zlibstatic.lib bz2_static.lib liblzma.lib zstd_static.lib

:: debug opts
@set CLOPTS=%CLOPTS% /Zi /Od /MTd
@set LINKOPTS=%LINKOPTS% /DEBUG

:: release opts
::@set CLOPTS=%CLOPTS% /O2 /MT

:: build
cl %CLOPTS% *.c
link %LINKOPTS% /DLL cfile.obj %LINKLIBS%
lib %LIBOPTS% cfile.obj %LINKLIBS%
link %LINKOPTS% test_cfile.obj cfile.lib %LINKLIBS%