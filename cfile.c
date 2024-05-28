#include <stdlib.h>
#include <stdint.h>
#include "cfile.h"

const uint32_t CFILE_GZ_BUFFER_SIZE = 128*1024;
#define NMAGIC_MAX 6

#ifdef _DEBUG
#define DEBUG_PRINT(msgString) printf_s(msgString); 
#else
#define DEBUG_PRINT(msgString)
#endif

// cfopen
// open possibly compressed file for reading, only mode "rb" is supported
errno_t CFILE_API cfopen_s(cfid_s* cfid, const char* filename, const char* mode)
{
  // only "rb" mode is supported
  if (strcmp(mode, "rb") != 0) return ENOSYS;
  
  // open file
  errno_t err = fopen_s(&cfid->fid, filename, mode);
  if (err != 0) return err;    
  
  // detect compression
  char filemagic[NMAGIC_MAX];
  size_t nmagic = fread_s(&filemagic, NMAGIC_MAX, sizeof(char), NMAGIC_MAX, cfid->fid);
  if (nmagic < NMAGIC_MAX) return EIO;
  if (memcmp(&filemagic, "\x1f\x8b", 2) == 0) {
    cfid->comp = COMP_GZ;
  } else if (memcmp(&filemagic, "BZ", 2) == 0) {
    cfid->comp = COMP_BZ;
  } else if (memcmp(&filemagic, "\xfd\x37\x7a\x5a\x00", 6) == 0) {
    cfid->comp = COMP_XZ;
  } else if (memcmp(&filemagic, "\x28\xb5\x2f\xfd", 4) == 0) {
    cfid->comp = COMP_ZSTD;
  } else {
    cfid->comp = COMP_NONE;
  }
  fseek(cfid->fid, 0, SEEK_SET);

  // assign appropriate pointers for read/close
  // call specific cfopen function to initialize compression 
  // specific stuff.
  if (cfid->comp == COMP_NONE) {
    // TODO: no compression
    cfid->cfread_s = &_cfread_none;
    cfid->cfclose = &_cfclose_none;
    DEBUG_PRINT("Reading uncompressed file with cfile.\n");
  } else if (cfid->comp == COMP_GZ) {
    // TODO: gzip
    //int fd = _fileno(cfid->fid);
    //cfid->fid_gz = gzdopen(fd, "rb");
    fclose(cfid->fid);
    cfid->fid_gz = gzopen(filename, "rb");
    if (cfid->fid_gz == NULL) return EIO;
    if (gzbuffer(cfid->fid_gz, CFILE_GZ_BUFFER_SIZE) != 0) return ENOMEM;
    cfid->cfread_s = &_cfread_gz;
    cfid->cfclose = &_cfclose_gz;
    DEBUG_PRINT("Reading GZ file with cfile.\n");
  } else if (cfid->comp == COMP_BZ) {
    DEBUG_PRINT("Reading BZ file with cfile.\n");
    // TODO: bzip
  } else if (cfid->comp == COMP_XZ) {
    DEBUG_PRINT("Reading XZ file with cfile.\n");
    // TODO: xz
  } else if (cfid->comp == COMP_ZSTD) {
    DEBUG_PRINT("Reading ZSTD file with cfile.\n");
    // TODO: zstd
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

size_t CFILE_API _cfread_gz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) 
{
  // note that gzread won't handle single reads outside of an integer length
  char* buf1 = buf; // need a complete type to do pointer math below
  size_t nread = elemSize*elemCount;
  size_t nPass = nread / UINT32_MAX + 1;
  size_t nreadOut = 0;
  for (size_t i = 0; i < nPass; i++) {
    uint32_t nReadThisPass = __min((uint32_t) (nread-i*UINT32_MAX), UINT32_MAX);
    nreadOut += gzread(cfid->fid_gz, buf1, nReadThisPass);
    buf1 += nReadThisPass;
  }
    
  return nreadOut;
}

int CFILE_API _cfclose_gz  (cfid_s* cfid) 
{
  return gzclose_r(cfid->fid_gz);
}


size_t CFILE_API _cfread_bz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}
size_t CFILE_API _cfread_xz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}
size_t CFILE_API _cfread_zstd(void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}

int CFILE_API _cfclose_bz  (cfid_s* cfid) {return 0;}
int CFILE_API _cfclose_xz  (cfid_s* cfid) {return 0;}
int CFILE_API _cfclose_zstd(cfid_s* cfid) {return 0;}