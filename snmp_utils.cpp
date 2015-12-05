//
// Created by sven on 11/3/15.
//



#include "snmp_utils.h"
#include <set>
#include <net-snmp/types.h>

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else

#include <arpa/inet.h>

#endif

//sometimes the octet string contains all printable charachters and this produces unexpected output when it is
//not translated.

const std::string ifPhysAddress = "1.3.6.1.2.1.2.2.1.6";
const std::string ipNetToMediaPhysAddress = ".1.3.6.1.2.1.4.22.1.2";
const std::string dotPhysAddres = ".1.3.6.1.2.1.17.4.3.1.1";

inline static void to_index_name(std::string& name) {
  auto pos = name.find(".");
  if(pos != name.npos) {
    name = name.substr(pos +1);
  }
}

static bool match_phys_address(const std::string& oid) {
  return (oid.find(ifPhysAddress) != oid.npos ||
      oid.find(ipNetToMediaPhysAddress) != oid.npos ||
      oid.find(dotPhysAddres) != oid.npos);
}

static std::string snmp_hex_to_string(const struct variable_list *pvars) {
  std::string str;
  char buf[MAX_OID_LEN] = {0};
  for (unsigned int i = 0; i < pvars->val_len ; ++i) {
    snprintf(buf,MAX_OID_LEN,"%02X:",pvars->buf[i]) ;
    str +=buf;
  }
  str = str.substr(0,str.length()-1);
  return str;
}

static std::string oid_to_string(const oid *poid, int len) {
  char str[MAX_OID_LEN] = {0};
  char tmp[128] = {0};
  for (long i = 0; i < len; ++i) {
    snprintf(tmp, sizeof(tmp), ".%lu", poid[i]);
    strcat(str, tmp);
  }
  return std::string(str);
}

static int parser_snmp_values(const struct variable_list *pvars, nlohmann::json &js) {
  int nRet = 0;
  try {

    switch (pvars->type) {
      case ASN_INTEGER:
      case ASN_UNSIGNED:
      case ASN_TIMETICKS:
      case ASN_COUNTER:
      case ASN_UINTEGER: {
        js.push_back(*pvars->val.integer);

      }
        break;

      case ASN_IPADDRESS: {
        uint32_t ipaddr;
        memcpy(&ipaddr, pvars->val.string, sizeof(ipaddr));
        struct in_addr ip;
        ip.s_addr = ipaddr;
        js.push_back(std::string(inet_ntoa(ip)));

      }
        break;
      case ASN_PRIV_IMPLIED_OBJECT_ID:
      case ASN_PRIV_INCL_RANGE:
      case ASN_PRIV_EXCL_RANGE:
      case ASN_OBJECT_ID: {
        js.push_back(oid_to_string(pvars->name, pvars->name_length));
      }
        break;

      case ASN_FLOAT: {
        js.push_back(*pvars->val.floatVal);
      }
        break;
      case ASN_DOUBLE: {
        js.push_back(*pvars->val.doubleVal);
      }
        break;
      case ASN_OPAQUE_U64:
      case ASN_OPAQUE_I64:
      case ASN_COUNTER64:
      case ASN_OPAQUE_COUNTER64: {
        js.push_back(((pvars->val.counter64->high) << 32) + pvars->val.counter64->low);
      }
        break;

      case ASN_BIT_STR:
      case ASN_OCTET_STR:
      case ASN_OPAQUE:
      case ASN_NSAP:
      case ASN_PRIV_IMPLIED_OCTET_STR:{
        char strval[256];
        memset(strval, 0, 256);
        memcpy(strval, pvars->val.string, pvars->val_len);
        js.push_back(std::string(strval));

      }
        break;

      default:
        js.push_back(std::string());
        break;
    }
  }
  catch (const std::exception &e) {
    js.push_back(std::string("Exception! parserSNMPValue :") + e.what());
    return 1;
  }
  catch (...) {
    js.push_back("Unexception Error! parserSNMPValue");
    return 1;
  }
  return nRet;
}


static void device_to_col_and_index(const std::string& name,
                                    std::string& colname,
                                    std::string& index){
  auto pos = name.find(".");
  if(pos != name.npos) {
    colname = name.substr(0,pos);
    index = name.substr(pos + 1);
  } else {
    colname = name;
    index = "";
  }

}

static size_t find_in_vec(const std::vector<std::string>& vec, const std::string& name) {

  size_t i = 0; bool found = false;
  for (; i != vec.size(); ++i) {
    if (name == vec[i]) {
      found = true; break;
    }
  }
  if(found) return i; else return name.npos;
}

size_t columns_to_table(const nlohmann::json& columns, nlohmann::json& table) {

  if(columns.size() == 1) {
    table = columns;
    return 1;
  }

  std::vector<std::string> fakeindex;

  for(auto it = columns.cbegin();it != columns.cend(); ++it) {

    std::string colname, rowname ;
    device_to_col_and_index(it.key(),colname,rowname);

    auto rowindex = find_in_vec(fakeindex,rowname);

    //did not find
    if(rowindex == rowname.npos) {
      table.push_back(nlohmann::json::object());
      fakeindex.push_back(rowname);
      rowindex = table.size() - 1;
      table[rowindex]["_fake_index"] = rowname;
    }
    table[rowindex][colname] = it.value();
  }

  return table.size();

}

