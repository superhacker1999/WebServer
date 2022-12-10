#ifndef SRC_CLIENT_CLIENT_H_
#define SRC_CLIENT_CLIENT_H_

#include <string.h>

#include "../NetHandler/NetHandler.h"

namespace tcp {
class Client {
 private:
  int m_fd_;     // fildes of client
  pollfd* m_poll_;
  std::string m_request_;
  int m_file_fd_;
  std::string m_response_body_;

 public:
  Client(int own_fd, pollfd* client_poll);
  ~Client();
  int GetFd();
  void SetFd(int new_fd);
  short GetEvent();
  void AddBody(const std::string& response);
  const std::string GetBody();
  void SetRequest(const std::string& request);
  const std::string GetRequest();
  void SetFileFD(int fd);
  int GetOpenedFileFD();
};  // class Client
}  // namespace tcp

#endif  // SRC_CLIENT_CLIENT_H_