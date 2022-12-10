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
  curr_config.locations = GetLocations();
  curr_config.error_pages_names = GetErrorPages();
  curr_config.status_page_is_on = GetStatusPage();
  return curr_config;
}

std::vector<int> ConfigParser::GetListeningPorts() {
  std::vector<int> ports;
  size_t port_count = 0;
  if (json_obj_["http"]["server"].contains("ports")) {
    port_count = json_obj_["http"]["server"]["ports"].size();
    for (size_t i = 1; i <= port_count; ++i) {
      std::string it = "listen" + std::to_string(i);
      ports.push_back(json_obj_["http"]["server"]["ports"][it]);  
    }
  } else {
    std::cout << "There is no listening port in config, so I'll use :80 by default";
    ports.push_back(80);
  }
  return ports;
}

std::map<std::string, std::string> ConfigParser::GetLocations() {
  std::map<std::string, std::string> locations;
  if (json_obj_["http"]["server"].contains("locations")) {
    auto obj = json_obj_["http"]["server"]["locations"];
    for (auto it = obj.begin(); it != obj.end(); ++it) {
      std::pair<std::string, std::string> location = {
        it.key(), it.value()["root"]
      };
      locations.insert(location);  
    }
  } else {
    throw std::invalid_argument("No locations at config file");
  }
  return locations;
}

std::map<int, std::string> ConfigParser::GetErrorPages() {
  std::map<int, std::string> error_pages;
  if (json_obj_["http"]["server"].contains("error_pages")) {
    auto obj = json_obj_["http"]["server"]["error_pages"];
    for (auto it = obj.begin(); it != obj.end(); ++it) {
      std::pair<int, std::string> error_page {
        std::stoi(it.key()), it.value()["path"]
      };
      error_pages.insert(error_page);
    }
  }
  // if there no err pages, then server will just return http header
  // with err code to client
  return error_pages;
}

bool ConfigParser::GetStatusPage() {
  bool status_page = false;
  if (json_obj_["http"]["server"].contains("status_page")) {
    if (json_obj_["http"]["server"]["status_page"] == "on") {
      status_page = true;
    }
  }
  return status_page;
}
