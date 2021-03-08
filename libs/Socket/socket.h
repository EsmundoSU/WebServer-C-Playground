#ifndef WEBSERVER_C_PLAYGROUND_SOCKET_H
#define WEBSERVER_C_PLAYGROUND_SOCKET_H
#include <winsock.h>

enum TransportProtocol
{
	TCP,
	UDP,
};
typedef enum TransportProtocol TransportProtocol;

struct Socket
{
	const char* port;
	int ipVersion;
	TransportProtocol transportProtocol;
	void* winSocketAddressInformation;
	void* socket;
};

typedef struct Socket Socket;

//! @brief Gets socket with default values.
//! @return Socket Handle.
Socket SocketGetDefaultHandle(void);

int SocketStart(SOCKET *hSocket);

#endif //WEBSERVER_C_PLAYGROUND_SOCKET_H
