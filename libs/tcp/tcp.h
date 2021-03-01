// Created by Adam Pelc on 22/02/2021.

#ifndef WEBSERVER_C_PLAYGROUND_TCP_H
#define WEBSERVER_C_PLAYGROUND_TCP_H

typedef struct
{
	int id;
}SocketHandle_t;

struct sockaddr {
	unsigned short sa_family; /* Address family (e.g. AF_INET) */
	char sa_data[14]; /* Family-specific address information */
};

struct in_addr {
	unsigned long s_addr; /* Internet address (32 bits) */
};

struct sockaddr_in {
	unsigned short sin_family; /* Internet protocol (AF_INET) */
	unsigned short sin_port; /* Address port (16 bits) */
	struct in_addr sin_addr; /* Internet address (32 bits) */
	char sin_zero[8]; /* Not used */
};


typedef enum
{
	TCP,
	UDP,
}Protocol;

SocketHandle_t socket(Protocol protocol);

#endif //WEBSERVER_C_PLAYGROUND_TCP_H