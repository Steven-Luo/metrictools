/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPCONNECTIONHANDLER_H
#define _FNORDMETRIC_HTTPCONNECTIONHANDLER_H
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>

namespace fnord {
namespace http {

class HTTPConnectionHandler {
public:

  virtual ~HTTPConnectionHandler() {}

  virtual bool handleHTTPRequest(
      HTTPRequest* request,
      HTTPResponse* response) = 0;

};

}
}
#endif
