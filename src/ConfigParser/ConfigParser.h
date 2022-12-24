#ifndef SRC_CONFIGPARSER_CONFIGPARSER_H_
#define SRC_CONFIGPARSER_CONFIGPARSER_H_

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include "../../includes/json.hpp"

struct WSConfig {
  std::vector<int> listening_ports;
  // 1st - location name, 2nd - path to location
  std::map<std::string, std::string> locations;
  // 1st - errnum, 2nd - path to static html page
  std::map<int, std::string> error_pages_names;
  // будет ли отдаваться страница статуса localhost/status
  bool status_page_is_on;
};

class ConfigParser {
 public:
  ConfigParser();
  ~ConfigParser();
  WSConfig Parse(const std::string& conf_path);


 private:
  std::vector<int> GetListeningPorts();
  std::map<std::string, std::string> GetLocations();
  std::map<int, std::string> GetErrorPages();
  bool GetStatusPage();

  using json = nlohmann::json;
  json json_obj_;
};

#endif  // SRC_CONFIGPARSER_CONFIGPARSER_H_