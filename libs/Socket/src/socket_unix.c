#include "logger.h"
#include "socket-unix.h"
#include "socket.h"
#include <errno.h>
<<<<<<< HEAD
#include <netdb.h>
#include <string.h>
#include <unistd.h>
=======
#include <string.h>
#include <unistd.h>
#include <uv.h>
>>>>>>> feat: Added unix compatibility for server.

//! Default socket port used by library.
static const in_port_t DEFAULT_SOCKET_PORT = 8080;

//! @brief Create new socket for server to listen for requests.
//! @param hSocket Handle for server socket.
//! @return 0 if successful, else socket error code.
static int SocketServerCreate(SOCKET *hSocket);
//! @brief Listen for data on socket.
//! @param hSocket Handle for socket to listen on.
//! @return 0 if successful, else socket error code.
static int SocketListen(const SOCKET *hSocket);

int SocketInitialize() { return 0; }

int SocketServerStart(SOCKET *hSocket) {
  int response = SocketServerCreate(hSocket);
  if (response != 0) {
    return response;
  }

  response = SocketListen(hSocket);
  if (response != 0) {
    return response;
  }

  return 0;
}

int SocketWaitForConnection(const SOCKET *hListeningSocket,
                            SOCKET *hClientSocket) {
  *hClientSocket = accept(*hListeningSocket, NULL, NULL);
  if (*hClientSocket == INVALID_SOCKET) {
    log_fatal("Error at accept(). ID: %d\tMessage: %d", errno, strerror(errno));
    close(*hListeningSocket);
    return errno;
  }

  log_debug("accept- successful");
  return 0;
}

int SocketSend(const SOCKET *hTargetSocket, const char *sendBuffer,
               int sendBufferLength) {
  int response = send(*hTargetSocket, sendBuffer, sendBufferLength, 0);
  if (response == INVALID_SOCKET) {
    log_fatal("Error at send(). ID: %d\tMessage: %d", errno, strerror(errno));
    close(*hTargetSocket);
    return errno;
  }
  log_debug("bytes to send:\t%d", sendBufferLength);
  log_debug("bytes send:\t%d", response);

  return 0;
}

int SocketClose(const SOCKET *hTargetSocket) {
  // Disconnects client socket from server
  int response = close(*hTargetSocket);
  if (response == INVALID_SOCKET) {
    log_fatal("Error at send(). ID: %d\tMessage: %d", errno, strerror(errno));
    return errno;
  }
  log_debug("Client socket disconnected");

  return 0;
}

static int SocketServerCreate(SOCKET *hSocket) {
  // API Calls response;
  struct sockaddr_in serv_addr;

  // Create server socket
<<<<<<< HEAD
  *hSocket = socket(AF_INET, SOCK_STREAM, 0);
=======
  *hSocket = socket(AF_INET, SOCK_STREAM, SOL_TCP);
>>>>>>> feat: Added unix compatibility for server.
  if (*hSocket == INVALID_SOCKET) {
    log_fatal("Error at socket(). ID: %d\tMessage: %d", errno, strerror(errno));
    return errno;
  }
  log_debug("Create server socket - successful");

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(DEFAULT_SOCKET_PORT);

  // Bind server socket to address
  if (bind(*hSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    log_fatal("Error at bind(). ID: %d\tMessage: %d", errno, strerror(errno));
    close(*hSocket);
    return errno;
  }
  log_debug("bind - successful");

  return 0;
}

static int SocketListen(const SOCKET *hSocket) {
  if (listen(*hSocket, SOMAXCONN) != 0) {
    log_fatal("Error at listen(). ID: %d\tMessage: %d", errno, strerror(errno));
    close(*hSocket);
    return errno;
  }
  log_debug("listen - successful");

  return 0;
}
