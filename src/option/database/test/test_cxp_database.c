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
    resNodePtr prnDb = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse a directory into database = ", i, __FILE__, __LINE__);

    pndTestDb = xmlNewNode(NULL, NAME_DB);
    xmlSetProp(pndTestDb, BAD_CAST"name", BAD_CAST TEMPPREFIX "testcode-db-1.db3");
    xmlSetProp(pndTestDb, BAD_CAST"write", BAD_CAST"yes");
    xmlSetProp(pndTestDb, BAD_CAST"append", BAD_CAST"no");
    
    pndTestDir = xmlNewChild(pndTestDb,NULL, NAME_DIR, NULL);
    xmlSetProp(pndTestDir, BAD_CAST"name", BAD_CAST TESTPREFIX);
    xmlSetProp(pndTestDir, BAD_CAST"verbosity", BAD_CAST"1");

    //domPutNodeString(stderr, NULL, pndTestDb);

    if ((prnDb = dbProcessDbSourceNode(pndTestDb,pccArg)) == NULL) {
      printf("Error dbProcessDbSourceNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    resNodeFree(prnDb);
    xmlFreeNode(pndTestDb);
  }


#ifdef HAVE_JS

  if (RUNTEST) {
    xmlNodePtr pndTestDb;
    xmlNodePtr pndTestScript;
    resNodePtr prnDb = NULL;

    i++;
    printf("TEST %i in '%s:%i': parse a directory into database = ", i, __FILE__, __LINE__);

    pndTestDb = xmlNewNode(NULL, NAME_DB); /*  */
    xmlSetProp(pndTestDb, BAD_CAST"name", BAD_CAST TEMPPREFIX "testcode-db-script.db3");
    xmlSetProp(pndTestDb, BAD_CAST"write", BAD_CAST"yes");
    xmlSetProp(pndTestDb, BAD_CAST"append", BAD_CAST"no");
    
    pndTestScript = xmlNewChild(pndTestDb, NULL, NAME_SCRIPT, 
				"var strQuery = 'CREATE TABLE IF NOT EXISTS test(number int, dummy text);'"
				" + 'INSERT INTO test(number,dummy) VALUES (1,\"TEST\");'"
				" + 'INSERT INTO test(number,dummy) VALUES (2,\"TEST\");';"
				"strQuery;");

    //domPutNodeString(stderr, NULL, pndTestDb);

    if ((prnDb = dbProcessDbSourceNode(pndTestDb,pccArg)) == NULL) {
      printf("Error dbProcessDbSourceNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    resNodeFree(prnDb);
    xmlFreeNode(pndTestDb);
  }

#endif


  if (RUNTEST) {
    xmlChar *pucT = NULL;
    xmlNodePtr pndTestDb;
    xmlNodePtr pndTestPlain;
    resNodePtr prnDb = NULL;

    i++;
    printf("TEST %i in '%s:%i': insert data into database = ", i, __FILE__, __LINE__);

    pndTestPlain = xmlNewNode(NULL, NAME_PLAIN);
    xmlSetProp(pndTestPlain, BAD_CAST"name", BAD_CAST "-");
    
    pndTestDb = xmlNewChild(pndTestPlain,NULL, NAME_DB, NULL);

    xmlNewChild(pndTestDb, NULL, NAME_PLAIN, BAD_CAST 
    	"CREATE TABLE IF NOT EXISTS 'test' (i INTEGER, name);"
	"INSERT INTO test VALUES(0,'ABC');"
	"INSERT INTO test VALUES(1,'DEF');"
	"INSERT INTO test VALUES(2,'GHI');"
	);

    xmlNewChild(pndTestPlain,NULL, NAME_QUERY, "SELECT DISTINCT name FROM test;");

    //domPutNodeString(stderr, NULL, pndTestPlain);

    if ((pucT = cxpProcessPlainNode(pndTestPlain,pccArg)) == NULL) {
      printf("Error cxpProcessPlainNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    xmlFree(pucT);
    resNodeFree(prnDb);
    xmlFreeNode(pndTestPlain);
  }


  if (RUNTEST) {
    xmlDocPtr pdocResultT;
    xmlNodePtr pndTestDb;
    xmlNodePtr pndTestXml;

    i++;
    printf("TEST %i in '%s:%i': dump a database = ", i, __FILE__, __LINE__);

    pndTestXml = xmlNewNode(NULL, NAME_XML);
    xmlSetProp(pndTestXml, BAD_CAST"name", BAD_CAST "-");
    
    pndTestDb = xmlNewChild(pndTestXml,NULL, NAME_DB, NULL);
    xmlSetProp(pndTestDb, BAD_CAST"name", BAD_CAST TEMPPREFIX "testcode-db-1.db3");
    xmlSetProp(pndTestDb, BAD_CAST"dump", BAD_CAST"yes");

    //domPutNodeString(stderr, NULL, pndTestXml);

    if ((pdocResultT = cxpProcessXmlNode(pndTestXml,pccArg)) == NULL) {
      printf("Error cxpProcessXmlNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    xmlFreeNode(pndTestDb);
    xmlFreeDoc(pdocResultT);
  }


  if (RUNTEST) {
    xmlDocPtr pdocResultT;
    xmlNodePtr pndTestXml;
    xmlNodePtr pndTestDb;
    xmlNodePtr pndTestDir;

    i++;
    printf("TEST %i in '%s:%i': parse a directory into database = ", i, __FILE__, __LINE__);

    pndTestXml = xmlNewNode(NULL, NAME_XML);
    xmlSetProp(pndTestXml, BAD_CAST"name", BAD_CAST "-");

    pndTestDb = xmlNewChild(pndTestXml, NULL, NAME_DB, NULL); /* in memory db */
    
    pndTestDir = xmlNewChild(pndTestDb,NULL, NAME_DIR, NULL);
    xmlSetProp(pndTestDir, BAD_CAST"name", BAD_CAST TESTPREFIX);
    
#if 0
    xmlSetProp(pndTestDb, BAD_CAST"dump", BAD_CAST"yes");
#else
    xmlNewChild(pndTestXml,NULL, NAME_QUERY, "SELECT DISTINCT name FROM directory;");
#endif

    //domPutNodeString(stderr, NULL, pndTestXml);

    if ((pdocResultT = cxpProcessXmlNode(pndTestXml,pccArg)) == NULL) {
      printf("Error cxpProcessXmlNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    xmlFreeDoc(pdocResultT);
    xmlFreeNode(pndTestXml);
  }


  if (RUNTEST) {
    xmlDocPtr pdocResultT;
    xmlNodePtr pndTestXml;
    xmlNodePtr pndTestDb;
    xmlNodePtr pndTestQuery;

    i++;
    printf("TEST %i in '%s:%i': nested query to a database = ", i, __FILE__, __LINE__);

    pndTestXml = xmlNewNode(NULL, NAME_XML);
    xmlSetProp(pndTestXml, BAD_CAST"name", BAD_CAST "-");

    pndTestDb = xmlNewChild(pndTestXml, NULL, NAME_DB, NULL); /* in memory db */
    xmlSetProp(pndTestDb, BAD_CAST"name", BAD_CAST TEMPPREFIX "testcode-db-1.db3");
    
    pndTestQuery = xmlNewChild(pndTestXml,NULL, NAME_QUERY, NULL);
    xmlNewChild(pndTestQuery,NULL, NAME_QUERY, BAD_CAST "SELECT * FROM queries WHERE rowid = 2;");

    //domPutNodeString(stderr, NULL, pndTestXml);

    if ((pdocResultT = cxpProcessXmlNode(pndTestXml,pccArg)) == NULL) {
      printf("Error cxpProcessXmlNode()\n");
    }
    else {
      printf("OK\n");
      n_ok++;
    }

    xmlFreeDoc(pdocResultT);
    xmlFreeNode(pndTestXml);
  }

#endif


  return (i - n_ok);
}
/* end of dbCxpTest() */
