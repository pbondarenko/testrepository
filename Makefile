all:
		gcc client/client.c -o exe/client
		gcc server/server.c -o exe/server -lpthread
