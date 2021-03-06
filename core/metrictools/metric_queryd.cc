/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <metrictools/util/flagparser.h>
#include <metrictools/util/logging.h>
#include <metrictools/util/fileutil.h>
#include <metrictools/util/daemonize.h>
#include <metrictools/config_list.h>
#include <metrictools/config_parser.h>
#include <metrictools/query_service.h>

using namespace fnordmetric;

void shutdown(int);

bool parseListenAddr(
    const std::string& addr,
    std::string* host,
    uint16_t* port) {
  std::smatch m;
  std::regex listen_regex("([0-9a-zA-Z-_.]+):([0-9]+)");
  if (std::regex_match(addr, m, listen_regex)) {
    *host = m[1];
    *port = std::stoul(m[2]);
    return true;
  } else {
    return false;
  }
}

int main(int argc, const char** argv) {
  signal(SIGPIPE, SIG_IGN);

  FlagParser flags;

  flags.defineFlag(
      "backend",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "listen_http",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "help",
      FlagParser::T_SWITCH,
      false,
      "?",
      NULL);

  flags.defineFlag(
      "version",
      FlagParser::T_SWITCH,
      false,
      "V",
      NULL);

  flags.defineFlag(
      "loglevel",
      FlagParser::T_STRING,
      false,
      NULL,
      "INFO");

  flags.defineFlag(
      "daemonize",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "pidfile",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "log_to_syslog",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "nolog_to_stderr",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "dev_assets",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  /* parse flags */
  {
    auto rc = flags.parseArgv(argc, argv);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  /* setup logging */
  if (!flags.isSet("nolog_to_stderr") && !flags.isSet("daemonize")) {
    Logger::logToStderr("metric-queryd");
  }

  if (flags.isSet("log_to_syslog")) {
    Logger::logToSyslog("metric-queryd");
  }

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* print help */
  if (flags.isSet("help") || flags.isSet("version")) {
    std::cerr <<
        StringUtil::format(
            "metric-queryd $0\n"
            "Part of the FnordMetric project (http://fnordmetric.io)\n"
            "Copyright (c) 2016, Paul Asmuth et al. All rights reserved.\n\n",
            FNORDMETRIC_VERSION);
  }

  if (flags.isSet("version")) {
    return 0;
  }

  if (flags.isSet("help")) {
    std::cerr <<
        "Usage: $ metric-queryd [OPTIONS]\n"
        "   --backend                 Set the backend database connection URI/string\n"
        "   --listen_http             Listen for HTTP connection on this address\n"
        "   --daemonize               Daemonize the server\n"
        "   --pidfile <file>          Write a PID file\n"
        "   --loglevel <level>        Minimum log level (default: INFO)\n"
        "   --[no]log_to_syslog       Do[n't] log to syslog\n"
        "   --[no]log_to_stderr       Do[n't] log to stderr\n"
        "   -?, --help                Display this help text and exit\n"
        "   -v, --version             Display the version of this binary and exit\n"
        "\n"
        "Examples:\n"
        "   $ metric-queryd --backend 'mysql://localhost:3306/mydb?user=root' --listen_http 0.0.0.0:8080\n";

    return 0;
  }

  /* check flags */
  if (!flags.isSet("backend")) {
    std::cerr << "ERROR: --backend flag must be set" << std::endl;
    return 1;
  }

  if (!flags.isSet("listen_http")) {
    std::cerr << "ERROR: --listen_http flag must be set" << std::endl;
    return 1;
  }

  /* daemonize */
  auto rc = ReturnCode::success();
  if (rc.isSuccess() && flags.isSet("daemonize")) {
    rc = daemonize();
  }

  /* write pidfile */
  int pidfile_fd = -1;
  auto pidfile_path = flags.getString("pidfile");
  if (rc.isSuccess() && !pidfile_path.empty()) {
    pidfile_fd = open(
        pidfile_path.c_str(),
        O_WRONLY | O_CREAT,
        0666);

    if (pidfile_fd < 0) {
      rc = ReturnCode::errorf(
          "IO_ERROR",
          "writing pidfile failed: $0",
          std::strerror(errno));
    }

    if (rc.isSuccess() && flock(pidfile_fd, LOCK_EX | LOCK_NB) != 0) {
      close(pidfile_fd);
      pidfile_fd = -1;
      rc = ReturnCode::error("IO_ERROR", "locking pidfile failed");
    }

    if (rc.isSuccess() && ftruncate(pidfile_fd, 0) != 0) {
      close(pidfile_fd);
      pidfile_fd = -1;
      rc = ReturnCode::error("IO_ERROR", "writing pidfile failed");
    }

    auto pid = StringUtil::toString(getpid());
    if (rc.isSuccess() &&
        write(pidfile_fd, pid.data(), pid.size()) != pid.size()) {
      close(pidfile_fd);
      pidfile_fd = -1;
      rc = ReturnCode::error("IO_ERROR", "writing pidfile failed");
    }
  }

  /* open backend */
  std::unique_ptr<Backend> backend;
  if (rc.isSuccess()) {
    rc = Backend::openBackend(flags.getString("backend"), &backend);
  }

  /* run http server */
  std::string http_bind;
  uint16_t http_port;
  if (rc.isSuccess()) {
    auto parse_rc = parseListenAddr(
        flags.getString("listen_http"),
        &http_bind,
        &http_port);

    if (!parse_rc) {
      rc = ReturnCode::error("ERUNTIME", "invalid value for --listen_http");
    }
  }

  if (rc.isSuccess()) {
    //QueryService query_service(backend.get());
    //if (flags.isSet("dev_assets")) {
    //  query_service.setAssetPath(flags.getString("dev_assets"));
    //}

    //query_service.listenAndRun(http_bind, http_port);
  }

  if (!rc.isSuccess()) {
    logFatal("ERROR: $0", rc.getMessage());
  }

  /* shutdown */
  logInfo("Exiting...");
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  /* unlock pidfile */
  if (pidfile_fd > 0) {
    unlink(pidfile_path.c_str());
    flock(pidfile_fd, LOCK_UN);
    close(pidfile_fd);
  }

  return rc.isSuccess() ? 0 : 1;
}

void shutdown(int) {
  // kill
}

