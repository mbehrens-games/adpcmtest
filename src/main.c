/*******************************************************************************
** ADPCMtest - No Shinobi Knows Me 2025
*******************************************************************************/

/*******************************************************************************
** main.c
*******************************************************************************/

#include <stdio.h>

#include <string.h>

#include "fileio.h"
#include "sample.h"

#define FILENAME_SIZE 256

/*******************************************************************************
** main()
*******************************************************************************/
int main(int argc, char *argv[])
{
  int m;

  char import_filename[FILENAME_SIZE];
  char export_filename[FILENAME_SIZE];

  /* initialize filenames */
  for (m = 0; m < FILENAME_SIZE; m++)
  {
    import_filename[m] = '\0';
    export_filename[m] = '\0';
  }

  /* parse command line arguments */
  for (m = 1; m < argc; m++)
  {
    /* import filename */
    if (!strcmp(argv[m], "-i"))
    {
      m++;

      if (m >= argc)
      {
        printf("Insufficient number of arguments. ");
        printf("Expected import filename. Exiting...\n");
        return 0;
      }

      strncpy(&import_filename[0], argv[m], FILENAME_SIZE);
    }
    /* export filename */
    else if (!strcmp(argv[m], "-o"))
    {
      m++;

      if (m >= argc)
      {
        printf("Insufficient number of arguments. ");
        printf("Expected export filename. Exiting...\n");
        return 0;
      }

      strncpy(&export_filename[0], argv[m], FILENAME_SIZE);
    }
    else
    {
      printf("Unknown command line argument %s. Exiting...\n", argv[m]);
      return 0;
    }
  }

  /* generate tables */
  sample_generate_tables();

  /* import the wave file */
  if (fileio_wav_import(&import_filename[0]))
  {
    printf("Unable to import from file %s. Exiting...\n", &import_filename[0]);
    return 0;
  }

  /* adpcm encode & decode */
  sample_adpcm_encode();
  sample_adpcm_decode();

  /* export to new wave file */
  if (fileio_wav_export(&export_filename[0]))
  {
    printf("Unable to export to file %s. Exiting...\n", &export_filename[0]);
    return 0;
  }

  return 0;
}
