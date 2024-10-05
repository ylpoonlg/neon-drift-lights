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

#include "endpoints.h"

/** Channel identifiers. */
enum ChannelIdx {
  CH_STEER = 0,
  CH_THROT,
  CH_AUX1,
  CH_AUX2,
  CH_MAXNUM
};

/** Endpoints data. */
struct Endpoints {
  /* Lower endpoint */
  uint32_t l = 1000;
  /* Center endpoint */
  uint32_t c = 1500;
  /* Higher endpoint */
  uint32_t h = 2000;
};

/** Channel input states. */
class Channel {
public:
  /** Channel identifier. */
  const uint32_t id; 
  /** Pin connected to the channel. */
  const uint32_t pin; 

  /** Scaled and calibrated value. [-100, 100] */
  int8_t value = 0;
  /** Raw pulse width (in us). */
  uint32_t raw_val = 0;
  /** Endpoints */
  Endpoints ep;

  /** Time of the rising edge of a pulse. */
  volatile uint32_t _rise_time = 0;
  /** Pulse width of the PWM signal (in us). */
  volatile uint32_t _pulse_width = 1500;

  /** Initialize channel. */
  Channel(uint32_t id, uint32_t pin) : id(id), pin(pin) {
    load_ep();
  }

  /**
   * Update value by scaling the raw value based on the endpoints.
   */
  inline void update_value() {
    if (raw_val >= ep.c) {
      value = map(min(raw_val, ep.h), ep.c, ep.h, 0, 100);
    } else {
      value = map(max(raw_val, ep.l), ep.l, ep.c, -100, 0);
    }
  }

  /** Load endpoints from EEPROM. Use fallback if data out of range. */
  void load_ep();

  /** Save endpoints to EEPROM. */
  void save_ep();
};

/** Global instance of channels. */
extern Channel channels[CH_MAXNUM];

/**
 * Propagate raw readings from shared variables to values.
 */
void poll_channels();

/**
 * ISR for handling PWM signals of a channel.
 */
void isr_pwm(Channel& ch);

/**
 * Set up pins and interrupts for reading channels.
 */
void setup_channels();
