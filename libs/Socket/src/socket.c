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

//! Initialize socket for communication.
static int SocketInitialization();
//! Gets address info for socket.
static int SocketGetAddressInfo(Socket *hSocket);
//! Gets platform connection socket
static int SocketCreate(Socket *hSocket);
//! Binds socket
static int SocketBind(Socket *hSocket);
//! Listen on the socket
static int SocketListen(Socket *hSocket);

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

int SocketStart(Socket *hSocket)
{
	// Initialize Socket
	int response = SocketInitialization(hSocket);
	if (response != SocketOk)
	{
		return SocketErrorInitialization;
	}

	// Gets information about the socket.
	response = SocketGetAddressInfo(hSocket);
	if (response != SocketOk)
	{
		return SocketErrorAddressData;
	}

	// Create server socket
	response = SocketCreate(hSocket);
	if (response != SocketOk)
	{
		return SocketErrorCreation;
	}

	// Bind server socket
	response = SocketBind(hSocket);
	if (response != SocketOk)
	{
		return SocketErrorBinding;
	}

	return 0;
}

static int SocketInitialization()
{
	//! Socket information type.
	WSADATA socketInformation;
	// Gets socket information.
	int response = WSAStartup(MAKEWORD(2,2), &socketInformation);

	return response;
}

static int SocketGetAddressInfo(Socket *hSocket)
{
	int response;
	ADDRINFOA hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	response = getaddrinfo(
			NULL,
			hSocket->port,
			&hints,
			(struct addrinfo**)&hSocket->winSocketAddressInformation);


	if (response != 0)
	{
		switch (response)
		{
		case 0:
			log_debug("Successfully get address information of socket.");
			break;
		case EAI_AGAIN:
			log_fatal("A temporary failure in name resolution occurred.");
			break;
		case EAI_BADFLAGS:
			log_fatal("An invalid value was provided for the ai_flags member of the pHints parameter.");
			break;
		case EAI_FAIL:
			log_fatal("A nonrecoverable failure in name resolution occurred.");
			break;
		case EAI_FAMILY:
			log_fatal("The ai_family member of the pHints parameter is not supported.");
			break;
		case EAI_MEMORY:
			log_fatal("A memory allocation failure occurred.");
			break;
		case EAI_NONAME:
			log_fatal("The name does not resolve for the supplied parameters or the pNodeName and pServiceName parameters "
			          "were not provided.");
			break;
		case EAI_SERVICE:
			log_fatal("The pServiceName parameter is not supported for the specified ai_socktype member of the pHints "
			          "parameter.");
			break;
		case EAI_SOCKTYPE:
			log_fatal("The ai_socktype member of the pHints parameter is not supported.");
			break;
		default:
			log_fatal("Unknown Error");
			break;
		}

		WSACleanup();
	}

	return response;
}

static int SocketCreate(Socket *hSocket)
{
	//! Socket to connect to server.
	static SOCKET serverListenSocket = INVALID_SOCKET;
	// Create a SOCKET for the server to listen for client connections
	serverListenSocket = socket(
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_family,
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_socktype,
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_protocol);
	// Save pointer to this socket.
	hSocket->socket = &serverListenSocket;

	if (serverListenSocket == INVALID_SOCKET)
	{
		// Gets last error
		int error = WSAGetLastError();
		switch (error)
		{
		case WSANOTINITIALISED:
			log_fatal("A successful WSAStartup call must occur before using this function.");
			break;
		case WSAENETDOWN:
			log_fatal("The network subsystem or the associated service provider has failed.");
			break;
		case WSAEAFNOSUPPORT:
			log_fatal("The specified address family is not supported. For example, an application tried to create a "
			          "socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on "
			          "the local computer.");
			break;
		case WSAEINPROGRESS:
			log_fatal("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing "
			          "a callback function.");
			break;
		case WSAEMFILE:
			log_fatal("No more socket descriptors are available.");
			break;
		case WSAEINVAL:
			log_fatal("An invalid argument was supplied. This error is returned if the af parameter is set to "
			          "AF_UNSPEC and the type and protocol parameter are unspecified.");
			break;
		case WSAEINVALIDPROVIDER:
			log_fatal("The service provider returned a version other than 2.2.");
			break;
		case WSAEINVALIDPROCTABLE:
			log_fatal("The service provider returned an invalid or incomplete procedure table to the WSPStartup.");
			break;
		case WSAENOBUFS:
			log_fatal("No buffer space is available. The socket cannot be created.");
			break;
		case WSAEPROTONOSUPPORT:
			log_fatal("The specified protocol is not supported.");
			break;
		case WSAEPROTOTYPE:
			log_fatal("The specified protocol is the wrong type for this socket.");
			break;
		case WSAEPROVIDERFAILEDINIT:
			log_fatal("The service provider failed to initialize. This error is returned if a layered service "
			          "provider (LSP) or namespace provider was improperly installed or the provider fails to operate "
			          "correctly.");
			break;
		case WSAESOCKTNOSUPPORT:
			log_fatal("The specified socket type is not supported in this address family.");
			break;
		default:
			log_fatal("Unknown error of ID:\t%d", error);
		}

		freeaddrinfo(((PADDRINFOA)hSocket->winSocketAddressInformation));
		WSACleanup();
		return error;
	}

	log_debug("Server socket created successfully");
	return 0;
}

static int SocketBind(Socket *hSocket)
{
	int response;

	response = bind(
			(SOCKET)hSocket->socket,
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_addr,
			(int)((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_addrlen);
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

		freeaddrinfo((PADDRINFOA)hSocket->winSocketAddressInformation);
		closesocket((SOCKET)hSocket->socket);
		WSACleanup();
		return error;
	}
	else
	{
		log_debug("Socket created successfully");
	}

	return 0;
}

static int SocketListen(Socket *hSocket)
{
	int response = listen((SOCKET)hSocket->socket, SOMAXCONN);
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

		closesocket((SOCKET)hSocket->socket);
		WSACleanup();
		return error;
	}
	else
	{
		log_debug("Successfully listening");
	}
	return 0;
}
