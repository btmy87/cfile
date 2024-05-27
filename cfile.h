// cfile
// libray providing a common interface for compressed
// and uncompressed files.
#pragma once

#include <stdio.h>

#include "zlib.h"
#include "bzlib.h"
#include "lzma.h"
#include "zstd.h"


#ifdef CFILE_EXPORTS
#define CFILE_API __declspec(dllexport)
#else
#define CFILE_API
#endif

//int n = fread(buf, elemSize, elemCount, fid);
//n = fclose(fid);
//FILE* fid = fopen()

enum comp_type {COMP_NONE, COMP_GZ, COMP_BZ, COMP_XZ, COMP_ZSTD};

// we'll use the structure below to hold any necessary file information
struct cfid_s; // forward declaration
typedef struct cfid_s cfid_s;
struct cfid_s {
  FILE* fid; // real file pointer
  size_t (*cfread_s)(void*, size_t, size_t, size_t, cfid_s*); // pointer to read function
  int (*cfclose)(cfid_s*); // pointer to close function
  gzFile *fid_gz;
  BZFILE *fid_bz;
  lzma_stream *fid_xz;
  ZSTD_DCtx *fid_zstd;
  enum comp_type comp;
};

// cfopen
// opens a file that might be compressed, and returns the cfid_struct
// that can be used for reading.
errno_t CFILE_API cfopen_s(cfid_s* cfid, const char* filename, const char* mode);

// don't expect user to call read function directly, use the pointer in cfid_struct
size_t CFILE_API _cfread_none(void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid);
size_t CFILE_API _cfread_gz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid);
size_t CFILE_API _cfread_bz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid);
size_t CFILE_API _cfread_xz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid);
size_t CFILE_API _cfread_zstd(void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid);

// don't call close functions directly, use pointer in cfid_struct
int CFILE_API _cfclose_none(cfid_s* cfid);
int CFILE_API _cfclose_gz  (cfid_s* cfid);
int CFILE_API _cfclose_bz  (cfid_s* cfid);
int CFILE_API _cfclose_xz  (cfid_s* cfid);
int CFILE_API _cfclose_zstd(cfid_s* cfid);
