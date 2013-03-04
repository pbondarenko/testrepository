/*
 * server.c
 *
 *  Created on: 28.02.2013
 *      Author: polina
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
//#include <event2/event.h>

void function(void * ptr){
	int clientSocket = (int)ptr;

	char path[1024];
	recv(clientSocket, path, 1024, 0);
	printf("%d Server: get file path: %s\n", clientSocket, path);
	int fd;
	usleep(clientSocket * 1000);
	if((fd = open(path, 0)) > 0){

		printf("%d Server: desc %d\n", clientSocket, fd);
		printf("%d Server: file exists\n", clientSocket);
		int i;

		char buf[1024];
		int size;
		while((size = read(fd, buf, 1024)) > 0){
			send(clientSocket, buf, size, 0);
		}

		printf("%d Server: sent file\n", clientSocket);

		close(fd);
	}else{

		printf("%d Server: file not exist\n", clientSocket);

		char message[1024];
		strcpy(message,"file not exist");
		printf("%d Server: message: %s\n", clientSocket, message);


		send(clientSocket, message, sizeof(message), 0);
	}
	printf("%d Server: thread exit\n", clientSocket);
	close(clientSocket);
	pthread_exit(0);
}
int main(){
	/*
	struct event_base *base;
	struct evconnlistener *listener;
	struct sockaddr_in sin;

	base = event_base_new();
	memset( &sin, 0, sizeof(sin) );

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(1234);

	listener = evconnlistener_new_bind(base, accept_connection_cb, NULL, (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE), -1, (struct sockaddr *)&sin, sizeof(sin));
	if(!listener)
	{
		perror( "Create listener" );
		return 1;
	}

	event_base_dispatch(base);
	return 0;
*/
	//libevent
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket server");
		exit(1);
	}
	unlink("/tmp/socket");
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(1234);

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind server");
		exit(2);
	}

	listen(sock, 10);
	pthread_t threads;

	int idxs = 0;
	while(1){

		int clientSocket= accept(sock, NULL, NULL);

		if(clientSocket < 0){
			perror("accept server");
			exit(3);
		}


	    pthread_create(&threads, NULL, &function,(void *) clientSocket);
	    pthread_detach(threads);
	    idxs++;


	}

	close(socket);
	return 0;
}
