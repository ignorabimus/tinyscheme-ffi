tinyscheme-ffi
==============

FFI Extension for TinyScheme based on [libffi](https://sourceware.org/libffi/).

This software is subject to the license terms contained in the LICENSE file.


FFI FUNCTIONS
-------------

FFI Extension incorporates the following functions:

* (ffi-load-lib libraryname)
  -  `libraryname` : string
  - Returns a library handle.

* (ffi-find-proc lib-handle procname arg-type-list ret-type)
  - `lib-handle` : handle (created by ffi-load-lib)
  - `procname` : string (procedure name)
  - `arg-type-list` : list (data-types for arguments)
  - `ret-type` : string (data-type of return value)
  - Returns a procedure handle.

* (ffi-call-proc proc-handle arg-list)
  - `proc-handle` : handle (created by ffi-find-proc)
  - `arg-list` : list (argument values)
  - Returns a return value of procedure.

* (ffi-free-proc proc-handle)
  - `proc-handle` : handle (created by ffi-find-proc)
  - Returns a free result (boolean #t/#f).

* (ffi-free-lib lib-handle)
  - `lib-handle` : handle (created by ffi-load-lib)
  - Returns a free result (boolean #t/#f).


Sample usage of TinyScheme FFI Extension
----------------------------------------

    (load-extension "ffi")
    (define libuser32 (ffi-load-lib "user32"))
    (define message-box (ffi-find-proc libuser32 "MessageBoxA" '("uint32" "pointer" "pointer" "uint32") "sint32"))
    (ffi-call-proc message-box '(0 "message" "title" 0))
    (ffi-free-proc message-box)
    (ffi-free-lib libuser32)


Building libffi with MSVC
-------------------------

Install "Visual C++ 2010"(or later) and [Cygwin](https://www.cygwin.com/).

### Setting up environemt

Open the Visual Studio command prompt, then run

    > C:\cygwin64\bin\mintty.exe -

Download libffi-3.2.1 from GitHub

    > git clone git://github.com/atgreen/libffi.git
    > cd libffi
    > git checkout -b v3.2.1-msvc v3.2.1
    > patch -p0 < libffi-3.2.1-msvc.patch
    > ./autogen.sh

### Build a static library

To make static release (/MT):

    > ./configure CC="$PWD/msvcc.sh -DUSE_STATIC_RTL" CXX="$PWD/msvcc.sh -DUSE_STATIC_RTL" CPP="cl -nologo -EP" CXXCPP="cl -nologo -EP" --build=i686-pc-cygwin
    > rm i686-pc-cygwin/include/ffitarget.h
    > cp src/x86/ffitarget.h i686-pc-cygwin/include/ffitarget.h
    > make

To make dynamic debug (/MDd):

    > ./configure CC="$PWD/msvcc.sh -DUSE_DEBUG_RTL" CXX="$PWD/msvcc.sh -DUSE_DEBUG_RTL" CPP="cl -nologo -EP" CXXCPP="cl -nologo -EP" --build=i686-pc-cygwin
    > rm i686-pc-cygwin/include/ffitarget.h
    > cp src/x86/ffitarget.h i686-pc-cygwin/include/ffitarget.h
    > make

and you'll get libffi static-library:

    > i686-pc-cygwin\.lib\libffi_convenience.lib


Links
-----

[D. Souflis, J. Shapiro - TinyScheme Home](http://tinyscheme.sourceforge.net/home.html)

[libffi - sourceware.org](https://sourceware.org/libffi/)

[Visual Studio Downloads](http://www.visualstudio.com/downloads/)

[Cygwin](https://www.cygwin.com/)


License
-------

Copyright (c) 2014 Tatsuya Watanabe. See the LICENSE file for license rights and limitations (MIT).
