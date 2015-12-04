//
// Created by sven on 11/17/15.
//

#include "switch_fetch.h"
#include "snmp_utils.h"
#include <cassert>
#include <thread>

const std::string IF_TABLE_OID = ".1.3.6.1.2.1.2.2";
const std::string IFXTABLE_OID = "1.3.6.1.2.1.31.1.1";
const std::string IP_ADDR_TABLE_OID = ".1.3.6.1.2.1.4.20";
const std::string IPNET_TO_MEDIA_TABLE_OID = ".1.3.6.1.2.1.4.22";

const std::string RUIJIE_MEMPOOL_OID = ".1.3.6.1.4.1.4881.1.1.10.2.35";
const std::string RUIJIE_MEMUTIL_OID = ".1.3.6.1.4.1.4881.1.1.10.2.35.1.1.1.3.1";

const std::string RUIJIE_CPU_OID = ".1.3.6.1.4.1.4881.1.1.10.2.36";
const std::string RUIJIE_CPU5SEC_OID = "1.3.6.1.4.1.4881.1.1.10.2.36.1.1.1.0";


int get_arp_table(void* ss, std::vector<Arp>& arp_list){
  int nRet = 0;
  try {

    SNMPOPT opt(ss);
    opt.oid = IPNET_TO_MEDIA_TABLE_OID;
    nlohmann::json arp_able{};

    if (snmp_table(opt, arp_able) > 0) {
      Arp arp;
      for (const auto &row : arp_able) {
        arp.ipNetToMediaIfIndex = row["ipNetToMediaIfIndex"].get<long>();
        arp.ipNetToMediaPhysAddress = row["ipNetToMediaPhysAddress"].get<std::string>();
        arp.ipNetToMediaNetAddress = row["ipNetToMediaNetAddress"].get<std::string>();
        arp.ipNetToMediaType = row["ipNetToMediaType"].get<long>();
        arp_list.push_back(arp);
        nRet +=1;
      }
    }
  }
  catch (const std::exception &e) {
    nRet = -1;
  }
  catch (...) {
    nRet = -1;
  }
  return nRet;

}

int get_ip_table(void* ss, std::vector<IpAddr>& ip_list) {
  int nRet = 0;
  try {

    SNMPOPT opt(ss);
    opt.oid = IP_ADDR_TABLE_OID;

    nlohmann::json ip_table;

    if (snmp_table(opt, ip_table) > 0) {
      IpAddr node_net;
      for (const auto &row : ip_table) {

        auto local_ip = row["ipAdEntAddr"].get<std::string>();
        if(local_ip == "127.0.0.1") continue;

        node_net.ipAdEntAddr = local_ip;
        node_net.ipAdEntIfIndex = row["ipAdEntIfIndex"].get<long>();
        node_net.ipAdEntNetMask = row["ipAdEntNetMask"].get<std::string>();
        ip_list.push_back(node_net);
        nRet +=1;
      }
    }

  }
  catch (const std::exception &e) {
    nRet = -1;
  }
  catch (...) {
    nRet = -1;
  }
  return nRet;
}

int get_if_table(void* ss, std::vector<Interface>& if_list) {
  int nRet = 0;
  try {

    SNMPOPT opt(ss);
    opt.oid = IF_TABLE_OID;
    nlohmann::json if_table{};

    if (snmp_table(opt, if_table) > 0) {
      Interface node_intf;
      for (const auto &row : if_table) {
        node_intf.ifIndex = row["ifIndex"].get<long>();
        node_intf.ifDescr = row["ifDescr"].get<std::string>();
        node_intf.ifSpeed = row["ifSpeed"].get<long>();
        node_intf.ifPhysAddress = row["ifPhysAddress"].get<std::string>();
        if_list.push_back(node_intf);
        nRet +=1;
      }
    }

  }
  catch (const std::exception &e) {
    nRet = -1;
  }
  catch (...) {
    nRet = -1;
  }
  return nRet;
}

