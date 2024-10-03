/*
  cxproc - Configurable Xml PROCessor
   
  Copyright (C) 2006..2022 by Alexander Tenbusch

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License 
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#define TESTFILE_SOURCE (TEMPPREFIX "file-1.bin")
#define TESTFILE_TARGET (TEMPPREFIX "file-2.bin")
#define TESTDIR (TEMPPREFIX "dir-1")

/*! 
*/
int
ioTest(void)
{
  int i;
  int n_ok;
  int iMode = 0;
  xmlChar *pucModuleTestReport = xmlStrdup(BAD_CAST"\n");
  xmlChar mucTestLabel[BUFFER_LENGTH];
  xmlChar mucTestResult[BUFFER_LENGTH];

#ifdef _MSC_VER
#else
  iMode = (S_IRUSR | S_IWUSR | S_IXUSR);
#endif

  n_ok=0;
  i=0;

  if (RUNTEST) {
    FILE *fd = NULL;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    xmlStrPrintf(mucTestLabel, BUFFER_LENGTH, "\nTEST %i in '%s:%i': create and rename a new file = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel, stderr);
    mucTestResult[0] = '\0';

    if ((fd = fopen(TESTFILE_SOURCE, "w")) == NULL) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 2 fopen()\n");
    }
    else if (fwrite((const void *)mucTestLabel,strlen(mucTestLabel),1,fd) < 1) {
      xmlStrPrintf(mucTestResult,BUFFER_LENGTH,"Error 1 fwrite()\n");
    }
    else if (fclose(fd) != 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 fclose()\n");
    }
    else if (rename(TESTFILE_SOURCE, TESTFILE_TARGET) != 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 rename()\n");
    }
    else if (unlink(TESTFILE_TARGET) != 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 unlink()\n");
    }
    else if (unlink(TESTFILE_SOURCE) == 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 unlink()\n");
    }
    else {
      n_ok++;
    }
  }


#ifdef _MSC_VER
#else
  if (RUNTEST) {
    DIR *dd = NULL;
    struct dirent *t = NULL;

    i++;
    printf("TEST %i in '%s:%i': ", i, __FILE__, __LINE__);

    xmlStrPrintf(mucTestLabel, BUFFER_LENGTH, "\nTEST %i in '%s:%i': create and read a new directory = ", i, __FILE__, __LINE__);
    fputs((const char *)mucTestLabel, stderr);
    mucTestResult[0] = '\0';

    if (mkdir(NULL, iMode) == 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 mkdir()\n");
    }
    else if (mkdir(TESTDIR, iMode) != 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 2 mkdir()\n");
    }
    else if ((dd = opendir(TESTDIR)) == NULL) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 opendir()\n");
    }
    else if ((t = readdir(dd)) == NULL) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 readdir()\n");
    }
    else if (closedir(dd) != 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 closedir()\n");
    }
    else if (rmdir(TESTDIR) != 0) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 rmdir()\n");
    }
    else if ((dd = opendir(TESTDIR)) != NULL) {
      xmlStrPrintf(mucTestResult, BUFFER_LENGTH, "Error 1 opendir()\n");
    }
    else {
      n_ok++;
    }
  }
#endif

  printf("Result in '%s': %i/%i OK\n\n",__FILE__,n_ok,i);

  return (i - n_ok);
}
/* end of ioTest() */
