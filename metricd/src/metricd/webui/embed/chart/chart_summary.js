/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
FnordMetricChart.Summary = function(elem, params) {
  'use strict';

  this.render = function(result) {
    var summary_series = result.series[0]; //FIXME make this configurable

    renderTitle(summary_series);
    renderMainSummary(summary_series, result.unit);
    renderAdditionalSummary(summary_series, result.unit);
  }

/********************************** private ***********************************/

  function renderTitle(series) {
    var title_elem = document.createElement("div");
    title_elem.classList.add("title");
    title_elem.innerHTML = DOMUtil.escapeHTML(series.series_id);

    elem.appendChild(title_elem);
  }

  function renderMainSummary(series, unit) {
    var summary_elem = document.createElement("div");
    summary_elem.classList.add("main_summary");

    if (series.summaries && series.summaries.length > 0) {
      summary_elem.innerHTML = FnordMetricUnits.formatValue(
          unit,
          series.summaries[0].value);
    }

    elem.appendChild(summary_elem);
  }

  function renderAdditionalSummary(series, unit) {
    var summary_elem = document.createElement("div");
    summary_elem.classList.add("small_summary");

    for (var i = 0; i < series.summaries.length; i++) {
      var span = document.createElement("span");
      span.innerHTML = series.summaries[i].sum + "=";
      span.innerHTML += FnordMetricUnits.formatValue(
          unit,
          series.summaries[i].value);
    }

    elem.appendChild(summary_elem);
  }
}

