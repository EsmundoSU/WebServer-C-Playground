#ifndef WEBSERVER_C_PLAYGROUND_HTTP_H
#define WEBSERVER_C_PLAYGROUND_HTTP_H

//! Used HTTP requests methods for connection.
typedef enum HttpRequest
{
	HttpRequestGet,
}HttpRequest;

//! Data structure representing full HTTP message.
typedef struct HttpMessage
{
	HttpRequest httpRequestMethod;
}HttpMessage;

//!
const char * const HTTP_GET = "GET";

#endif //WEBSERVER_C_PLAYGROUND_HTTP_H
