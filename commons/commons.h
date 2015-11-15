//
// Created by sven on 7/22/15.
//

#ifndef COLLECTOR_UTILS_H
#define COLLECTOR_UTILS_H

#include "json.hpp"

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>


#include <string>
#include <fstream>
#include <sstream>
#include <vector>


const int IPV4 = 0x0001;
const int IPV6 = 0x0002;
const int IPALL = IPV4 | IPV6;





std::vector<std::string> get_local_ip(int);

/// simple implements for perfect-forwards
template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&... params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

bool init_logger(const std::string& path, const std::string& id) noexcept;

std::vector<std::string> split(const std::string &s, char delim);


std::vector<std::string> &split(const std::string &, char, std::vector<std::string> &);

inline std::string get_current_path() {
  std::string current_absolute_path;
  char current_absolute[256];
  if (NULL == getcwd(current_absolute, 256))
    return "";

  current_absolute_path = current_absolute;
  return (current_absolute_path + "/");
}



inline bool file_exists(const std::string &name) {
  return (access(name.c_str(), F_OK) != -1);
}

inline void mkdir_if_not_exists(const std::string &path) {
  if (!file_exists(path))
    mkdir(path.c_str(), 0755);
}

inline nlohmann::json get_json_from_file(const std::string &path) {
  nlohmann::json js;
  std::ifstream file(path);
  if (file) {
    std::stringstream ss;
    ss << file.rdbuf();
    js = nlohmann::json::parse(ss);
    file.close();
  }
  return js;
}


inline bool verify_ip_addr(const std::string &ip) {
  struct sockaddr_in sa;
  auto res = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
  return res != 0;
}


inline std::string daytime_string() {
  time_t now = time(nullptr);
  char buf[80];
  strftime(buf, sizeof(buf), "%F %T", std::localtime(&now));
  return std::string(buf);
}



#endif //COLLECTOR_UTILS_H
