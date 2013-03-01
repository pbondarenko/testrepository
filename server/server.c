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

	listen(sock, 1);

	char buf[1024];

	while(1){

		int clientSocket= accept(sock, NULL, NULL);
		if(clientSocket < 0){
			perror("accept server");
			exit(3);
		}
		int read = recv(clientSocket, buf, 1024, 0);
		printf("Server: get Clients message: %s\n", buf);
		if(read <= 0) continue;
		printf("Server: send message to Client: %s\n", buf);
		send(clientSocket, buf, strlen(buf), 0);
		close(clientSocket);
	}
	close(socket);
	return 0;
}
