#include "RequestHandler.h"

// std::pair<std::string, std::string> http::GetResponse(const std::string& request, const WSConfig& config) {
//   if (request.compare(9, 4, "GET /")) {
//     std::cout << "This is a get request";
//   }
//   return {"NULL", "NULL"};
// }

// возвращает статус. все ли записано?
// если все, необходимо подготовить хедер и отпрваить
// если не все, закинуть клиента в очередь для дальнешей обработки
int http::RequestHandler::ProcessRequest(tcp::Client& curr_client, const WSConfig& cfg) {
  std::cout << "starting to process request from client with fd = " << curr_client.GetFd() << std::endl;
  int file_fd, status = RS_OK;
  char buffer[BUFF_LEN]{};
  // запрос верный, нужно открыть под него файл
  if (curr_client.GetRequest().compare(9, 4, "GET /")) {
    std::cout << "request is ok, starting to process\n";
    std::string request = curr_client.GetRequest();
    request.erase(0, request.find("GET /") + 4);
    request.erase(request.find(' '), request.size());
    request += ".html";
    // если файл еще не открыт
    if (curr_client.GetOpenedFileFD() < 0) {
      // открываем файл
      file_fd = open((cfg.locations.at("/") + request).c_str(), O_RDONLY);
      // если не открылся, значит у нас нет такой страницы как в запросе
      // возвращаем 404
      if (file_fd < 0) {
        curr_client.AddBody(Get404(cfg));
        status = RS_NOT_FOUND;
      // если открылся, читаем BUFF_LEN символов
      } else {
        status = read(file_fd, buffer, BUFF_LEN);
        // если прочли EOF нужно:
        // отчистить запрос, закрыть fd, вернуть OK
        if (status == 0) {
          status = RS_OK;
          close(file_fd);
          curr_client.SetFileFD(-1);
          curr_client.SetRequest("");
        } else if (status < (int)BUFF_LEN) {
          status = RS_OK;
          close(file_fd);
          curr_client.AddBody(buffer);
          curr_client.SetRequest("");
          curr_client.SetFileFD(-1);
        } else {
          curr_client.AddBody(buffer);
          status = RS_OK_NEED_NEXT;
          curr_client.SetFileFD(file_fd);
        }
      }
      // если файл уже открыт (это не первый цикл ответа)
    } else {
      file_fd = curr_client.GetOpenedFileFD();
      status = read(file_fd, buffer, BUFF_LEN);
      // если прочли EOF нужно:
      // отчистить запрос, закрыть fd, вернуть OK
      if (status == 0) {
        status = RS_OK;
        close(file_fd);
        curr_client.SetRequest("");
        curr_client.SetFileFD(-1);
      } else if (status < (int)BUFF_LEN) {
          status = RS_OK;
          close(file_fd);
          curr_client.AddBody(buffer);
          curr_client.SetRequest("");
          curr_client.SetFileFD(-1);
      } else {
        curr_client.AddBody(buffer);
        status = RS_OK_NEED_NEXT;
      }
    }
  } else {
    status = RS_BAD_REQUEST;
  }
  return status;
}

const std::string http::RequestHandler::Get404(const WSConfig& cfg) {
  int           fd = 0;
  char          buf[BUFF_LEN];
  std::string   body;
  if (cfg.error_pages_names.find(404) != cfg.error_pages_names.end()) {
    fd = open(cfg.error_pages_names.at(404).c_str(), O_RDONLY);
    if (fd < 0) {
      body = def_404;
    } else {
      while (read(fd, buf, BUFF_LEN) > 0) {
        body.append(buf);
      }
      close(fd);
    }
  } else {
    body = def_404;
  }
  return body;
}