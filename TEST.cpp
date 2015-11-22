//
// Created by sven on 10/30/15.
//


#include "switch_fetch.h"

#include <iostream>
#include <thread>

using namespace std;


int main() {


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

  return 0;
  
}