#include "http.h"
#include <stdbool.h>
#include <string.h>

//! @brief Parsing http request method from buffer.
//! @param parsingPointer Pointer to exchange information with others functions
//! where parsing was done.
//! @param httpMessage is pointer to buffer storing parsed data.
static HttpError ParseRequestMethod(const char **parsingPointer,
                                    HttpMessage *httpMessage);

//! @brief Parsing http request path from buffer.
//! @param parsingPointer Pointer to exchange information with others functions
//! where parsing was done.
//! @param httpMessage is pointer to buffer storing parsed data.
static HttpError ParseRequestPath(const char **parsingPointer,
                                  HttpMessage *httpMessage);

HttpError HttpParseRequest(const char *dataBuffer, HttpMessage *httpMessage) {
  // This pointer is giving starting point to find proper data during parsing.
  const char *parsingPointer = dataBuffer;

  // Parse request method from buffer.
  HttpError httpError = ParseRequestMethod(&parsingPointer, httpMessage);
  if (httpError != HttpOk) {
    return httpError;
  }

  // Parse request path
  httpError = ParseRequestPath(&parsingPointer, httpMessage);
  if (httpError != HttpOk) {
    return httpError;
  }
  return httpError;
}

static HttpError ParseRequestMethod(const char **parsingPointer,
                                    HttpMessage *httpMessage) {
  HttpError response = HttpOk;
  // Request method can have max size defined here.
  static const size_t maxRequestMethodLength = 10;
  // Index of currently analyzed character.
  size_t charIndex = 0;
  // Currently analyzed character.
  char charCurrent = (*parsingPointer)[charIndex];
  while (charCurrent != ' ' && charCurrent != '\n' && charCurrent != '\0') {
    charIndex++;
    charCurrent = (*parsingPointer)[charIndex];
    // Check for buffer overflow
    if (charIndex >= maxRequestMethodLength) {
      return HttpInvalidRequestFormat;
    }
  }

  // Checking for request method.
  if ((charIndex == HTTP_OPTIONS_SIZE) &&
      (memcmp(*parsingPointer, HTTP_OPTIONS, HTTP_OPTIONS_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_OPTIONS;
  } else if ((charIndex == HTTP_GET_SIZE) &&
             (memcmp(*parsingPointer, HTTP_GET, HTTP_GET_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_GET;
  } else if ((charIndex == HTTP_HEAD_SIZE) &&
             (memcmp(*parsingPointer, HTTP_HEAD, HTTP_HEAD_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_HEAD;
  } else if ((charIndex == HTTP_POST_SIZE) &&
             (memcmp(*parsingPointer, HTTP_POST, HTTP_POST_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_POST;
  } else if ((charIndex == HTTP_PUT_SIZE) &&
             (memcmp(*parsingPointer, HTTP_PUT, HTTP_PUT_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_PUT;
  } else if ((charIndex == HTTP_DELETE_SIZE) &&
             (memcmp(*parsingPointer, HTTP_DELETE, HTTP_DELETE_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_DELETE;
  } else if ((charIndex == HTTP_TRACE_SIZE) &&
             (memcmp(*parsingPointer, HTTP_TRACE, HTTP_TRACE_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_TRACE;
  } else if ((charIndex == HTTP_CONNECT_SIZE) &&
             (memcmp(*parsingPointer, HTTP_CONNECT, HTTP_CONNECT_SIZE) == 0)) {
    httpMessage->requestMethod = HTTP_CONNECT;
  } else {
    response = HttpNotSupportedRequestMethod;
  }

  if (charIndex == 0) {
    response = HttpInvalidRequestFormat;
  }

  // Move parsing pointer to new position.
  *parsingPointer += charIndex;

  return response;
}

static HttpError ParseRequestPath(const char **parsingPointer,
                                  HttpMessage *httpMessage) {
  // -1 byte to add null termination.
  static const size_t maxLength = HTTP_MAX_PATH_LENGTH - 1;
  // Index of currently analyzed character
  size_t charIndex = 0;
  // Currently analyzer character
  char currentCharacter = (*parsingPointer)[charIndex];

  // Before path should be an empty space.
  if (currentCharacter != ' ') {
    return HttpInvalidRequestFormat;
  }

  charIndex++;
  currentCharacter = (*parsingPointer)[charIndex];
  while (currentCharacter != ' ' && currentCharacter != '\n' &&
         currentCharacter != '\0') {
    // Check for buffer overflow
    if (charIndex >= maxLength) {
      return HttpRequestPathTooLong;
    }
    httpMessage->requestPath[charIndex - 1] = currentCharacter;

    charIndex++;
    currentCharacter = (*parsingPointer)[charIndex];
  }
  httpMessage->requestPath[charIndex - 1] = '\0';

  // Move parsing pointer to new position.
  *parsingPointer += charIndex;
  return HttpOk;
}
