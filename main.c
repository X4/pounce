#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "mtime.h"
#include "options.h"
#include "clients.h"

int main(int argc, char **argv)
{
  options_t options;
  clients_t clients;
  int e;
  
  e = options_parse(&options, argc, argv);
  if (e == -1)
    options_usage();

  e = clients_init(&clients, options.nrequests, options.nclients, options.ip, options.port, options.path);
  if (e == -1)
    err(1, "clients_init");
  
  e = clients_loop(&clients);
  if (e == -1)
    err(1, "clients_loop");
  
  (void) fprintf(stderr, "%lld us, %lld rps, %ld kB, %lld kB/s\n", 
		 clients.time_done - clients.time_started,
		 (1000000 * (mtime_t) options.nrequests) / (clients.time_done - clients.time_started),
		 clients.received_total / 1000,
		 (1000 * (mtime_t) clients.received_total / (clients.time_done - clients.time_started)));
  
  exit(EXIT_SUCCESS);
}
