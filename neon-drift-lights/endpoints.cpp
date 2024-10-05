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
#include "endpoints.h"
#include "utils.h"

static inline void set_status_light(bool is_on, uint32_t blinks) {
  static uint32_t blink_st_time = 0;
  if (is_on) {
    if (blinks == 0) {
      digitalWrite(PIN_LED_STATUS, HIGH);
    } else {
      const uint32_t cur_time = millis();
      const uint32_t blink_int = 1000 / REFRESH_INTERVAL;
      const uint32_t blink_per = 250 / REFRESH_INTERVAL;
      const uint32_t blink_cyc = (cur_time / REFRESH_INTERVAL) % blink_int;
      if (blink_cyc < blink_per * blinks &&
          blink_cyc % blink_per < blink_per / 2) {
        digitalWrite(PIN_LED_STATUS, HIGH);
      } else {
        digitalWrite(PIN_LED_STATUS, LOW);
      }
    }
  } else {
    digitalWrite(PIN_LED_STATUS, LOW);
  }

#if VERBOSE == 0
  Serial.print("\033[2K\r Status LED: ");
  if (digitalRead(PIN_LED_STATUS))
    Serial.print("\033[42m  ");
  else
    Serial.print("\033[40m  ");
  Serial.print("\033[0m");
#endif
}

static inline uint32_t read_calibrate_ep(const Channel& ch,
    uint32_t status_blinks) {
  do delay(REFRESH_INTERVAL); while (IS_BTN_DN(PIN_EP_BTN));
  while (IS_BTN_UP(PIN_EP_BTN)) {
    set_status_light(true, status_blinks);
    delay(REFRESH_INTERVAL);
  }
  poll_channels();
  set_status_light(false, 0);
  do delay(REFRESH_INTERVAL); while (IS_BTN_DN(PIN_EP_BTN));
  return ch.raw_val;
}

static inline void calibrate_ch_ep(Channel& ch) {
  static char log_buf[256];

  LOGPRINT(2, "[EP] Start Calibration\r\n");
  set_status_light(true, 0);

  LOGPRINT(2, "[EP] Set EP_L then press the button:\r\n");
  ch.ep.l = read_calibrate_ep(ch, 1);
  sprintf(log_buf, "[EP] Calibrated EP_L: %d\r\n", ch.ep.l);
  LOGPRINT(2, log_buf);

  LOGPRINT(2, "[EP] Set EP_H then press the button:\r\n");
  ch.ep.h = read_calibrate_ep(ch, 2);
  sprintf(log_buf, "[EP] Calibrated EP_H: %d\r\n", ch.ep.h);
  LOGPRINT(2, log_buf);

  LOGPRINT(2, "[EP] Set EP_C then press the button:\r\n");
  ch.ep.c = read_calibrate_ep(ch, 3);
  sprintf(log_buf, "[EP] Calibrated EP_C: %d\r\n", ch.ep.c);
  LOGPRINT(2, log_buf);

  ch.save_ep();
  LOGPRINT(2, "[EP] End Calibration\r\n");
}

static inline void clear_eeprom() {
  LOGPRINT(2, "[EP] Clearing EEPROM...\r\n");
  for (int i = 0; i < EEPROM.length(); i++) EEPROM.write(i, 0);
  for (int i = 0; i < CH_MAXNUM; i++) channels[i].load_ep();
}

void poll_ep_btn() {
  static bool ep_btn_is_pressed = false;
  static bool rst_btn_is_pressed = false;
  static uint32_t ep_btn_st_time = 0;

  // Handle ep button
  if (IS_BTN_DN(PIN_EP_BTN)) {
    const uint32_t cur_time = millis();
    if (!ep_btn_is_pressed) {
      ep_btn_st_time = cur_time;
      ep_btn_is_pressed = true;
    }
    if (cur_time - ep_btn_st_time >= EP_BTN_HOLD) {
      calibrate_ch_ep(channels[CH_THROT]);
      ep_btn_is_pressed = false;
    }
  } else {
    ep_btn_is_pressed = false;
  }
  // Handle reset button
  if (IS_BTN_DN(PIN_RST_BTN)) {
    if (!rst_btn_is_pressed) {
      clear_eeprom();
      rst_btn_is_pressed = true;
    }
  } else {
    rst_btn_is_pressed = false;
  }
}

void setup_ep_btn() {
  pinMode(PIN_EP_BTN, INPUT_PULLUP);
  pinMode(PIN_RST_BTN, INPUT_PULLUP);
  pinMode(PIN_LED_STATUS, OUTPUT);

  set_status_light(false, 0);
}