int snmp_get(const SNMPOPT &opt, nlohmann::json &result) {
  int nRet = 2;
  int nStatus = 0;
  std::string msg;
  snmp_pdu *pdu = nullptr;
  snmp_pdu *response = nullptr;
  variable_list *vars;

  oid root[MAX_OID_LEN] = {0};
  size_t rootlen = MAX_OID_LEN;
  nlohmann::json js = nlohmann::json::array();

  try {

    pdu = snmp_pdu_create(opt.op_type);

    if (read_objid(opt.oid.c_str(), root, &rootlen)) {
      snmp_add_null_var(pdu, root, rootlen);

      nStatus = snmp_sess_synch_response(opt.session, pdu, &response);

      if (nStatus == STAT_SUCCESS) {
        if (response->errstat == SNMP_ERR_NOERROR) {

          for (vars = response->variables; vars; vars = vars->next_variable) {
            if ((vars->type != SNMP_ENDOFMIBVIEW) && (vars->type != SNMP_NOSUCHOBJECT) &&
                (vars->type != SNMP_NOSUCHINSTANCE) && (vars->type != ASN_NULL)) {


              if(match_phys_address(opt.oid)) {
                result[opt.name] = snmp_hex_to_string(vars);
              }else if (parser_snmp_values(vars, js) == 0) {
                result[opt.name] = js[0];
                nRet = 0;
              } else msg = js[0].get<std::string>();
            }
            else msg = "an exception value";
          }
        }
        else {

          msg = "Error in packet Reason:" + std::string(snmp_errstring(response->errstat));
          if (response->errindex != 0) {
            msg += "Failed object: ";
            int i = 1;
            for (i = 1, vars = response->variables;
                 vars && i != response->errindex;
                 vars = vars->next_variable, i++)
              /*EMPTY*/;
            if (vars) {
              msg += oid_to_string(vars->name, vars->name_length);
            }
          }
        }

      }
      else if (nStatus == STAT_TIMEOUT) {
        struct snmp_session *sptr = snmp_sess_session(opt.session);
        msg = std::string("Timeout: No Response from ") + sptr->peername;
        nRet = 1;
      }
      else { /* status == STAT_ERROR */
        msg = "Other error occurs !";
      }
      if (response)
        snmp_free_pdu(response);
    }
    else // error read_objid
    {

      msg = std::string("Read_objid error! oid: ") + opt.oid;
    }


  }
  catch (const std::exception &e) {
    msg = std::string("Exception: ") + e.what();
  }
  catch (...) {
    msg = "Unexcepitons occurs!";
  }
  if (!msg.empty())
    result["error"] = msg;
  return nRet;
}


int snmp_bulkwalk(const SNMPOPT &opt, nlohmann::json &subtree) {
  int ret = 0;

  int reps = 20, non_reps = 0;
  std::string msg{};
  snmp_pdu *pdu;
  snmp_pdu *response;
  variable_list *vars=NULL;

  oid name[MAX_OID_LEN] = {0};
  size_t name_length = MAX_OID_LEN;
  oid root[MAX_OID_LEN] = {0};
  size_t rootlen = MAX_OID_LEN;

  int nStatus = 0;
  bool running = true;

  char buf[MAX_OID_LEN] = {0};
  std::string subname;
  try {
    if (read_objid(opt.oid.c_str(), name, &name_length)) {
      read_objid(opt.oid.c_str(), root, &rootlen);
      while (running) {

        pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
        pdu->non_repeaters = non_reps;
        pdu->max_repetitions = reps;    /* fill the packet */
        snmp_add_null_var(pdu, name, name_length);

        nStatus = snmp_sess_synch_response(opt.session, pdu, &response);

        if (nStatus == STAT_SUCCESS) {
          if (response->errstat == SNMP_ERR_NOERROR) {

            for (vars = response->variables; vars;
                 vars = vars->next_variable) {
              if ((vars->name_length < rootlen)
                  || (memcmp(root,vars->name, rootlen * sizeof(oid) ) != 0)) {
                running = false;
                continue;
              }


              if ((vars->type != SNMP_ENDOFMIBVIEW) && (vars->type != SNMP_NOSUCHOBJECT) &&
                  (vars->type != SNMP_NOSUCHINSTANCE) && (vars->type != ASN_NULL)) {

                snprint_objid(buf,MAX_OID_LEN,vars->name,vars->name_length);
                subname = buf;
                auto pos = subname.find("::");
                if(pos != subname.npos) subname = subname.substr(pos+2);
                if(match_phys_address(oid_to_string(vars->name,vars->name_length))) {
                  subtree[subname] = snmp_hex_to_string(vars); ret += 1;
                }else {
                  nlohmann::json v;
                  if (0 == parser_snmp_values(vars, v)) {
                    subtree[subname] = v[0];
                    ret += 1;
                  }
                }

                memmove((char *) name, (char *) vars->name,
                        vars->name_length * sizeof(oid));
                name_length = vars->name_length;

              }
              else //an exception value, so stop
              {

                msg = "walk subtree, an exception value!";
                running = false;
              }
            }
          }
          else { // Error in packet.
            running = false;
            msg = std::string("Error in packet. Reason: ") + snmp_errstring(response->errstat);
            if (response->errstat == SNMP_ERR_NOSUCHNAME) {
              msg += "The request for this object identifier failed: ";

              if (vars)
                msg += oid_to_string(vars->name, vars->name_length);
            }
          }
        }

        else if (nStatus == STAT_TIMEOUT) {
          struct snmp_session *sptr = snmp_sess_session(opt.session);
          msg = std::string("Timeout: No Response from ") + sptr->peername;
          ret = -1; //timeout
          running = false;
        }

        else {
          msg = "snmpSubTree Other error occurs !";
          running = false;
        }

        if (response) snmp_free_pdu(response);
      }
    }
    else {  // error read_objid_xxxx
      msg = std::string("read_objid error! oid: ") + opt.oid;
    }
  }
  catch (const std::exception &e) {
    msg = std::string("Exception: ") + e.what();
  }
  catch (...) {
    msg = std::string("Unexcepitons occurs!");
  }
  if (ret <= 0)
    subtree["error"] = msg;
  return ret;
}

