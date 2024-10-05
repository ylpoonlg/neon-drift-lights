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
#include <FastLED.h>

#include "channel.h"
#include "config.h"
#include "utils.h"
#include "lights.h"

static CRGB decel_lights[DECEL_LED_PIXELS];
static CRGB brake2_lights[BRAKE2_LED_PIXELS];

static inline void handle_head_lights(const Channel channels[]) {
  static CHFilter filter(BRAKE_SMOOTHING);
  static uint32_t last_tar = 0;

  uint32_t tar_value = 0;
#if PIN_CH_AUX1 >= 0
  if (abs(channels[CH_AUX1].value) >= 50) {
    tar_value = HEAD_LIGHT_MAX;
  } else {
    tar_value = 0;
  }
#else
  static uint32_t idle_st_time = (uint32_t)(-HEADLIGHT_DIM_TIMEOUT);
  const uint32_t cur_time = millis();
  if (abs(channels[CH_THROT].value) >= 10 || HEADLIGHT_DIM_TIMEOUT < 0) {
    tar_value = HEAD_LIGHT_MAX;
    idle_st_time = cur_time;
  } else {
    if (cur_time - idle_st_time >= HEADLIGHT_DIM_TIMEOUT) {
      tar_value = HEAD_LIGHT_MID;
      idle_st_time = cur_time - HEADLIGHT_DIM_TIMEOUT;
    } else {
      tar_value = HEAD_LIGHT_MAX;
    }
  }
#endif

  if (tar_value > last_tar) {
    filter.set_value(255);
    tar_value = (tar_value * 1.2 + last_tar) / 2;
  } else {
    filter.update_step(tar_value);
  }
  analogWrite(PIN_LED_HEAD, filter.get_value());
  last_tar = tar_value;
}

static inline void handle_brake_lights(const Channel channels[]) {
  static CHFilter filter(BRAKE_SMOOTHING);

  uint32_t tar_value = 0;
  if (channels[CH_THROT].value < BRAKE_THRESH) {
    tar_value = BRAKE_LIGHT_MAX;
    fill_solid(brake2_lights, BRAKE2_LED_PIXELS, BRAKE2_COLOR_RED);
  } else {
#if BRAKE_AS_TAIL_LIGHTS
    tar_value = BRAKE_LIGHT_MID;
    fill_solid(brake2_lights, BRAKE2_LED_PIXELS, CRGB::Black);
#else
    tar_value = 0;
#endif
  }

  analogWrite(PIN_LED_BRAKE, filter.update_step(tar_value));
}

static inline void set_decel_pixel(uint32_t i, uint32_t color) {
#if DECEL_LED_REVERSE
  decel_lights[DECEL_LED_PIXELS - i - 1] = color;
#else
  decel_lights[i] = color;
#endif
}

static inline void handle_decel_lights(const Channel channels[]) {
  static CHFilter filter(DECEL_SMOOTHING_UP);
  static CHFilter filter2(DECEL_SMOOTHING_DN);
  static uint32_t last_update = 0;

  const int32_t value = channels[CH_THROT].value;
  const int32_t throt2 = filter2.update_step(value);
  int32_t throt = filter.update_step(value);
  if (throt2 < throt) {
    throt = throt2;
  }

  // Skip updating
  const uint32_t cur_time = millis();
  if (cur_time - last_update < DECEL_UPDATE_RATE) return;
  last_update = cur_time;

  fill_solid(decel_lights, DECEL_LED_PIXELS, CRGB::Black);
  if (abs(throt) < DECEL_NULL_THRESH) {
    const uint32_t bar_len = round((float)DECEL_LED_PIXELS/3);
    for (int i = 1; i <= bar_len; i++)
      set_decel_pixel(DECEL_LED_PIXELS - i, DECEL_COLOR_BLUE);
  } else if (throt >= 0) {
    const uint32_t bar_len = ceil(
        ((float)throt - DECEL_NULL_THRESH) / (100.0f - DECEL_NULL_THRESH)
        * DECEL_LED_PIXELS);
    for (int i = 0; i < bar_len; i++)
      set_decel_pixel(i, DECEL_COLOR_GREEN);
  } else if (throt < DECEL_BRAKE_THRESH) {
    for (int i = 0; i < DECEL_LED_PIXELS; i++)
      set_decel_pixel(i, DECEL_COLOR_RED);
  } else {
    const uint32_t bar_len = ceil(throt / -100.0f * DECEL_LED_PIXELS);
    for (int i = 1; i <= bar_len; i++)
      set_decel_pixel(DECEL_LED_PIXELS - i, DECEL_COLOR_YELLOW);
  }
}

