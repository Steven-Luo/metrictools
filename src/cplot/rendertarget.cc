/**
 * This file is part of the "libcplot" project
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <unordered_map>
#include <cplot/rendertarget.h>

namespace stx {
namespace chart {

static const std::unordered_map<std::string, std::string> kColourMap = {
  { "color1", "#4572a7" },
  { "color2", "#aa4643" },
  { "color3", "#89a54e" },
  { "color4", "#80699b" },
  { "color5", "#3d96ae" },
  { "color6", "#db843d" }
};

Colour getColour(const std::string& str) {
  std::string hex;

  auto iter = kColourMap.find(str);
  if (iter == kColourMap.end()) {
    hex = str;
  } else {
    hex = iter->second;
  }

  return Colour {
    .hex = hex
  };
}

} // namespace chart
} // namespace stx

