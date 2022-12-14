#include "Server.h"

/**
 * Server constructor
 * Creates an fd for listening socket,
 * sets this socket to listen with queue of 10 clients
 * @param server_port The port that server will use as listening to connect new
 * users
 * @param db_ip IP of Database
 * @param db_port port of Database
 */
tcp::Server::Server(const std::string& conf_file)
    : m_fds_counter_(0),
      m_compress_arr_(false) {
  ConfigParser parser;
  config_ = parser.Parse(conf_file);
  InitializeListeningPorts();
}

void tcp::Server::InitializeListeningPorts() {
  int listening = -1;
  int i = 0;
  for (auto it : config_.listening_ports) {
    listening = tcp::NetHandler::Socket(AF_INET, SOCK_STREAM, 0);
    tcp::NetHandler::MakeSocketReuseable(listening);
    tcp::NetHandler::SetNonBlockingSocket(listening);
    auto addr = tcp::NetHandler::InitAddr(it);
    tcp::NetHandler::Bind(listening, (sockaddr*)&addr, sizeof(addr));
    tcp::NetHandler::Listen(listening, 10);
    m_fds_[i].fd = listening;
    m_fds_[i++].events = POLL_IN;
    m_fds_counter_++;
  }
}

tcp::Server::~Server() {
  for (size_t i = 0; i < config_.listening_ports.size(); ++i)
    close(m_fds_[i].fd);
  for (auto it : m_clients_) {
    close(it.GetFd());
  }
}

/**
 * Method to connect client and db,
 * create a pair of client's fildes and DB's fildes
 * @param fd fildes of new accepted client
 */
void tcp::Server::ConnectNewUser(int fd) {
  m_fds_[m_fds_counter_].fd = fd;
  m_fds_[m_fds_counter_++].events = POLLIN;
  std::cout << "new fd  = " << fd << std::endl;
  m_clients_.push_back(Client(fd, &m_fds_[m_fds_counter_ - 1]));
  std::cout << "Client has been connected to server. Listening now" << std::endl;
}

/**
 * Method for accepting new users,
 * calls ConnectToDB_() method after accepts
 * incoming request to connect
 */
// to do
// method should get listening fd that has revent AddNewUsers(int fd)
void tcp::Server::AddNewUsers(int listening_fd) {
  int new_fd = 0;
  std::cout << "\nGot the request to add new user\n";
  do {
    new_fd = accept(listening_fd, NULL, NULL);
    if (new_fd < 0) {
      if (errno != EWOULDBLOCK) {
        perror("ERROR: Accept failed ");
        m_shutdown_server_ = true;
      }
      break;
    }
    ConnectNewUser(new_fd);
  } while (new_fd != -1);
}

/**
 * Sends a packet of data from user to DB,
 * calls CreateLog() method if request marked as an SQL request
 * @param client the one client that has request to DB
 */
void tcp::Server::FromUser(tcp::Client& client) {
  (void)client;
  char buff[BUFF_LEN]{};
  bool close_connection = false;
  auto status = tcp::NetHandler::Read(client.GetFd(), buff, BUFF_LEN);
  std::cout << buff;
  if (status.second) close_connection = true;
  if (close_connection) {
    DisconnectUser(client); 
    m_compress_arr_ = true;
  }
}

/**
 * Sends a packet of data from DB to user,
 * @param client the one client that has something to get from DB
 */
// todo add ensuring read and write processes well
void tcp::Server::RequestProcessing(tcp::Client& client) {
  // ???????????? ???????????? ???? ??????????????, ???????????????????? ?? ??????????????, ???????????????? ?????? ?? RequestHandler
  char buffer[BUFF_LEN]{};
  if (client.GetRequest().empty()) {
    tcp::NetHandler::Read(client.GetFd(), buffer, BUFF_LEN);
    client.SetRequest(buffer);
  }
  std::cout << "read request from user with prev fd\n";
  int status = http::RequestHandler::ProcessRequest(client, config_);
  if (status == RS_OK) {
    std::cout << "processed, page was given to the client with fd = " << client.GetFd() << std::endl;
    std::string header = GetHeader(client.GetBody().size(), OK);
    tcp::NetHandler::Write(client.GetFd(), header.c_str(), header.size());
    tcp::NetHandler::Write(client.GetFd(), client.GetBody().c_str(), client.GetBody().size()); 
  } else if (status == RS_NOT_FOUND) {
    std::cout << "page not found\n";
    std::string header = GetHeader(client.GetBody().size(), PAGE_NOT_FOUND);
    tcp::NetHandler::Write(client.GetFd(), header.c_str(), header.size());
    tcp::NetHandler::Write(client.GetFd(), client.GetBody().c_str(), client.GetBody().size()); 
  } else if (status == RS_OK_NEED_NEXT) {
    // ???????? ???? ?????? ?????????????????? ???????????????????? ?????????????? ?????????? ?? ??????????????
    std::cout << "something left in file, waiting for the next iteration of processing this client fd = " << client.GetFd() << std::endl;
    m_request_queue.push(&client);
  }
}

