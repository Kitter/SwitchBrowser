#include "Utils.h"
#include <arpa/inet.h>

#ifdef __APPLE__

#include <machine/endian.h>
#define be32toh(x) htonl(x)

#else
#include <endian.h>
#endif

#include "commons.h"

using namespace std;

std::string int_to_ip(unsigned int ip) {
  in_addr addr;
  addr.s_addr = be32toh(ip);
  return std::string(inet_ntoa(addr));
}


std::string NetworkIP(const std::string& ip, const std::string& mask) {

  auto ip_int = inet_network(ip.c_str());

  auto mask_int = inet_network(mask.c_str());
  unsigned int network = ip_int & mask_int;
  return int_to_ip(network);
}

std::string BroadcastIP(const unsigned int &IP, const unsigned int &Mask) {
  unsigned int network = IP & Mask;
  unsigned int Broadcast = ((network) | (~Mask));
  return int_to_ip(Broadcast);
}

std::string first_ip(const std::string& ip, const std::string& mask) {
  //计算该网段的第一个可用IP地址

  auto ip_int = inet_network(ip.c_str());

  auto mask_int = inet_network(mask.c_str());
  unsigned int network = ip_int & mask_int;
  unsigned int firstIPAdd = network + 1;

  return int_to_ip(firstIPAdd);
}

std::string last_ip(const std::string& ip, const std::string& mask) {
  //计算该网段的第一个可用IP地址

  auto ip_int = inet_network(ip.c_str());

  auto mask_int = inet_network(mask.c_str());
  unsigned int network = ip_int & mask_int;
  unsigned int lastIPAdd = ((network) | (~mask_int)) - 1;

  return int_to_ip(lastIPAdd);
}


unsigned int first_int_ip(const std::string& ip, const std::string& mask) {
  //计算该网段的第一个可用IP地址

  auto ip_int = inet_network(ip.c_str());

  auto mask_int = inet_network(mask.c_str());
  unsigned int network = ip_int & mask_int;
  return network + 1;

}

unsigned int last_int_ip(const std::string& ip, const std::string& mask) {
  //计算该网段的第一个可用IP地址

  auto ip_int = inet_network(ip.c_str());

  auto mask_int = inet_network(mask.c_str());
  unsigned int network = ip_int & mask_int;
  unsigned int lastIPAdd = ((network) | (~mask_int)) - 1;

  return lastIPAdd;
}


std::string FirstIP(const unsigned int &IP, const unsigned int &Mask) {
  //计算该网段的第一个可用IP地址

  unsigned int network = IP & Mask;
  unsigned int firstIPAdd = network + 1;

  return int_to_ip(firstIPAdd);
}

unsigned int IntFirstIP(const unsigned int &IP, const unsigned int &Mask) {
  unsigned int network = IP & Mask;
  unsigned int firstIPAdd = network + 1;
  return firstIPAdd;
}

std::string LastIP(const unsigned int &IP, const unsigned int &Mask) {
  //计算该网段的最后一个可用IP地址

  unsigned int network = IP & Mask;
  unsigned int lastIPAdd = ((network) | (~Mask)) - 1;

  return int_to_ip(lastIPAdd);
}

unsigned int IntLastIP(const unsigned int &IP, const unsigned int &Mask) {
  //计算该网段的最后一个可用IP地址

  unsigned int network = IP & Mask;
  unsigned int lastIPAdd = ((network) | (~Mask)) - 1;
  return lastIPAdd;
}

unsigned int Nmask(const int &n) {
  //通过子网掩码的位数计算子网掩码;

  unsigned int re = 0;

  if (n == 0) {
    return re;
  }
  if (n == 32) {
    return ~re;
  }

  re = re | 1;  //在计算子网掩码的时候是反着走的，因为这样只需要与１进行或运算;
  for (int i = 1; i < 32 - n; i++) {
    re = re << 1;
    re = re | 1;
  }

  return ~re;
}

unsigned int maskBits(const std::string &maskip) {
  auto Mask =inet_network(maskip.c_str());

  unsigned int cnt = 0, p = 0x80000000;

  while ((Mask & p) == p) {
    cnt++;
    p = p >> 1;
  }
  return cnt;
}
