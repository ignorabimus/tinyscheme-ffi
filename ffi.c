/* TinyScheme FFI Extension
 */

#include <windows.h>
#include "scheme-private.h"
#include "ffi.h"

typedef struct {
  void *proc;
  ffi_cif cif;
} ffi_procinfo;

typedef union {
  int i;
  double r;
  char *s;
} ffi_avalue;

static ffi_type *get_ffi_type_ptr(char *ffi_type_str)
{
  if (0 == strcmp(ffi_type_str, "void")) {
    return &ffi_type_void;
  } else if (0 == strcmp(ffi_type_str, "uint8")) {
    return &ffi_type_uint8;
  } else if (0 == strcmp(ffi_type_str, "sint8")) {
    return &ffi_type_sint8;
  } else if (0 == strcmp(ffi_type_str, "uint16")) {
    return &ffi_type_uint16;
  } else if (0 == strcmp(ffi_type_str, "sint16")) {
    return &ffi_type_sint16;
  } else if (0 == strcmp(ffi_type_str, "uint32")) {
    return &ffi_type_uint32;
  } else if (0 == strcmp(ffi_type_str, "sint32")) {
    return &ffi_type_sint32;
  } else if (0 == strcmp(ffi_type_str, "uint64")) {
    return &ffi_type_uint64;
  } else if (0 == strcmp(ffi_type_str, "sint64")) {
    return &ffi_type_sint64;
  } else if (0 == strcmp(ffi_type_str, "float")) {
    return &ffi_type_float;
  } else if (0 == strcmp(ffi_type_str, "double")) {
    return &ffi_type_double;
  } else if (0 == strcmp(ffi_type_str, "pointer")) {
    return &ffi_type_pointer;
  } else {
    return &ffi_type_void;
  }
}

static int get_ffi_type_num(ffi_type *ptype)
{
  if (ptype == &ffi_type_void) {
    return FFI_TYPE_VOID;
  } else if (ptype == &ffi_type_uint8) {
    return FFI_TYPE_UINT8;
  } else if (ptype == &ffi_type_sint8) {
    return FFI_TYPE_SINT8;
  } else if (ptype == &ffi_type_uint16) {
    return FFI_TYPE_UINT16;
  } else if (ptype == &ffi_type_sint16) {
    return FFI_TYPE_SINT16;
  } else if (ptype == &ffi_type_uint32) {
    return FFI_TYPE_UINT32;
  } else if (ptype == &ffi_type_sint32) {
    return FFI_TYPE_SINT32;
  } else if (ptype == &ffi_type_uint64) {
    return FFI_TYPE_UINT64;
  } else if (ptype == &ffi_type_sint64) {
    return FFI_TYPE_SINT64;
  } else if (ptype == &ffi_type_float) {
    return FFI_TYPE_FLOAT;
  } else if (ptype == &ffi_type_double) {
    return FFI_TYPE_DOUBLE;
  } else if (ptype == &ffi_type_pointer) {
    return FFI_TYPE_POINTER;
  } else {
    return FFI_TYPE_VOID;
  }
}

/* ffi-load-lib */
pointer foreign_ffiloadlib(scheme * sc, pointer args)
{
  pointer first_arg;
  char *varname;
  HINSTANCE hinst;

  if (args == sc->NIL) {
    return sc->F;
  }

  first_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_string(first_arg)) {
    return sc->F;
  }

  varname = sc->vptr->string_value(first_arg);
  hinst = LoadLibrary(varname);
  if (!hinst) {
    return sc->F;
  }

  return sc->vptr->mk_integer(sc, (long)hinst);
}

