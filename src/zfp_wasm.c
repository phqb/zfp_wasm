#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "zfp.h"
#include "zfp/macros.h"
#include "template/template.h"
#include "emscripten.h"

uint8_t *
EMSCRIPTEN_KEEPALIVE
compress_2d(float * input, size_t nx, size_t ny, double tolerance, uint32_t * size, uint32_t * status)
{
  *status = 0;            /* 0 = success */
  zfp_field * field;      /* array meta data */
  zfp_stream * zfp;       /* compressed stream */
  void * output;          /* storage for compressed stream */
  size_t est_size;        /* estimated byte size of compressed buffer */
  bitstream * stream;     /* bit stream to write to or read from */
  size_t compressed_size;

  /* allocate meta data for the 2D array a[nx][ny] */
  field = zfp_field_2d(input, zfp_type_float, nx, ny);

  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);

  /* set tolerance */
  zfp_stream_set_accuracy(zfp, tolerance);

  /* allocate buffer for compressed data */
  est_size = zfp_stream_maximum_size(zfp, field);
  output = malloc(est_size);

  /* associate bit stream with allocated buffer */
  stream = stream_open(output, est_size);
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);

  /* compress array and output compressed stream */
  compressed_size = zfp_compress(zfp, field);
  if (!compressed_size) 
    *status = 1;
  else
    *size = compressed_size;

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);

  return output;
}

/* compress or decompress array */
float *
EMSCRIPTEN_KEEPALIVE
decompress_2d(uint8_t * input, size_t nx, size_t ny, double tolerance, size_t size, uint32_t * status)
{
  *status = 0;        /* 0 = success */
  zfp_field * field;  /* array meta data */
  zfp_stream * zfp;   /* compressed stream */
  float * output;     /* storage for decompressed stream */
  bitstream * stream; /* bit stream to read from */

  /* allocate the 2D array a[nx][ny] */
  output = malloc(nx * ny * sizeof(float));
  field = zfp_field_2d(output, zfp_type_float, nx, ny);

  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);

  /* set tolerance */
  zfp_stream_set_accuracy(zfp, tolerance);

  /* associate bit stream with allocated array */
  stream = stream_open(input, size);
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);

  /* read compressed stream and decompress array */
  if (!zfp_decompress(zfp, field))
    *status = 1;

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);

  return output;
}
