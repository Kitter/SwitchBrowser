//
// Created by sven on 10/30/15.
//


#include "switch_fetch.h"
#include "snmp_utils.h"

#include <iostream>

using namespace std;


int main() {

  init_snmp("test");
  add_mibdir("../lib/juniper-mibs/snmpv2");
  read_all_mibs();

  Session _session("210.73.204.7","Test");

  auto ss = _session.get_session();

  if(!ss) {
    cout << "error when init session. quit!" <<endl;
    return 0;
  }

//  SNMPOPT opt(ss);
//  opt.oid = "1.3.6.1.2.1.31.1.1";
//  opt.oid = ".1.3.6.1.2.1.2.2"; //if_table
//
//  nlohmann::json j;
//  cout << snmp_table(opt,j) <<endl;
//
//  cout << j.dump(1);

//  std::vector<Interface> intflist;
//
//  cout <<"Get interface size: " << get_if_table(ss,intflist) << endl;
//
//  for(const auto& intf : intflist) {
//    cout << "current intf ifIndex: " <<intf.ifIndex <<" ifDescr " << intf.ifDescr  << " ifPhysAddress: " << intf.ifPhysAddress << endl;
//  }

  std::vector<IntfUtil> intflist;

  cout <<"Get interface size: " << get_interface_util(ss,intflist) << endl;

  for(const auto& intf : intflist) {
    cout << "current intf ifIndex: " <<intf.ifIndex <<" recvBitsPerSec " << intf.recvBitsPerSec  << endl;
  }

  shutdown_mib();

  return 0;
  
}