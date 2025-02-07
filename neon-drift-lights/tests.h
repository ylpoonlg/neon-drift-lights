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

inline void do_test() {
  const uint32_t cur_time = millis();
  const uint32_t time_cyc = (cur_time / 1000) % 20;
  if (time_cyc < 5) {
    channels[CH_THROT].value = 100;
  } else {
    channels[CH_THROT].value = 0;
  }
}
