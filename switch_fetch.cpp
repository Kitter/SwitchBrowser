//
// Created by sven on 11/17/15.
//

#include "switch_fetch.h"
#include "snmp_utils.h"
#include <cassert>
#include <thread>
#include <chrono>
#include <mutex>

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;

// base info
const std::string IF_DESCR_OID = ".1.3.6.1.2.1.2.2.1.2";
const std::string IF_PHYS_ADDR_OID = ".1.3.6.1.2.1.2.2.1.6";
const std::string IF_ADMIN_OID = ".1.3.6.1.2.1.2.2.1.7";
const std::string IFX_NAME_OID = "1.3.6.1.2.1.31.1.1.1.1";
const std::string IP_ADDR_TABLE_OID = ".1.3.6.1.2.1.4.20";

//rate info
const std::string IF_IN_OCT_OID = "1.3.6.1.2.1.31.1.1.1.6";
const std::string IF_IN_UCAST_OID = "1.3.6.1.2.1.31.1.1.1.7";
const std::string IF_IN_MCAST_OID = "1.3.6.1.2.1.31.1.1.1.8";
const std::string IF_IN_BCAST_OID = "1.3.6.1.2.1.31.1.1.1.9";
const std::string IF_OUT_OCT_OID = "1.3.6.1.2.1.31.1.1.1.10";
const std::string IF_OUT_UCAST_OID = "1.3.6.1.2.1.31.1.1.1.11";
const std::string IF_OUT_MCAST_OID = "1.3.6.1.2.1.31.1.1.1.12";
const std::string IF_OUT_BCAST_OID = "1.3.6.1.2.1.31.1.1.1.13";


const std::string IF_TABLE_OID = ".1.3.6.1.2.1.2.2";
const std::string IFXTABLE_OID = "1.3.6.1.2.1.31.1.1";

const std::string IPNET_TO_MEDIA_TABLE_OID = ".1.3.6.1.2.1.4.22";

const std::string IF_IN_OCTETS_OID = "1.3.6.1.2.1.2.2.1.10";
const std::string IF_OUT_OCTETS_OID = "1.3.6.1.2.1.2.2.1.16";
const std::string IF_SPEED_OID = "1.3.6.1.2.1.2.2.1.5";

const std::string RUIJIE_TEMP_OID = ".1.3.6.1.4.1.4881.1.1.10.2.1.1.23.1.5.1";
const std::string RUIJIE_MEMPOOL_OID = ".1.3.6.1.4.1.4881.1.1.10.2.35";
const std::string RUIJIE_MEMUTIL_OID = ".1.3.6.1.4.1.4881.1.1.10.2.35.1.1.1.3.1";

const std::string RUIJIE_CPU_OID = ".1.3.6.1.4.1.4881.1.1.10.2.36";
const std::string RUIJIE_CPU5SEC_OID = "1.3.6.1.4.1.4881.1.1.10.2.36.1.1.1.0";

const std::string CISCO_CPUUTIL_OID = ".1.3.6.1.4.1.9.2.1.56.0";
const std::string CISCO_MEMUTIL_OID = ".1.3.6.1.4.1.2011.10.2.6.1.1.1.1.6";
const std::string CISCO_MEMORY_POOL_OID = ".1.3.6.1.4.1.9.9.48.1.1"; //ciscoMemoryPool


const std::string ENT_PHYSICAL_CLASS_OID = "1.3.6.1.2.1.47.1.1.1.1.5"; //entPhysicalClass

const std::string HH3C_CPUUTIL_PRE_OID = "1.3.6.1.4.1.25506.2.6.1.1.1.1.6";
const std::string HH3C_MEMUTIL_PRE_OID = "1.3.6.1.4.1.25506.2.6.1.1.1.1.8";

const std::string SYS_UPTIME_OID = ".1.3.6.1.6.3.10.2.1.3.0";

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


struct InOutRate {
  std::string ifIndex{};
  std::string name;
  long recvBitsPerSec{0};
  long sentBitsPerSec{0};
  long recvPktsPerSec{0};
  long sentPktsPerSec{0};
  long ifHighSpeed{0};
};


namespace  {

