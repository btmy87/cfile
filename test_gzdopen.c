//
#include <io.h>
#include <stdio.h>
#include "zlib.h"

int main(void)
{
  char filename[] = "./test/test.tif.gz";
  FILE* fid = fopen(filename, "rb");
  int fn = fileno(fid);
  int fn2 = _dup(fn);
  gzFile fid_gz = gzdopen(fn, "rb");
  printf("Opened file\n");
  char buf[16];
  int nread = gzread(fid_gz, &buf, 16);
  printf("Read %d bytes\n", nread);
  int status = gzclose(fid_gz);
  printf("Status %d\n", status);
  return status;
}