// std::pair<std::string, int> tcp::Server::GetBody(const std::string& request) {
//   char buffer[BUFF_LEN]{};
//   std::string body;
//   int status = OK;
//   int file_fd = open((config_.locations.at("/") + request).c_str(), O_RDONLY);
//   if (file_fd < 0) {
//     status = PAGE_NOT_FOUND;
//     if (config_.error_pages_names.find(404) != config_.error_pages_names.end()) {
//       file_fd = open(config_.error_pages_names.at(404).c_str(), O_RDONLY);
//       if (file_fd < 0) {
//         body = "<!DOCTYPE html>\n<html>\n<head>\n<title>be1.ru</title>\n</head>\n<body>\n<p>ERROR 404: Page not found :(</p>\n</body>\n</html>";
//       } else {
//         while (read(file_fd, buffer, BUFF_LEN) > 0) {
//           body.append(buffer);
//         }
//         close(file_fd);
//       }
//     } else {
//       body = "<!DOCTYPE html>\n<html>\n<head>\n<title>be1.ru</title>\n</head>\n<body>\n<p>ERROR 404: Page not found :(</p>\n</body>\n</html>";
//     }
//   } else {
//     while (read(file_fd, buffer, BUFF_LEN) > 0) {
//       body.append(buffer);
//     }
//     close(file_fd);
//   }
//   return {body, status};
// }

std::string tcp::Server::GetHeader(size_t content_length, int status) {
  std::string header = "\r\nContent-Type: text/html;\r\ncharset=UTF-8\r\nContent-Length: ";
  if (status == OK) {
    header = "HTTP/1.1 200 OK" + header;
  } else if (status == PAGE_NOT_FOUND) {
    header = "HTTP/1.1 404 Not Found" + header;
  } else if (status == FORBIDDEN) {
    header = "HTTP/1.1 403 Forbidden" + header;
  }
  header += std::to_string(content_length) + "\r\n\r\n";
  return header;
}

/**
 * Disconnects user from server,
 * closes client's fildes and the DB's fildes
 * @param client the one client that has to be disconnected
 */
void tcp::Server::DisconnectUser(Client& client) {
  close(client.GetFd());
  client.SetFd(-1);
}

/**
 * processes the events from every fildes,
 * calls DisconnectUser() when got the request to disconnect
 * if client has smth calls - FromUser()
 * if DB has smth - calls ToUser()
 */
void tcp::Server::EventsProcessing() {
  for (size_t i = 0; i < config_.listening_ports.size(); ++i)
    if (m_fds_[i].revents == POLLIN) AddNewUsers(m_fds_[i].fd);
  for (auto &curr_client : m_clients_) {
    auto event = curr_client.GetEvent();
    // there is no requests from DB neither from client
    if (event == 0) {
      continue;
      // request to disconnect
    } else if (event == SIGSTOP) {
      DisconnectUser(curr_client);
      // ready to be read from user
    // } else if (event == POLL_IN) {
    //   FromUser_(curr_client);
    //   // ready to be read from DB
    } else if (event == POLLIN) {
      std::cout << "request for page from client with fd = " << curr_client.GetFd() << std::endl;
      //m_request_queue.push(&curr_client);
      RequestProcessing(curr_client);
    }
  }
  // if (!m_request_queue.empty()) {
  //   std::cout << "there is something in queue, starting to process the request from client with fd = " << m_request_queue.front()->GetFd() << std::endl;
  //   RequestProcessing(*m_request_queue.front());
  //   m_request_queue.pop();
  // }
  std::remove_if(m_clients_.begin(), m_clients_.end(),
                 [](Client client) { return client.GetFd() == -1; });
}

/**
 * Compresses the array of pollfd structures
 */
void tcp::Server::CompressArray() {
  m_compress_arr_ = false;
  for (int i = 0; i < m_fds_counter_; ++i) {
    if (m_fds_[i].fd == -1) {
      for (int j = i; j < m_fds_counter_; ++j) {
        m_fds_[j].fd = m_fds_[j + 1].fd;
      }
      --i;
      --m_fds_counter_;
    }
  }
}

/**
 * Main method that calls poll()
 * if any client has any request calls EventProcessing()
 */
void tcp::Server::HandlingCycle() {
  int status = 0;
  do {
    // setting poll timeout to infinite
    status = poll(m_fds_, m_fds_counter_, -1);
    std::cout << "poll poimal event\n";
    if (status < 0) {
      perror("ERROR: Poll fails while waiting for the request ");
      break;
    } else if (status == 0) {
      perror("ERROR: Poll timed out ");
      break;
    }
    EventsProcessing();
    if (m_compress_arr_) {
      CompressArray();
    }
  } while (!m_shutdown_server_);
}

void tcp::Server::SigHandler(int signum) {
  std::cout << "\nSig exit" << std::endl;
  (void)signum;
  m_shutdown_server_ = false;
}

int main(int argc, char** argv) {
  if (argc == 2) {
    signal(SIGINT, tcp::Server::SigHandler);
    tcp::Server server(argv[1]);
    server.HandlingCycle();
  } else {
    std::cout << "You'd better type like this:\n\
    make server SERV_PORT='port' DB_IP='0.0.0.0' DB_PORT=3306\n";
  }
  return 0;
}