  class MIBLoader {
  public:
    MIBLoader() noexcept {

      init_snmp("SWITCH Fetch");

#ifdef _WIN32

      char szFilePath[MAX_PATH + 1] = { 0 };
      GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
      (strrchr(szFilePath, ('\\')))[1] = 0;

      std::string strDBPath(szFilePath);
      // strDBPath += "local.db";
      std::string strcisco = strDBPath;
      std::string strruijie = strDBPath;
      std::string strh3c = strDBPath;

      strcisco += "mibs\\cisco-mibs";
      strruijie += "mibs\\ruijie-mibs";
      strh3c += "mibs\\h3c-mibs";
      add_mibdir(strcisco.c_str());
      add_mibdir(strruijie.c_str());
      add_mibdir(strh3c.c_str());
#else

      add_mibdir("../mibs/cisco-mibs");
      add_mibdir("../mibs/ruijie-mibs");
      add_mibdir("../mibs/h3c-mibs");
#endif
      read_all_mibs();
      printf("init snmp and loading all mibs.\r\n");

      SOCK_STARTUP;
    }

    ~MIBLoader(){
      shutdown_mib();
      SOCK_CLEANUP;
    }

  };

}

static std::string ifAdminStatus2str(int nType);


class SwitchFetcher {

public:
  SwitchFetcher(const std::string& ip,const std::string& community):_ip(ip) {

    try {
      snmp_sess_init(&_session);
      _session.version = SNMP_VERSION_2c;
      //session.peername = peer.c_str();
#ifdef _WIN32

      _session.peername = _strdup(ip.c_str());
#else
      _session.peername = strdup(ip.c_str());

#endif
      _session.community = (u_char *) community.c_str();
      _session.community_len = community.length();
      _session.timeout = 1000 * 500;
      _ss = snmp_sess_open(&_session);
    }catch (...) {
      _ss = nullptr;
    }

  }

  ~SwitchFetcher(){
    if(_ss) {
      snmp_sess_close(_ss);
    }
    free(_session.peername);
    snmp_close(&_session);

  }

  bool get_temperature(SwitchInfo::TYPE type, double& temp);
  bool get_cpu_usage(SwitchInfo::TYPE type, double& usage);
  bool get_mem_usage(SwitchInfo::TYPE type, double& usage);
  bool get_sys_uptime(long& uptime);

  size_t get_intf_usage_pairs(SwitchInfo::TYPE type,std::vector<std::pair<double, double>>& util_pairs);

  size_t get_intf_info_list(SwitchInfo::TYPE type,std::vector<InterfaceInfo>& info_list);

  size_t get_intf_base_info(SwitchInfo::TYPE type,std::vector<InterfaceBaseInfo>& base_info);

  size_t get_intf_rate_info(SwitchInfo::TYPE type,std::vector<InterfaceRateInfo>& rate_info);

protected:

  size_t get_intf_rate_table(SwitchInfo::TYPE type,nlohmann::json& table);

  bool get_double_with_one_oid(const std::string& oid, double& usage);

  size_t get_intf_inout_rate(std::map<std::string,InOutRate>&,const bool bits_only = true);

  size_t get_intf_rate_map(std::map<std::string,InOutRate>&);

  size_t get_arp_table(std::map<std::string,Arp>& arps);

  size_t get_ip_table(std::map<std::string,IpAddr>& ips);

  bool get_h3c_util_index(std::string& index);

  bool get_h3c_cpu_usage(double& usage);

  bool get_h3c_mem_usage(double& usage);

private:

  const std::string _ip;

  void* _ss{nullptr};

  netsnmp_session _session;

  const static MIBLoader _mib_loader; //loading mibs

  static std::mutex _mutex;
  static std::map<std::string,std::string> _cached_h3c_index;

};

const MIBLoader SwitchFetcher::_mib_loader{};
std::map<std::string,std::string> SwitchFetcher::_cached_h3c_index;
std::mutex SwitchFetcher::_mutex;


bool SwitchFetcher::get_temperature(SwitchInfo::TYPE type, double& temp) {
  bool ret = false;
  switch (type) {
    case SwitchInfo::H3C: {
      //ret = get_double_with_one_oid(H3C_CPUUTIL_OID, usage);
      break;
    }

    case SwitchInfo::RUIJIE:
      ret = get_double_with_one_oid(RUIJIE_TEMP_OID, temp);
      break;
    case SwitchInfo::CISCO:
      //ret = get_double_with_one_oid(CISCO_CPUUTIL_OID, usage);
      break;

    default:
      break;
  }
  return ret;
}

