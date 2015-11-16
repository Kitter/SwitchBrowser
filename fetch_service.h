//
// Created by sven on 11/16/15.
//

#ifndef SWITCHBROWSER_FETCH_SERVICE_H
#define SWITCHBROWSER_FETCH_SERVICE_H

#include "switch.h"
#include "logger.h"
#include <mutex>
#include <thread>

namespace switchboard {


class FetchService : public Logger {

public:
  FetchService();
  ~FetchService();

  void start();
  void stop();

  void set_frequency(unsigned int);

  void add_switch(const std::string& ip, const std::string& community);

  std::string search(const std::string& keywords);

  std::string get_table(const std::string& ip);

protected:

  void fetch_thread();

  bool fresh_swtich();

  void update_local_cache();


private:

  std::vector<Switch> _switchs;

  std::unique_ptr<std::thread> _thread;

  bool _running{true};
  unsigned int _frequency{600};

  std::mutex _mtx;
};

}

#endif //SWITCHBROWSER_FETCH_SERVICE_H
