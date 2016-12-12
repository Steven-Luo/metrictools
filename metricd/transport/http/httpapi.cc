/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/metric_service.h>
#include <metricd/transport/http/httpapi.h>
#include <metricd/util/stringutil.h>
#include <metricd/util/json.h>

namespace fnordmetric {

HTTPAPI::HTTPAPI(
    MetricService* metric_service) :
    metric_service_(metric_service) {}

void HTTPAPI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  URI uri(request->uri());
  auto path = uri.path();
  StringUtil::stripTrailingSlashes(&path);

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "GET, POST");
  response->addHeader("Access-Control-Allow-Headers", "Authorization");

  if (request->method() == http::HTTPMessage::M_OPTIONS) {
    response->setStatus(http::kStatusOK);
    return;
  }

  // PATH: /api/v1/metrics/list
  if (path == "/api/v1/metrics/list") {
    renderMetricList(request, response, uri);
    return;
  }

  // PATH: /api/v1/metrics/list_series
  if (path == "/api/v1/metrics/list_series") {
    renderMetricSeriesList(request, response, uri);
    return;
  }

  // PATH: /api/v1/metrics/fetch_series
  if (path == "/api/v1/metrics/fetch_series") {
    performMetricFetchSeries(request, response, uri);
    return;
  }

  // PATH: /api/v1/metrics/insert
  if (path == "/api/v1/metrics/insert") {
    insertSample(request, response, uri);
    return;
  }

  response->setStatus(http::kStatusNotFound);
  response->addBody("not found");
}

void HTTPAPI::renderMetricList(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream json(response->getBodyOutputStream());

  json.beginObject();
  json.addObjectEntry("metrics");
  json.beginArray();

  auto cursor = metric_service_->listMetrics();
  for (int i = 0; cursor.isValid(); cursor.next()) {
    if (i++ > 0) { json.addComma(); }
    json.beginObject();

    json.addObjectEntry("metric_id");
    json.addString(cursor.getMetricID());
    //json.addComma();

    //json->addObjectEntry("total_bytes");
    //json->addLiteral<size_t>(metric->totalBytes());
    //json->addComma();

    //json->addObjectEntry("last_insert");
    //json->addLiteral<uint64_t>(static_cast<uint64_t>(metric->lastInsertTime()));
    //json->addComma();

    //json->addObjectEntry("labels");
    //json->beginArray();
    //auto labels = metric->labels();
    //for (auto cur = labels.begin(); cur != labels.end(); ++cur) {
    //  if (cur != labels.begin()) {
    //    json->addComma();
    //  }
    //  json->addString(*cur);
    //}
    //json->endArray();

    json.endObject();
  }

  json.endArray();
  json.endObject();
}

void HTTPAPI::renderMetricSeriesList(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  auto params = uri.queryParams();

  std::string metric_id;
  if (!URI::getParam(params, "metric_id", &metric_id)) {
    response->setStatus(http::kStatusBadRequest);
    response->addBody("ERROR: missing parameter ?metric_id=...");
    return;
  }

  MetricSeriesListCursor cursor;
  auto rc = metric_service_->listMetricSeries(metric_id, &cursor);
  if (!rc.isSuccess()) {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody("ERROR: " + rc.getMessage());
    return;
  }

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream json(response->getBodyOutputStream());

  json.beginObject();
  json.addObjectEntry("metric_id");
  json.addString(metric_id);
  json.addComma();
  json.addObjectEntry("series");
  json.beginArray();

  for (int i = 0; cursor.isValid(); cursor.next()) {
    if (i++ > 0) { json.addComma(); }
    json.beginObject();

    json.addObjectEntry("series_id");
    json.addInteger(cursor.getSeriesID());
    json.addComma();

    json.addObjectEntry("labels");
    json.beginObject();
    auto labels = cursor.getLabels();
    for (auto cur = labels->begin(); cur != labels->end(); ++cur) {
      if (cur != labels->begin()) {
        json.addComma();
      }
      json.addObjectEntry(cur->first);
      json.addString(cur->second);
    }
    json.endObject();

    json.endObject();
  }

  json.endArray();
  json.endObject();
}

