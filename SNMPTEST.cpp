//
// Created by sven on 11/30/15.
//

#include "snmp_utils.h"
#include <iostream>


using namespace std;


int main() {

  string ip = "210.73.204.7";

  string pw = "Test";

//  ip = "202.181.254.36";
//
//  pw = "chinalink";
  init_snmp("SNMPTEST Fetch");
  //add_mibdir("../mibs/cisco-mibs");
  //add_mibdir("../mibs/ruijie-mibs");
  //add_mibdir("../mibs/h3c-mibs");
  read_all_mibs();
  printf("init snmp and loading all mibs.\r\n");
  const std::string RUIJIE_SYSPOOL_OID = ".1.3.6.1.2.1.17.4.3";
  Session sess(ip,pw);
  if(sess.get_session()) {
    SNMPOPT opt(sess.get_session());

    opt.oid = RUIJIE_SYSPOOL_OID;

    nlohmann::json j;

    if(snmp_walk(opt,j) >0) {
      cout << j.dump(1) << endl;
    }


  }


  shutdown_mib();
  return 0;

}