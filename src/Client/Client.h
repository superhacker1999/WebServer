#ifndef SRC_CLIENT_CLIENT_H_
#define SRC_CLIENT_CLIENT_H_

#include <string.h>

#include "../NetHandler/NetHandler.h"

namespace tcp {
class Client {
 private:
  int m_fd_;     // fildes of client
  pollfd* m_poll_;

 public:
  Client(int own_fd, pollfd* client_poll);
  ~Client();
  int GetFd();
  void SetFd(int new_fd);
  short GetEvent();
};  // class Client
}  // namespace tcp

#endif  // SRC_CLIENT_CLIENT_H_