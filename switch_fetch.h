//
// Created by sven on 11/17/15.
//

#ifndef SWITCHBROWSER_SWITCH_FETCH_H
#define SWITCHBROWSER_SWITCH_FETCH_H
#include <string>
#include <vector>

enum class SWitch {
  H3C,RUIJIE
};


struct Interface {
  // if table
  long ifIndex;
  std::string ifDescr;
  long ifType;
  long ifSpeed;
  std::string ifPhysAddress;
  long ifAdminStatus;
  long ifOperStatus;
  long ifLastChange;

//  long ifMtu;
//  long ifInOctets;
//  long ifInUcastPkts;
//  long ifInNUcastPkts;
//  long ifInDiscards;
//  long ifInErrors;
//  long ifInUnknownProtos;
//  long ifOutOctets;
//  long ifOutUcastPkts;
//  long ifOutNUcastPkts;
//  long ifOutDiscards;
//  long ifOutErrors;
//  long ifOutQLen;
//  std::string  ifSpecific;
};

struct IpAddr {
  std::string ipAdEntAddr;
  long ipAdEntIfIndex;
  std::string ipAdEntNetMask;
//  long ipAdEntBcastAddr;
//  long ipAdEntReasmMaxSize;
};


struct Arp {
  long ipNetToMediaIfIndex;
  std::string ipNetToMediaPhysAddress;
  std::string ipNetToMediaNetAddress;
  long ipNetToMediaType;
};

struct IntfUtil {
  std::string ifIndex{};
  long ifSpeed{0};
  long recvBitsPerSec{0};
  long sentBitsPerSec{0};
  double inIntfUtil{0.0};
  double outIntfUtil{0.0};
  long recvPkts{0};
  long sentPkts{0};
};


int get_arp_table(void*, std::vector<Arp>& arp_list);

int get_ip_table(void*, std::vector<IpAddr>& ip_list);

int get_if_table(void*, std::vector<Interface>& if_list);

int get_mem_usage(void *, SWitch type, double&);

int get_cpu_usage(void *, SWitch type, double&);

int get_interface_util(void *, std::vector<IntfUtil>& util_list);


#endif //SWITCHBROWSER_SWITCH_FETCH_H
