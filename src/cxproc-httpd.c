/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>

#include <errno.h>

#include <iconv.h>

#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>

#define PORT 8888

/*! https://www.gnu.org/software/libmicrohttpd/tutorial.html#Exploring-requests
*/
int
print_out_key(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
  printf("%s: %s\n", key, value);
  return MHD_YES;
}

static int
answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
		     size_t *upload_data_size, void **con_cls)
{
  // const char *page = "<html><body>Hello, browser!</body></html>";
  struct MHD_Response *response;
  int ret;

  //response = MHD_create_response_from_buffer (strlen (page), (void *) page, MHD_RESPMEM_PERSISTENT);
  response = MHD_create_response_from_buffer (strlen (url), (void *) url, MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  //ret = MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_out_key, NULL);

  fputs(url, stderr);
  fputs(" OK\n", stderr);

  return ret;
}


int
main()
{
  struct MHD_Daemon *daemon;

  daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
  if (NULL == daemon) {
    return 1;
  }

  (void)getchar();

  MHD_stop_daemon(daemon);
  return 0;
}
