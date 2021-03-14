#include "include/http-requestmethod.h"
#include "include/http-response.h"
#include <limits.h>
#include <stddef.h>

#ifndef WEBSERVER_C_PLAYGROUND_HTTP_H
#define WEBSERVER_C_PLAYGROUND_HTTP_H

//! Http request max path length
#define HTTP_MAX_PATH_LENGTH 512

//! Data structure representing HTTP message.
typedef struct HttpMessage {
  HttpRequestMethod requestMethod;
  char requestPath[HTTP_MAX_PATH_LENGTH];
} HttpMessage;

//! @brief Parse HTTP request from raw buffer to httpMessage.
//! @param dataBuffer String buffer with HTTP raw request.
//! @param httpMessage Pointer to structure for parsed response data.
//! @see HttpMessage
//! @return HttpError (0) is OK, else error. @see HttpError
HttpError HttpParseRequest(const char *dataBuffer, HttpMessage *httpMessage);

#endif // WEBSERVER_C_PLAYGROUND_HTTP_H
