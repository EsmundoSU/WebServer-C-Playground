#include <stdbool.h>
#include "include/server_errors.h"
#include "include/server_defaults.h"
#include "include/server_options.h"

#ifndef WEBSERVER_C_PLAYGROUND_SERVER_H
#define WEBSERVER_C_PLAYGROUND_SERVER_H

//! Windows default version of socket.
#define DEFAULT_WINDOWS_SOCKET_VERSION MAKEWORD(2,2)

//! Information about configuration of this server.
//! @details Use ServerConfigurationDefault as base struct.
struct ServerConfiguration
{
	//! Listening port. (ex. "8080")
	const char *pPort;
	//! Path for index.html
	const char *indexPath;
};
typedef struct ServerConfiguration ServerConfiguration;
extern ServerConfiguration ServerConfigurationDefault;

//! @brief Starts HTTP Server
//! @return OK - Successful, #ServerError if not.
int ServerStart();

int ServerRun();

#endif //WEBSERVER_C_PLAYGROUND_SERVER_H
