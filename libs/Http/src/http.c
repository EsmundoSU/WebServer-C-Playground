#include "http.h"
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
  // Request method can have max size defined here.
  const size_t maxRequestMethodLength = 10;
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

  if ((charIndex == HTTP_GET_SIZE) &&
      (memcmp(*parsingPointer, HTTP_GET, HTTP_GET_SIZE) == 0)) {
    httpMessage->requestMethod = HttpRequestMethodGet;

    // Move parsing pointer to new position.
    *parsingPointer += charIndex;
    return HttpOk;
  }

  if (charIndex == 0) {
    return HttpInvalidRequestFormat;
  }

  return HttpNotSupportedRequestMethod;
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
