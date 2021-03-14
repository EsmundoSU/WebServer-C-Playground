#ifndef WEBSERVER_C_PLAYGROUND_SOCKET_UNIX_H
#define WEBSERVER_C_PLAYGROUND_SOCKET_UNIX_H

#include <string.h>

#ifndef WIN32
#define INVALID_SOCKET -1
#define ZeroMemory(Destination, Length) memset((Destination), 0, (Length))
typedef int SOCKET;
#endif

#endif // WEBSERVER_C_PLAYGROUND_SOCKET_UNIX_H
