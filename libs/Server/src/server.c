#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "server.h"
#include "logger.h"

const char * const SERVER_PARAM_LIST = "p:l:";

ServerConfiguration ServerConfigurationDefault;

//! Client socket
static SOCKET clientSocket = INVALID_SOCKET;

//! Opens index file for sending via socket.
static int OpenIndexFile(void);

//! Bind server socket
static int BindSocket();
//! Listen on the socket
static int ListenOnSocket();
//! Client socket
static int AcceptClientConnection();

ServerError StartServer(ServerConfiguration serverConfiguration)
{
	ServerError response;

	response = SocketInitialization();
	if (response != ServerOk)
	{
		return ServerErrorSocketInitialization;
	}

	response = CreateSocket(serverConfiguration);
	if (response != ServerOk)
	{
		return ServerErrorSocketCreation;
	}

	response = BindSocket();
	if (response != ServerOk)
	{
		return ServerErrorSocketBind;
	}

	response = ListenOnSocket();
	if (response != ServerOk)
	{
		return ServerErrorStartListening;
	}

	response = AcceptClientConnection();
	if(response != ServerOk)
	{
		return ClientConnectionNotAccepted;
	}

	return ServerOk;
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





int BindSocket()
{
	int response;

	response = bind(
			serverListenSocket,
			serverAddressInformation->ai_addr,
			(int)serverAddressInformation->ai_addrlen);
	if (response == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		switch (error)
		{
		case WSANOTINITIALISED:
			log_fatal("Note  A successful WSAStartup call must occur before using this function.");
			break;
		case WSAENETDOWN:
			log_fatal("The network subsystem has failed.");
			break;
		case WSAEACCES:
			log_fatal("An attempt was made to access a socket in a way forbidden by its access permissions.\n"
			          "This error is returned if nn attempt to bind a datagram socket to the broadcast address failed "
			            "because the setsockopt option SO_BROADCAST is not enabled.");
			break;
		case WSAEADDRINUSE:
			log_fatal("Only one usage of each socket address (protocol/network address/port) is normally permitted.\n"
			   "This error is returned if a process on the computer is already bound to the same fully qualified "
			   "address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For "
			   "example, the IP address and port specified in the name parameter are already bound to another socket "
	            "being used by another application. For more information, see the SO_REUSEADDR socket option in the "
			    "SOL_SOCKET Socket Options reference, Using SO_REUSEADDR and SO_EXCLUSIVEADDRUSE, and "
	            "SO_EXCLUSIVEADDRUSE.");
			break;
		case WSAEADDRNOTAVAIL:
			log_fatal("The requested address is not valid in its context.\n"
			          "This error is returned if the specified address pointed to by the name parameter is not a "
			            "valid local IP address on this computer.");
			break;
		case WSAEFAULT:
			log_fatal("The system detected an invalid pointer address in attempting to use a pointer argument in "
			 "a call.\n"
			"This error is returned if the name parameter is NULL, the name or namelen parameter is not a valid part "
            "of the user address space, the namelen parameter is too small, the name parameter contains an incorrect "
			"address format for the associated address family, or the first two bytes of the memory block specified by "
            "name do not match the address family associated with the socket descriptor s.");
			break;
		case WSAEINPROGRESS:
			log_fatal("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing"
			 " a callback function.");
			break;
		case WSAEINVAL:
			log_fatal("An invalid argument was supplied.\n"
			          "This error is returned of the socket s is already bound to an address.");
			break;
		case WSAENOBUFS:
			log_fatal("Typically, WSAENOBUFS is an indication that there aren't enough ephemeral ports to allocate "
			 "for the bind.");
			break;
		case WSAENOTSOCK:
			log_fatal("An operation was attempted on something that is not a socket.\n"
			          "This error is returned if the descriptor in the s parameter is not a socket.");
			break;
		default:
			log_fatal("Unknown error of ID:\t%d", error);
			break;
		}

		freeaddrinfo(serverAddressInformation);
		closesocket(serverListenSocket);
		WSACleanup();
		return error;
	}
	else
	{
		log_debug("Socket created successfully");
	}

	return 0;
}

int ListenOnSocket()
{
	int response = listen(serverListenSocket, SOMAXCONN);
	if (response == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		switch (error)
		{
		case WSANOTINITIALISED:
			log_fatal("A successful WSAStartup call must occur before using this function.");
			break;
		case WSAENETDOWN:
			log_fatal("The network subsystem has failed.");
			break;
		case WSAEADDRINUSE:
			log_fatal("The socket's local address is already in use and the socket was not marked to allow address "
			    "reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could "
	            "be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY)"
			    " and if a specific address needs to be committed at the time of this function.");
			break;
		case WSAEINPROGRESS:
			log_fatal("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still "
			 "processing a callback function.");
			break;
		case WSAEINVAL:
			log_fatal("The socket has not been bound with bind.");
			break;
		case WSAEISCONN:
			log_fatal("The socket is already connected.");
			break;
		case WSAEMFILE:
			log_fatal("No more socket descriptors are available.");
			break;
		case WSAENOBUFS:
			log_fatal("No buffer space is available.");
			break;
		case WSAENOTSOCK:
			log_fatal("The descriptor is not a socket.");
			break;
		case WSAEOPNOTSUPP:
			log_fatal("\tThe referenced socket is not of a type that supports the listen operation");
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
		log_debug("Successfully listening");
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
