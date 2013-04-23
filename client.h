#ifndef _CLIENT_H
#define _CLIENT_H

#define CLIENT_RCVBUF 1024*1024

enum client_state 
{
  CLIENT_READY,
  CLIENT_CONNECTING,
  CLIENT_REQUESTING
};

typedef struct client client_t;
struct client
{
  int                 fd;
  enum client_state   state;
  struct epoll_event  ev;
  clients_t          *clients;
};

void client_update(client_t *);

#endif /* _CLIENT_H */
