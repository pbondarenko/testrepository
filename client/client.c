/*
 * client.c
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
		perror("socket client");
		exit(1);
	}

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "server");
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect client");
        exit(2);
    }

    char message[1024];
    scanf("%s\n", message);
    printf("Client: send message to Server: %s\n", message);
    send(sock, message, sizeof(message), 0);
    char buf[1024];

    recv(sock, buf, 1024, 0);
    printf("Client: get Servers message : %s\n", buf);
    close(sock);

	return 0;
}
