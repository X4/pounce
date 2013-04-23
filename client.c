#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "mtime.h"
#include "clients.h"
#include "client.h"

inline void client_update(client_t *client)
{
  char buffer[CLIENT_RCVBUF];
  ssize_t size;
  struct linger l;
  int e, n;
  
  switch(client->state)
    {
    case CLIENT_READY:
      client->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (client->fd == -1)
	err(1, "socket");

      e = fcntl(client->fd, F_SETFL, O_NONBLOCK);
      if (e == -1)
	err(1, "fcntl");

      n = 1;
      e = setsockopt(client->fd, IPPROTO_TCP, TCP_QUICKACK, (char *) &n, sizeof n);
      if (e == -1)
        err(1, "setsockopt TCP_QUICKACK");

      n = 1;
      e = setsockopt(client->fd, IPPROTO_TCP, TCP_NODELAY, (char *) &n, sizeof n);
      if (e == -1)
	err(1, "setsockopt NODELAY");

      n = CLIENT_RCVBUF;
      e = setsockopt(client->fd, SOL_SOCKET,  SO_RCVBUF, (char *) &n, sizeof n);
      if (e == -1)
        err(1, "setsockopt SO_RCVBUF");

      l.l_onoff = 1;
      l.l_linger = 0;
      e = setsockopt(client->fd, SOL_SOCKET, SO_LINGER, &l, sizeof l);
      if (e == -1)
	err(1, "setsockopt SO_LINGER");

      client->state = CLIENT_CONNECTING;
      e = connect(client->fd, (struct sockaddr *) &client->clients->server_addr, sizeof client->clients->server_addr);
      if (e == -1 && errno != EINPROGRESS)
	err(1, "connect");

      if (errno == EINPROGRESS)
	{
	  client->ev.events = EPOLLOUT | EPOLLET;
	  client->ev.data.ptr = client;
	  e = epoll_ctl(client->clients->epollfd, EPOLL_CTL_ADD, client->fd, &client->ev);
	  if (e == -1)
	    err(1, "epoll_ctl add");
	  break;
	}
      /* fall through */
    case CLIENT_CONNECTING:
      n = client->clients->request_buffer_length;
      size = send(client->fd, client->clients->request_buffer, n, 0);
      if (size != n)
	err(1, "send");
      
      client->ev.events = EPOLLIN | EPOLLET;
      e = epoll_ctl(client->clients->epollfd, EPOLL_CTL_MOD, client->fd, &client->ev);
      if (e == -1)
	err(1, "epoll_ctl mod");
      
      client->state = CLIENT_REQUESTING;
      break;
    case CLIENT_REQUESTING:
      size = recv(client->fd, buffer, sizeof buffer, 0);
      if (size == -1)
	err(1, "recv");     
      if (size > 0)
	{
	  client->ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	  e = epoll_ctl(client->clients->epollfd, EPOLL_CTL_MOD, client->fd, &client->ev);
	  if (e == -1)
	    err(1, "epoll_ctl mod");

	  client->clients->received_total += size;
	}
      else
	{
	  e = epoll_ctl(client->clients->epollfd, EPOLL_CTL_DEL, client->fd, NULL);
	  if (e == -1)
	    err(1, "epoll_ctl del");
	  e = close(client->fd);
	  if (e == -1)
	    err(1, "close");

	  client->state = CLIENT_READY;
	  client->clients->nrequests_done ++;
	}
      break;
    }
}
