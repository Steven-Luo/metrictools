/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
FnordMetricChart.Tooltip = function(elem, points) {
  const MAX_SNAP = 50;

  var tooltip_elem;

  elem.addEventListener("mousemove", function(e) {
    chartHover(e);
  }, false);

  function chartHover(e) {
    var mx = e.x + window.scrollX;
    var my = e.y + window.scrollY;

    var point = findClosesPoint(mx, my);
    if (point == null) {
      hideTooltip();
    } else {
      showTooltip(point);
    }
  }

  function findClosesPoint(x, y) {
    var best_point = null;
    var best_distance = MAX_SNAP;

    for (var i = 0; i < points.length; i++) {
      //var diff_x = Math.pow((x - points[i].x), 2);
      //var diff_y = Math.pow((y - points[i].y), 2);
      //var dist = Math.sqrt(diff_x + diff_y);

      //if (dist < best_distance) {
      //  best_distance = dist;
      //  best_point = points[i];
      //}
    }

    return best_point;
  }

  function showTooltip(point) {
    console.log("show");
    if (tooltip_elem == null) {
      /* setup tooltip elem */
      tooltip_elem = document.createElement("div");
      tooltip_elem.style.display = "none";
      document.body.appendChild(tooltip_elem);
      tooltip_elem.className = 'fm-tooltip';
      tooltip_elem.onmousemove = chartHover;
    }

    tooltip_elem.innerHTML = point.label;
    tooltip_elem.style.display = "block";

    var pos_x = Math.round(point.x - tooltip_elem.offsetWidth * 0.5);
    tooltip_elem.style.left = pos_x + "px";

    var pos_y = Math.round(point.y - tooltip_elem.offsetHeight )-5;
    tooltip_elem.style.top = pos_y + "px";
    console.log(pos_y);
  }

  function hideTooltip() {

  }

}

