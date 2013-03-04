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
char buff[10];
char * itoa(int i){

	memset(buff, 0, sizeof(buff));
	int j = 0;
	do{
		buff[j++] = i%10 + '0';
		i/=10;
	}while(i > 0);

	for(i = 0 ;i < j/2;i++){

		char c = buff[i];
		buff[i] = buff[j-i-1];
		buff[j-i-1] = c;
	}

	return buff;
}
void function(void * ptr){
	int clientSocket = (int)ptr;

	char path[1024];
	recv(clientSocket, path, 1024, 0);
	printf("%d Server: get file path: %s\n", clientSocket, path);
	int fd;
	usleep(clientSocket * 1000);
	if((fd = fopen(path, "rb")) > 0){

		printf("%d Server: desc %d\n", clientSocket, fd);
		printf("%d Server: file exists\n", clientSocket);
		int i;

		char buf[1024];
		i = 0;
		while(!feof(fd)){
			int j;
			if(i % 1024 == 0){
				if(i != 0){
					send(clientSocket, buf, 1024, 0);
					puts("send");

				}else
					printf("%d Server: start reading\n", clientSocket);
				for(j = 0; j < 1024; j++)
					buf[j] = 10;


			}
			fscanf(fd, "%c", &buf[i%1024]);
			i++;
		}
		printf("%d Server: for end\n", clientSocket);
		send(clientSocket, buf, 1024, 0);

		printf("%d Server: send file\n", clientSocket);

		fclose(fd);
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
