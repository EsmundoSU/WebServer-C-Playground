#include <ws2tcpip.h>
#include "socket.h"
#include "logger.h"

//! Default socket port used by library.
static const char * const DEFAULT_SOCKET_PORT = "8080";

//! @brief Initialize Socket API for this process.
//! @return 0 if successful, else socket error code.
static int SocketApiInitialize();
//! @brief Create new socket for server to listen for requests.
//! @param hSocket Handle for server socket.
//! @return 0 if successful, else socket error code.
static int SocketServerCreate(SOCKET *hSocket);
//! @brief Listen for data on socket.
//! @param hSocket Handle for socket to listen on.
//! @return 0 if successful, else socket error code.
static int SocketListen(const SOCKET *hSocket);

int SocketInitialize()
{
	int response = SocketApiInitialize();
	if (response != 0)
	{
		return response;
	}

	return 0;
}

int SocketServerStart(SOCKET *hSocket)
{
	int response = SocketServerCreate(hSocket);
	if (response != 0)
	{
		return response;
	}

	response = SocketListen(hSocket);
	if (response != 0)
	{
		return response;
	}

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

int SocketClose(const SOCKET *hTargetSocket)
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

static int SocketListen(const SOCKET *hSocket)
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
