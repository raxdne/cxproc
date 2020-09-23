/*
  This lib provides libxml2 read/write handlers for zip files
  
  the code uses the zip.c/.h and unzip.c/.h for the minizip example
  provided with zlib

  zip.c/.h and unzip.c/.h are published under the terms of zlib/libpng license

  xmlzipio.c/.h are published under the terms of the MIT License

  Copyright (c) 2003 Christian Engwer
*/

#include "xmlzipio.h"
#include "unzip.h"
#include "zip.h"

#include <libxml/uri.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#include <time.h>
#elif defined WIN32
#include <io.h>
#include <unistd.h>
#include <dirent.h>
#else
#include <sys/dir.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include <assert.h>

/* Global variables */

// How much do we want to compress?
static int xmlCompress = Z_DEFAULT_COMPRESSION;

/* These hold the current state for zipping or unzipping */

typedef struct zipContext_s {
  zipFile zip;
  int     state;  
} zipContext;

typedef struct unzConext_s {
  unzFile zip;
  int     state;
} unzContext;

/* helper functions */

// get zip name from path
char* xmlZipName (char const *path);
// create a tmp name from a zip name
char* xmlTmpZipName (char const *ZipName);
// get file name from path
char* xmlFileInZipName (char const *path);
// copy a zip file to an other (needed for merge)
int xmlCopyZip (unzFile in, zipFile out, const char* pcNot);
// checks wether the file exists or not
int xmlFileExists (const char *filename);
// checks wether we can create the file
int xmlFileWriteable (const char *filename);

/* Implementation of the interface */

int xmlZipMatch (char const *filename) {
  char* uri;
  char* zipname;  
  if (filename == 0) return 0;
  uri = xmlURIUnescapeString(filename, strlen(filename), NULL);
#ifdef XMLZIPIO_VERBOSE
  fprintf(stderr,"Matching for %s\n",uri);
#endif
  if (strncmp(uri, "zip:", 4) == 0) {
    zipname = xmlZipName(filename);
    if (zipname != 0) {
      if (xmlFileExists(zipname) == 1) {
        xmlFree(uri);
        free(zipname);
        return 1;
      }
    }
  }
  xmlFree(uri);
  return 0;
}

int xmlZipMatchW(char const *filename) {
  char* uri;
  char* zipname;  
  if (filename == 0) return 0;
  uri = xmlURIUnescapeString(filename, strlen(filename), NULL);
#ifdef XMLZIPIO_VERBOSE
  fprintf(stderr,"Matching for %s\n",uri);
#endif
  if (strncmp(uri, "zip:", 4) == 0) {
    zipname = xmlZipName(filename);
    if (zipname != 0) {
      if (xmlFileWriteable(zipname) == 1) {
        xmlFree(uri);
        return 1;
      }
    }
  }
  xmlFree(uri);
  return 0;
}

void*  xmlZipOpen  (char const *filename) {
  unzContext* context = (unzContext*) malloc(sizeof(unzContext));
  int ret;
  char* ZipName;
  char* FileInZipName;
  char* uri;
  // retrieve filenames
  if (filename == 0) {
    return 0;
  }
  uri = xmlURIUnescapeString(filename, strlen(filename), NULL);
  if (uri == 0) {
    return 0;
  }
  ZipName = xmlZipName(uri);
  if (ZipName == 0) {
    xmlFree(uri);
    return 0;
  }
  FileInZipName = xmlFileInZipName(uri);
  if (FileInZipName == 0) {
    xmlFree(uri);
    free(ZipName);
    return 0;
  }
  // init context
  context->zip = 0;
  context->state = -UNZ_OK;
#ifdef XMLZIPIO_VERBOSE
  fprintf(stderr,"%s -> >>%s<< - >>%s<<\n",
         filename, ZipName, FileInZipName);
#endif
  context->zip = unzOpen (ZipName);
  // open file
  ret = unzLocateFile (context->zip, FileInZipName, 1);
  if (ret != UNZ_OK || context->zip == 0) {
    xmlZipClose(context);
    xmlFree(uri);
    free (ZipName);
    free (FileInZipName);
    return 0;
  }
  else {
    context->state = unzOpenCurrentFile(context->zip);
  }
  xmlFree(uri);
  free (ZipName);
  free (FileInZipName);
  return context;
}

