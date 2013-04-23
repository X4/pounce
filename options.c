#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "options.h"

void options_usage()
{
  extern char *__progname;

  (void) fprintf(stderr, "usage: %s [-n nrequests] [-c nclients] http://<ip>[:<port>]/<path>\n", __progname);
  exit(EXIT_FAILURE);
}

int options_parse(options_t *options, int argc, char **argv)
{
  char *uri, hostname[256];
  int port, opt, n, e;

  port = 80;
  options->nrequests = 10000;
  options->nclients = 100;

  while (1)
    { 
      opt = getopt(argc, argv , "n:c:h");
      if (opt == -1)
	break;
      switch(opt)
	{
	case 'n': 
	  options->nrequests = strtol(optarg, NULL, 0);
	  break;
	case 'c':
	  options->nclients = strtol(optarg, NULL, 0);
	  break;
	case 'h':
	default: 
	  return -1;
	}
    }
  
  argc -= optind;
  argv += optind;
  if (argc != 1)
    return -1;

  uri = argv[0];
  n = sscanf(uri, "http://%255[^:]:%d%4095s", hostname, &port, options->path) - 3;
  if (n)
    n = sscanf(uri, "http://%255[^/]%4095s", hostname, options->path) - 2;
  if (n)
    return -1;
  
  options->port = htons(port);
  e = inet_pton(AF_INET, hostname, (void *) &options->ip);
  if (e != 1)
    return -1;

  return 0;
}