static inline void handle_backfire(const Channel channels[]) {
  static CHFilter filter(BACKFIRE_SMOOTHING);
  static int32_t throt_last = 0;
  static uint32_t bfire_st_time = 0, bfire_int = 0, bfire_dur = 0;
  static bool bfire_is_on = false;

  const uint32_t cur_time = millis();
  const int32_t throt = filter.update_step(channels[CH_THROT].value);
  if ((throt < throt_last && throt_last >= BACKFIRE_THRESH_L) ||
      throt >= BACKFIRE_THRESH_H) {
    if (!bfire_is_on) {
      bfire_is_on = true;
      bfire_st_time = cur_time;
      bfire_int = random(BACKFIRE_MAX_INTERVAL);
      bfire_dur = random(BACKFIRE_MAX_DURATION);
    }
  }

  if (bfire_is_on) {
    if (cur_time - bfire_st_time >= bfire_int + bfire_dur) {
      digitalWrite(PIN_LED_BACKFIRE, LOW);
      bfire_is_on = false;
    } else if (cur_time - bfire_st_time >= bfire_int) {
      digitalWrite(PIN_LED_BACKFIRE, HIGH);
    }
  } else {
    digitalWrite(PIN_LED_BACKFIRE, LOW);
  }

  throt_last = throt;
}

#if VERBOSE == 0
/**
 * Print simulated virtual lights in serial.
 */
static inline void show_virtual_lights() {
  Serial.print("\r ");

  if (digitalRead(PIN_LED_HEAD))
    Serial.print("\033[47m  ");
  else
    Serial.print("\033[40m  ");
  Serial.print("\033[0m | ");

  if (digitalRead(PIN_LED_BRAKE))
    Serial.print("\033[41m  ");
  else
    Serial.print("\033[40m  ");
  Serial.print("\033[0m | ");

  static char strip_s[DECEL_LED_PIXELS * 10];
  for (int i = 0; i < DECEL_LED_PIXELS; i++) {
    const uint32_t color = decel_lights[i].as_uint32_t();
    const uint8_t r = color >> 16 & 0xff;
    const uint8_t g = color >> 8 & 0xff;
    const uint8_t b = color & 0xff;
    sprintf(strip_s, "\033[48;2;%u;%u;%um  \033[0m ", r, g, b);
    Serial.print(strip_s);
  }
  Serial.print("\033[0m| ");

  if (digitalRead(PIN_LED_BACKFIRE))
    Serial.print("\033[43m  ");
  else
    Serial.print("\033[40m  ");
  Serial.print("\033[0m");
}
#endif

void handle_lights(const Channel channels[]) {
  handle_head_lights(channels);
  handle_brake_lights(channels);
  handle_decel_lights(channels);
  handle_backfire(channels);

  FastLED.show();
#if VERBOSE == 0
  show_virtual_lights();
#endif
}

void setup_lights() {
  pinMode(PIN_LED_HEAD, OUTPUT);
  pinMode(PIN_LED_BRAKE, OUTPUT);
  pinMode(PIN_LED_BRAKE2, OUTPUT);
  pinMode(PIN_LED_DECEL, OUTPUT);
  pinMode(PIN_LED_BACKFIRE, OUTPUT);

  FastLED.addLeds<WS2812B, PIN_LED_DECEL, GRB>(decel_lights, DECEL_LED_PIXELS);
  FastLED.addLeds<WS2812B, PIN_LED_BRAKE2, GRB>(brake2_lights, BRAKE2_LED_PIXELS);
}
