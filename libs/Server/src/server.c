#include <winsock2.h>
#include <ws2tcpip.h>
#include "server.h"
#include "logger.h"

ServerConfiguration ServerConfigurationDefault = { DEFAULT_PORT };

//! Socket information type.
static WSADATA socketInformation;
//! Socket to connect to server.
static SOCKET serverListenSocket = INVALID_SOCKET;
//! Host address information (pointer).
static PADDRINFOA serverAddressInformation;
//! Client socket
static SOCKET clientSocket = INVALID_SOCKET;

//! Initialize socket for client-server communication.
static int SocketInitialization(void);
//! Creates new server socket.
static int CreateSocket(ServerConfiguration serverConfiguration);
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

static int SocketInitialization(void)
{
	// Gets socket information.
	int response = WSAStartup(DEFAULT_WINDOWS_SOCKET_VERSION, &socketInformation);
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

	return response;
}

static int CreateSocket(ServerConfiguration serverConfiguration)
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
			serverConfiguration.pPort,
			&hints,
			&serverAddressInformation);
	switch (response)
	{
	case 0:
		log_debug("Successfully response from \"getaddreinfo\"");
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
		return response;
	}

	// Create a SOCKET for the server to listen for client connections
	serverListenSocket = socket(
			serverAddressInformation->ai_family,
			serverAddressInformation->ai_socktype,
			serverAddressInformation->ai_protocol);

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

		freeaddrinfo(serverAddressInformation);
		WSACleanup();
		return error;
	}
	else
	{
		log_debug("Server socket created successfully");
	}

	return response;
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
