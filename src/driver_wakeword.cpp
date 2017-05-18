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

#include <unistd.h>
#include <sys/stat.h>
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
  stopPipe();
  WakeWordParams wakeword_params(config.wakeword());
  if (wakeword_params.stop_recognition()) {
    std::cout << "==> disable wakeword service.." << std::endl;
    return true;
  }
  loadParameters(wakeword_params);
  if (validatePaths()) {
    verbose = wakeword_params.enable_verbose();
    enable = startPipe();
    return enable;
  } else {
    zmq_push_error_->Send("invalid configuration paths");
    return false;
  }
}

void WakeWordDriver::loadParameters(WakeWordParams wakeword_params) {
  channel = static_cast<int16_t>(wakeword_params.channel());
  wakeword = std::string("" + wakeword_params.wake_word());
  lm_path = std::string("" + wakeword_params.lm_path());
  dic_path = std::string("" + wakeword_params.dic_path());
  std::cout << "==> wakeword: " << wakeword << std::endl;
  std::cout << "==> lenguaje path: " << lm_path << std::endl;
  std::cout << "==> dictionary path: " << dic_path << std::endl;
}

bool WakeWordDriver::startPipe() {
  std::string cmd = std::string(
      "./malos_psphinx -keyphrase \"" + wakeword +
      "\" -kws_threshold 1e-20 -dict \"" + dic_path + "\" -lm \"" + lm_path +
      "\" -inmic yes -adcdev mic_channel" + std::to_string(channel));

  if (!verbose)
    cmd = cmd + " 2> /dev/null";

  std::cout << "Starting PocketSphinx thread.." << std::endl;
  if (!(sphinx_pipe_ = popen(cmd.c_str(), "r"))) {
    return false;
  }
  // alsa thread.
  std::thread pocketsphinx_thread(&WakeWordDriver::PocketSphinxProcess, this);
  pocketsphinx_thread.detach();
  returnMatch("voice recognition ready");

  return true;
}

bool WakeWordDriver::stopPipe() {
  if (sphinx_pipe_ != NULL && enable) {
    enable = false;
    std::cout << "Stoping PocketSphinx thread.." << std::endl;
    if (system(std::string("killall malos_psphinx").c_str()) == -1) {
      return false;
    }
    sleep(2);
  }
  return true;
}

void WakeWordDriver::PocketSphinxProcess() {
  char buff[512];
  char match[100];
  snprintf(match, sizeof(match), "match: %s", wakeword.c_str());

  // processing pipe output
  while (fgets(buff, sizeof(buff), sphinx_pipe_) != NULL && enable) {
    std::string sbuff = buff;
    std::size_t found = sbuff.find(match);
    if (found != std::string::npos) {
      std::cout << "[SEND] recognition " << sbuff;
      sbuff = sbuff.substr(sbuff.find_first_of(" \t") + 1);
      sbuff = sbuff.erase(sbuff.length() - 1);
      returnMatch(sbuff);
    } else {
      std::cout << "[SKIP] recognition " << sbuff;
    }
  }
  pclose(sphinx_pipe_);
}

void WakeWordDriver::returnMatch(std::string match) {
  WakeWordParams wakewordUpdate;
  wakewordUpdate.set_wake_word(match);
  std::string buffer;
  wakewordUpdate.SerializeToString(&buffer);
  zqm_push_update_->Send(buffer);
}

bool WakeWordDriver::validatePaths() {
  if (!exists_path(dic_path))
    return false;
  if (!exists_path(lm_path))
    return false;
  return true;
}

inline bool WakeWordDriver::exists_path(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

}  // namespace matrix_malos

