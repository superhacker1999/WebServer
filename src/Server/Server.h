#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <algorithm>
#include <vector>
#include <csignal>
#include <fstream>

#include "../Client/Client.h"
#include "../NetHandler/NetHandler.h"

bool m_shutdown_server_ = false;

namespace tcp {
class Server {
 public:
  Server(int server_port);
  ~Server();
  void HandlingCycle();
  static void SigHandler(int signum);

 private:
  void ConnectNewUser_(int fd);
  void EventsProcessing_();
  void AddNewUsers_();
  void CompressArray_();
  void FromUser_(Client& client);
  void ToUser_(Client& client);
  void DisconnectUser_(Client& client);
  

  int m_listening_server_fd_;
  pollfd m_fds_[200]{};
  int m_fds_counter_;
  int file_fd_ = -1;
  
  bool m_compress_arr_;
  std::vector<Client> m_clients_;
};  // class Server
}  // namespace tcp

#endif  // SRC_SERVER_SEVRER_H_
