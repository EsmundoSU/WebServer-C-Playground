#ifndef WEBSERVER_C_PLAYGROUND_SERVER_ERRORS_H
#define WEBSERVER_C_PLAYGROUND_SERVER_ERRORS_H

//! List of errors given by the server.
enum ServerError{
	//! No errors detected for server.
	ServerOk = 0,
	//! Socket initialization error.
	ServerErrorSocketInitialization,
	//! Socket creation error.
	ServerErrorSocketCreation,
	//! Server could not bind.
	ServerErrorSocketBind,
	//! Server could not start listening using socket.
	ServerErrorStartListening,
	//! Server could not accept client connection and create socket.
	ClientConnectionNotAccepted,
};
typedef enum ServerError ServerError;

#endif //WEBSERVER_C_PLAYGROUND_SERVER_ERRORS_H
