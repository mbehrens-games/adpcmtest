/*******************************************************************************
** sample.c (sample compression and effects)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sample.h"

#define SAMPLE_ADPCM_ADJUST_TABLE_SIZE 8

#if 0
/* 4 bit mantissa */
#define SAMPLE_ADPCM_STEP_DIVISOR (1 << 4)

#define SAMPLE_ADPCM_STEP_LOWEST  (32 * SAMPLE_ADPCM_STEP_DIVISOR)
#define SAMPLE_ADPCM_STEP_HIGHEST (3 * 2048 * SAMPLE_ADPCM_STEP_DIVISOR)
#endif

/* 8 bit mantissa */
#define SAMPLE_ADPCM_STEP_DIVISOR (1 << 8)

#define SAMPLE_ADPCM_STEP_LOWEST  (2 * SAMPLE_ADPCM_STEP_DIVISOR)
#define SAMPLE_ADPCM_STEP_HIGHEST (8 * 1024 * SAMPLE_ADPCM_STEP_DIVISOR)

short G_sample_buffer[SAMPLE_BUFFER_SIZE];
int   G_sample_count;

/* for reference, see the ym2610 data sheet, p.5-7  */
/* also, the ym2608 manual, p. 44                   */

/* the values in this table are 2^(-1/6), 2^(1/3), 2^(2/3), 2^(3/3), 2^(4/3), */
/* expressed as fractions / 256 (the numerators are stored in the table).     */
static short  S_sample_adpcm_adjust_table[SAMPLE_ADPCM_ADJUST_TABLE_SIZE] = 
              { 228, 228, 228, 228, 308, 406, 512, 645};

/*******************************************************************************
** sample_generate_tables()
*******************************************************************************/
short int sample_generate_tables()
{
  int m;

  /* reset sample buffer */
  for (m = 0; m < SAMPLE_BUFFER_SIZE; m++)
    G_sample_buffer[m] = 0;

  G_sample_count = 0;

  return 0;
}

/*******************************************************************************
** sample_adpcm_encode()
*******************************************************************************/
short int sample_adpcm_encode()
{
  int m;

  int val;
  int diff;
  int delta;
  int step;

  unsigned char nybble;

  /* initialize values */
  val = 0;
  step = SAMPLE_ADPCM_STEP_LOWEST;

  for (m = 0; m < G_sample_count; m++)
  {
    /* compute difference */
    diff = G_sample_buffer[m] - val;

    if (diff >= 0)
      delta = (SAMPLE_ADPCM_STEP_DIVISOR * diff) / step;
    else
      delta = (SAMPLE_ADPCM_STEP_DIVISOR * (-diff)) / step;

    if (delta > 7)
      delta = 7;

    /* compute compressed sample */
    nybble = ((unsigned char) delta) & 0x07;

    if (diff < 0)
      nybble |= 0x08;

    /* adjust predicted value */
    delta = ((2 * delta + 1) * step) / (2 * SAMPLE_ADPCM_STEP_DIVISOR);

    if (diff < 0)
      val -= delta;
    else
      val += delta;

    /* compute next step */
    step = (S_sample_adpcm_adjust_table[nybble & 0x07] * step) / 256;

    if (step < SAMPLE_ADPCM_STEP_LOWEST)
      step = SAMPLE_ADPCM_STEP_LOWEST;
    else if (step > SAMPLE_ADPCM_STEP_HIGHEST)
      step = SAMPLE_ADPCM_STEP_HIGHEST;

    /* save compressed sample */
    G_sample_buffer[m] = nybble;
  }

  return 0;
}

/*******************************************************************************
** sample_adpcm_decode()
*******************************************************************************/
short int sample_adpcm_decode()
{
  int m;

  int val;
  int delta;
  int step;

  unsigned char nybble;

  /* initialize values */
  val = 0;
  step = SAMPLE_ADPCM_STEP_LOWEST;

  for (m = 0; m < G_sample_count; m++)
  {
    /* compute uncompressed sample */
    nybble = (unsigned char) G_sample_buffer[m];

    delta = ((2 * (nybble & 0x07) + 1) * step) / (2 * SAMPLE_ADPCM_STEP_DIVISOR);

    if (nybble & 0x08)
      val -= delta;
    else
      val += delta;

    if (val > 32767)
      val = 32767;
    else if (val < -32768)
      val = -32768;

    /* compute next step */
    step = (S_sample_adpcm_adjust_table[nybble & 0x07] * step) / 256;

    if (step < SAMPLE_ADPCM_STEP_LOWEST)
      step = SAMPLE_ADPCM_STEP_LOWEST;
    else if (step > SAMPLE_ADPCM_STEP_HIGHEST)
      step = SAMPLE_ADPCM_STEP_HIGHEST;

    /* save uncompressed sample */
    G_sample_buffer[m] = (short) val;
  }

  return 0;
}

