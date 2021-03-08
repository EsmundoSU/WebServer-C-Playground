#ifndef WEBSERVER_C_PLAYGROUND_SERVER_H
#define WEBSERVER_C_PLAYGROUND_SERVER_H

//! @brief Initialize server instance.
//! @details Function must be run at the beginning of server start-up.
//! @return 0 if initialization was successful, else socket error code.
int ServerInitialization();
//! @brief Starts server and begin listening for new connections.
//! @return 0 if successful, else socket error code.
int ServerStart();
//! @brief (Noreturn) Continuous listening and responding.
void ServerRun();

#endif //WEBSERVER_C_PLAYGROUND_SERVER_H
