#ifndef SRC_REQUESTHANDLER_REQUESTHANDLER_H_
#define SRC_REQUESTHANDLER_REQUESTHANDLER_H_

#include "../ConfigParser/ConfigParser.h"
#include "../Client/Client.h"
#include <memory>
#include <iostream>
#include <fcntl.h>

enum request_status {
  RS_OK,
  RS_OK_NEED_NEXT,
  RS_NOT_FOUND,
  RS_BAD_REQUEST
};

constexpr char def_404[] = "<!DOCTYPE html>\n<html>\n\
<head>\n<title>be1.ru</title>\n</head>\n<body>\n<p>ERROR\
 404: Page not found :(</p>\n</body>\n</html>";

namespace http {
class RequestHandler {
  public:
    static int ProcessRequest(tcp::Client& curr_client, const WSConfig& cfg);
    static const std::string Get404(const WSConfig& cfg);
};  // class RequesHandler
}  // namespace http

#endif  // SRC_REQUESTHANDLER_REQUESTHANDLER_H_