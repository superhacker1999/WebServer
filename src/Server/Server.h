#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <algorithm>
#include <vector>
#include <csignal>
#include <fstream>

#include "../Client/Client.h"
#include "../NetHandler/NetHandler.h"
#include "../ConfigParser/ConfigParser.h"

bool m_shutdown_server_ = false;

namespace tcp {
class Server {
 public:
  Server(const std::string& conf_file);
  ~Server();
  void HandlingCycle();
  static void SigHandler(int signum);

 private:
  void ConnectNewUser(int fd);
  void EventsProcessing();
  void AddNewUsers(int listening_fd);
  void CompressArray();
  void FromUser(Client& client);
  void ToUser(Client& client);
  void DisconnectUser(Client& client);
  void InitializeListeningPorts();
  

  pollfd m_fds_[200]{};
  int m_fds_counter_;
  int file_fd_ = -1;
  
  bool m_compress_arr_;
  std::vector<Client> m_clients_;

  WSConfig config_;
};  // class Server
}  // namespace tcp

#endif  // SRC_SERVER_SEVRER_H_