int xmlZipRead (void *_context, char *buffer, int len) {
  unzContext* context = (unzContext*) _context;
  if (context == 0) return -1;
  return unzReadCurrentFile(context->zip, buffer, len);
}

int xmlZipClose(void* _context) {
  unzContext* context = (unzContext*) _context;
  int ret = 0;
  if (context != 0) {
    if (context->state == UNZ_OK)
      if (UNZ_OK != unzCloseCurrentFile(context->zip))
        ret = -1;
    if (context->zip != 0) {
      if (UNZ_OK != unzClose(context->zip))
        ret = -1;
    }
    free(context);
  }
  return ret;
}

void*  xmlZipOpenW  (char const *filename) {
  zipContext* context = (zipContext*) malloc(sizeof(zipContext));
  int ret;
  time_t epoch;
  struct tm* date;
  char* error;
  char* ZipName;
  char* TmpZipName;
  char* FileInZipName;
  char* uri;
  zip_fileinfo info_out;
  unzFile input;
  /* retrieve filenames */
  if (filename == 0) {
    return 0;
  }
  uri = xmlURIUnescapeString(filename, strlen(filename), NULL);
  if (uri == 0) {
    return 0;
  }
  ZipName = xmlZipName(uri);
  if (ZipName == 0) {
    xmlFree(uri);
    return 0;
  }
  TmpZipName = xmlTmpZipName(ZipName);
  if (TmpZipName == 0) {
    xmlFree(uri);
    free(ZipName);
    return 0;
  }
  FileInZipName = xmlFileInZipName(uri);
  if (FileInZipName == 0) {
    xmlFree(uri);
    free(ZipName);
    free(TmpZipName);
    return 0;
  }
  /* create context */
  context->zip = 0;
  context->state = -ZIP_OK;
#ifdef XMLZIPIO_VERBOSE
  fprintf(stderr,"%s -> >>%s<< (%s) - >>%s<<\n",
         uri, ZipName, TmpZipName, FileInZipName);
#endif
  /* copy old file if needed */
  if (xmlFileExists(ZipName)) {
    ret = rename(ZipName, TmpZipName);
    if (ret == -1) {
      xmlZipCloseW(context);
      error = strerror(errno);
#ifdef XMLZIPIO_VERBOSE
      fprintf(stderr,"error renaming %s - %s\n", ZipName, error);      
#endif
      free(error);
    }
    else {
      /* open zip */
#ifdef XMLZIPIO_DEBUG
      system ("ls -l test.zip");
      system ("ls -l test.zip_");
#endif
      context->zip = zipOpen (ZipName, 0);
      input = unzOpen (TmpZipName);
      if (context->zip == 0 || input == 0) {
#ifdef XMLZIPIO_VERBOSE
        fprintf(stderr,"error copying data from orig zip");
        if (input != 0)
          fprintf(stderr," - could not read %s\n", TmpZipName);
        else
          fprintf(stderr," - could not write %s\n", ZipName);
#endif
        xmlZipCloseW(context);
        if (input != 0)
          unzClose(input);
        rename(TmpZipName, ZipName);
        xmlFree(uri);
        free (ZipName);
        free (TmpZipName);
        free (FileInZipName);
        return 0;
      }
      xmlCopyZip(input, context->zip, FileInZipName);
      unzClose(input);
      unlink(TmpZipName);
    }
  }
  else {
    /* open zip */
    context->zip = zipOpen (ZipName, 0);
  }
  /* get current time */  
  epoch = time(0);
  date = localtime(&epoch);
  /* create new entry */
  info_out.tmz_date.tm_sec = date->tm_sec;
  info_out.tmz_date.tm_min = date->tm_min;
  info_out.tmz_date.tm_hour = date->tm_hour;
  info_out.tmz_date.tm_mday = date->tm_mday;
  info_out.tmz_date.tm_mon = date->tm_mon;
  info_out.tmz_date.tm_year = date->tm_year;
  info_out.dosDate = 0;
  info_out.internal_fa = 0;
  info_out.external_fa = 0;
//  free (date);
  ret = zipOpenNewFileInZip (context->zip, FileInZipName, &info_out,
                             NULL, 0, NULL, 0, NULL,
                             (xmlCompress != 0) ? Z_DEFLATED : 0,
                             xmlCompress);
  if (ret != ZIP_OK) {
#ifdef XMLZIPIO_VERBOSE
    fprintf(stderr,"error opening file in zip\n");
#endif
    xmlZipCloseW(context);
    xmlFree(uri);
    free (ZipName);
    free (TmpZipName);
    free (FileInZipName);
    return 0;
  }
  else {
    context->state = unzOpenCurrentFile(context->zip);
  }
  xmlFree(uri);
  free (ZipName);
  free (TmpZipName);
  free (FileInZipName);
  return context;
}

