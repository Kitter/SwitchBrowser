//
// Created by sven on 10/30/15.
//


#include "switch_fetch.h"
#include "snmp_utils.h"
#include <iostream>
#include <thread>

using namespace std;


int main() {
  
  string ip = "210.73.204.6";
  
  string pw = "Test";
  
  ip = "202.181.250.36";
  
  pw = "chinalink";
  
  SwitchInfo::TYPE  t =  SwitchInfo::H3C;  // only support H3C,RUIJIE,CISCO
  
  
  SwitchInfo info;
  
  
  int i = 0;
  
  while (++i < 10) {
    cout << "-----------------------------------" << endl;
    if (get_switch_info(ip, pw, t, info) == 0) {
      cout << info.ip << "   " << info.cpuUtil << "   " << info.memUtil << "   "  << info.sysUpTime ;
      cout << "   "  << info.intfHighInUtil << "   "  << info.intfHighOutUtil << endl;
    
    }
    
    std::this_thread::sleep_for(chrono::seconds(3));
  }

  return 0;
  
}