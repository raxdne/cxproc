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

#if 0

 https://daniel.haxx.se/blog/2018/09/09/libcurl-gets-a-url-api/ starting with 7.62.0
 https://curl.haxx.se/libcurl/c/parseurl.html
 libxml2/include/libxml/uri.h
 https://tools.ietf.org/html/rfc3986

file:///tmp
file:///tmp?glob=*.txt
file:///tmp/test.db3
file:///tmp/test.db3?query=SELECT%20*%20FROM%20directory
file:///tmp/test.mp3
file:///tmp/test.png
file:///tmp/test.txt
file:///tmp/test.txt?grep=abc
file:///tmp/test.txt?xpath=/pie/task
file:///tmp/test.zip
file:///tmp/test.zip?glob=*.png
file:///tmp/test.zip?name=sub/plain.txt
file:///tmp/test.zip?name=sub/test.docx&name=word/content.xml&xpath=/w/h
ftp://127.0.0.1/demo
http://127.0.0.1/cxproc/exe?cxp=info
http://127.0.0.1/demo
http://127.0.0.1/demo?glob=*.txt
http://127.0.0.1/demo/test.db3?query=SELECT%20*%20FROM%20directory
http://127.0.0.1/demo/test.mp3?info
http://127.0.0.1/demo/test.txt
http://127.0.0.1/demo/test.txt?grep=abc
http://127.0.0.1/demo/test.xml?xpath=/pie/task
http://127.0.0.1/demo/test.zip?name=sub/plain.txt?grep=abc
mem:///tmp/test.txt
mem:///tmp/test.db3?query=SELECT%20*%20FROM%20directory

#endif
  
/*!
*/
int
curlTest(void)
{
  int i;
  int n_ok;

  n_ok=0;
  i=0;

#ifdef HAVE_LIBCURL

  if (RUNTEST) {
    /* TEST:
     */
    CURLU *h;
    CURLUcode uc;
    char *host = NULL;
    char *path = NULL;
    char *path2 = NULL;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if ((h = curl_url()) == NULL) { /* get a handle to work with */
      printf("Error curl_url()\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, "http://example.com/path/index.html", 0)) != CURLE_OK) { /* parse a full URL */
      printf("Error curl_url_set() ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_HOST, &host, 0)) != CURLE_OK) { /* extract host name from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path, 0)) != CURLE_OK) { /* extract the path from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, "../another/second.html", 0)) != CURLE_OK) { /* redirect with a relative URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path2, 0)) != CURLE_OK) { /* extract the new, updated path */
      printf("Error () ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    curl_free(host);
    curl_free(path);
    curl_free(path2);
    curl_url_cleanup(h); /* free url handle */
  }

  if (RUNTEST) {
    /* TEST:
     */
    CURLU *h;
    CURLUcode uc;
    char *host = NULL;
    char *path = NULL;
    char *query = NULL;
    char *fragment = NULL;
    char *path2 = NULL;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if ((h = curl_url()) == NULL) { /* get a handle to work with */
      printf("Error curl_url()\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, "file:///tmp/test%20-%2022.txt?grep=abc#s1_2_2", 0)) != CURLE_OK) { /* parse a full URL */
      printf("Error curl_url_set() ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_HOST, &host, 0)) != CURLUE_NO_HOST) { /* no host name from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path, 0)) != CURLE_OK) { /* extract the path from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_QUERY, &query, 0)) != CURLE_OK) { /* extract the query from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_FRAGMENT, &fragment, 0)) != CURLE_OK) { /* extract the fragment from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, "../another/second.html", 0)) != CURLE_OK) { /* redirect with a relative URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path2, 0)) != CURLE_OK) { /* extract the new, updated path */
      printf("Error () ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    curl_free(host);
    curl_free(path);
    curl_free(path2);
    curl_url_cleanup(h); /* free url handle */
  }

  if (SKIPTEST) {
    /* TEST:
     */
    CURLU *h;
    CURLUcode uc;
    char *host = NULL;
    char *path = NULL;
    char *query = NULL;
    char *fragment = NULL;
    char *path2 = NULL;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if ((h = curl_url()) == NULL) { /* get a handle to work with */
      printf("Error curl_url()\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, "file://C:/tmp/test%20-%2022.txt", 0)) != CURLE_OK) { /* parse a full URL */
      printf("Error curl_url_set() ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_HOST, &host, 0)) != CURLUE_NO_HOST) { /* no host name from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path, 0)) != CURLE_OK) { /* extract the path from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_QUERY, &query, 0)) != CURLE_OK) { /* extract the query from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_FRAGMENT, &fragment, 0)) != CURLE_OK) { /* extract the fragment from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, "../another/second.html", 0)) != CURLE_OK) { /* redirect with a relative URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path2, 0)) != CURLE_OK) { /* extract the new, updated path */
      printf("Error () ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    curl_free(host);
    curl_free(path);
    curl_free(path2);
    curl_url_cleanup(h); /* free url handle */
  }

  
  if (RUNTEST) {
    /* TEST:
     */
    CURLU *h;
    CURLUcode uc;
    char *host = NULL;
    char *path = NULL;
    char *query = NULL;
    char *fragment = NULL;

    i++;
    printf("TEST %i in '%s:%i': ",i,__FILE__,__LINE__);

    if ((h = curl_url()) == NULL) { /* get a handle to work with */
      printf("Error curl_url()\n");
    }
    else if ((uc = curl_url_set(h, CURLUPART_URL, HTTPPREFIX "demo/test.db3?query=SELECT%20*%20FROM%20directory", 0)) != CURLE_OK) { /* parse a full URL */
      printf("Error curl_url_set() ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_HOST, &host, 0)) != CURLE_OK) { /* no host name from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_PATH, &path, 0)) != CURLE_OK) { /* extract the path from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_QUERY, &query, 0)) != CURLE_OK) { /* extract the query from the parsed URL */
      printf("Error () ...\n");
    }
    else if ((uc = curl_url_get(h, CURLUPART_FRAGMENT, &fragment, 0)) != CURLUE_NO_FRAGMENT) { /* extract the fragment from the parsed URL */
      printf("Error () ...\n");
    }
    else {
      n_ok++;
      printf("OK\n");
    }
    
    curl_free(host);
    curl_free(path);
    curl_url_cleanup(h); /* free url handle */
  }

#endif

  return (i - n_ok);
}
/* end of curlTest() */
