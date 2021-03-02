#include "include/server_errors.h"
#include "include/server_defaults.h"

#ifndef WEBSERVER_C_PLAYGROUND_SERVER_H
#define WEBSERVER_C_PLAYGROUND_SERVER_H

//! Windows default version of socket.
#define DEFAULT_WINDOWS_SOCKET_VERSION MAKEWORD(2,2)

//! Information about configuration of this server.
//! @details Use ServerConfigurationDefault as base struct.
struct ServerConfiguration
{
	//! Listening port.
	const char *pPort;
};
typedef struct ServerConfiguration ServerConfiguration;
extern ServerConfiguration ServerConfigurationDefault;
//! @brief Starts HTTP Server
//! @return OK - Successful, #ServerError if not.
ServerError StartServer(ServerConfiguration serverConfiguration);

int ReceiveData(void* buffer, size_t length);

#endif //WEBSERVER_C_PLAYGROUND_SERVER_H
