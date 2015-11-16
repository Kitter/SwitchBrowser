//
// Created by sven on 11/16/15.
//

#include "switch.h"


namespace switchboard {

Switch::Switch() { }

Switch::~Switch() { }

size_t Switch::get_iftable(std::vector<Interface>& table) const {

  table.insert(table.end(),_iftable.cbegin(),_iftable.cend());
  return _iftable.size();
}

size_t Switch::get_arptable(std::vector<Arp>& table) const {
  table.insert(table.end(),_arptable.cbegin(),_arptable.cend());
  return _arptable.size();
}
size_t Switch::get_ipaddrtable(std::vector<IpAddr>& table) const {
  table.insert(table.end(),_ipaddrtable.cbegin(),_ipaddrtable.cend());
  return _ipaddrtable.size();
}


}

