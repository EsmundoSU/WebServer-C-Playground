#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include "server.h"
#include "configuration.h"
#include "logger.h"

int main()
{
	// Set global app logger.
	log_enable(LOG_ENABLE);
	log_set_level(LOG_DEBUG);

	int response; // Buffer for functions responses

	log_info("WebServer is starting...");

	ServerConfiguration serverConfiguration = ServerConfigurationDefault;
	response = StartServer(serverConfiguration);

	if(response != 0)
	{
		return 1;
	}

	char recvbuf[DEFAULT_BUFFER_LENGTH];
	int recvbuflen = DEFAULT_BUFFER_LENGTH;

	ReceiveData(recvbuf, recvbuflen);

	return 0;
}