void HTTPAPI::performMetricFetchSeries(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  auto params = uri.queryParams();

  std::string metric_id;
  if (!URI::getParam(params, "metric_id", &metric_id)) {
    response->setStatus(http::kStatusBadRequest);
    response->addBody("ERROR: missing parameter ?metric_id=...");
    return;
  }

  MetricSeriesListCursor cursor;
  auto rc = metric_service_->listMetricSeries(metric_id, &cursor);
  if (!rc.isSuccess()) {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody("ERROR: " + rc.getMessage());
    return;
  }

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream json(response->getBodyOutputStream());

  json.beginObject();
  json.addObjectEntry("metric_id");
  json.addString(metric_id);
  json.addComma();
  json.addObjectEntry("series");
  json.beginArray();

  for (int i = 0; cursor.isValid(); cursor.next()) {
    if (i++ > 0) { json.addComma(); }
    json.beginObject();

    json.addObjectEntry("series_id");
    json.addInteger(cursor.getSeriesID());
    json.addComma();

    json.addObjectEntry("labels");
    json.beginObject();
    auto labels = cursor.getLabels();
    for (auto cur = labels->begin(); cur != labels->end(); ++cur) {
      if (cur != labels->begin()) {
        json.addComma();
      }
      json.addObjectEntry(cur->first);
      json.addString(cur->second);
    }
    json.endObject();

    json.endObject();
  }

  json.endArray();
  json.endObject();
}

void HTTPAPI::insertSample(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  // FIXME check POST

  auto params = uri.queryParams();

  std::string metric_id;
  if (!URI::getParam(params, "metric_id", &metric_id)) {
    response->addBody("error: missing ?metric_id=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::string value_str;
  if (!URI::getParam(params, "value", &value_str)) {
    response->addBody("error: missing ?value=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  static const char kLabelParamPrefix[] = "label[";
  LabelSet labels;
  for (const auto& param : params) {
    const auto& key = param.first;
    const auto& value = param.second;

    if (key.compare(0, sizeof(kLabelParamPrefix) - 1, kLabelParamPrefix) == 0 &&
        key.back() == ']') {
      auto label_key = key.substr(
          sizeof(kLabelParamPrefix) - 1,
          key.size() - sizeof(kLabelParamPrefix));

      labels[label_key] = value;
    }
  }

  double sample_value;
  try {
    sample_value = std::stod(value_str);
  } catch (std::exception& e) {
    response->addBody("error: invalid value: " + value_str);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  auto now = WallClock::unixMicros();
  auto rc = metric_service_->insertSample(
      metric_id,
      LabelledSample(Sample(now, sample_value), labels));

  if (rc.isSuccess()) {
    response->setStatus(http::kStatusCreated);
  } else {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody("ERROR: " + rc.getMessage());
  }
}

//
//void HTTPAPI::renderMetricSampleScan(
//    http::HTTPRequest* request,
//    http::HTTPResponse* response,
//    const URI& uri) {
//  auto metric_key = uri->path().substr(sizeof(kMetricsUrlPrefix) - 1);
//  if (metric_key.size() < 3) {
//    response->addBody("error: invalid metric key: " + metric_key);
//    response->setStatus(http::kStatusBadRequest);
//    return;
//  }
//
//  auto metric = metric_repo_->findMetric(metric_key);
//  if (metric == nullptr) {
//    response->addBody("metric not found: " + metric_key);
//    response->setStatus(http::kStatusNotFound);
//    return;
//  }
//
//  response->setStatus(http::kStatusOK);
//  response->addHeader("Content-Type", "application/json; charset=utf-8");
//  JSONOutputStream json(response->getBodyOutputStream());
//
//  json.beginObject();
//
//  json.addObjectEntry("metric");
//  renderMetricJSON(metric, &json);
//  json.addComma();
//
//  json.addObjectEntry("samples");
//  json.beginArray();
//
//  int i = 0;
//  metric->scanSamples(
//      fnord::util::DateTime::epoch(),
//      fnord::util::DateTime::now(),
//      [&json, &i] (Sample* sample) -> bool {
//        if (i++ > 0) { json.addComma(); }
//        json.beginObject();
//
//        json.addObjectEntry("time");
//        json.addLiteral<uint64_t>(static_cast<uint64_t>(sample->time()));
//        json.addComma();
//
//        json.addObjectEntry("value");
//        json.addLiteral<double>(sample->value());
//        json.addComma();
//
//        json.addObjectEntry("labels");
//        json.beginObject();
//        auto labels = sample->labels();
//        for (int n = 0; n < labels.size(); n++) {
//          if (n > 0) {
//            json.addComma();
//          }
//
//          json.addObjectEntry(labels[n].first);
//          json.addString(labels[n].second);
//        }
//        json.endObject();
//
//        json.endObject();
//        return true;
//      });
//
//  json.endArray();
//  json.endObject();
//}

} // namespace fnordmetric

