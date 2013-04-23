#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "mtime.h"
#include "clients.h"
#include "client.h"

int clients_init(clients_t *clients, int nrequests, int nclients, u_int32_t ip, u_int16_t port, char *path)
{
  int n, i;

  bzero(clients, sizeof *clients);
  clients->nrequests = nrequests;
  clients->nrequests_started = 0;
  clients->nrequests_done = 0;
  clients->nclients = nclients;
  clients->received_total = 0;
  
  n = snprintf(clients->request_buffer, sizeof clients->request_buffer,
	       "GET %s HTTP/1.0\r\n" 
	       "Connection: close\r\n"
	       "\r\n",
	       path);
  if (n == -1 || n >= sizeof clients->request_buffer)
    return -1;
  clients->request_buffer_length = n;
  
  bzero(&clients->server_addr, sizeof clients->server_addr);
  clients->server_addr.sin_family = AF_INET;
  clients->server_addr.sin_addr.s_addr = ip;
  clients->server_addr.sin_port = port;
      
  clients->client = calloc(clients->nclients, sizeof *clients->client);
  if (!clients->client)
    return -1;  
  
  for (i = 0; i < clients->nclients; i++)
    {
      clients->client[i].state = CLIENT_READY;
      clients->client[i].clients = clients;
    }
  
  clients->epollfd = epoll_create(1024);
  if (clients->epollfd == -1)
    return -1;

  clients->time_done = 0;
  clients->time_started = mtime();
  clients_update(clients);
  
  return 0;
}

int clients_loop(clients_t *clients)
{
  struct epoll_event events[CLIENTS_MAX_EVENTS];
  int n = -1, i;

  while (clients->nrequests_done < clients->nrequests)
    {
      n = epoll_wait(clients->epollfd, events, CLIENTS_MAX_EVENTS, -1);
      if (n <= 0)
	break;

      for (i = 0; i < n; i ++)
	client_update(events[i].data.ptr);
      
      clients_update(clients);
    }

  clients->time_done = mtime();
  return n;
}

void clients_update(clients_t *clients)
{
  int i;
  
  for (i = 0; i < clients->nclients && clients->nrequests_started < clients->nrequests; i++)
    if (clients->client[i].state == CLIENT_READY)
      {
	client_update(&clients->client[i]);
	clients->nrequests_started ++;
      }
}