/* ffi-find-proc */
pointer foreign_ffifindproc(scheme * sc, pointer args)
{
  pointer first_arg, second_arg, third_arg, fourth_arg;
  HINSTANCE hinst;
  ffi_procinfo *p;

  if (args == sc->NIL) {
    return sc->F;
  }

  first_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_integer(first_arg)) {
    return sc->F;
  }

  args = sc->vptr->pair_cdr(args);
  second_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_string(second_arg)) {
    return sc->F;
  }

  args = sc->vptr->pair_cdr(args);
  third_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_list(sc, third_arg)) {
    return sc->F;
  }

  args = sc->vptr->pair_cdr(args);
  fourth_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_string(fourth_arg)) {
    return sc->F;
  }

  hinst = (HINSTANCE)sc->vptr->ivalue(first_arg);
  if (!hinst) {
    return sc->F;
  }

  p = (ffi_procinfo *)malloc(sizeof(ffi_procinfo));
  if (p) {
    char *proc_name;
    int list_len;
    ffi_type **arg_types;
    ffi_type *arg_rtype;

    proc_name = sc->vptr->string_value(second_arg);
    p->proc = GetProcAddress(hinst, proc_name);
    if (!p->proc) {
      free(p);
      return sc->F;
    }

    list_len = sc->vptr->list_length(sc, third_arg);
    arg_types = (ffi_type **)malloc(sizeof(ffi_type *) * list_len);
    if (arg_types) {
      int i;
      for (i = 0; i < list_len; i++) {
        pointer arg = sc->vptr->pair_car(third_arg);

        if (!sc->vptr->is_string(arg)) {
          free(p);
          free(arg_types);
          return sc->F;
        }

        arg_types[i] = get_ffi_type_ptr(sc->vptr->string_value(arg));
        third_arg = sc->vptr->pair_cdr(third_arg);
      }

      arg_rtype = get_ffi_type_ptr(sc->vptr->string_value(fourth_arg));
      if (ffi_prep_cif(&p->cif, FFI_DEFAULT_ABI, list_len, arg_rtype, arg_types) == FFI_OK) {
        return sc->vptr->mk_integer(sc, (long)p);
      }

      free(arg_types);
    }

    free(p);
  }

  return sc->F;
}

/* ffi-call-proc */
pointer foreign_fficallproc(scheme * sc, pointer args)
{
  pointer first_arg, second_arg;
  int list_len;
  ffi_procinfo *p;
  void **values;
  ffi_avalue *avalue;
  int i;

  if (args == sc->NIL) {
    return sc->F;
  }

  first_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_integer(first_arg)) {
    return sc->F;
  }

  args = sc->vptr->pair_cdr(args);
  second_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_list(sc, second_arg)) {
    return sc->F;
  }

  p = (ffi_procinfo *)sc->vptr->ivalue(first_arg);
  if (!p) {
    return sc->F;
  }

  list_len = sc->vptr->list_length(sc, second_arg);
  if (list_len != p->cif.nargs) {
    return sc->F;
  }

  values = (void *)malloc(sizeof(void *) * list_len);
  if (!values) {
    return sc->F;
  }

  avalue = (ffi_avalue *)malloc(sizeof(ffi_avalue) * list_len);
  if (!avalue) {
    free(values);
    return sc->F;
  }

  for (i = 0; i < list_len; i++) {
    pointer arg = sc->vptr->pair_car(second_arg);

    switch (get_ffi_type_num(p->cif.arg_types[i])) {
    case FFI_TYPE_UINT8:
    case FFI_TYPE_SINT8:
    case FFI_TYPE_UINT16:
    case FFI_TYPE_SINT16:
    case FFI_TYPE_UINT32:
    case FFI_TYPE_SINT32:
    case FFI_TYPE_UINT64:
    case FFI_TYPE_SINT64:
      avalue[i].i = sc->vptr->ivalue(arg);
      break;
    case FFI_TYPE_FLOAT:
    case FFI_TYPE_DOUBLE:
      avalue[i].r = sc->vptr->rvalue(arg);
      break;
    case FFI_TYPE_POINTER:
      avalue[i].s = sc->vptr->string_value(arg);
      break;
    case FFI_TYPE_VOID:
    default:
      free(values);
      free(avalue);
      return sc->F;
    }

    values[i] = &avalue[i];
    second_arg = sc->vptr->pair_cdr(second_arg);
  }

  switch (get_ffi_type_num(p->cif.rtype)) {
  case FFI_TYPE_UINT8:
  case FFI_TYPE_SINT8:
  case FFI_TYPE_UINT16:
  case FFI_TYPE_SINT16:
  case FFI_TYPE_UINT32:
  case FFI_TYPE_SINT32:
  case FFI_TYPE_UINT64:
  case FFI_TYPE_SINT64:
  {
    int rvalue;
    ffi_call(&p->cif, FFI_FN(p->proc), &rvalue, values);

    free(values);
    free(avalue);
    return sc->vptr->mk_integer(sc, rvalue);
  }
  case FFI_TYPE_FLOAT:
  case FFI_TYPE_DOUBLE:
  {
    double rvalue;
    ffi_call(&p->cif, FFI_FN(p->proc), &rvalue, values);

    free(values);
    free(avalue);
    return sc->vptr->mk_real(sc, rvalue);
  }
  case FFI_TYPE_POINTER:
  {
    char *rvalue;
    ffi_call(&p->cif, FFI_FN(p->proc), &rvalue, values);

    free(values);
    free(avalue);
    return (pointer)rvalue;
  }
  case FFI_TYPE_VOID:
    free(values);
    free(avalue);
    return sc->T;
  default:
    free(values);
    free(avalue);
    return sc->F;
  }
}

