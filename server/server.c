/*
 * server.c
 *
 *  Created on: 28.02.2013
 *      Author: polina
 */


#include <event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void function_write( struct bufferevent *buf_ev, void * arg);
static void echo_event_cb( struct bufferevent *buf_ev, short events, void *arg );

/* Функция обратного вызова для события: данные готовы для чтения в buf_ev */
static void function_read( struct bufferevent *buf_ev, void *arg )
{

	char path[1024];
	bufferevent_read(buf_ev, path, 1024);
	printf("Server: get file path: %s\n", path);
	int fd = open(path, O_RDONLY);

	if(fd > 0) {
		printf("Server: file desc %d\n", fd);
		bufferevent_enable( buf_ev, EV_WRITE);

		bufferevent_setcb(buf_ev, NULL, function_write, echo_event_cb, (void *)fd);

	} else {
		arg = NULL;
		printf("Server: file not exist\n");

		char message[1024];
		strcpy(message,"file not exist");
		printf("Server: message: %s\n", message);

		bufferevent_write(buf_ev, message, 1024);
		bufferevent_disable(buf_ev, EV_READ);


	}

}
static void function_write( struct bufferevent *buf_ev, void * arg){


	int fd = (int)arg;
	printf("Server writer: file desc %d\n", fd);
	if(fd > 0){
		char buf[1024];
		int size;

		memset(buf, 0, sizeof buf);
		size = read(fd, buf, 1024);
		printf("Server: size of buffer %d\n", size);
		if(size > 0){
			bufferevent_write(buf_ev, buf, size);
			bufferevent_setcb(buf_ev, NULL, function_write, echo_event_cb, (void*)fd);
		}else{
			close(fd);
			printf("Server: sent file\n");
			bufferevent_free(buf_ev);
		}


	}else{

		bufferevent_free(buf_ev);
	}

}
static void echo_event_cb( struct bufferevent *buf_ev, short events, void *arg )
{
	if( events & BEV_EVENT_ERROR )
	    perror( "error bufferevent" );
  if( events & (BEV_EVENT_EOF | BEV_EVENT_ERROR) ){
    bufferevent_free( buf_ev );
    printf("Server: im free 2\n");
  }
}
static void accept_error_cb( struct evconnlistener *listener, void *arg )
{
  struct event_base *base = evconnlistener_get_base( listener );
  int error = EVUTIL_SOCKET_ERROR();
  fprintf( stderr, "Ошибка %d (%s) в мониторе соединений. Завершение работы.\n",
           error, evutil_socket_error_to_string( error ) );
  event_base_loopexit( base, NULL );
}
static void accept_connection_cb( struct evconnlistener *listener,
                   evutil_socket_t fd, struct sockaddr *addr, int sock_len,
                   void *arg )
{
  /* При обработке запроса нового соединения необходимо создать для него
     объект bufferevent */
  struct event_base *base = evconnlistener_get_base(listener);
  struct bufferevent *buf_ev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb( buf_ev, function_read, function_write, echo_event_cb, NULL);
  bufferevent_enable( buf_ev, EV_READ);
}


int main(){

  struct event_base *base;
  struct evconnlistener *listener;
  struct sockaddr_in sin;

  base = event_base_new();
  if(!base){
    perror("event base" );
    return 1;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl( INADDR_ANY );
  sin.sin_port = htons(1234);
  void * arg = NULL;
  listener = evconnlistener_new_bind(base, accept_connection_cb, arg,
                                     (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE),
                                     -1, (struct sockaddr *)&sin, sizeof(sin));
  if(!listener){
    perror("listener");
    return -1;
  }

  event_base_dispatch(base);
  return 0;
}
