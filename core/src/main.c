#include <winsock2.h>
#include "logger.h"
#include "server.h"
#include "http.h"

int main() {
  // Set global app logger.
  log_enable(LOG_ENABLE);
  log_set_level(LOG_DEBUG);


  HttpParseRequest("asdasd", 2, NULL);
  //! Initialize server
  if (ServerInitialization() != 0) {
    exit(EXIT_FAILURE);
  }

  // Create server socket && starts listening.
  if (ServerStart() != 0) {
    exit(EXIT_FAILURE);
  }

  ServerRun();
}