int get_interface_util(void * ss, std::vector<IntfUtil>& util_list){
  int nRet = 0;
  try{

    SNMPOPT opt(ss);
    nlohmann::json iftable{}, ifxtable1{}, ifxtable2{};

    opt.oid = IF_TABLE_OID;
    auto ts = snmp_table(opt, iftable);
    if (ts == 0) return -1;

    opt.oid = IFXTABLE_OID;

    auto ts1 = snmp_table(opt, ifxtable1);
    if (ts1 == 0) return -1;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto ts2 = snmp_table(opt, ifxtable2);
    if (ts2 == 0) return -1;

    if(ts1 != ts2) return -1;

    std::map<std::string,IntfUtil> utils_map{};

    for (size_t i = 0; i != ts1; ++i) {
      auto key = ifxtable2[i]["_fake_index"].get<std::string>();
      IntfUtil base;

      if( not ifxtable2[i]["ifHCInOctets"].is_null()) {
        base.recvBytes = ifxtable2[i]["ifHCInOctets"].get<long>();
        base.recvBitsPerSec = base.recvBytes - ifxtable1[i]["ifHCInOctets"].get<long>();
      }

      if(not ifxtable2[i]["ifHCOutOctets"].is_null()) {
        base.sentBytes = ifxtable2[i]["ifHCOutOctets"].get<long>();
        base.sentBitsPerSec = base.sentBytes - ifxtable1[i]["ifHCOutOctets"].get<long>();
      }

      if( not ifxtable2[i]["ifHCInUcastPkts"].is_null())
        base.recvPkts += ifxtable2[i]["ifHCInUcastPkts"].get<long>();
      if( not ifxtable2[i]["ifHCInMulticastPkts"].is_null())
        base.recvPkts += ifxtable2[i]["ifHCInMulticastPkts"].get<long>();
      if( not ifxtable2[i]["ifHCInBroadcastPkts"].is_null())
        base.recvPkts += ifxtable2[i]["ifHCInBroadcastPkts"].get<long>();


      if( not ifxtable2[i]["ifHCOutUcastPkts"].is_null())
        base.sentPkts += ifxtable2[i]["ifHCOutUcastPkts"].get<long>();
      if( not ifxtable2[i]["ifHCOutMulticastPkts"].is_null())
        base.sentPkts += ifxtable2[i]["ifHCOutMulticastPkts"].get<long>();
      if( not ifxtable2[i]["ifHCOutBroadcastPkts"].is_null())
        base.sentPkts += ifxtable2[i]["ifHCOutBroadcastPkts"].get<long>();

      utils_map[key] = base;
    }


    for (size_t i = 0; i != ts; ++i) {
      IntfUtil util;
      util.ifIndex = iftable[i]["_fake_index"].get<std::string>();
      if(utils_map.find(util.ifIndex) != utils_map.end()) {
        util = utils_map[util.ifIndex];
      }
      util.ifSpeed = iftable[i]["ifSpeed"].get<long>();
     util_list.push_back(util);
      ++nRet;
    }


  }catch (...) {
    nRet = -1;
  }

  return nRet;

}


int get_ruijjie_mem_usage(void*ss , double& usage) {
  int nRet = 0;
  try{
    SNMPOPT opt(ss);
    opt.name = "memutil";
    opt.oid = RUIJIE_MEMUTIL_OID;
    nlohmann::json j;

    if(snmp_get(opt,j) == 0) {
      usage = j["memutil"].get<double>();
    }

  }catch (...) {
    nRet = -1;
  }

  return nRet;
}

int get_ruijjie_cpu_usage(void* ss, double& usage) {
  int nRet = 0;
  try{
    SNMPOPT opt(ss);
    opt.name = "cpuutil";
    opt.oid = RUIJIE_CPU5SEC_OID;
    nlohmann::json j;

    if(snmp_get(opt,j) == 0) {
      usage = j["cpuutil"].get<double>();
    }

  }catch (...) {
    nRet = -1;
  }

  return nRet;
}

int get_mem_usage(void * ss, const SWitch type, double& usage){
  int nRet = 0;
  try{

  }catch (...) {
    nRet = -1;
  }

  return nRet;
}

int get_cpu_usage(void * ss, const SWitch type, double& usage){
  int nRet = 0;
  try{

  }catch (...) {
    nRet = -1;
  }

  return nRet;
}
