#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL _LimaNumPy
#include "numpy/arrayobject.h"
extern "C"
{
  void lima_import_array()
  {
    import_array();
  }
}
