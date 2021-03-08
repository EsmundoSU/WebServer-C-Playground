#ifndef WEBSERVER_C_PLAYGROUND_SOCKET_ERRORS_H
#define WEBSERVER_C_PLAYGROUND_SOCKET_ERRORS_H

//! List of errors given by the server.
enum SocketError{
	//! No errors detected for server.
	SocketOk = 0,
	//! Socket initialization error.
	SocketErrorInitialization,
	//! Socket error address get.
	SocketErrorAddressData,
	//! Socket creation error.
	SocketErrorCreation,
	//! Server could not bind.
	SocketErrorBinding,
	//! Server could not start listening using socket.
	SocketErrorListening,
	//! Server could not accept client connection and create socket.
	ClientConnectionNotAccepted,
};
typedef enum SocketError SocketError;

#endif //WEBSERVER_C_PLAYGROUND_SOCKET_ERRORS_H
