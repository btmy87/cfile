#include <stdlib.h>
#include "cfile.h"

// cfopen
// open possibly compressed file for reading, only mode "rb" is supported
errno_t CFILE_API cfopen_s(cfid_s* cfid, const char* filename, const char* mode)
{
  // only "rb" mode is supported
  if (strcmp(mode, "rb") != 0) return ENOSYS;
  
  // open file
  errno_t err = fopen_s(&cfid->fid, filename, mode);
  if (err == 0) {
    // detect compression
    // TODO

    // assign appropriate pointers for read/close
    // call specific cfopen function to initialize compression 
    // specific stuff.
    if (cfid->comp == COMP_NONE) {
      // TODO: no compression
      cfid->cfread_s = &_cfread_none;
      cfid->cfclose = &_cfclose_none;
    } else if (cfid->comp == COMP_GZ) {
      // TODO: gzip
    } else if (cfid->comp == COMP_BZ) {
      // TODO: bzip
    } else if (cfid->comp == COMP_XZ) {
      // TODO: xz
    } else if (cfid->comp == COMP_ZSTD) {
      // TODO: zstd
    }
  }

  return err;
}

size_t CFILE_API _cfread_none(void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid)
{
  return fread_s(buf, bufSize, elemSize, elemCount, cfid->fid);
}

int CFILE_API _cfclose_none(cfid_s* cfid)
{
  return fclose(cfid->fid);
}

size_t CFILE_API _cfread_gz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}
size_t CFILE_API _cfread_bz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}
size_t CFILE_API _cfread_xz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}
size_t CFILE_API _cfread_zstd(void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}

int CFILE_API _cfclose_gz  (cfid_s* cfid) {return 0;}
int CFILE_API _cfclose_bz  (cfid_s* cfid) {return 0;}
int CFILE_API _cfclose_xz  (cfid_s* cfid) {return 0;}
int CFILE_API _cfclose_zstd(cfid_s* cfid) {return 0;}