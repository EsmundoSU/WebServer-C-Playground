#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include "configuration.h"
#include "internalConfiguration.h"
#include "logger.h"

int main(int argc, char *argv[])
{
	log_enable(LOG_ENABLE);
	log_set_level(LOG_DEBUG);

	// Contains Windows socket information.
	WSADATA wsa;
	int response = 0; // Buffer for functions responses

	log_info("WebServer is starting...");

	response = WSAStartup(DEFAULT_WINDOWS_SOCKET_VERSION, &wsa);
	switch (response)
	{
	case 0:
		log_debug("Correct initialization use of the Winsock DLL by this process");
		break;
	case WSASYSNOTREADY:
		log_fatal("The underlying network subsystem is not ready for network communication.");
		break;
	case WSAVERNOTSUPPORTED:
		log_fatal("The version of Windows Sockets support requested is not provided by this particular Windows "
			"Sockets implementation.");
		break;
	case WSAEINPROGRESS:
		log_fatal("A blocking Windows Sockets 1.1 operation is in progress.");
		break;
	case WSAEPROCLIM:
		log_fatal("A limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
		break;
	case WSAEFAULT:
		log_fatal("The lpWSAData parameter is not a valid pointer.");
		break;
	default:
		log_debug("Unknown ERROR");
		break;
	}

	if(response != 0)
	{
		return 1;
	}

	struct addrinfo *result = NULL, hints;

	SecureZeroMemory(&hints, sizeof (hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the local address and port to be used by the server
	int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;

	ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	char recvbuf[DEFAULT_BUFFER_LENGTH];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFFER_LENGTH;

// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	printf("Initialised.");

	return 0;
}
