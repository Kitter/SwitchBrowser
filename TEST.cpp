//
// Created by sven on 10/30/15.
//


#include "switch_fetch.h"
//#include "snmp_utils.h"

#include <iostream>
#include <thread>

using namespace std;


int main() {

//  init_snmp("test");
//  add_mibdir("../mibs/cisco-mibs");
//  add_mibdir("../mibs/ruijie-mibs");
//  add_mibdir("../mibs/h3c-mibs");
//  read_all_mibs();

  string ip = "116.213.79.60";
  
  string pw = "cnlink";
  
  SwitchInfo::TYPE  t =  SwitchInfo::H3C;
  
  vector<InterfaceInfo> infolist;
  
  int i = 0;
  
  while (++i < 10) {
    cout << "-----------------------------------" << endl;
    if (get_interface_info(ip, pw, t, infolist) == 0) {
      for (const auto& info : infolist) {
        cout << info.ip << " " << info.desc << " " << info.mac << endl;
      }
    }
    
    std::this_thread::sleep_for(chrono::seconds(3));
  }
  
  
  
  
  
  //Session _session("210.73.204.6","Test");
//  Session _session("202.181.254.36","chinalink");
//
//  auto ss = _session.get_session();
//
//  if(!ss) {
//    cout << "error when init session. quit!" <<endl;
//    return 0;
//  }
//
//  SNMPOPT opt(ss);
//  opt.oid = "1.3.6.1.2.1.31.1.1";
//  opt.oid = ".1.3.6.1.2.1.2.2"; //if_table
//  
////  opt.oid = ".1.3.6.1.2.1.4.20";
////  opt.oid = ".1.3.6.1.2.1.4.22";
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
//
//  std::vector<IntfUtil> intflist;
//
//  cout <<"Get interface size: " << get_interface_util(ss,intflist) << endl;
//
//  for(const auto& intf : intflist) {
//    cout << "current intf ifIndex: " <<intf.ifIndex <<" recvBitsPerSec " << intf.recvBitsPerSec  << endl;
//  }
//
//  double memusage = 0;
//  get_ruijjie_mem_usage(ss,memusage);
//  cout << memusage << endl;
//
//  get_ruijjie_cpu_usage(ss,memusage);
//  cout << memusage << endl;
//  shutdown_mib();

  return 0;
  
}