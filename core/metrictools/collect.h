/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <list>
#include <thread>
#include <regex>
#include <metrictools/util/return_code.h>
#include <metrictools/metric.h>
#include <metrictools/measure.h>
#include <metrictools/task.h>

namespace fnordmetric {
class ConfigList;
class IngestionTask;
class Backend;
struct IngestionTaskConfig;
class InsertStorageOp;

struct IngestionTaskConfig {
  IngestionTaskConfig();
  virtual ~IngestionTaskConfig() = default;
  bool metric_id_rewrite_enabled;
  std::regex metric_id_rewrite_regex;
  std::string metric_id_rewrite_replace;
};

ReturnCode mkIngestionTask(
    Backend* storage_backend,
    const ConfigList* config_list,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task);

ReturnCode startIngestionTasks(
    Backend* storage_backend,
    const ConfigList* config_list,
    TaskRunner* task_runner);


} // namespace fnordmetric