bool SwitchFetcher::get_cpu_usage(SwitchInfo::TYPE type, double& usage) {
  bool ret = false;
  switch (type) {
    case SwitchInfo::H3C: {
      //ret = get_double_with_one_oid(H3C_CPUUTIL_OID, usage);
      ret = get_h3c_cpu_usage(usage);
      break;
    }

    case SwitchInfo::RUIJIE:
      ret = get_double_with_one_oid(RUIJIE_CPU5SEC_OID, usage);
      break;
    case SwitchInfo::CISCO:
      ret = get_double_with_one_oid(CISCO_CPUUTIL_OID, usage);
      break;

    default:
      break;
  }
  return ret;
}

bool SwitchFetcher::get_mem_usage(SwitchInfo::TYPE type, double& usage) {
  bool ret = false;
  switch (type) {
    case SwitchInfo::H3C:
      ret = get_h3c_mem_usage(usage);
      break;
    case SwitchInfo::RUIJIE:
      ret = get_double_with_one_oid(RUIJIE_MEMUTIL_OID, usage);
      break;
    case SwitchInfo::CISCO: {

      try {
        if (_ss) {
          SNMPOPT opt(_ss);
          opt.oid = CISCO_MEMORY_POOL_OID;
          nlohmann::json table{};
          if(snmp_table(opt, table) > 0) {

            long fre{ 0 }, used{ 0 };

            for(const auto& row : table) {

              fre += row["ciscoMemoryPoolFree"].get<long>();

              used += row["ciscoMemoryPoolUsed"].get<long>();

            }
            if (fre != 0 || used != 0)
              usage = used*100.0 / (used + fre);
            ret = true;
          }

        }
      } catch (...) {
        ;
      }

      break;
    }


    default:
      break;
  }
  return ret;
}

bool SwitchFetcher::get_sys_uptime(long &uptime) {
  bool ret = false;

  if (_ss) {

    SNMPOPT opt(_ss);
    opt.name = "uptime";
    opt.oid = SYS_UPTIME_OID;
    nlohmann::json j;
    if(snmp_get(opt,j) == 0) {
      uptime = j["uptime"].get<long>();
    }
    ret = true;
  }

  return ret;
}



bool SwitchFetcher::get_double_with_one_oid(const std::string& oid,double& usage) {
  bool ret = false;

  if (_ss) {

    SNMPOPT opt(_ss);
    opt.name = "usage";
    opt.oid = oid;
    nlohmann::json j;
    if(snmp_get(opt,j) == 0) {
      usage = j["usage"].get<double>();
      ret = true;
    }

  }

  return ret;
}


//size_t SwitchFetcher::get_intf_rate_map(std::map<std::string,InOutRate>&) {
//  size_t rc = 0;
//
//  try {
//    if (_ss) {
//      SNMPOPT opt(_ss);
//      nlohmann::json ifxtable1{}, ifxtable2{};
//
//      opt.oid = IFXTABLE_OID;
//
//      auto ts1 = snmp_table(opt, ifxtable1);
//      auto start = get_time::now();
//
//      if (ts1 == 0) return rc;
//      std::this_thread::sleep_for(std::chrono::seconds(1));
//      auto ts2 = snmp_table(opt, ifxtable2);
//      auto end = get_time::now();
//      if (ts2 == 0) return rc;
//      if (ts1 != ts2) return rc;
//
//      auto diff = std::chrono::duration_cast<ms>(end - start).count();
//
//      std::cout <<diff;
//      for (size_t i = 0; i != ts1; ++i) {
//        auto key = ifxtable2[i]["_fake_index"].get<std::string>();
//        InOutRate rate;
//        rate.ifIndex = key;
//        rate.name = ifxtable2[i]["ifName"].get<std::string>();
//
//        if( not ifxtable2[i]["ifHCInOctets"].is_null()) {
//          rate.recvBitsPerSec = (ifxtable2[i]["ifHCInOctets"].get<long>() - ifxtable1[i]["ifHCInOctets"].get<long>())*1000/diff;
//        }
//
//        if(not ifxtable2[i]["ifHCOutOctets"].is_null()) {
//          rate.sentBitsPerSec = (ifxtable2[i]["ifHCOutOctets"].get<long>() - ifxtable1[i]["ifHCOutOctets"].get<long>())*1000/diff;
//        }
//
//
//        rc = ts1;
//
//      }
//    }
//  } catch (const std::exception& ex) {
//    rc = 0;
//  }
//
//  return rc;
//
//
//}

