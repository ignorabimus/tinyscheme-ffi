tinyscheme-ffi
==============

FFI Extension for TinyScheme based on libffi.

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

    (define libuser32 (ffi-load-lib "user32"))
    (define message-box (ffi-find-proc libuser32 "MessageBoxA" '("uint32" "pointer" "pointer" "uint32") "sint32"))
    (ffi-call-proc message-box '(0 "message" "title" 0))
    (ffi-free-proc message-box)
    (ffi-free-lib libuser32)
