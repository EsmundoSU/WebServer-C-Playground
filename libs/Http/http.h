#include "include/http-requestmethod.h"
#include "include/http-response.h"
#include <limits.h>

#ifndef WEBSERVER_C_PLAYGROUND_HTTP_H
#define WEBSERVER_C_PLAYGROUND_HTTP_H

//! Data structure representing HTTP message.
typedef struct HttpMessage {
  HttpRequestMethod httpRequestMethod;
} HttpMessage;

//! @brief Parse HTTP request from raw buffer to httpResponseMessage.
//! @param dataBuffer Buffer with HTTP raw request.
//! @param dataBufferSize Size of dataBuffer size.
//! @param httpResponseMessage Pointer to structure for parsed response data.
//! @see HttpMessage
//! @return HttpResponse (0) is OK, else error. @see HttpResponse
HttpResponse HttpParseRequest(const char *dataBuffer, size_t dataBufferSize,
                     HttpMessage *httpResponseMessage);

#endif // WEBSERVER_C_PLAYGROUND_HTTP_H
