/*
 * server.c
 *
 *  Created on: 28.02.2013
 *      Author: polina
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define OPEN_MAX 1024
int fd[OPEN_MAX];

int main() {

	fd_set master;

	/* temp file descriptor list for select() */
	fd_set read_fds;
	fd_set write_fds;

	struct sockaddr_in serv_addr, cli_addr;
	int sockfd, newsockfd, fdmax, i;   /* int fdmax is the maximum file descriptor number */
	socklen_t clilen;

	char buf[1024];

	/* clear the master and temp sets */

	FD_ZERO(&master);

	FD_ZERO(&read_fds);

	FD_ZERO(&write_fds);


	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket");

	/* bind */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(1234);

	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		perror("bind");


	listen(sockfd, 10);
	clilen = sizeof(cli_addr);

	/* add the sockfd to the master set */
	FD_SET(sockfd, &master);

	/* keep track of the biggest file descriptor */
	fdmax = sockfd; /* so far, it's this one*/


	/*Main loop */
	for( ; ; ) {

	/* copy it */
		read_fds = master;
		write_fds = master;
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if(select(fdmax+1, &read_fds, &write_fds, NULL, NULL) < 0)
			perror("select");

		/*run through the existing connections looking for data to be read*/
		if(fdmax >= OPEN_MAX){
			perror("open_max");
			return 1;
		}
		for(i = 0; i <= fdmax; i++) {
			if(FD_ISSET(i, &read_fds)) {  /* we got one... */
				printf("Server %d: read\n", i);
					if(i == sockfd) {

						if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
							perror("accept");

						FD_SET(newsockfd, &master); /* add to master set */
						printf("Server %d: accept new client %d\n", i, newsockfd);
						/* keep track of the maximum */
						fd[newsockfd] = -1;
						if(newsockfd > fdmax) {
							fdmax = newsockfd;
						}
					}
					else {
						/* handle data from a client */
						memset(buf, 0, sizeof buf);
						int size;
						if((size = recv(i, buf, 1024, 0)) <= 0) {
							continue;
						}
						else {
							/* we got some data from a client*/
							printf("Server %d: path %s\n", i, buf);
							fd[i] = open(buf, O_RDONLY);
							printf("Server %d: file desc %d\n", i, fd[i]);
							if(fd[i] < 0){
								memset(buf, 0, sizeof buf);
								memcpy(buf,"file not exist", 14);
								close(fd[i]);
								if(send(i, buf, 1024, 0) < 0)
									perror("send");
								printf("Server %d: %s\n", i, buf);
								close(i);
								FD_CLR(i, &master);
								FD_CLR(i, &read_fds);
								FD_CLR(i, &write_fds);
								continue;
							}
						}
					}
			}
			if(FD_ISSET(i, &write_fds) && fd[i] >= 0){
				printf("Server %d: write\n", i);

				memset(buf, 0, sizeof buf);
				int size = read(fd[i], buf, 1024);

				if(size == 0){
					close(fd[i]);
					printf("Server %d: sent file\n", i);
					close(i);
					FD_CLR(i, &master);
					FD_CLR(i, &read_fds);
					FD_CLR(i, &write_fds);
					continue;
				}
				printf("Server %d: send %d\n", i, size);
				if(send(i, buf, 1024, 0) < 0)
					perror("send");

			}
		}
	}
  return 0;
}
