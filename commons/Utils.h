#pragma once

#include <vector>
#include <string>

std::string net_to_ip(unsigned int ip);

std::string first_ip(const std::string& ip, const std::string& mask);
std::string last_ip(const std::string& ip, const std::string& mask);

unsigned int first_int_ip(const std::string& ip, const std::string& mask);
unsigned int last_int_ip(const std::string& ip, const std::string& mask);

std::string NetworkIP(const std::string& ip, const std::string& mask);

std::string BroadcastIP(const unsigned int &, const unsigned int &);

std::string FirstIP(const unsigned int &, const unsigned int &);
std::string LastIP(const unsigned int &, const unsigned int &);


unsigned int IntFirstIP(const unsigned int &, const unsigned int &);
unsigned int IntLastIP(const unsigned int &, const unsigned int &);

unsigned int Nmask(const int &);

unsigned int maskBits(const std::string &);


