/*
 * server.c
 *
 *  Created on: 28.02.2013
 *      Author: polina
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
int main(){

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket server");
		exit(1);
	}
	unlink("server");
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "server");

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind server");
		exit(2);
	}

	listen(sock, 10);

	char buf[1024];

	while(1){
		struct sockaddr_un caddr;
		int clen;
		int clientSocket= accept(sock, (struct sockaddr *)& caddr, &clen);
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
