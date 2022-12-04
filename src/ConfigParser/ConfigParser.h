#ifndef SRC_CONFIGPARSER_CONFIGPARSER_H_
#define SRC_CONFIGPARSER_CONFIGPARSER_H_

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
// #include "/Users/padmemur/Desktop/WebServer/include/json/single_include/nlohmann/json.hpp"
#include <iostream>
#include "../../json.hpp"

struct WSConfig {
  std::vector<int> listening_ports;
  // 1st - location name, 2nd - path to location
  std::map<std::string, std::string> locations;
  // 1st - errnum, 2nd - path to static html page
  std::map<int, std::string> error_pages_names;
};

class ConfigParser {
 public:
  ConfigParser();
  ~ConfigParser();
  WSConfig Parse(const std::string& conf_path);


 private:
  std::vector<int> GetListeningPorts();
  std::map<std::string, std::string> GetLocations();

  using json = nlohmann::json;
  json json_obj_;
};

#endif  // SRC_CONFIGPARSER_CONFIGPARSER_H_