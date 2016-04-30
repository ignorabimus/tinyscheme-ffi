@setlocal
@set SCHEME_H_DIR=..\src
@set TSCOMPILE=cl /nologo /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DUSE_DL=1 /DFFI_BUILDING /I%SCHEME_H_DIR% /Ilibffi\include
@set TSLIB=lib /nologo
@set TSLINK=link /nologo

@if "%1" == "static" goto STATIC
@if not exist dll\ (
  mkdir dll
)

%TSCOMPILE% /MT ffi.c
%TSLINK% /DLL /out:dll\ffi.dll /export:init_ffi ffi.obj libffi\libffi.lib

%TSCOMPILE% /MDd ffi.c
%TSLINK% /DLL /out:dll\ffi_d.dll /export:init_ffi ffi.obj libffi\libffi_d.lib

@goto END

:STATIC
@if not exist lib\ (
  mkdir lib
)

%TSCOMPILE% /MT ffi.c
%TSLIB% /out:lib\ffi.lib ffi.obj libffi\libffi.lib

%TSCOMPILE% /MDd ffi.c
%TSLIB% /out:lib\ffi_d.lib ffi.obj libffi\libffi_d.lib

:END
del ffi.obj
