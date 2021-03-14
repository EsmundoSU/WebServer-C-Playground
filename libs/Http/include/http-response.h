#ifndef WEBSERVER_C_PLAYGROUND_HTTP_RESPONSE_H
#define WEBSERVER_C_PLAYGROUND_HTTP_RESPONSE_H

typedef enum HttpResponse {
  HttpOk,
  HttpInvalidRequestFormat,
  HttpNotSupportedRequestMethod,
  HttpRequestPathTooLong,
  HttpParsingError,
} HttpError;

#endif // WEBSERVER_C_PLAYGROUND_HTTP_RESPONSE_H
