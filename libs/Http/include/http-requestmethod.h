#include <limits.h>
#include <stddef.h>

#ifndef WEBSERVER_C_PLAYGROUND_HTTP_REQUESTMETHOD_H
#define WEBSERVER_C_PLAYGROUND_HTTP_REQUESTMETHOD_H

//! HTTP "GET" request method.
#define HTTP_GET "GET"
//! Size of HTTP_GET string (subtracted by \0). @see HTTP_GET
extern const size_t HTTP_GET_SIZE;

//! Used HTTP requests methods for connection.
typedef enum HttpRequestMethod {
  HttpRequestMethodGet,
  HttpRequestMethodError = INT_MAX,
} HttpRequestMethod;

#endif // WEBSERVER_C_PLAYGROUND_HTTP_REQUESTMETHOD_H
