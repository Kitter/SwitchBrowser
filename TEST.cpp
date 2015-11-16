//
// Created by sven on 10/30/15.
//

#include "json.hpp"
#include "snmp_utils.h"
#include <future>
#include <iostream>

using namespace std;
using namespace snmp;

int get_if_table() {

  Session _session("210.73.204.6","Test");

  auto ss = _session.get_session();

  if(!ss) {
    cout << "error when init session. quit!" <<endl;
    return 0;
  }
  SNMPOPT opt(ss);
  nlohmann::json table;
  opt.name = "iftable";
  opt.oid = ".1.3.6.1.2.1.2.2";
  //opt.oid = "1.3.6.1.2.1.31.1.1";
 // opt.oid = ".1.3.6.1.4.1.9.9.48.1.1";
  opt.oid =".1.3.6.1.2.1.4.22";


  cout << snmp_table(opt,table) << endl;
  //opt.oid = "1.3.6.1.2.1.1.1.0";
 // cout << snmp_get(opt,table) << endl;
  cout << table.dump(1) << endl;
  return 100;
}

void lasy_print(int i) {
  this_thread::sleep_for(chrono::seconds(5));
  cout << "int: " << i << endl;
}

int main() {

  //init_snmp("v7 test");
  netsnmp_init_mib();


  //add_mibdir("../lib/juniper-mibs/snmpv2");

  read_all_mibs();




  auto fu = std::async(std::launch::async,&get_if_table);



  while (fu.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
    cout << "....." << endl;
  }

 cout << fu.get();

  shutdown_mib();

  return 0;
  
}