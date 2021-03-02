#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "configuration.h"
#include "logger.h"

int main()
{
	// Set global app logger.
	log_enable(LOG_ENABLE);
	log_set_level(LOG_DEBUG);

	// Starts server and listening
	StartServer(serverConfiguration);



	return 0;
}
