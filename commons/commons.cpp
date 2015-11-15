//
// Created by sven on 7/22/15.
//

#include "commons.h"
#include "spdlog/spdlog.h"
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

using std::string;




std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }
  return elems;
}


bool init_logger(const std::string& path, const std::string& id) noexcept {
  
  //throw out exception if init_logger failed.
  bool ret = false;
  
  auto j = get_json_from_file(path);
  
  for (const auto& conf : j) {
    if (id == conf["id"].get<std::string>()) {
      
      auto filename = conf["file"].get<std::string>();
      auto max_file_size = conf["maxFileSize"].get<long>();
      auto max_index = conf["maxBackupIndex"].get<int>();
      
#ifdef NDEBUG
      auto file_logger = spdlog::rotating_logger_mt(id, filename, max_file_size, max_index,true);
#else
      auto file_logger = spdlog::rotating_logger_mt(id, filename, max_file_size, max_index,true);
#endif
      
      spdlog::set_pattern(conf["pattern"].get<std::string>());
      
      std::string level = conf["level"].get<std::string>();
      
      int l = 0;
      
      do {
        if (level == spdlog::level::level_names[l]) break;
      }
      while (++l < 10);
      
      if(l < 10 ) {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(l));
        ret = true;
      }
      break;
    }
    
  }
  
  return ret;
}


std::vector<std::string> get_local_ip(int flag) {

  std::vector<std::string> ips;

  struct ifaddrs *ifaddr, *ifa;

  int family, s, n;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1) {
    return ips;
  }
  if (flag & IPV4) {

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
      if (ifa->ifa_addr == NULL)
        continue;

      family = ifa->ifa_addr->sa_family;
      if (family == AF_INET) {
        s = getnameinfo(ifa->ifa_addr,
                        sizeof(struct sockaddr_in),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
          printf("getnameinfo() failed: %s\n", gai_strerror(s));
          return ips;
        }
        ips.push_back(std::string(host));
      }
    }
    freeifaddrs(ifaddr);


  } else if (flag & IPV6) {
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
      if (ifa->ifa_addr == NULL)
        continue;

      family = ifa->ifa_addr->sa_family;
      if (family == AF_INET || family == AF_INET6) {
        s = getnameinfo(ifa->ifa_addr,
                        sizeof(struct sockaddr_in6),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
          printf("getnameinfo() failed: %s\n", gai_strerror(s));
          return ips;
        }
        ips.push_back(std::string(host));
      }
    }
    freeifaddrs(ifaddr);

  }
  else if (flag & IPALL) {
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
      if (ifa->ifa_addr == NULL)
        continue;

      family = ifa->ifa_addr->sa_family;
      if (family == AF_INET || family == AF_INET6) {
        s = getnameinfo(ifa->ifa_addr,
                        (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
          printf("getnameinfo() failed: %s\n", gai_strerror(s));
          return ips;
        }
        ips.push_back(std::string(host));
      }
    }
    freeifaddrs(ifaddr);

  }
  return ips;

}







