#ifndef _CLIENTS_H
#define _CLIENTS_H

#define CLIENTS_MAX_EVENTS          128
#define CLIENTS_REQUEST_BUFFER_SIZE 4096

typedef struct clients clients_t;
struct clients
{
  int                 nrequests;
  int                 nrequests_started;
  int                 nrequests_done;
  int                 nclients;
  int                 epollfd;
  struct sockaddr_in  server_addr;
  char                request_buffer[CLIENTS_REQUEST_BUFFER_SIZE];
  size_t              request_buffer_length;
  size_t              received_total;
  mtime_t             time_started;
  mtime_t             time_done;
  struct client      *client;
};

int  clients_init(clients_t *, int, int, u_int32_t, u_int16_t, char *);
void clients_update(clients_t *);
int  clients_loop(clients_t *);

#endif /* _CLIENTS_H */
