//
// Created by sven on 11/16/15.
//

#include "fetch_service.h"
#include "snmp_utils.h"
#include "commons.h"

namespace switchboard {

FetchService::FetchService():Logger(TAG) {

#ifndef NDEBUG
  assert(file_exists("../mibs/cisco-mibs"));
#endif
  init_snmp("FetchService");
  add_mibdir("../mibs/cisco-mibs");
  read_all_mibs();
}

FetchService::~FetchService() {
  shutdown_mib();
}

void FetchService::start() {
  _logger->info("start fetch thread...");
  _thread.reset(new std::thread(&FetchService::fetch_thread,this));
}

void FetchService:: stop() {
  _running = false;
  _thread->join();
}

void FetchService::fetch_thread() {

  std::time_t last_run_time{0};
  try{

    while (_running) {
      std::time_t now = std::time(nullptr);
      auto dif = std::difftime(now, last_run_time);
      {
        std::unique_lock<std::mutex> lk(_mtx);
        if(dif < _frequency) {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          continue;
        }
        last_run_time = now;
      }

      update_local_cache();



    }



  } catch (const std::exception& ex) {
    _logger->error("Runing fetch thread with Exception: ") << ex.what();
  }

}
bool FetchService::fresh_swtich() {
  return false;
}

void FetchService::update_local_cache(){

}

} //namespace