//      const std::string IF_IN_OCT_OID = "1.3.6.1.2.1.31.1.1.1.6";
//      const std::string IF_IN_UCAST_OID = "1.3.6.1.2.1.31.1.1.1.7";
//      const std::string IF_IN_MCAST_OID = "1.3.6.1.2.1.31.1.1.1.8";
//      const std::string IF_IN_BCAST_OID = "1.3.6.1.2.1.31.1.1.1.9";
//      const std::string IF_OUT_OCT_OID = "1.3.6.1.2.1.31.1.1.1.10";
//      const std::string IF_OUT_UCAST_OID = "1.3.6.1.2.1.31.1.1.1.11";
//      const std::string IF_OUT_MCAST_OID = "1.3.6.1.2.1.31.1.1.1.12";
//      const std::string IF_OUT_BCAST_OID = "1.3.6.1.2.1.31.1.1.1.13";

size_t SwitchFetcher::get_intf_rate_table(SwitchInfo::TYPE type,nlohmann::json& table) {
  size_t rc = 0;
  try {
    SNMPOPT opt(_ss);
    nlohmann::json subtree{};
    opt.oid = IF_IN_OCT_OID;
    rc = snmp_bulkwalk(opt, subtree);
    if (rc == 0) {return rc;}

    opt.oid = IF_IN_UCAST_OID;
    rc = snmp_bulkwalk(opt, subtree);
    opt.oid = IF_IN_MCAST_OID;
    rc = snmp_bulkwalk(opt, subtree);
    opt.oid = IF_IN_BCAST_OID;
    rc = snmp_bulkwalk(opt, subtree);
    opt.oid = IF_OUT_OCT_OID;
    rc = snmp_bulkwalk(opt, subtree);
    opt.oid = IF_OUT_UCAST_OID;
    rc = snmp_bulkwalk(opt, subtree);
    opt.oid = IF_OUT_MCAST_OID;
    rc = snmp_bulkwalk(opt, subtree);
    opt.oid = IF_OUT_BCAST_OID;
    rc = snmp_bulkwalk(opt, subtree);

    rc = columns_to_table(subtree, table);


  } catch (const std::exception& ex) {
    rc = 0;
  }

  return rc;
}

size_t SwitchFetcher::get_intf_inout_rate(std::map<std::string,InOutRate>& rates,const bool bits_only) {

  size_t rc = 0;

  try {
    if (_ss) {
      SNMPOPT opt(_ss);
      nlohmann::json ifxtable1{}, ifxtable2{};

      opt.oid = IFXTABLE_OID;

      auto ts1 = snmp_table(opt, ifxtable1);
      auto start = get_time::now();

      if (ts1 == 0) return rc;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      auto ts2 = snmp_table(opt, ifxtable2);
      auto end = get_time::now();
      if (ts2 == 0) return rc;
      if (ts1 != ts2) return rc;

      auto diff = std::chrono::duration_cast<ms>(end - start).count();

      std::cout <<diff;
      for (size_t i = 0; i != ts1; ++i) {
        auto key = ifxtable2[i]["_fake_index"].get<std::string>();
        InOutRate rate;
        rate.ifIndex = key;
        rate.name = ifxtable2[i]["ifName"].get<std::string>();
        rate.ifHighSpeed = ifxtable2[i]["ifHighSpeed"].get<long>() * 1000 * 1000;

        if( not ifxtable2[i]["ifHCInOctets"].is_null()) {
          rate.recvBitsPerSec = (ifxtable2[i]["ifHCInOctets"].get<long>() - ifxtable1[i]["ifHCInOctets"].get<long>())*1000/diff;
        }

        if(not ifxtable2[i]["ifHCOutOctets"].is_null()) {
          rate.sentBitsPerSec = (ifxtable2[i]["ifHCOutOctets"].get<long>() - ifxtable1[i]["ifHCOutOctets"].get<long>())*1000/diff;
        }

        if (not bits_only) {

          long recvPkts2{0},recvPkts1{0};
          if( not ifxtable2[i]["ifHCInUcastPkts"].is_null()) {
            recvPkts2 += ifxtable2[i]["ifHCInUcastPkts"].get<long>();
            recvPkts1 += ifxtable1[i]["ifHCInUcastPkts"].get<long>();
          }
          if( not ifxtable2[i]["ifHCInMulticastPkts"].is_null()) {
            recvPkts2 += ifxtable2[i]["ifHCInMulticastPkts"].get<long>();
            recvPkts1 += ifxtable1[i]["ifHCInMulticastPkts"].get<long>();
          }
          if( not ifxtable2[i]["ifHCInBroadcastPkts"].is_null()) {
            recvPkts2 += ifxtable2[i]["ifHCInBroadcastPkts"].get<long>();
            recvPkts1 += ifxtable1[i]["ifHCInBroadcastPkts"].get<long>();
          }

          rate.recvBitsPerSec = (recvPkts2 - recvPkts1)*1000/diff;


          long sentPkts2{0},sentPkts1{0};
          if( not ifxtable2[i]["ifHCOutUcastPkts"].is_null()) {
            sentPkts2 += ifxtable2[i]["ifHCOutUcastPkts"].get<long>();
            sentPkts1 += ifxtable1[i]["ifHCOutUcastPkts"].get<long>();
          }
          if( not ifxtable2[i]["ifHCOutMulticastPkts"].is_null()) {
            sentPkts2 += ifxtable2[i]["ifHCOutMulticastPkts"].get<long>();
            sentPkts1 += ifxtable1[i]["ifHCOutMulticastPkts"].get<long>();
          }
          if( not ifxtable2[i]["ifHCOutBroadcastPkts"].is_null()) {
            sentPkts2 += ifxtable2[i]["ifHCOutBroadcastPkts"].get<long>();
            sentPkts1 += ifxtable1[i]["ifHCOutBroadcastPkts"].get<long>();
          }
          rate.sentPktsPerSec = (sentPkts2 - sentPkts1)*1000/diff;
        }

        rates[key] = rate;
      }
      rc = ts1;

    }
  } catch (const std::exception& ex) {
    rc = 0;
  }

  return rc;

}

