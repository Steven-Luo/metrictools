/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <metrictools/collect.h>
#include <metrictools/collect_http.h>
#include <metrictools/collect_proc.h>
#include <metrictools/util/time.h>
#include <metrictools/util/logging.h>
#include <metrictools/listen_udp.h>
#include <metrictools/listen_http.h>
#include <metrictools/config_list.h>
#include <metrictools/statsd.h>
#include <metrictools/storage/backend.h>

namespace fnordmetric {

IngestionTaskConfig::IngestionTaskConfig() :
    metric_id_rewrite_enabled(false) {}

ReturnCode mkTask(
    Backend* storage_backend,
    const ConfigList* config_list,
    const IngestionTaskConfig* config,
    std::unique_ptr<Task>* task) {
  if (dynamic_cast<const CollectHTTPTaskConfig*>(config)) {
    return CollectHTTPTask::start(storage_backend, config_list, config, task);
  }

  if (dynamic_cast<const CollectProcTaskConfig*>(config)) {
    return CollectProcTask::start(storage_backend, config_list, config, task);
  }

  return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
}

ReturnCode startIngestionTasks(
    Backend* storage_backend,
    const ConfigList* config_list,
    TaskRunner* task_runner) {
  for (const auto& ic : config_list->getIngestionTaskConfigs()) {
    std::unique_ptr<Task> task;
    auto rc = mkTask(storage_backend, config_list, ic.get(), &task);
    if (!rc.isSuccess()) {
      return rc;
    }

    task_runner->addTask(std::move(task));
  }

  return ReturnCode::success();
}

} // namespace fnordmetric

