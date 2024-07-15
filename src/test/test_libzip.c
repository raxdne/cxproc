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

/*!
*/
int
zipTest()
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (SKIPTEST) {

    struct zip *za;
    char buf[BUFFER_LENGTH];
    int err;
    int i, len;
    long long sum;
    char *pcNameFile = TESTPREFIX "option/archive/Demo.mmap";

    i++;
    printf("TEST %i in '%s:%i': zipProcessZipNode() = ",i,__FILE__,__LINE__);

    if ((za = zip_open(pcNameFile, 0, &err)) == NULL) {
        zip_error_to_str(buf, sizeof(buf), err, errno);
        fprintf(stderr, "can't open zip archive `%s': %s\n", pcNameFile, buf);
        return 1;
    }
 
    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
    struct zip_stat sb;
        if (zip_stat_index(za, i, 0, &sb) == 0) {
            printf("==================\n");
            len = strlen(sb.name);
            printf("Name: [%s], ", sb.name);
            printf("Size: [%llu], ", sb.size);
            printf("mtime: [%u]\n", (unsigned int)sb.mtime);
            if (sb.name[len - 1] == '/') {
	      //safe_create_dir(sb.name);
            } else {
    struct zip_file *zf;
                zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    fprintf(stderr, "boese, boese\n");
                    exit(100);
                }
 
                sum = 0;
                while (sum != sb.size) {
                    len = zip_fread(zf, buf, sizeof(buf));
                    if (len < 0) {
                        fprintf(stderr, "boese, boese\n");
                        exit(102);
                    }
                    sum += len;
                }
                zip_fclose(zf);
            }
        } else {
            printf("File[%s] Line[%d]\n", __FILE__, __LINE__);
        }
    }   
 
    if (zip_close(za) == -1) {
        fprintf(stderr, "can't close zip archive `%s'\n", pcNameFile);
    }
 
  }

#if 0
  if (SKIPTEST) {
    xmlNodePtr pndZip;
    xmlNodePtr pndDir;
    xmlNodePtr pndChild;
    cxpContextPtr pccT = cxpContextNew(NULL);

    i++;
    printf("TEST %i in '%s:%i': parse a directory and zip childs = ",i,__FILE__,__LINE__);

    pndZip = xmlNewNode(NULL,NAME_ZIP);
    xmlSetProp(pndZip,BAD_CAST"name",BAD_CAST"tmp/d-5.zip");

    pndDir = xmlNewChild(pndZip,NULL,NAME_PLAIN,BAD_CAST TESTPREFIX "test\test.bat\ntest/test.mak\ntest/dummy.txt\n");

    if (zipProcessZipNode(pndZip,pccT)==NULL) {
      printf("OK\n");
      n_ok++;
    }
    else {
      printf("Error zipProcessZipNode()\n");
    }
    xmlFreeNodeList(pndZip);
    cxpCtxtFree(pccT);
  }
#endif

  return (i - n_ok);
}
/* end of zipTest() */