size_t SwitchFetcher::get_intf_usage_pairs(SwitchInfo::TYPE type,std::vector<std::pair<double, double>>& util_pairs) {

  size_t rc = 0;
  try{
    if (_ss) {

      SNMPOPT opt(_ss);
      nlohmann::json iftable{};

      opt.oid = IF_TABLE_OID;
      auto ts = snmp_table(opt, iftable);
      if (ts == 0) return rc;
      std::map<std::string,InOutRate> rates{};

      rc = get_intf_inout_rate(rates,true);

      if (rc == 0) {
        return rc;
      }
      util_pairs.clear();
      for (size_t i = 0; i != ts; ++i) {
        auto inout_util = std::make_pair(0.0, 0.0);

        auto index = iftable[i]["_fake_index"].get<std::string>();

        InOutRate rate;

        if (rates.find(index) != rates.end()) {
          rate = rates[index];
        }

        //        auto mac = iftable[i]["ifPhysAddress"].get<std::string>();
        //
        //        if (mac != "") {
        if(rate.ifHighSpeed == 0) {
          rate.ifHighSpeed = iftable[i]["ifSpeed"].get<long>();
        }

        if (rate.ifHighSpeed != 0) {
          inout_util.first = rate.recvBitsPerSec*800.0/rate.ifHighSpeed;
          inout_util.second = rate.sentBitsPerSec*800.0/rate.ifHighSpeed;
        } else {
          inout_util.first = 0;
          inout_util.second = 0;
        }


        if(inout_util.first > 100 || inout_util.second > 100) {
          printf("\n-------------------------\n");
          printf("index: %s current ifSpeed: %ld \n",index.c_str(),rate.ifHighSpeed);


          printf("recv: %f send: %f \n",inout_util.first,inout_util.second);
          printf("\n-------------------------\n");

        }

        if(inout_util.first < 100 && inout_util.second < 100)
          util_pairs.push_back(inout_util);

      }
    }

  }catch (...) {
    rc = -1;
  }

  return rc;


}

size_t SwitchFetcher::get_intf_info_list(SwitchInfo::TYPE type,std::vector<InterfaceInfo>& info_list) {

  size_t rc = 0;
  try{
    if (_ss) {

      SNMPOPT opt(_ss);
      nlohmann::json iftable{};

      opt.oid = IF_TABLE_OID;
      auto ts = snmp_table(opt, iftable);

      if (ts == 0) return rc;

      std::map<std::string,InOutRate> rates{};

      rc = get_intf_inout_rate(rates);

      if (rc == 0) return rc;

      std::map<std::string,IpAddr> ipaddrs{};

      get_ip_table(ipaddrs);

      info_list.clear();
      for (size_t i = 0; i != ts; ++i) {

        InterfaceInfo info;

        //get ip_table info
        auto index = iftable[i]["_fake_index"].get<std::string>();

        info.mac = iftable[i]["ifPhysAddress"].get<std::string>();

        info.desc = iftable[i]["ifDescr"].get<std::string>();

        info.status = ifAdminStatus2str(iftable[i]["ifAdminStatus"].get<int>());


        //get ifxtable info
        if (rates.find(index) != rates.end()) {
          auto rate = rates[index];
          info.sentBitsPerSec = rate.sentBitsPerSec;
          info.recvBitsPerSec = rate.recvBitsPerSec;
          info.sentPktsPerSec = rate.sentPktsPerSec;
          info.recvPktsPerSec = rate.recvPktsPerSec;
          info.name = rate.name;

        }

        // get ipaddr table info
        if (ipaddrs.find(index) != ipaddrs.end()) {

          info.ip = ipaddrs[index].ipAdEntAddr;
          info.mask = ipaddrs[index].ipAdEntNetMask;
          info.index = atol(index.c_str());
        }


        info_list.push_back(info);
      }
    }

  }catch (...) {
    rc = -1;
  }

  return rc;


}


