/**
 * Copyright 2025 Yat Long Poon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <Arduino.h>

#include "channel.h"
#include "config.h"

#define LOGPRINT(v, s) if (VERBOSE >= v) Serial.print(s);

#define IS_BTN_UP(p) digitalRead(p)
#define IS_BTN_DN(p) !digitalRead(p)

/** Interval between each refresh loop (in milliseconds) */
#define REFRESH_INTERVAL 50

inline void debug_channel(const Channel& ch) {
  static char log_buf[256];
  sprintf(log_buf, "[CHANNEL %u]", ch.id);
  LOGPRINT(4, log_buf);
  sprintf(log_buf, " value = %d", ch.value);
  LOGPRINT(4, log_buf);
  sprintf(log_buf, " raw_val = %u", ch.raw_val);
  LOGPRINT(4, log_buf);
  sprintf(log_buf, " ep.l = %u", ch.ep.l);
  LOGPRINT(4, log_buf);
  sprintf(log_buf, " ep.c = %u", ch.ep.c);
  LOGPRINT(4, log_buf);
  sprintf(log_buf, " ep.h = %u\r\n", ch.ep.h);
  LOGPRINT(4, log_buf)
}

class CHFilter {
private:
  float avg_value = 0;
  const uint32_t smooth = 0;
public:
  CHFilter(uint32_t smooth) : smooth(smooth) {}

  int32_t update_step(int32_t new_value) {
    avg_value = (avg_value * smooth + new_value) / (smooth + 1);
    return round(avg_value);
  }

  int32_t get_value() {
    return round(avg_value);
  }

  void set_value(int32_t value) {
    avg_value = (float) value;
  }
};
