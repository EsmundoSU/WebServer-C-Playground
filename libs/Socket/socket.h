#ifndef WEBSERVER_C_PLAYGROUND_SOCKET_H
#define WEBSERVER_C_PLAYGROUND_SOCKET_H
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include "include/socket-unix.h"
#include <sys/socket.h>
#include <sys/types.h>
#endif

//! @brief Initialize usage of socket.
//! @details Must be called once before socket usage.
int SocketInitialize();
//! @brief Create new server socket and starts listening on it.
//! @param hSocket Handle for socket.
//! @return 0 if successful, else socket error code.
int SocketServerStart(SOCKET *hSocket);
//! @brief Blocking function is waiting for connection.
//! @details Wait for new connection on listening port. Function is blocking
//! until new connection arrives.
//! @param hListeningSocket Server socket listening for new connection.
//! @param hClientSocket Remote client socket created for new connection.
//! @return 0 if connection is successful, else socket error code.
int SocketWaitForConnection(const SOCKET *hListeningSocket,
                            SOCKET *hClientSocket);
//! @brief Sends data buffer to the selected socket.
//! @param hTargetSocket socket to write data buffer to.
//! @param sendBuffer data buffer pointer.
//! @param sendBufferLength data buffer length.
//! @return 0 if successful, else socket error.
int SocketSend(const SOCKET *hTargetSocket, const char *sendBuffer,
               int sendBufferLength);
//! @brief Closes and release socket connection.
//! @param hTargetSocket Socket to be closed.
//! @return 0 if successful, else socket error.
int SocketClose(const SOCKET *hTargetSocket);
#endif // WEBSERVER_C_PLAYGROUND_SOCKET_H
