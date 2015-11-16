//
// Created by sven on 11/16/15.
//

#ifndef SWITCHBROWSER_SWITCH_H
#define SWITCHBROWSER_SWITCH_H

#include "switch_types.h"
#include "json.hpp"
#include <vector>

namespace switchboard {

class Switch {

public:
  Switch();
  ~Switch();

public:

  void set_iftable(const std::vector<Interface>& iftable) {_iftable = iftable;}
  void set_arptable(const std::vector<Arp>& arptable) {_arptable = arptable;}
  void set_ipaddrtable(const std::vector<IpAddr>& iptable) {_ipaddrtable = iptable;}

  size_t get_iftable(std::vector<Interface>& iftable) const;
  size_t get_arptable(std::vector<Arp>& arptable) const;
  size_t get_ipaddrtable(std::vector<IpAddr>& iptable) const;


private:

  std::string ip{};

  std::vector<Interface> _iftable;
  std::vector<Arp> _arptable;
  std::vector<IpAddr> _ipaddrtable;

};


}




#endif //COLLECTOR_NETWORK_FETCH_H
