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

#ifndef SRC_DRIVER_WAKEWORD_H_
#define SRC_DRIVER_WAKEWORD_H_

#include <cstdio>
#include <memory>
#include <thread>

#include <matrix_io/malos/v1/driver.pb.h>
#include <matrix_malos/malos_base.h>

const char kWakeWordDriverName[] = "WakeWord";

namespace pb = matrix_io::malos::v1;

namespace matrix_malos {

class WakeWordDriver : public MalosBase {
 public:
  WakeWordDriver() : MalosBase(kWakeWordDriverName) {
    SetNeedsKeepalives(true);
    SetMandatoryConfiguration(true);
    SetNotesForHuman("WakeWord Driver v0.1.2");
  }

  // Reads configuration from the outside workd.
  bool ProcessConfig(const pb::driver::DriverConfig &config) override;

 private:
  // Thread that read events from malos_psphinx command
  void PocketSphinxProcess();

  // Reads config parameters from proto
  void loadParameters(const pb::io::WakeWordParams &wakeword_params);

  // Starts malos_psphinx thread
  bool startPipe();

  // Kills malos_psphinx thread
  bool stopPipe();

  // Sends via zmq_push voice commands found
  void returnMatch(std::string match);

  // Validates language and dictionary paths
  bool validatePaths();

  // Pipe handler for pocketsphinx
  FILE *sphinx_pipe_ = NULL;

  // sets main wakeword parameter, like "MATRIX"
  std::string wakeword;

  // sets language models path
  std::string lm_path;

  // sets dictionary models path
  std::string dic_path;

  // sets microphone channel (0-8)
  // 0-7 individual mic, 8 all mics on the same time
  int16_t channel = 8;

  // sets enable/disable voice detection service
  bool enabled = false;

  // sets pocketsphinx verbose output
  bool verbose = false;
};

}  // namespace matrix_malos

#endif  // SRC_DRIVER_WAKEWORD_H_
