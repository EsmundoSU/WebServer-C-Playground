#include <winsock2.h>
#include "server.h"
#include "logger.h"

int main(int argc, char *argv[])
{
	log_debug("Launch loc:\t%s", argv[0]);
	// Set global app logger.
	log_enable(LOG_ENABLE);
	log_set_level(LOG_DEBUG);

	//! Initialize server
	if (ServerInitialization() != 0)
	{
		exit(EXIT_FAILURE);
	}

	// Create server socket && starts listening.
	if (ServerStart() != 0)
	{
		exit(EXIT_FAILURE);
	}

	ServerRun();
}
