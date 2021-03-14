#include "gtest/gtest.h"
extern "C"
{
  #include "http.h"
}

const char GET_REQUEST[] =
    "GET /hello.htm HTTP/1.1\n"
    "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
    "Host: www.tutorialspoint.com\n"
    "Accept-Language: en-us\n"
    "Accept-Encoding: gzip, deflate\n"
    "Connection: Keep-Alive";

const char POST_REQUEST[] =
    "POST /cgi-bin/process.cgi HTTP/1.1\n"
    "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
    "Host: www.tutorialspoint.com\n"
    "Content-Type: application/x-www-form-urlencoded\n"
    "Content-Length: length\n"
    "Accept-Language: en-us\n"
    "Accept-Encoding: gzip, deflate\n"
    "Connection: Keep-Alive\n"
    "\n"
    "licenseID=string&content=string&/paramsXML=string";

const char EMPTY_REQUEST[] ="";

//! Test if parsing GET correctly.
TEST(HttpLibraryTest, HttpParseRequest_GET) {
  // Arrange
  HttpMessage parsedMessage;

  // Act
  HttpError response = HttpParseRequest(GET_REQUEST, &parsedMessage);

  // Assert
  ASSERT_EQ(HttpOk, response);
  ASSERT_STREQ(HTTP_GET, parsedMessage.requestMethod);
  ASSERT_STREQ("/hello.htm", parsedMessage.requestPath);
}

//! Test if throwing error when getting POST.
TEST(HttpLibraryTest, HttpParseRequest_POST) {
  // Arrange
  HttpMessage parsedMessage;

  // Act
  HttpError response = HttpParseRequest(POST_REQUEST, &parsedMessage);

  // Assert
  ASSERT_EQ(HttpOk, response);
  ASSERT_STREQ(HTTP_POST, parsedMessage.requestMethod);
  ASSERT_STREQ("/cgi-bin/process.cgi", parsedMessage.requestPath);
}

//! Parse Empty string
TEST(HttpLibraryTest, HttpParseRequest_EmptyString) {
  // Arrange
  HttpMessage parsedMessage;

  // Act
  HttpError response = HttpParseRequest(EMPTY_REQUEST, &parsedMessage);

  // Assert
  ASSERT_EQ(HttpInvalidRequestFormat, response)
      << "HttpError id:" << response << std::endl;
}
