#include "http.h"
#include <string.h>

static HttpRequestMethod ParseRequestMethod(const char *dataBuffer,
                                            size_t dataBufferSize,
                                            const char *parsingPointer);

HttpResponse HttpParseRequest(const char *dataBuffer, size_t dataBufferSize,
                              HttpMessage *httpResponseMessage) {

  // This pointer is giving starting point to find proper data during parsing.
  const char *parsingPointer = dataBuffer;

  // Parse request method from buffer.
  int response = ParseRequestMethod(dataBuffer, dataBufferSize, parsingPointer);
  if (response == HttpRequestMethodError) {
    return HttpNotSupportedRequestMethod;
  }
  httpResponseMessage->httpRequestMethod = (HttpRequestMethod)response;

  return HttpOk;
}

static HttpRequestMethod ParseRequestMethod(const char *dataBuffer,
                                            size_t dataBufferSize,
                                            const char *parsingPointer) {

  size_t requestMethodLength = 0;
  char currentChar = *parsingPointer;
  while (currentChar != ' ' && currentChar != '\n' && currentChar != '\0') {
    // If not detect invalid character add to length.
    requestMethodLength++;
    // Check for buffer overflow
    if (requestMethodLength > dataBufferSize) {
      return HttpRequestMethodError;
    }

    // Go to next char in data buffer.
    parsingPointer++;
    // Dereference pointing char for while check.
    currentChar = *parsingPointer;
  }

  if ((requestMethodLength == HTTP_GET_SIZE) &&
      (memcmp(dataBuffer, HTTP_GET, HTTP_GET_SIZE) == 0)) {
    return HttpRequestMethodGet;
  }

  return HttpRequestMethodError;
}