size_t SwitchFetcher::get_intf_base_info(SwitchInfo::TYPE type,std::vector<InterfaceBaseInfo>& base_info) {
  size_t rc = 0;
  try{
    if (_ss) {

      SNMPOPT opt(_ss);
      nlohmann::json subtree{};

      opt.oid = IFX_NAME_OID;
      auto ts = snmp_bulkwalk(opt, subtree);
      if (ts == 0) return rc;

      opt.oid = IF_ADMIN_OID;
      ts = snmp_bulkwalk(opt, subtree);
      if (ts == 0) return rc;

      opt.oid = IF_PHYS_ADDR_OID;
      ts = snmp_bulkwalk(opt, subtree);
      if (ts == 0) return rc;

      opt.oid = IF_DESCR_OID;
      ts = snmp_bulkwalk(opt, subtree);
      if (ts == 0) return rc;

      nlohmann::json table;
      columns_to_table(subtree, table);

      std::map<std::string,IpAddr> ipaddrs{};

      get_ip_table(ipaddrs);

      base_info.clear();
      for (size_t i = 0; i != table.size(); ++i) {

        InterfaceBaseInfo info;


        info.desc = table[i]["ifDescr"].get<std::string>();

        auto index = table[i]["_fake_index"].get<std::string>();

        info.mac = table[i]["ifPhysAddress"].get<std::string>();

        info.status = ifAdminStatus2str(table[i]["ifAdminStatus"].get<int>());
        info.name = table[i]["ifName"].get<std::string>();


        // get ipaddr table info
        if (ipaddrs.find(index) != ipaddrs.end()) {

          info.ip = ipaddrs[index].ipAdEntAddr;
          info.mask = ipaddrs[index].ipAdEntNetMask;
          info.index = atol(index.c_str());
        }


        base_info.push_back(info);
        ++rc;
      }
    }

  }catch (...) {
    rc = -1;
  }

  return rc;



}

