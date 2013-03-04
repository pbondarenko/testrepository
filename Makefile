all:

CFLAGS+=-MMD -MF $@.d -MP

-include $(shell find ./exe -name '*.d')

exe/client: client/client.c
		$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
		
exe/server: LDFLAGS+=-lpthread
exe/server: server/server.c
		$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	
all: exe/client exe/server

.PHONY: all
