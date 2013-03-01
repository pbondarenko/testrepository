/*
 * client.c
 *
 *  Created on: 28.02.2013
 *      Author: polina
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <getopt.h>
int main(int argc, char **argv){
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket client");
		exit(1);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(1234);
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect client");
        exit(2);
    }

    char message[1024];
    scanf("%s", message);
    printf("Client: send message to Server: %s\n", message);
    send(sock, message, sizeof(message), 0);
    char buf[1024];

    recv(sock, buf, 1024, 0);
    printf("Client: get Servers message : %s\n", buf);
    close(sock);

	return 0;
}
