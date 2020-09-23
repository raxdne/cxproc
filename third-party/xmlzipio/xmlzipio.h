/*
  This lib provides libxml2 read/write handlers for zip files
  
  the code uses the zip.c/.h and unzip.c/.h for the minizip example
  provided with zlib

  zip.c/.h and unzip.c/.h are published under the terms of zlib/libpng license

  xmlzipio.c/.h are published under the terms of the MIT License

  Copyright (c) 2003 Christian Engwer
 */

#ifndef XMLZIPIO_H
#define XMLZIPIO_H

#include <libxml/xmlIO.h>

#ifdef __cplusplus
extern "C" {
#endif
  
int    xmlZipMatch (char const *filename);
int    xmlZipMatchW(char const *filename);
void*  xmlZipOpen  (char const *filename);
void*  xmlZipOpenW (char const *filename);
int    xmlZipRead  (void *context, char *buffer, int len);
int    xmlZipWrite (void *context, const char *buffer, int len);
int    xmlZipClose (void *context);
int    xmlZipCloseW(void *context);

int    xmlCompressionLevel (int c);

int    xmlZipRegisterInputCallback ();
int    xmlZipRegisterOutputCallback ();

char*  xmlZipName (char const *path);
  
#ifdef __cplusplus
}
#endif  
  
#endif // XMLZIPIO_H