/* ffi-free-proc */
pointer foreign_ffifreeproc(scheme * sc, pointer args)
{
  pointer first_arg;
  ffi_procinfo *p;

  if (args == sc->NIL) {
    return sc->F;
  }

  first_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_integer(first_arg)) {
    return sc->F;
  }

  p = (ffi_procinfo *)sc->vptr->ivalue(first_arg);
  if (!p) {
    return sc->F;
  }

  free(p->cif.arg_types);
  free(p);
  return sc->T;
}

/* ffi-free-lib */
pointer foreign_ffifreelib(scheme * sc, pointer args)
{
  pointer first_arg;
  HINSTANCE hinst;

  if (args == sc->NIL) {
    return sc->F;
  }

  first_arg = sc->vptr->pair_car(args);
  if (!sc->vptr->is_integer(first_arg)) {
    return sc->F;
  }

  hinst = (HINSTANCE)sc->vptr->ivalue(first_arg);
  if (!hinst) {
    return sc->F;
  }

  if (!FreeLibrary(hinst)) {
    return sc->F;
  }

  return sc->T;
}

/* This function gets called when TinyScheme is loading the extension */
void init_ffi(scheme * sc)
{
  sc->vptr->scheme_define(sc, sc->global_env,
                          sc->vptr->mk_symbol(sc, "ffi-load-lib"),
                          sc->vptr->mk_foreign_func(sc, foreign_ffiloadlib));
  sc->vptr->scheme_define(sc, sc->global_env,
                          sc->vptr->mk_symbol(sc, "ffi-find-proc"),
                          sc->vptr->mk_foreign_func(sc, foreign_ffifindproc));
  sc->vptr->scheme_define(sc, sc->global_env,
                          sc->vptr->mk_symbol(sc, "ffi-call-proc"),
                          sc->vptr->mk_foreign_func(sc, foreign_fficallproc));
  sc->vptr->scheme_define(sc, sc->global_env,
                          sc->vptr->mk_symbol(sc, "ffi-free-proc"),
                          sc->vptr->mk_foreign_func(sc, foreign_ffifreeproc));
  sc->vptr->scheme_define(sc, sc->global_env,
                          sc->vptr->mk_symbol(sc, "ffi-free-lib"),
                          sc->vptr->mk_foreign_func(sc, foreign_ffifreelib));
}