size_t SwitchFetcher::get_intf_rate_info(SwitchInfo::TYPE type,std::vector<InterfaceRateInfo>& rate_info){
  size_t rc = 0;
  try{
    if (_ss) {

      SNMPOPT opt(_ss);
      nlohmann::json table2{}, table1{};

      auto ts1 = get_intf_rate_table(type,table1);
      auto start = get_time::now();
      if (ts1 == 0) return rc;
      
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      auto ts2 = get_intf_rate_table(type,table2);
      auto end = get_time::now();
      if (ts2 != ts1) return rc;
      auto diff = std::chrono::duration_cast<ms>(end - start).count();

      for (size_t i = 0; i != ts2; ++i) {

        InterfaceRateInfo info;

        //get ip_table info
        auto index = table2[i]["_fake_index"].get<std::string>();

        if( not table2[i]["ifHCInOctets"].is_null()) {
          info.recvBitsPerSec = (table2[i]["ifHCInOctets"].get<long>() - table1[i]["ifHCInOctets"].get<long>())*8000.0/diff;
        }

        if(not table2[i]["ifHCOutOctets"].is_null()) {
          info.sentBitsPerSec = (table2[i]["ifHCOutOctets"].get<long>() - table1[i]["ifHCOutOctets"].get<long>())*8000.0/diff;
        }

        long recvPkts2{0},recvPkts1{0};
        if( not table2[i]["ifHCInUcastPkts"].is_null()) {
          recvPkts2 += table2[i]["ifHCInUcastPkts"].get<long>();
          recvPkts1 += table1[i]["ifHCInUcastPkts"].get<long>();
        }
        if( not table2[i]["ifHCInMulticastPkts"].is_null()) {
          recvPkts2 += table2[i]["ifHCInMulticastPkts"].get<long>();
          recvPkts1 += table1[i]["ifHCInMulticastPkts"].get<long>();
        }
        if( not table2[i]["ifHCInBroadcastPkts"].is_null()) {
          recvPkts2 += table2[i]["ifHCInBroadcastPkts"].get<long>();
          recvPkts1 += table1[i]["ifHCInBroadcastPkts"].get<long>();
        }

        info.recvBitsPerSec = (recvPkts2 - recvPkts1)*1000.0/diff;


        long sentPkts2{0},sentPkts1{0};
        if( not table2[i]["ifHCOutUcastPkts"].is_null()) {
          sentPkts2 += table2[i]["ifHCOutUcastPkts"].get<long>();
          sentPkts1 += table1[i]["ifHCOutUcastPkts"].get<long>();
        }
        if( not table2[i]["ifHCOutMulticastPkts"].is_null()) {
          sentPkts2 += table2[i]["ifHCOutMulticastPkts"].get<long>();
          sentPkts1 += table1[i]["ifHCOutMulticastPkts"].get<long>();
        }
        if( not table2[i]["ifHCOutBroadcastPkts"].is_null()) {
          sentPkts2 += table2[i]["ifHCOutBroadcastPkts"].get<long>();
          sentPkts1 += table1[i]["ifHCOutBroadcastPkts"].get<long>();
        }
        info.sentPktsPerSec = (sentPkts2 - sentPkts1)*1000.0/diff;

        
        info.index = atol(index.c_str());
        
        rate_info.push_back(info);
        ++rc;
      }
    }

  }catch (...) {
    rc = -1;
  }

  return rc;
}



std::string ifAdminStatus2str(int nType) {
  const std::string strType[] = {"", "up", "down", "testing"};
  std::string strret = "";
  if (nType >= 1 && nType <= 3) {
    strret = strType[nType];
  }
  return strret;
}

bool get_h3c_cpu_usage(void *ss, double& usage) {
  bool rc = false;

  try {

    if (not ss) return rc;

    SNMPOPT opt(ss);
    opt.oid = ENT_PHYSICAL_CLASS_OID;
    nlohmann::json j;
    if (snmp_walk(opt, j) > 0) {
      std::string index{};
      for (auto iter = j.cbegin(); iter != j.cend(); ++iter) {
        if (int(iter.value()) == 9) {
          index = std::string(iter.key());
          break;
        }
      }
      if (index.empty()) return rc;

      auto pos = index.find(".");
      if(pos != index.npos){

        opt.oid = HH3C_CPUUTIL_PRE_OID + index.substr(pos);

        opt.name = "usage";
        nlohmann::json u{};

        if (snmp_get(opt, u) == 0) {
          usage = u["usage"].get<double>();
          rc = true;
        }
      }
    }

  } catch (const std::exception& ex) {
    ;
  }
  return rc;
}

bool SwitchFetcher::get_h3c_cpu_usage(double& usage) {
  bool ret = false;
  std::string index{};
  if (get_h3c_util_index(index)) {
    return get_double_with_one_oid(HH3C_CPUUTIL_PRE_OID + index, usage);
  }
  return ret;

}

bool SwitchFetcher::get_h3c_mem_usage(double& usage) {
  bool ret = false;
  std::string index{};
  if (get_h3c_util_index(index)) {
    return get_double_with_one_oid(HH3C_MEMUTIL_PRE_OID + index, usage);
  }
  return ret;
}

bool SwitchFetcher::get_h3c_util_index(std::string& index){
  bool rc = false;
  if (_cached_h3c_index.find(_ip) != _cached_h3c_index.end()) {
    index = _cached_h3c_index[_ip];
    return !rc;
  }
  else {

    try {
      if (_ss) {
        SNMPOPT opt(_ss);
        opt.oid = ENT_PHYSICAL_CLASS_OID;
        nlohmann::json j;
        if (snmp_walk(opt, j) > 0) {
          std::string temp{};
          for (auto iter = j.cbegin(); iter != j.cend(); ++iter) {
            if (int(iter.value()) == 9) {
              temp = std::string(iter.key());
              break;
            }
          }
          if (temp.empty()) return rc;

          auto pos = temp.find(".");
          if(pos != temp.npos){
            index = temp.substr(pos);
            rc = true;
            std::unique_lock<std::mutex> t1(_mutex);
            _cached_h3c_index[_ip] = index;
          }
        }

      }

    }catch(...) {
      return false;
    }
    return rc;
  }

}



