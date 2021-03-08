#include <winsock2.h>
#include <stdio.h>
#include "socket.h"
#include "logger.h"

int main()
{
	// Set global app logger.
	log_enable(LOG_ENABLE);
	log_set_level(LOG_DEBUG);

	// Initialize socket API.
	SocketInitialize();

	// Client socket.
	SOCKET clientSocket = INVALID_SOCKET;
	// Server socket
	SOCKET serverSocket = INVALID_SOCKET;

	// Create server socket && starts listening.
	if (SocketServerStart(&serverSocket) != 0)
	{
		exit(EXIT_FAILURE);
	}

	const char header[] = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-1 \r\n\r\n";
	const int headerSize = sizeof(header) / sizeof(header[0]);

	while (TRUE)
	{
		if (SocketWaitForConnection(&serverSocket, &clientSocket) != 0)
		{
			exit(EXIT_FAILURE);
		}

		FILE *pFile;
		pFile = fopen("index.html", "r");
		if (pFile == NULL)
		{
			log_fatal("Could not find \"index.html\"");
			exit(EXIT_FAILURE);
		}
		fseek(pFile, 0, SEEK_END);
		long fileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);  /* same as rewind(pFile); */

		char *fileData = malloc(fileSize);
		fread(fileData, 1, fileSize, pFile);
		fclose(pFile);

		char recvBuffer[512];
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		printf("%s\n", recvBuffer);

		// Send Headers
		if (SocketSend(&clientSocket, header, headerSize) != 0)
		{
			exit(EXIT_FAILURE);
		}

		// Send file data.
		if (SocketSend(&clientSocket, fileData, fileSize) != 0)
		{
			exit(EXIT_FAILURE);
		}

		// Close connection with client/
		if (SocketClose(&clientSocket) != 0)
		{
			exit(EXIT_FAILURE);
		}
	}
}
