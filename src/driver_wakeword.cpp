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
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <string>

#include "./driver_wakeword.h"
#include "./src/driver.pb.h"

namespace {}  // namespace

namespace matrix_malos {

bool WakeWordDriver::ProcessConfig(const DriverConfig &config) {
  WakeWordParams wakeword_params(config.wakeword());
  const int16_t mode = static_cast<int16_t>(wakeword_params.channel());
  const std::string wakeword = std::string("" + wakeword_params.wake_word());

  std::string cmd =
      std::string("psphix_wakeword -keyphrase \"" + wakeword +
                  "\" -kws_threshold 1e-10 -inmic yes -adcdev mic_channel" +
                  std::to_string(mode));
  std::cerr << "cmd: " << cmd << std::endl;

  if (!(sphinx_pipe_ = popen(cmd.c_str(), "r"))) {
    return false;
  }

  // alsa thread.
  std::thread pocketsphinx_thread(&WakeWordDriver::PocketSphinxProcess, this);
  pocketsphinx_thread.detach();

  return true;
}

void WakeWordDriver::PocketSphinxProcess() {
  char buff[512];

  char matchbuff[100];
  snprintf(matchbuff, sizeof(matchbuff), "match: %s\n", wakeword.c_str());
  std::string matchAsStdStr = matchbuff;

  while (fgets(buff, sizeof(buff), sphinx_pipe_) != NULL) {
    if (std::strcmp(buff, matchbuff) == 0) {
      std::cerr << "run_callback!: " << cmd << std::endl;
      WakeWordParams wakewordUpdate;
      wakewordUpdate.set_wake_word(wakeword);
      std::string buffer;
      wakewordUpdate.SerializeToString(&buffer);
      zqm_push_update_->Send(buffer);
    }
  }

  pclose(in);
}

bool WakeWordDriver::SendUpdate() { return true; }
}
