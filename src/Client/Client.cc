#include "Client.h"

/**
 * Client's constructor
 * @param own_fd fildes that has been accepted for this client
 * @param db_fd fildes that has been given to DB to work with this client
 * @param client_poll the client's instance of pollfd structure
 * @param db_poll the db's instance of pollfd structure
 */
tcp::Client::Client(int own_fd,  pollfd* client_poll)
    : m_fd_(own_fd),
      m_poll_(client_poll) {}

tcp::Client::~Client() {}

/**
 * Gets a fildes of client
 */
int tcp::Client::GetFd() { return m_fd_; }


/**
 * Gets pair of client's and DB's revents
 */
short tcp::Client::GetEvent() {
  return m_poll_->revents;
}

/**
 * Sets new fd for this client
 */
void tcp::Client::SetFd(int new_fd) { m_poll_->fd = new_fd; }

void tcp::Client::AddBody(const std::string& response) {
  m_response_body_ += response;
}

const std::string tcp::Client::GetBody() {
  return m_response_body_;
}

void tcp::Client::SetRequest(const std::string& request) {
  m_request_ = request;
}

const std::string tcp::Client::GetRequest() {
  return m_request_;
}

void tcp::Client::SetFileFD(int fd) {
  m_file_fd_ = fd;
}

int tcp::Client::GetOpenedFileFD() {
  return m_file_fd_;
}