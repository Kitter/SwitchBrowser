//
// Created by sven on 10/30/15.
//


#include "switch_fetch.h"
#include <iostream>
#include <thread>

using namespace std;


int main() {
  
  string ip = "210.73.204.6";
  
  string pw = "Test";
  
//  ip = "116.213.79.50";
//
//  pw = "cnlink";
  
  SwitchInfo::TYPE  t =  SwitchInfo::RUIJIE;  // only support H3C,RUIJIE,CISCO
  
  
  SwitchInfo info;
  std::vector<InterfaceBaseInfo> base_info;
  std::vector<InterfaceRateInfo> rate_info;
  std::vector<Arp> arp_info;
  std::vector<Dot1dTpFdbTable> dots_info;

  if(get_interfacebase_info(ip, pw, t, base_info) == 0) cout << "get base info success " << endl;
  if(get_interfacerate_info(ip, pw, t, rate_info) == 0) cout << "get rate info success " << endl;


  if(get_arp_list(ip, pw, t, arp_info) == 0) cout << "get arp_info info success " << endl;
  if(get_dot1_list(ip, pw, t, dots_info) == 0) cout << "get dots_info info success " << endl;
  
  int i = 0;
  
  while (++i < 10) {
    cout << "-----------------------------------" << endl;
    double usage;
    if(get_switch_cpuutil(ip,pw,t,usage) == 0) cout << "ruijie cpuutil: " << usage << endl;
    if(get_switch_memutil(ip,pw,t,usage) == 0) cout << "ruijie memuti: " << usage << endl;
    if(get_switch_temperature(ip,pw,t,usage) == 0) cout << "ruijie te: " << usage << endl;
    if (get_switch_info(ip, pw, t, info) == 0) {
      cout << info.ip << "   " << info.cpuUtil << "   " << info.memUtil << "   "  << info.sysUpTime ;
      cout << "   "  << info.intfHighInUtil << "   "  << info.intfHighOutUtil << endl;
    
    }
    
    std::this_thread::sleep_for(chrono::seconds(3));
  }

  return 0;
  
}