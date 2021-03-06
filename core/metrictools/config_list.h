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
#include <map>
#include <memory>
#include <string>
#include "metrictools/util/return_code.h"
#include "metrictools/metric.h"
#include "metrictools/units.h"
#include "metrictools/collect.h"
#include "metrictools/listen.h"

namespace fnordmetric {

/**
 * The GlobalConfig struct contains system-wide settings. Most of these
 * parameters con be controlled from the configuration file
 */
struct GlobalConfig {

  /**
   * The global metric label path definition. The global metric label
   * path can be used to prepened elements to the label paths of each metric.
   * A good example use case would be prepending `datacenter`, `rack` and
   * `hostname` elements to the label paths of all metrics
   */
  MetricLabelConfig global_label_config;

  std::vector<MetricLabelOverride> global_label_overrides;

};

class ConfigList {
public:

  ConfigList();

  const std::string& getBackendURL() const;
  void setBackendURL(const std::string& backend_url);

  const std::map<MetricIDType, std::shared_ptr<MetricConfig>>& getMetricConfigs() const;
  std::shared_ptr<const MetricConfig> getMetricConfig(MetricIDType metric_id) const;
  void addMetricConfig(MetricConfig config);

  const std::map<std::string, UnitConfig>& getUnitConfigs() const;
  const UnitConfig* getUnitConfig(std::string unit_id) const;
  void addUnitConfig(UnitConfig config);

  const std::vector<std::unique_ptr<IngestionTaskConfig>>& getIngestionTaskConfigs() const;
  void addIngestionTaskConfig(std::unique_ptr<IngestionTaskConfig> config);

  const std::vector<std::unique_ptr<ListenerConfig>>& getListenerConfigs() const;
  void addListenerConfig(std::unique_ptr<ListenerConfig> config);

  void addDashboardPath(const std::string& path);
  const std::set<std::string>& getDashboardPaths() const;

  std::shared_ptr<const GlobalConfig> getGlobalConfig() const noexcept;
  std::shared_ptr<GlobalConfig> getGlobalConfig() noexcept;

  const std::string& getDatadir() const;
  void setDatadir(const std::string& dir);

  const std::string& getPlugindir() const;
  void setPlugindir(const std::string& dir);

protected:
  std::string backend_url_;
  bool create_metrics_;
  std::shared_ptr<GlobalConfig> global_config_;
  std::map<MetricIDType, std::shared_ptr<MetricConfig>> metric_configs_;
  std::map<std::string, UnitConfig> unit_configs_;
  std::vector<std::unique_ptr<IngestionTaskConfig>> ingestion_configs_;
  std::vector<std::unique_ptr<ListenerConfig>> listener_configs_;
  std::set<std::string> dashboard_paths_;
  std::string datadir_;
  std::string plugindir_;
};

} // namespace fnordmetric

