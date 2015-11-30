//
// Created by sven on 11/17/15.
//

#ifndef SWITCHBROWSER_SWITCH_FETCH_H
#define SWITCHBROWSER_SWITCH_FETCH_H

#include <string>
#include <vector>


struct SwitchInfo {
  
  enum TYPE {
    H3C, RUIJIE,CISCO //,LINUX,JUNIPER
  };
  TYPE type{H3C};
  std::string name{};
  std::string ip{};
  double cpuUtil{0.0};
  double memUtil{0.0};
  double intfHighInUtil{0.0};
  double intfHighOutUtil{0.0};
  long sysUpTime{0};
  
};

struct InterfaceInfo {
  
  std::string name{};
  std::string ip{};
  std::string desc{};
  std::string mask{};
  std::string mac{};
  std::string status;
  double recvBitsPerSec{0.0};
  double sentBitsPerSec{0.0};
  double recvPktsPerSec{0.0};
  double sentPktsPerSec{0.0};
  
};

int get_switch_cpuutil(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       double& usage);

int get_switch_memutil(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       double& usage);

int get_switch_temperature(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       double& temp);

//if get success return 0;
int get_switch_info(const std::string& ip,
                    const std::string& community,
                    const SwitchInfo::TYPE type,
                    SwitchInfo& info);

//if get success return 0;
int get_interface_info(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       std::vector<InterfaceInfo>& if_list);




#endif //SWITCHBROWSER_SWITCH_FETCH_H
