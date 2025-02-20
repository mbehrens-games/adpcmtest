/*******************************************************************************
** sample.h (sample compression and effects)
*******************************************************************************/

#ifndef SAMPLE_H
#define SAMPLE_H

#define SAMPLE_BUFFER_SIZE (1 << 16)

extern short G_sample_buffer[SAMPLE_BUFFER_SIZE];
extern int   G_sample_count;

/* function declarations */
short int sample_generate_tables();

short int sample_adpcm_encode();
short int sample_adpcm_decode();

#endif

