:: simple build script for now
:: should probably setup for cmake later
 
@setlocal
@echo off

set DEBUG=0
if /I "%~1"=="debug" set DEBUG=1

:: common options
set CLOPTS=/nologo /c /IC:\apps\include /W4 /D CFILE_EXPORTS
set LINKOPTS=/NOLOGO /INCREMENTAL:NO /LIBPATH:C:\apps\lib /MANIFEST:EMBED
set LIBOPTS=/NOLOGO /LIBPATH:C:\apps\lib

::set LINKLIBS=zlibstatic.lib bz2_static.lib liblzma.lib zstd_static.lib

:: debug opts
if %DEBUG%==1 (
  echo Building in debug mode
  set CLOPTS=%CLOPTS% /Zi /Od /MDd
  set LINKOPTS=%LINKOPTS% /DEBUG
  set LINKLIBS=zlibstaticd.lib 
  set LINKLIBS=%LINKLIBS% "C:\Users\Brian\Documents\programming\bzip2\build_debug\bz2_static.lib"
  set LINKLIBS=%LINKLIBS% liblzma.lib zstd.lib
) else (
  echo Building in release mode
  set CLOPTS=%CLOPTS% /O2 /MD
  set LINKLIBS=zlibstatic.lib bz2_static.lib liblzma.lib zstd.lib
)

:: build
@echo on
cl %CLOPTS% cfile.c test_cfile.c
link %LINKOPTS% /DLL cfile.obj %LINKLIBS%
::lib %LIBOPTS% cfile.obj %LINKLIBS%
link %LINKOPTS% test_cfile.obj cfile.obj %LINKLIBS%