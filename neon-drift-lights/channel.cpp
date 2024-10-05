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

#include <Arduino.h>
#include <EEPROM.h>

#include "channel.h"
#include "config.h"

Channel channels[CH_MAXNUM] = {
  Channel(CH_STEER, PIN_CH_STEER),
  Channel(CH_THROT, PIN_CH_THROT),
  Channel(CH_AUX1, PIN_CH_AUX1),
  Channel(CH_AUX2, PIN_CH_AUX2)
};

void Channel::load_ep() {
  const uint32_t addr = id * sizeof(Endpoints);
  EEPROM.get(addr, ep);
  // Fallback
  if (ep.l < 500 || ep.l > 2500) ep.l = 1000;
  if (ep.c < 500 || ep.c > 2500) ep.c = 1500;
  if (ep.h < 500 || ep.h > 2500) ep.h = 2000;
}

void Channel::save_ep() {
  const uint32_t addr = id * sizeof(Endpoints);
  EEPROM.put(addr, ep);
}

void isr_pwm(Channel& ch) {
  const uint32_t cur_time = micros();
  if (digitalRead(ch.pin) == HIGH) { // Rise
    ch._rise_time = cur_time;
  } else { // Fall
    ch._pulse_width = cur_time - ch._rise_time;
  }
}

void poll_channels() {
  // Access global raw states while temporarily disabling interrupts.
  noInterrupts();
  for (int i = 0; i < CH_MAXNUM; i++) {
    channels[i].raw_val = channels[i]._pulse_width;
  }
  interrupts();

  for (int i = 0; i < CH_MAXNUM; i++) {
    channels[i].update_value();
  }
}

void setup_channels() {
  // Set up PWM interrupts
#if PIN_CH_STEER >= 0
  pinMode(PIN_CH_STEER, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CH_STEER), []() {
    isr_pwm(channels[CH_STEER]);
  }, CHANGE);
#endif
#if PIN_CH_THROT >= 0
  pinMode(PIN_CH_THROT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CH_THROT), []() {
    isr_pwm(channels[CH_THROT]);
  }, CHANGE);
#endif
#if PIN_CH_AUX1 >= 0
  pinMode(PIN_CH_AUX1, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CH_AUX1), []() {
    isr_pwm(channels[CH_AUX1]);
  }, CHANGE);
#endif
#if PIN_CH_AUX2 >= 0
  pinMode(PIN_CH_AUX2, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CH_AUX2), []() {
    isr_pwm(channels[CH_AUX2]);
  }, CHANGE);
#endif
}
