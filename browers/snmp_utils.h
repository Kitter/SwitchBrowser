//
// Created by sven on 11/3/15.
//

#pragma once

#include "json.hpp"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <string>


struct SNMPOPT {
  void		*session{nullptr};
  int			op_type{SNMP_MSG_GET};
  std::string oid{};
  std::string name{};
  SNMPOPT() = default;
  SNMPOPT(void *ss):session(ss){}
};


class Session {

public:
  Session(const std::string& peer,const std::string& pwd) {
    try {
      snmp_sess_init(&session);
      session.version = SNMP_VERSION_2c;
      //session.peername = peer.c_str();
      session.peername = strdup(peer.c_str());
      session.community = (u_char *) pwd.c_str();
      session.community_len = pwd.length();
      session.timeout = 1000 * 500;
      ss = snmp_sess_open(&session);
    }catch (...) {
      ss = nullptr;
    }
  }

  ~Session() {
    if(ss) {
      snmp_sess_close(ss);
    }
    free(session.peername);
    snmp_close(&session);
  }

  void* get_session() const { return ss;}

private:
  netsnmp_session session;
  void *ss{nullptr};
};


int snmp_walk(const SNMPOPT &opt, nlohmann::json &columns);
int snmp_bulkwalk(const SNMPOPT &opt, nlohmann::json &columns);
int snmp_get(const SNMPOPT &opt, nlohmann::json &values);
size_t snmp_table(const SNMPOPT&opt, nlohmann::json & table);


