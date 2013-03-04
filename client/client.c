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
	char path[1024];
	static struct option long_options[] =
	{
			{"get", required_argument, 0, 'get'},

    };

   int option_index = 0;

   int c = getopt_long (argc, argv, "get", long_options, &option_index);

   switch (c){
	   case 'get':
		   strcpy(path, optarg);
		   break;
	   default:
		   printf("Client: haven't query");
		   return 0;

   }

   printf("Client: want file %s\n", path);


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


    printf("Client: want file from Server: %s\n", path);
    send(sock, path, sizeof(path), 0);
    char name[1024];
    int i = strlen(path)-1;
    while(i > 0 && path[i] != '/') i--;
    i++;
    int j;
    for(j = i; j < strlen(path); j++)
    	name[j-i] = path[j];
    printf("Client: file name %s\n", name);

    int was = 0;
    char buf[1024];
    int fd;
   while(recv(sock, buf, 1024, 0) > 0){
    	 if(!was && strcmp(buf, "file not exist") == 0){
			printf("Client: file not exist\n");
			close(sock);
			return 0;
		}

    	fd = open(name, 0);

    	write(fd, buf, 1024);

    }
	printf("Client: wrote file\n");
    close(fd);

    printf("Client: ok\n");
    close(sock);

	return 0;
}