int xmlZipWrite (void *_context, const char *buffer, int len) {
  int ret;
  unzContext* context = (unzContext*) _context;
  if (context == 0) return -1;
  ret = zipWriteInFileInZip (context->zip, (char* )buffer, len);
#ifdef XMLZIPIO_VERBOSE
  fprintf(stderr,"Write %i returned %i\n", len, ret);
#endif
  if (ret == ZIP_OK) return len;
  return -1;
}

int xmlZipCloseW(void* _context) {
  zipContext* context = (zipContext*) _context;
  int ret = 0;
  if (context != 0) {
    if (context->state == ZIP_OK)
      if (ZIP_OK != zipCloseFileInZip(context->zip))
        ret = -1;
    if (context->zip != 0) {
      if (ZIP_OK != zipClose(context->zip, 0))
        ret = -1;
    }
    free(context);
  }  
  return ret;
}

int xmlCompressionLevel (int c) {
  int oldc = xmlCompress;
  xmlCompress = c;
  return oldc;
}

int xmlZipRegisterInputCallback () {
  /* register our callbacks */
  return xmlRegisterInputCallbacks (xmlZipMatch,
                                    xmlZipOpen,
                                    xmlZipRead,
                                    xmlZipClose);
}

int xmlZipRegisterOutputCallback () {
  /* register our callbacks */
  return xmlRegisterOutputCallbacks (xmlZipMatchW,
                                     xmlZipOpenW,
                                     xmlZipWrite,
                                     xmlZipCloseW);
}

/* Implementation of the helper functions */

char* xmlZipName (char const *path) {
  int count = 0;
  char* sub = 0;
  char* end = 0;
  char* ZipName = 0;
  if (path == 0) return NULL;
  if (strlen(path) < 4) return NULL;
  sub = ((char*)path) + 4;
  while (NULL != (sub = strchr(sub, '!'))) {
    if (0 == strncmp(sub, "!/", 2)) {
      end=sub;
    }
    sub++;
  }
  if (end == 0) return NULL;
  sub = ((char*)path) + 4;
  if (strlen(sub) >= 8) {
    if (strncmp(sub, "file:///", 8) == 0) {
      sub += 7;
    }
  }
  count = end - sub;
  ZipName = (char*) malloc(count+1);
  strncpy(ZipName, sub, count);
  ZipName[count] = 0;
  return ZipName;      
}

char* xmlTmpZipName (const char* ZipName) {
  char* TmpZipName;
  int len;
  int add;
  int a;
  if (ZipName == 0) return 0;
  len = strlen(ZipName);
  for (add=1;; add++) {
    TmpZipName = (char*) malloc(len+1+add);
    assert (TmpZipName);
    strcpy (TmpZipName, ZipName);
    for (a=len; a<len+add; a++) TmpZipName[a]='_';
    TmpZipName[a]=0;
    sprintf(TmpZipName,"%s_",ZipName);
    if (0 == xmlFileExists(TmpZipName))
      return TmpZipName;
    free(TmpZipName);
  }
  return 0;
}

char* xmlFileInZipName (char const *path) {
  char* sub = 0;
  char* end = 0;
  char* FileInZipName = 0;
  if (path == 0) return NULL;
  if (strlen(path) < 4) return NULL;
  sub = ((char*)path) + 4;
  while (NULL != (sub = strchr(sub, '!'))) {
    if (0 == strncmp(sub, "!/", 2)) {
      end=sub;
    }
    sub++;
  }
  if (end == 0) return NULL;
  FileInZipName = strdup(end+2);
  return FileInZipName;
}

