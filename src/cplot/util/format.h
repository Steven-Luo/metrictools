/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <ctime>
#include <inttypes.h>
#include <limits>
#include <string>
#include "cplot/util/return_code.h"

namespace fnordmetric {

ReturnCode parseDuration(const std::string& str, uint64_t* duration);

ReturnCode parsePointInTime(const std::string& str, uint64_t* timestamp);

} // namespace fnordmetric

