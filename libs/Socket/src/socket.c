#include <ws2tcpip.h>
#include "socket.h"
#include "include/socket_errors.h"
#include "logger.h"

//! Default socket port.
static const char * const DEFAULT_SOCKET_PORT = "8080";
//! Default IP version.
static const int DEFAULT_SOCKET_IP_VERSION = 4;
//! Default transport protocol.
static const TransportProtocol DEFAULT_SOCKET_TRANSPORT_PROTOCOL = TCP;

//! Initialize Socket API for this process.
static int SocketApiInitialize();
//! Create new socket for server to listen for requests.
static int SocketServerCreate(SOCKET *hSocket);

Socket SocketGetDefaultHandle(void)
{
	Socket defaultSocket;
	defaultSocket.port = DEFAULT_SOCKET_PORT;
	defaultSocket.ipVersion = DEFAULT_SOCKET_IP_VERSION;
	defaultSocket.transportProtocol = DEFAULT_SOCKET_TRANSPORT_PROTOCOL;

	// Windows only
	defaultSocket.winSocketAddressInformation = NULL;

	return defaultSocket;
}

int SocketStart(SOCKET *hSocket)
{
	// Initialize Socket
	int response = SocketApiInitialize();
	if (response != 0)
	{
		return response;
	}

	response = SocketServerCreate(hSocket);
	if (response != 0)
	{
		return response;
	}

	return 0;
}

static int SocketApiInitialize()
{
	//! Windows Socket Data
	WSADATA windowsSocketData;
	int result = WSAStartup(MAKEWORD(2,2), &windowsSocketData);
	if (result != 0)
	{
		log_fatal("WSAStartup failed with error code:\t%d", result);
		return result;
	}
	log_debug("Socket API init - successful");
	return 0;
}

static int SocketServerCreate(SOCKET *hSocket)
{
	// API Calls response;
	int apiResponse;
	// Server socket address info.
	struct addrinfo *serverSocketAddressInfo = NULL;
	// Server socket address hints for API.
	struct addrinfo serverSocketAddressInfoHints;

	// Setup serverSocketAddressInfoHints to give hints to API how to set up socket address structure.
	ZeroMemory(&serverSocketAddressInfoHints, sizeof(serverSocketAddressInfoHints));
	serverSocketAddressInfoHints.ai_family = AF_INET; // IPv4
	serverSocketAddressInfoHints.ai_socktype = SOCK_STREAM; // TCP
	serverSocketAddressInfoHints.ai_protocol = IPPROTO_TCP; // TCP
	serverSocketAddressInfoHints.ai_flags = AI_PASSIVE;

	// Gets struct with information about socket connection.
	apiResponse = getaddrinfo(
			NULL,
			DEFAULT_SOCKET_PORT,
			&serverSocketAddressInfoHints,
			&serverSocketAddressInfo);
	if (apiResponse != 0)
	{
		log_fatal("\"getaddrinfo\" failed with error code:\t%d", apiResponse);
		WSACleanup();
		return apiResponse;
	}
	log_debug("\"getaddrinfo\" - successful");

	// Create server socket
	*hSocket = socket(
			serverSocketAddressInfo->ai_family,
			serverSocketAddressInfo->ai_socktype,
			serverSocketAddressInfo->ai_protocol);
	if (*hSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		log_fatal("Error at socket(): %ld\n", errorCode);
		freeaddrinfo(serverSocketAddressInfo);
		WSACleanup();
		return errorCode;
	}
	log_debug("create server socket - successful");

	// Bind server socket to address
	apiResponse = bind(*hSocket, serverSocketAddressInfo->ai_addr, (int)serverSocketAddressInfo->ai_addrlen);
	if (apiResponse == SOCKET_ERROR)
	{
		log_fatal("bind failed with error code:\t%d", apiResponse);
		freeaddrinfo(serverSocketAddressInfo);
		closesocket(*hSocket);
		WSACleanup();
		return apiResponse;
	}
	log_debug("bind - successful");
	freeaddrinfo(serverSocketAddressInfo);

	return apiResponse;
}
