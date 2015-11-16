//
// Created by sven on 11/16/15.
//

#ifndef SWITCHBROWSER_SQLITE_CLIENT_H
#define SWITCHBROWSER_SQLITE_CLIENT_H

#include "logger.h

namespace switchboard {

class SqlClient : public Logger {

public:
  SqlClient(const std::string path);
  ~SqlClient();


private:

};

}

#endif //SWITCHBROWSER_SQLITE_CLIENT_H
