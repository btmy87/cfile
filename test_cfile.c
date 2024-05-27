// simple test script for cfile

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cfile.h"

const char filename[] = "./test/test.tif";

int main(void) {
  // read in the raw file using standard read calls
  FILE* fid;
  if (fopen_s(&fid, filename, "rb") != 0) {
    printf_s("Error opening file using fopen_s\n");
    return 1;
  }

  // get file stats
  struct _stat64 fstats;
  if (_fstat64(_fileno(fid), &fstats) != 0) {
    printf_s("Error getting file stats\n");
    return 2;
  }
  size_t filesize = fstats.st_size;

  // initialize buffers for orignal file and for cfile test
  char* buf1 = malloc(filesize);
  char* buf2 = malloc(filesize);

  // read raw file
  size_t n1 = fread_s(buf1, filesize, 1, filesize, fid);
  fclose(fid);
  if (n1 != filesize) {
    printf_s("Error reading file with fread_s\n");
    return 3;
  }

  // now read the raw file with cfile
  cfid_s* cfid = calloc(1, sizeof(cfid_s));
  if (cfopen_s(cfid, filename, "rb") != 0) {
    printf_s("Error opening raw file with cfopen_s\n");
    return 4;
  }

  // read the file
  size_t n2 = (*(cfid->cfread_s))(buf2, filesize, 1, filesize, cfid);
  if (n2 != filesize) {
    printf_s("Error reading raw file with cfread_s\n");
    return 5;
  }

  // verify files are equal
  if (memcmp(buf1, buf2, filesize) != 0) {
    printf("Error: native raw file doesn't match raw file with cfile\n");
  }


  free(buf1);
  free(buf2);
  return 0;
}