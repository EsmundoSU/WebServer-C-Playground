#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "server.h"
#include "logger.h"
#include "socket.h"

static Socket hServerSocket;

static const int READ_BUFFER_SIZE = 512;

//! Client socket
static int AcceptClientConnection();

int ServerStart()
{
	return SocketStart(&hServerSocket);
}

int ServerRun()
{
	const char sendBuffer[] = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n"
	                          "<h1>Hello, client! Welcome to the Virtual Machine Web..</h1>";
	char recvBuffer[READ_BUFFER_SIZE];
	SOCKET clientSocket;
	int bytesToSend = sizeof (sendBuffer)/sizeof(sendBuffer[0]);
	int bytesReceived;
	int bytesSend;

	while (TRUE)
	{
		bytesReceived = recv(clientSocket, recvBuffer, READ_BUFFER_SIZE, 0);
		if (bytesReceived > 0)
		{
			log_debug("Bytes received: %d\n", bytesReceived);

			bytesSend = send(clientSocket, sendBuffer, bytesToSend, 0);
			if (bytesSend == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				log_fatal("Send failed: %d", error);

				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}
		}
		else if (bytesReceived == 0)
		{
			log_debug("Connection closing");
			return 0;
		}
		else
		{
			int error = WSAGetLastError();
			log_fatal("Receive data failed. Error:\t%d", error);
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	}
}

int ReceiveData(void* buffer, size_t length)
{
	const char sendBuffer[] = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n"
	                          "<h1>Hello, client! Welcome to the Virtual Machine Web..</h1>";

	int bytesSend;
	int bytesToSend = sizeof (sendBuffer)/sizeof(sendBuffer[0]);
	int bytesReceived = recv(clientSocket, buffer, length, 0);
	// Correctly received data.
	if(bytesReceived > 0)
	{
		log_debug("Bytes received: %d\n", bytesReceived);

		bytesSend = send(clientSocket, sendBuffer, bytesToSend, 0);
		if (bytesSend == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			log_fatal("Send failed: %d", error);

			closesocket(clientSocket);
			WSACleanup();
			return -1;
		}
	}
	else if(bytesReceived == 0)
	{
		log_debug("Connection closing");
		return 0;
	}
	else // Error
	{
		int error = WSAGetLastError();
		log_fatal("Receive data failed. Error:\t%d", error);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	return 0;
}



int AcceptClientConnection()
{
	clientSocket = accept(serverListenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		switch (error)
		{
		case WSANOTINITIALISED:
			log_fatal("A successful WSAStartup call must occur before using this function.");
			break;
		case WSAECONNRESET:
			log_fatal("An incoming connection was indicated, but was subsequently terminated by the remote peer "
			    "prior to accepting the call.");
			break;
		case WSAEFAULT:
			log_fatal("The addrlen parameter is too small or addr is not a valid part of the user address space.");
			break;
		case WSAEINTR:
			log_fatal("A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.");
			break;
		case WSAEINVAL:
			log_fatal("The listen function was not invoked prior to accept.");
			break;
		case WSAEINPROGRESS:
			log_fatal("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still "
			    "processing a callback function.");
			break;
		case WSAEMFILE:
			log_fatal("The queue is nonempty upon entry to accept and there are no descriptors available.");
			break;
		case WSAENETDOWN:
			log_fatal("The network subsystem has failed.");
			break;
		case WSAENOBUFS:
			log_fatal("No buffer space is available.");
			break;
		case WSAENOTSOCK:
			log_fatal("The descriptor is not a socket.");
			break;
		case WSAEOPNOTSUPP:
			log_fatal("The referenced socket is not a type that supports connection-oriented service.");
			break;
		case WSAEWOULDBLOCK:
			log_fatal("The socket is marked as nonblocking and no connections are present to be accepted.");
			break;
		default:
			log_fatal("Unknown error of ID:\t%d", error);
			break;
		}

		closesocket(serverListenSocket);
		WSACleanup();
		return error;
	}
	else
	{
		log_debug("Client connection accepted and socket created");
	}
	return 0;
}

int OpenIndexFile(void)
{
	FILE* indexPtr = fopen(serverCo)
	return 0;
}
