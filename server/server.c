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
	printf("Server: get file path: %s\n", path);
	int fd;

	if(fd = fopen(path, "rb")){
		printf("Server: file exists\n");
		int size, i;

		fseek(fd, 0, SEEK_END);
		size = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		printf("Server: file length: %d\n", size);
		char buf[1024];
		for(i = 0; i < size; i++){
			int j;
			if(i % 1024 == 0){
				if(i != 0){
					send(clientSocket, buf, 1024, 0);
					puts("send");

				}
				for(j = 0; j < 1024; j++)
					buf[j] = 10;


			}
			printf("%d\n", i);
			fscanf(fd, "%c", &buf[i%1024]);
		}

		send(clientSocket, buf, 1024, 0);

		printf("Server: send file");

		fclose(fd);
	}else{

		printf("Server: file not exist\n");
		char message[1024];
		strcpy(message,"file not exist");
		printf("Server: message: %s\n", message);


		send(clientSocket, message, sizeof(message), 0);
	}
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
