//
// Created by sven on 11/16/15.
//

#ifndef SWITCHBROWSER_LOGGER_H
#define SWITCHBROWSER_LOGGER_H

#include "spdlog/spdlog.h"

#ifndef NDEBUG
#include <cassert>
#endif

#include <string>

namespace switchboard {

const std::string TAG = "switch";

class Logger {
public:
  Logger() = delete;

  Logger(const std::string &tag) : _logger(spdlog::get(tag)) {
#ifndef NDEBUG
    assert(_logger);
#endif

  }

protected:
  std::shared_ptr<spdlog::logger> _logger;

};

}
#endif //SWITCHBROWSER_LOGGER_H
