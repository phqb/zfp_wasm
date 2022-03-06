#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "zfp.h"
#include "zfp/macros.h"
#include "template/template.h"
#include "emscripten.h"

float *
EMSCRIPTEN_KEEPALIVE
foo(uint32_t * size)
{
  *size = 1024;
  float * arr = malloc(1024 * sizeof(float));
  int i;
  for (i = 0; i < 1024; i += 1) arr[i] = exp(-i);
  return arr;
}
