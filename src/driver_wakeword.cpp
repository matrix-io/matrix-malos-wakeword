/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator MALOS
 *
 * MATRIX Creator MALOS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "./driver_wakeword.h"
#include "./src/driver.pb.h"

namespace {

} // namespace

namespace matrix_malos {

bool WakeWordDriver::ProcessConfig(const DriverConfig& config) {
  // TODO @hpsaturn: Validate all the data that comes from the protos
  WakeWordParams wakeword_params(config.wakeword());

  return true;
}

bool WakeWordDriver::SendUpdate() {

  return true;
}
}
