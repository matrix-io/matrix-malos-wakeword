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
#include <sphinxbase/ad.h>
#include <sphinxbase/err.h>
#include "pocketsphinx.h"

#include "./driver_wakeword.h"
#include "./src/driver.pb.h"

namespace {

/* Sleep for specified msec */
static void sleep_msec(int32 ms) {
#if (defined(_WIN32) && !defined(GNUWINCE)) || defined(_WIN32_WCE)
  Sleep(ms);
#else
  /* ------------------- Unix ------------------ */
  struct timeval tmo;

  tmo.tv_sec = 0;
  tmo.tv_usec = ms * 1000;

  select(0, NULL, NULL, NULL, &tmo);
#endif
}

/*
 * Main utterance processing loop:
 *     for (;;) {
 *        start utterance and wait for speech to process
 *        decoding till end-of-utterance silence will be detected
 *        print utterance result;
 *     }
 */
static void recognize_from_microphone() {
  ad_rec_t *ad;
  if ((ad = ad_open_dev(cmd_ln_str_r(config_, "-adcdev"),
                        (int)cmd_ln_float32_r(config_, "-samprate"))) == NULL)
    E_FATAL("Failed to open audio device\n");
  if (ad_start_rec(ad) < 0)
    E_FATAL("Failed to start recording\n");

  if (ps_start_utt(ps_) < 0)
    E_FATAL("Failed to start utterance\n");
  uint8 utt_started = FALSE;
  E_INFO("Ready....\n");

  int32 audio;
  int16 adbuf[2048];
  const char *hyp;
  for (;;) {
    if ((audio = ad_read(ad, adbuf, 2048)) < 0)
      E_FATAL("Failed to read audio\n");
    ps_process_raw(ps_, adbuf, audio, FALSE, FALSE);
    const int8 in_speech = ps_get_in_speech(ps_);
    if (in_speech && !utt_started) {
      utt_started = TRUE;
      E_INFO("Listening...\n");
    }
    if (!in_speech && utt_started) {
      /* speech -> silence transition, time to start new utterance  */
      ps_end_utt(ps_);
      hyp = ps_get_hyp(ps_, NULL);
      if (hyp != NULL) {
        process_rules(hyp);
        fflush(stdout);
      }

      if (ps_start_utt(ps_) < 0)
        E_FATAL("Failed to start utterance\n");
      utt_started = FALSE;
      E_INFO("Ready....\n");
    }
    sleep_msec(10);
  }
  ad_close(ad);
}

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
