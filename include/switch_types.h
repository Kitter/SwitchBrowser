//
// Created by sven on 11/16/15.
//

#ifndef COLLECTOR_SNMP_OIDS_H
#define COLLECTOR_SNMP_OIDS_H

#include <string>

const std::string IF_TABLE_OID = ".1.3.6.1.2.1.2.2";
const std::string IFXTABLE_OID = "1.3.6.1.2.1.31.1.1";
const std::string IP_ADDR_TABLE_OID = ".1.3.6.1.2.1.4.20";
const std::string IPNET_TO_MEDIA_TABLE_OID = ".1.3.6.1.2.1.4.22";

namespace switchboard {

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

}


#endif //COLLECTOR_SNMP_OIDS_H
