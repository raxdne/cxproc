/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2020 by Alexander Tenbusch

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
dbCxpTest(cxpContextPtr pccArg)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

#ifdef HAVE_LIBSQLITE3
  if (RUNTEST) {
    xmlNodePtr pndTestDb;
    xmlNodePtr pndTestDir;

    i++;
    printf("TEST %i in '%s:%i': parse a directory into database = ", i, __FILE__, __LINE__);

    pndTestDb = xmlNewNode(NULL, NAME_DB);
    xmlSetProp(pndTestDb, BAD_CAST"name", BAD_CAST TEMPPREFIX "testcode-db-1.db3");
    xmlSetProp(pndTestDb, BAD_CAST"write", BAD_CAST"yes");
    xmlSetProp(pndTestDb, BAD_CAST"append", BAD_CAST"no");
    
    pndTestDir = xmlNewChild(pndTestDb,NULL, NAME_DIR, NULL);
    xmlSetProp(pndTestDir, BAD_CAST"name", BAD_CAST TESTPREFIX);
    xmlSetProp(pndTestDir, BAD_CAST"verbosity", BAD_CAST"1");

    if (dbProcessDbNode(pndTestDb,pccArg) == FALSE) {
      printf("Error dbProcessDbNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    xmlFreeNode(pndTestDb);
  }
#endif


  return (i - n_ok);
}
/* end of dbCxpTest() */
