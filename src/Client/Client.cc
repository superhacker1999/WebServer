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