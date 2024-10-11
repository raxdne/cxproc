/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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
arcTestRead(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    arcPtr a;

    i++;
    printf("TEST %i in '%s:%i': test supported archive formats = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_filter_all(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_filter_all(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_format_empty(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_empty(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_format_gnutar(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_empty(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_format_raw(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_raw(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_format_zip(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_zip()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    archive_read_free(a);
  }

  if (RUNTEST) {
    arcPtr a;

    i++;
    printf("TEST %i in '%s:%i': test opening of non-existing archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_open_filename(a, TESTPREFIX "option/archive/test-arc-fail.tar", 2) == ARCHIVE_OK) {
      printf("error of archive_read_open_filename(): '%s'\n", archive_error_string(a));
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    archive_read_free(a);
  }

  if (SKIPTEST) {
    arcPtr a;
    arcEntryPtr ae;
    char *pcNameFile = strdup(TESTPREFIX "option/archive\\test-arc.txt.gz");
    int cchReadInput;		/*! counter for collected string length */
    char mcContent[BUFFER_LENGTH];

    i++;
    printf("TEST %i in '%s:%i': check data of existing gzip archive = ", i, __FILE__, __LINE__);

    resPathChangeToSlashes(BAD_CAST pcNameFile);
    
    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_support_filter_all(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_filter_all(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_open_filename(a, pcNameFile, BUFFER_LENGTH) != ARCHIVE_OK) {
      printf("error of archive_read_open_filename(): '%s'\n", archive_error_string(a));
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK) {
      printf("Could not read next file from %s\n", pcNameFile);
    }
    else if (cchReadInput = archive_read_data(a, mcContent, BUFFER_LENGTH) != 1024) {
      printf("Archive read error: %s", archive_error_string(a));
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_EOF) {
      printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
    }
    else {
      printf("OK\n");
      n_ok++;
    }
    archive_read_close(a);
    archive_read_free(a);
    free(pcNameFile);
  }


  if (RUNTEST) {
    int j = 0;
    arcPtr a;
    arcEntryPtr ae;
    char *pcNameFile = TESTPREFIX "option/archive/test-arc-1.tar";
    char *pcT;

    i++;
    printf("TEST %i in '%s:%i': check entries of existing TAR archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new()\n");
    }
    else if (archive_read_support_format_gnutar(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_gnutar()\n");
    }
    else if (archive_read_open_filename(a, pcNameFile, 2) != ARCHIVE_OK) {
      printf("error of archive_read_open_filename()\n");
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("2446.ics", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_format(a) != ARCHIVE_FORMAT_TAR) {
      printf("Could not read file format from %s\n", pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("test-pie-11.txt", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("test-pie-19.mm", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("test-pie-20.odt", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_EOF) {
      printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    archive_read_close(a);
    archive_read_free(a);
  }


  if (RUNTEST) {
    int j = 0;
    arcPtr a;
    arcEntryPtr ae;
    char *pcNameFile = TESTPREFIX "option/archive/test-zip-7.zip";
    char *pcT;

    i++;
    printf("TEST %i in '%s:%i': check entries of existing ZIP archive = ", i, __FILE__, __LINE__);

    if ((a = archive_read_new()) == NULL) {
      printf("error of archive_read_new()\n");
    }
    else if (archive_read_support_format_zip(a) != ARCHIVE_OK) {
      printf("error of archive_read_support_format_zip()\n");
    }
    else if (archive_read_open_filename(a, pcNameFile, 2) != ARCHIVE_OK) {
      printf("error of archive_read_open_filename()\n");
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("sub/", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_format(a) != ARCHIVE_FORMAT_ZIP) {
      printf("Could not read file format from %s\n", pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("sub/a.txt", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("sub/b.txt", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("sub/plain.txt", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("sub/weiter.png", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_OK || (pcT = (char *)archive_entry_pathname(ae)) == NULL || strcmp("TestContent.odt", pcT) != 0) {
      printf("Error filename %d from %s\n", j, pcNameFile);
    }
    else if (archive_read_next_header(a, &ae) != ARCHIVE_EOF) {
      printf("Could not detect ARCHIVE_EOF from %s\n", pcNameFile);
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    archive_read_close(a);
    archive_read_free(a);
  }



  return (i - n_ok);
}
/* end of arcTestRead() */


/*!
*/
int
arcTestWrite(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

  if (RUNTEST) {
    arcPtr a;
    arcEntryPtr ae;
    char *pcNameFile = TEMPPREFIX "test-1.zip";

    i++;
    printf("TEST %i in '%s:%i': packing existing files into ZIP archive = ", i, __FILE__, __LINE__);

    /* https://github.com/libarchive/libarchive/wiki/Examples#user-content-A_Basic_Write_Example */

    if ((a = archive_write_new()) == NULL) {
      printf("error of archive_write_new()\n");
    }
    else if (archive_write_set_format_zip(a) != ARCHIVE_OK) {
      printf("error of archive_write_set_format_pax_restricted()\n");
    }
    else if (archive_write_open_filename(a, pcNameFile) != ARCHIVE_OK) {
      printf("error of archive_write_open_filename()\n");
    }
    else if ((ae = archive_entry_new()) == NULL) {
      printf("error of archive_entry_new()\n");
    }
    else {
      char *pcContent = "0123456789";
      char *pcT;

      archive_entry_set_pathname(ae, "entry-1");
      archive_entry_set_size(ae, strlen(pcContent));
      archive_entry_set_filetype(ae, AE_IFREG);
      archive_entry_set_perm(ae, 0644);
      if (archive_write_header(a, ae) != ARCHIVE_OK) {
	printf("error archive_write_header()");
      }
      else if (archive_write_data(a, pcContent, strlen(pcContent)) != strlen(pcContent)) {
	printf("error of archive_write_data()\n");
      }
      else {
	printf("OK\n");
	n_ok++;
      }

      archive_entry_free(ae);
    }
    archive_write_close(a);
    archive_write_free(a);
  }

  return (i - n_ok);
}
/* end of arcTestWrite() */