int get_switch_cpuutil(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       double& usage) {
  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);
    if (fetcher.get_cpu_usage(type, usage)) { ret = 0; }
  } catch (const std::exception& ex) {
    ret = 1;
  }
  return ret;
}

int get_switch_memutil(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       double& usage) {
  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);
    if (fetcher.get_mem_usage(type, usage)) { ret = 0; }
  } catch (const std::exception& ex) {
    ret = 1;
  }
  return ret;
}

int get_switch_uptime(const std::string& ip,
                      const std::string& community,
                      long& uptime) {
  int ret = 1;

  try {

    SwitchFetcher fetcher(ip, community);
    if (fetcher.get_sys_uptime(uptime)) { ret = 0; }
  }
  catch (const std::exception& ex) {
    ret = 1;
  }
  return ret;
}


int get_switch_temperature(const std::string& ip,
                           const std::string& community,
                           const SwitchInfo::TYPE type,
                           double& temp){

  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);
    if (fetcher.get_temperature(type, temp)) { ret = 0; }
  } catch (const std::exception& ex) {
    ret = 1;
  }
  return ret;
}


int get_switch_info(const std::string& ip,
                    const std::string& community,
                    const SwitchInfo::TYPE type,
                    SwitchInfo& info) {

  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);


    if (fetcher.get_cpu_usage(type, info.cpuUtil) &&
        fetcher.get_mem_usage(type, info.memUtil) &&
        fetcher.get_sys_uptime(info.sysUpTime) ) {

      std::vector<std::pair<double, double>> util_pairs;

      if (fetcher.get_intf_usage_pairs(type, util_pairs) > 0) {

        for (const auto& p : util_pairs) {

          if (info.intfHighInUtil < p.first) info.intfHighInUtil = p.first;

          if (info.intfHighOutUtil < p.second) info.intfHighOutUtil = p.second;

        }
        ret = 0;
      }
      info.ip = ip;
    }

  } catch (const std::exception& ex) {
    ret = 1;
  }


  return ret;
}


int get_interface_info(const std::string& ip,
                       const std::string& community,
                       const SwitchInfo::TYPE type,
                       std::vector<InterfaceInfo>& info_list) {

  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);

    ret = fetcher.get_intf_info_list(type, info_list);
    if (ret > 0) {
      ret = 0;
    }

  } catch (const std::exception& ex) {
    ret = 1;
  }

  return ret;

}


//if get success return 0;
int get_interfacebase_info(const std::string& ip,
                           const std::string& community,
                           const SwitchInfo::TYPE type,
                           std::vector<InterfaceBaseInfo>& base_info) {
  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);

    ret = fetcher.get_intf_base_info(type, base_info);
    if (ret > 0) {
      ret = 0;
    }

  } catch (const std::exception& ex) {
    ret = 1;
  }

  return ret;
}

//if get success return 0;
int get_interfacerate_info(const std::string& ip,
                           const std::string& community,
                           const SwitchInfo::TYPE type,
                           std::vector<InterfaceRateInfo>& rate_info) {
  int ret = 1;

  try {

    SwitchFetcher fetcher(ip,community);

    ret = fetcher.get_intf_rate_info(type, rate_info);
    if (ret > 0) {
      ret = 0;
    }

  } catch (const std::exception& ex) {
    ret = 1;
  }

  return ret;
}


size_t SwitchFetcher::get_ip_table(std::map<std::string,IpAddr>& ipaddrs) {
  size_t rc = 0;
  try {
    SNMPOPT opt(_ss);
    opt.oid = IP_ADDR_TABLE_OID;

    nlohmann::json ip_table;

    if (snmp_table(opt, ip_table) > 0) {
      IpAddr node_net;
      for (const auto &row : ip_table) {

        auto local_ip = row["ipAdEntAddr"].get<std::string>();
        //if(local_ip == "127.0.0.1") continue;

        node_net.ipAdEntAddr = local_ip;
        node_net.ipAdEntIfIndex = row["ipAdEntIfIndex"].get<long>();
        node_net.ipAdEntNetMask = row["ipAdEntNetMask"].get<std::string>();
        auto key = std::to_string(node_net.ipAdEntIfIndex);
        ipaddrs[key] = node_net;
        ++rc;
      }
    }

  }
  catch (const std::exception &e) {
    rc = 0;
  }
  catch (...) {
    rc = 0;
  }
  return rc;
}
