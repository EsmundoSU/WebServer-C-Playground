#include <stdio.h>
#include "logger.h"

int main(int argc, char *argv[])
{
	// Sets logger config
	log_set_level(LOG_TRACE);
	log_enable(LOG_ENABLE);

	log_info("%s", "Server is launching...");
	if (argc == 1)
	{
		//! @todo Implement help function
		printf("No argument passed.\n");
		printf("Launch Help Menu\n");
	}
	else
	{
		for (int argIndex = 1; argIndex < argc; argIndex+=1)
		{
			printf("\narg[%d]: %s", argIndex, argv[argIndex]);
		}
	}
	return 0;
}
