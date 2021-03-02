#include <ws2tcpip.h>
#include "socket.h"
#include "logger.h"

//! Default socket port.
static const char * const DEFAULT_SOCKET_PORT = "8080";
//! Default IP version.
static const int DEFAULT_SOCKET_IP_VERSION = 4;
//! Default transport protocol.
static const TransportProtocol DEFAULT_SOCKET_TRANSPORT_PROTOCOL = TCP;

//! Initialize socket for communication.
static int SocketInitialization(Socket *hSocket);
//! Gets address info for socket.
static int SocketGetAddressInfo(Socket *hSocket);
//! Gets platform connection socket
static int SocketCreate(Socket *hSocket);

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

int SocketStartListening(Socket *hSocket)
{
	// Initialize Socket
	int response = SocketInitialization(hSocket);
	if (response != 0)
	{
		return response;
	}

	//Gets information about the socket.
	response = SocketGetAddressInfo(hSocket);
	if (response != 0)
	{
		return response;
	}
	return 0;
}

static int SocketInitialization(Socket *hSocket)
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

	if (response != 0)
	{
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	return 0;
}

int SocketCreate(Socket *hSocket)
{
	//! Socket to connect to server.
	static SOCKET serverListenSocket = INVALID_SOCKET;
	// Create a SOCKET for the server to listen for client connections
	serverListenSocket = socket(
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_family,
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_socktype,
			((PADDRINFOA)hSocket->winSocketAddressInformation)->ai_protocol);

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
		exit(EXIT_FAILURE);
	}

	log_debug("Server socket created successfully");
	return 0;
}

