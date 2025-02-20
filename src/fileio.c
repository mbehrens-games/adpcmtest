/*******************************************************************************
** fileio.c (wave file import and export)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "sample.h"

/*******************************************************************************
** fileio_wav_import()
*******************************************************************************/
short int fileio_wav_import(char* filename)
{
  FILE* fp;

  char id_field[4];

  unsigned int   chunk_size;
  unsigned int   header_subchunk_size;
  unsigned int   data_subchunk_size;

  unsigned short audio_format;
  unsigned short num_channels;
  unsigned int   sampling_rate;
  unsigned int   byte_rate;
  unsigned short sample_size;
  unsigned short bit_resolution;

  unsigned int num_samples;

  /* make sure filename exists */
  if (filename == NULL)
    return 1;

  /* open file */
  fp = fopen(filename, "rb");

  if (fp == NULL)
    return 1;

  /* read 'RIFF' */
  if (fread(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if (strncmp(&id_field[0], "RIFF", 4))
  {
    fclose(fp);
    return 1;
  }

  /* read chunk size */
  if (fread(&chunk_size, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* read 'WAVE' */
  if (fread(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if (strncmp(&id_field[0], "WAVE", 4))
  {
    fclose(fp);
    return 1;
  }

  /* read 'fmt ' */
  if (fread(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if (strncmp(&id_field[0], "fmt ", 4))
  {
    fclose(fp);
    return 1;
  }

  /* read header subchunk size */
  if (fread(&header_subchunk_size, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (header_subchunk_size != 16) /* always 16 for PCM data */
  {
    fclose(fp);
    return 1;
  }

  /* read header subchunk */
  if (fread(&audio_format, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fread(&num_channels, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fread(&sampling_rate, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fread(&byte_rate, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fread(&sample_size, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fread(&bit_resolution, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* make sure the values from the header are correct */
  if ((audio_format != 1) ||  /* 1 denotes PCM */
      (num_channels != 1) ||  /* mono */
      (sample_size != 2)  ||  /* 2 bytes (16 bits, 1 channel) */
      (bit_resolution != 16)) /* 16 bit */
  {
    fclose(fp);
    return 1;
  }

  if (sampling_rate != 44091) /* not 44100 because of Milkytracker */
  {
    fclose(fp);
    return 1;
  }

  if (byte_rate != (sampling_rate * sample_size))
  {
    fclose(fp);
    return 1;
  }

  /* read 'data' */
  if (fread(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if (strncmp(&id_field[0], "data", 4))
  {
    fclose(fp);
    return 1;
  }

  /* read data subchunk size */
  if (fread(&data_subchunk_size, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* determine number of samples */
  num_samples = data_subchunk_size / sample_size;

  if (num_samples > SAMPLE_BUFFER_SIZE)
  {
    fclose(fp);
    return 1;
  }

  /* read data subchunk */
  if (fread(&G_sample_buffer[0], 2, num_samples, fp) < num_samples)
  {
    fclose(fp);
    return 1;
  }

  G_sample_count = num_samples;

  /* close file */
  fclose(fp);

  return 0;
}

/*******************************************************************************
** fileio_wav_export()
*******************************************************************************/
short int fileio_wav_export(char* filename)
{
  FILE* fp;

  char id_field[4];

  unsigned int   chunk_size;
  unsigned int   header_subchunk_size;
  unsigned int   data_subchunk_size;

  unsigned short audio_format;
  unsigned short num_channels;
  unsigned int   sampling_rate;
  unsigned int   byte_rate;
  unsigned short sample_size;
  unsigned short bit_resolution;

  unsigned int num_samples;

  /* make sure filename exists */
  if (filename == NULL)
    return 1;

  /* set and compute values */
  audio_format = 1;
  num_channels = 1;
  sampling_rate = 44100;
  bit_resolution = 16;
  sample_size = num_channels * (bit_resolution / 8);
  byte_rate = sampling_rate * sample_size;

  header_subchunk_size = 16;
  data_subchunk_size = G_sample_count * sample_size;
  chunk_size = 4 + (8 + header_subchunk_size) + (8 + data_subchunk_size);

  /* open file */
  fp = fopen(filename, "wb");

  if (fp == NULL)
    return 1;

  /* write 'RIFF' */
  strncpy(&id_field[0], "RIFF", 4);

  if (fwrite(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write chunk size */
  if (fwrite(&chunk_size, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* write 'WAVE' */
  strncpy(&id_field[0], "WAVE", 4);

  if (fwrite(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write 'fmt ' */
  strncpy(&id_field[0], "fmt ", 4);

  if (fwrite(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write header subchunk size */
  if (fwrite(&header_subchunk_size, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* write header subchunk */
  if (fwrite(&audio_format, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fwrite(&num_channels, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fwrite(&sampling_rate, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fwrite(&byte_rate, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fwrite(&sample_size, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  if (fwrite(&bit_resolution, 2, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* write 'data' */
  strncpy(&id_field[0], "data", 4);

  if (fwrite(id_field, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write data subchunk size */
  if (fwrite(&data_subchunk_size, 4, 1, fp) < 1)
  {
    fclose(fp);
    return 1;
  }

  /* determine number of samples */
  num_samples = G_sample_count;

  /* write data subchunk */
  if (fwrite(&G_sample_buffer[0], 2, num_samples, fp) < num_samples)
  {
    fclose(fp);
    return 1;
  }

  /* close file */
  fclose(fp);

  return 0;
}