int xmlCopyZip(unzFile in, zipFile out, const char* pcNot) {
  zip_fileinfo info_out;
  unz_file_info info_in;
  unz_global_info ginfo_in;
  char * name = (char*) malloc(4096);
  char * buf = (char*) malloc(4096);
  int err = 0;
  int read;
  int i;
  
  err = unzGetGlobalInfo (in, &ginfo_in);  
  if (err != UNZ_OK) {
    free(name);
    free(buf);
    return -1;
  }
  err = unzGoToFirstFile(in);
  if (err != UNZ_OK) {
    free(name);
    free(buf);
    return -1;
  }
  for (i=0; i<(int)ginfo_in.number_entry; i++) {
    err = unzGetCurrentFileInfo (in, &info_in, name, 4096, NULL, 0, NULL, 0);
    if (err != UNZ_OK) {
      free(name);
      free(buf);
      return -1;
    }

    if (strcmp(name,pcNot) != 0) {    
      info_out.tmz_date.tm_sec = info_in.tmu_date.tm_sec;
      info_out.tmz_date.tm_min = info_in.tmu_date.tm_min;
      info_out.tmz_date.tm_hour = info_in.tmu_date.tm_hour;
      info_out.tmz_date.tm_mday = info_in.tmu_date.tm_mday;
      info_out.tmz_date.tm_mon = info_in.tmu_date.tm_mon;
      info_out.tmz_date.tm_year = info_in.tmu_date.tm_year;
      info_out.dosDate = info_in.dosDate;
      info_out.internal_fa = info_in.internal_fa;
      info_out.external_fa = info_in.external_fa;
      
      err = zipOpenNewFileInZip (out, name, &info_out,
                                 NULL, 0, NULL, 0, NULL,
                                 (xmlCompress != 0) ? Z_DEFLATED : 0,
                                 xmlCompress);
      if (err != ZIP_OK) {
#ifdef XMLZIPIO_VERBOSE
        fprintf(stderr,"error in opening %s in zipfile\n", name);
#endif
        free(name);
        free(buf);
        return -1;
      }
      else {
        err = ZIP_OK;
        read = -1;
        err = unzOpenCurrentFile(in);
        if (err != UNZ_OK) {
#ifdef XMLZIPIO_VERBOSE
          fprintf(stderr,"error reading %s in the zipfile\n", name);        
#endif
          free(name);
          free(buf);
          return -1;
        }
        while (read != 0) {
          read = unzReadCurrentFile(in, buf, 4096);
          err = zipWriteInFileInZip (out, buf, read);
          if (err < 0) {
#ifdef XMLZIPIO_VERBOSE
            fprintf(stderr,"error in writing %s in the zipfile\n", name);
#endif
            free(name);
            free(buf);
            return -1;
          }
        }
        err = unzCloseCurrentFile(in);
        if (err == UNZ_CRCERROR) {
#ifdef XMLZIPIO_VERBOSE
          fprintf(stderr,"crc-error while reading %s in the zipfile\n", name);
#endif
          free(name);
          free(buf);
          return -1;
        }
      }
      
      err = zipCloseFileInZip (out);
      if (err != ZIP_OK) {
#ifdef XMLZIPIO_VERBOSE
        fprintf(stderr,"error in closing %s in the zipfile\n", name);
#endif
        free(name);
        free(buf);
        return -1;
      }
    }
    unzGoToNextFile(in);
  }
  free (name);
  free (buf);
  return 0;
}

int xmlFileExists (const char *filename) {
  FILE *ftestexist = NULL;
  int ret = 1;
  if (filename == 0) return 0;
  ftestexist = fopen (filename, "rb");
  if (ftestexist == NULL)
    ret = 0;
  else
    fclose (ftestexist);
  return ret;
}

int xmlFileWriteable (const char *filename) {
  FILE *ftestwrite = NULL;
  int ret = 1;
  int fileexists = 0;
  fileexists = xmlFileExists(filename);
  if (filename == 0) return 0;
  ftestwrite = fopen (filename, "ab");
  if (ftestwrite == NULL)
    ret = 0;
  else
    fclose (ftestwrite);
  if (fileexists == 0)
    unlink(filename);
  return ret;
}

