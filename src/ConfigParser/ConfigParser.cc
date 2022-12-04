  // std::cout << data.dump();  // перевод всего джсона в одну строчку
  // обращаться можно через []

#include "ConfigParser.h"

ConfigParser::ConfigParser() {;}

ConfigParser::~ConfigParser() {;}

WSConfig ConfigParser::Parse(const std::string& conf_path) {
  std::fstream conf_file(conf_path);
  if (!conf_file.is_open()) {
    perror("Error: error occured while reading file : ");
    throw std::invalid_argument("Couldnt open config file.");
  }
  WSConfig curr_config;
  json_obj_ = json::parse(conf_file);
  curr_config.listening_ports = GetListeningPorts();

  return curr_config;
}

std::vector<int> ConfigParser::GetListeningPorts() {
  std::vector<int> ports;
  size_t port_count = 0;
  if (json_obj_["http"]["server"].contains("ports")) {
    port_count = json_obj_["http"]["server"]["ports"].size();
    for (int i = 1; i <= port_count; ++i) {
      std::string it = "listen" + std::to_string(i);
      ports.push_back(json_obj_["http"]["server"]["ports"][it]);  
    }
  } else {
    std::cout << "You havent type port in config, so I'll use :80 by default";
    ports.push_back(80);
  }
  return ports;
}

std::map<std::string, std::string> ConfigParser::GetLocations() {
  std::map<std::string, std::string> locations;
  size_t locations_count = 0;
  if (json_obj_["http"]["server"].contains("locations")) {
    locations_count = json_obj_["http"]["server"]["locations"].size();
    
  } else {
    throw std::invalid_argument("No locations at config file");
  }
}