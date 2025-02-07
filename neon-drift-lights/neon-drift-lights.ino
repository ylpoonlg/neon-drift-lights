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

#include "channel.h"
#include "config.h"
#include "endpoints.h"
#include "lights.h"
#include "tests.h"
#include "utils.h"

void setup() {
#if VERBOSE >= 0
  Serial.begin(115200);
#endif

  setup_ep_btn();
  setup_channels();
  setup_lights();
}

void loop() {
#ifdef RUN_TEST
  do_test();
#else
  poll_ep_btn();
  poll_channels();
#endif

  handle_lights(channels);

  delay(REFRESH_INTERVAL);
}
