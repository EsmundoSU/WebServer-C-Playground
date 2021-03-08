#include <winsock2.h>
#include <stdio.h>
#include "server.h"
#include "logger.h"
#include "socket.h"
#include "http.h"
#include "include/server_configuration.h"

//! Server socket handle.
static SOCKET serverSocketHandle;
//! Client socket handle.
static SOCKET clientSocketHandle;
//! Receive buffer for client socket data.
static char clientSocketReceiveBuffer[SERVER_RECEIVE_BUFFER_SIZE];

//! @brief Reads data from client socket and tries to parse it.
//! @param parsedData Parsed data from client connection
static int ParseReceivedData();

int ServerInitialization()
{
	clientSocketHandle = INVALID_SOCKET;
	serverSocketHandle = INVALID_SOCKET;
	ZeroMemory(clientSocketReceiveBuffer, sizeof(clientSocketReceiveBuffer));


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

		(void)ParseReceivedData(NULL);

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

		// Clear receive data.
		ZeroMemory(clientSocketReceiveBuffer, sizeof(clientSocketReceiveBuffer));
	}
}

static int ParseReceivedData()
{
	recv(clientSocketHandle, clientSocketReceiveBuffer, sizeof(clientSocketReceiveBuffer), 0);
	printf("%s\n", clientSocketReceiveBuffer);

	//! @todo Implement parsing received data to HttpMessage.
	//if(strstr(clientSocketReceiveBuffer, HTTP_GET);

	return 0;
}
