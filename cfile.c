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
    // TODO: Couldn't get gzdopen to work, not sure why
    //int fd = _fileno(cfid->fid);
    //int fd2 = _dup(fd);
    //fclose(cfid->fid);
    //cfid->fid_gz = gzdopen(fd2, "rb");
    fclose(cfid->fid);
    cfid->fid_gz = gzopen(filename, "rb");
    if (cfid->fid_gz == NULL) {
      printf("Error opening gz file\n");
      return EIO;
    }
    if (gzbuffer(cfid->fid_gz, CFILE_GZ_BUFFER_SIZE) != 0) {
      printf("Error calling gzbuffer.\n");
      return ENOMEM;
    }
    cfid->cfread_s = &_cfread_gz;
    cfid->cfclose = &_cfclose_gz;
    DEBUG_PRINT("Reading GZ file with cfile.\n");
  } else if (cfid->comp == COMP_BZ) {
    // TODO: bzip
    //fclose(cfid->fid);
    int bzerr;
    cfid->fid_bz = BZ2_bzReadOpen(&bzerr, cfid->fid, 0, 0, NULL, 0);
    //cfid->fid_bz = BZ2_bzopen(filename, "rb");
    if (cfid->fid_bz == NULL) {
      printf("Error opening bz file\n");
      return EIO;
    }
    cfid->cfread_s = &_cfread_bz;
    cfid->cfclose = &_cfclose_bz;
    DEBUG_PRINT("Reading BZ file with cfile.\n");
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
  size_t nPass = nread / INT32_MAX + 1;
  size_t nreadOut = 0;
  for (size_t i = 0; i < nPass; i++) {
    uint32_t nReadThisPass = __min((uint32_t) (nread-i*INT32_MAX), INT32_MAX);
    nreadOut += gzread(cfid->fid_gz, buf1, nReadThisPass);
    buf1 += nReadThisPass;
  }
    
  return nreadOut;
}

int CFILE_API _cfclose_gz  (cfid_s* cfid) 
{  
  int status1 = gzclose(cfid->fid_gz);
  if (status1 != Z_OK) {
    printf("Error in gzclose: %d\n", status1);
    //printf(gzerror(cfid->fid_gz, status1));
  }
  //int status2 = fclose(cfid->fid);
  //if (status2 != 0) printf("Error using fclose in _cfclose_gz\n");
  return status1;
}


size_t CFILE_API _cfread_bz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {
    // note that gzread won't handle single reads outside of an integer length
  char* buf1 = buf; // need a complete type to do pointer math below
  size_t nread = elemSize*elemCount;
  size_t nPass = nread / INT32_MAX + 1;
  size_t nreadOut = 0;
  for (size_t i = 0; i < nPass; i++) {
    int32_t nReadThisPass = (int32_t) __min((size_t) (nread-i*INT32_MAX), (size_t) INT32_MAX);
    nreadOut += BZ2_bzread(cfid->fid_bz, buf1, nReadThisPass);
    buf1 += nReadThisPass;
  }
    
  return nreadOut;
}
int CFILE_API _cfclose_bz  (cfid_s* cfid) 
{
  BZ2_bzclose(cfid->fid_bz);
  return(0);
}

size_t CFILE_API _cfread_xz  (void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}
size_t CFILE_API _cfread_zstd(void* buf, size_t bufSize, size_t elemSize, size_t elemCount, cfid_s* cfid) {return 0;}

int CFILE_API _cfclose_xz  (cfid_s* cfid) {return 0;}
int CFILE_API _cfclose_zstd(cfid_s* cfid) {return 0;}