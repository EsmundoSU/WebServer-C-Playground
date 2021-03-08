#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "socket.h"
#include "logger.h"

int SocketListen(const SOCKET *hSocket)
{
	if (listen(*hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		log_fatal("\"listen\" failed with error code:\t%d", error);
		closesocket(*hSocket);
		WSACleanup();
		return error;
	}
	log_debug("listen - successful");

	return 0;
}

int SocketWaitForConnection(const SOCKET *hListeningSocket, SOCKET *hClientSocket)
{
	*hClientSocket = accept(*hListeningSocket, NULL, NULL);
	if (*hClientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		log_fatal("\"accept\" failed with error code:\t%d", errorCode);
		closesocket(*hListeningSocket);
		WSACleanup();
		return errorCode;
	}

	log_debug("accept- successful");
	return 0;
}

int SocketSend(const SOCKET *hTargetSocket, const char* sendBuffer, int sendBufferLength)
{
	int response = send(*hTargetSocket, sendBuffer, sendBufferLength, 0);
	if (response == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		log_fatal("send failed with error code:\t%d", errorCode);
		closesocket(*hTargetSocket);
		WSACleanup();
		return errorCode;
	}
	log_debug("bytes to send:\t%d", sendBufferLength);
	log_debug("bytes send:\t%d", response);

	return 0;
}

int SocketDisconnect(const SOCKET *hTargetSocket)
{
	// Disconnects client socket from server
	int response = shutdown(*hTargetSocket, SD_SEND);
	if (response == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		log_fatal("shutdown failed with error code:\t%d", errorCode);
		closesocket(*hTargetSocket);
		WSACleanup();
		return errorCode;
	}
	log_debug("Client socket disconnected");

	return 0;
}

int main()
{
	// Set global app logger.
	log_enable(LOG_ENABLE);
	log_set_level(LOG_DEBUG);

//	const char sendBuffer[] = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-1 \r\n\r\n"
//	                          "<h1>HelloWorld!</h1><p>Adam Pelc2</p>";
	//int sendBufferSize = sizeof(sendBuffer) / sizeof(sendBuffer[0]);

	SOCKET clientSocket = INVALID_SOCKET;
	//! Server socket
	SOCKET serverSocket = INVALID_SOCKET;

	// Initialize Socket API && Create ne server socket.
	if (SocketStart(&serverSocket) != 0)
	{
		exit(EXIT_FAILURE);
	}

	// Listen on socket
	if (SocketListen(&serverSocket) != 0)
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

		if (SocketDisconnect(&clientSocket) != 0)
		{
			exit(EXIT_FAILURE);
		}
	}

	// Clean up data
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	exit(EXIT_SUCCESS);
}
