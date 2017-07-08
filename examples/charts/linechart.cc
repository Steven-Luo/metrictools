/**
 * This file is part of the "libcplot" project
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * libcplot is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <cplot/cplot.h>
#include "cplot/axisdefinition.h"
#include "cplot/areachart.h"
#include "cplot/barchart.h"
#include "cplot/canvas.h"
#include "cplot/domain.h"
#include "cplot/linechart.h"
#include "cplot/pointchart.h"
#include "cplot/series.h"
#include "cplot/svgtarget.h"

using namespace cplot;

int main() {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(10, 19);
  series2->addDatum(15, 18);
  series2->addDatum(20, 22);
  series2->addDatum(30, 23);
  series2->addDatum(40, 18);
  series2->addDatum(50, 21);

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  Canvas canvas;
  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
      &x_domain, &y_domain);
  line_chart->addSeries(series1);
  line_chart->addSeries(series2);
  line_chart->addAxis(AxisDefinition::TOP);
  line_chart->addAxis(AxisDefinition::RIGHT);
  line_chart->addAxis(AxisDefinition::BOTTOM);
  line_chart->addAxis(AxisDefinition::LEFT);

  std::string svg;
  SVGTarget target(&svg);
  canvas.render(&target);

  std::cout << svg << std::endl;

  return 0;
}
