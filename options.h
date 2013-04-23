#ifndef _OPTIONS_H
#define _OPTIONS_H

typedef struct options options_t;
struct options
{
  int       nrequests;
  int       nclients;
  u_int32_t ip;
  u_int16_t port;
  char      path[4096];
};

void options_usage();
int  options_parse(options_t *, int, char **);

#endif /* _OPTIONS_H */
