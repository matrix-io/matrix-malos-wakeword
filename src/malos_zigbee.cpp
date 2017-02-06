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

#include <chrono>
#include <iostream>
#include <thread>

#include "./driver_manager.h"
#include "./driver_zigbee.h"

const int kBasePort = 20012;

const char kUnsecureBindScope[] = "*";

namespace matrix_malos {

int RunServer() {
  std::cerr << "**************" << std::endl;
  std::cerr << "MALOS ZigBee starting" << std::endl;
  std::cerr << "**************" << std::endl;
  std::cerr << std::endl;

  DriverManager driver_manager(kBasePort, kUnsecureBindScope);
  std::cerr << "You can query specific driver info using port " +
                   std::to_string(20012)
            << "." << std::endl;


  ZigbeeDriver driver_zigbee;
  if (!driver_zigbee.Init(kBasePort + 4 * 5 + 1, kUnsecureBindScope)) {
    return 1;
  }
  driver_manager.RegisterDriver(&driver_zigbee);

  driver_manager.ServeInfoRequestsForEver();

  return 0;  // Never reached.
}
}  // namespace matrix_malos

int main(int, char* []) { return matrix_malos::RunServer(); }
