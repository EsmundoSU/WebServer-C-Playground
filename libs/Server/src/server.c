#include <winsock2.h>
#include <stdio.h>
#include "server.h"
#include "logger.h"
#include "socket.h"

//! Client socket handle.
static SOCKET clientSocketHandle;
//! Server socket handle.
static SOCKET serverSocketHandle;

int ServerInitialization()
{
	clientSocketHandle = INVALID_SOCKET;
	serverSocketHandle = INVALID_SOCKET;

	return SocketInitialize();
}

int ServerStart()
{
	return SocketServerStart(&serverSocketHandle);
}

void ServerRun()
{
	const char header[] = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-1 \r\n\r\n";
	const int headerSize = sizeof(header) / sizeof(header[0]);

	while (TRUE)
	{
		if (SocketWaitForConnection(&serverSocketHandle, &clientSocketHandle) != 0)
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
		recv(clientSocketHandle, recvBuffer, sizeof(recvBuffer), 0);
		printf("%s\n", recvBuffer);

		// Send Headers
		if (SocketSend(&clientSocketHandle, header, headerSize) != 0)
		{
			exit(EXIT_FAILURE);
		}

		// Send file data.
		if (SocketSend(&clientSocketHandle, fileData, fileSize) != 0)
		{
			exit(EXIT_FAILURE);
		}

		// Close connection with client/
		if (SocketClose(&clientSocketHandle) != 0)
		{
			exit(EXIT_FAILURE);
		}
	}
}