int snmp_walk(const SNMPOPT &opt, nlohmann::json &subtree) {

  int ret = 0;
  std::string msg{};
  snmp_pdu *pdu;
  snmp_pdu *response;
  variable_list *vars=NULL;

  oid name[MAX_OID_LEN] = {0};
  size_t name_length = MAX_OID_LEN;
  oid root[MAX_OID_LEN] = {0};
  size_t rootlen = MAX_OID_LEN;

  int nStatus = 0;
  bool running = true;

  char buf[MAX_OID_LEN] = {0};
  std::string subname;
  try {
    if (read_objid(opt.oid.c_str(), name, &name_length)) {
      read_objid(opt.oid.c_str(), root, &rootlen);
      while (running) {

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);

        nStatus = snmp_sess_synch_response(opt.session, pdu, &response);

        if (nStatus == STAT_SUCCESS) {
          if (response->errstat == SNMP_ERR_NOERROR) {

            for (vars = response->variables; vars; vars = vars->next_variable) {

              //* not part of this subtree, 不属于同一个树
//              if (snmp_oidtree_compare(root, rootlen, vars->name, vars->name_length) < 0) {
//                running = false;
//                continue;
//              }
              if ((vars->name_length < rootlen) || (memcmp(root,vars->name, rootlen * sizeof(oid) ) != 0)) {
                running = false;
                continue;
              }


              if ((vars->type != SNMP_ENDOFMIBVIEW) && (vars->type != SNMP_NOSUCHOBJECT) &&
                  (vars->type != SNMP_NOSUCHINSTANCE) && (vars->type != ASN_NULL)) {

                snprint_objid(buf,MAX_OID_LEN,vars->name,vars->name_length);
                subname = buf;
                auto pos = subname.find("::");
                if(pos != subname.npos) subname = subname.substr(pos+2);
                if(match_phys_address(oid_to_string(vars->name,vars->name_length))) {
                  subtree[subname] = snmp_hex_to_string(vars); ret += 1;
                }else {
                  nlohmann::json v;
                  if (0 == parser_snmp_values(vars, v)) {
                    subtree[subname] = v[0];
                    ret += 1;
                  }
                }

                memmove((char *) name, (char *) vars->name,
                        vars->name_length * sizeof(oid));
                name_length = vars->name_length;

              }
              else //an exception value, so stop
              {

                msg = "walk subtree, an exception value!";
                running = false;
              }
            }
          }
          else { // Error in packet.
            running = false;
            msg = std::string("Error in packet. Reason: ") + snmp_errstring(response->errstat);
            if (response->errstat == SNMP_ERR_NOSUCHNAME) {
              msg += "The request for this object identifier failed: ";

              if (vars)
                msg += oid_to_string(vars->name, vars->name_length);
            }
          }
        }

        else if (nStatus == STAT_TIMEOUT) {
          struct snmp_session *sptr = snmp_sess_session(opt.session);
          msg = std::string("Timeout: No Response from ") + sptr->peername;
          ret = -1; //timeout
          running = false;
        }

        else {
          msg = "snmpSubTree Other error occurs !";
          running = false;
        }

        if (response) snmp_free_pdu(response);
      }
    }
    else {  // error read_objid_xxxx
      msg = std::string("read_objid error! oid: ") + opt.oid;
    }
  }
  catch (const std::exception &e) {
    msg = std::string("Exception: ") + e.what();
  }
  catch (...) {
    msg = std::string("Unexcepitons occurs!");
  }
  if (ret <= 0)
     subtree["error"] = msg;
  return ret;
}


size_t snmp_table(const SNMPOPT& opt, nlohmann::json& table) {
  nlohmann::json columns;
  auto ret = snmp_bulkwalk(opt,columns);
  if(ret > 0) {
    ret = columns_to_table(columns,table);
  } else {
    table = columns; // ret <=0; -1 == timeout
  }

  return ret;
}



