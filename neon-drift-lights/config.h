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

/**
 * Configurations
 *
 * This default configuration is designed to work on Arduino Pro Micro boards,
 * but can be changed accordingly to be used on other boards.
 */

/**
 * Input Pins
 *
 * Set -1 to disable the channel.
 *
 * Currently only support interrupt pins.
 * Check which pins on your board support intperrupts at
 * https://reference.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
// Pin for steering channel
#define PIN_CH_STEER -1
// Pin for throttle channel
#define PIN_CH_THROT 2
// Pin for aux1 channel
#define PIN_CH_AUX1 -1
// Pin for aux2 channel
#define PIN_CH_AUX2 -1

/**
 * Output Pins
 */
// Pin for head lights
#define PIN_LED_HEAD 9
// Pin for brake lights
#define PIN_LED_BRAKE 10
// Pin for brake light strip (WS2812B)
#define PIN_LED_BRAKE2 16
// Pin for decel light strip (WS2812B)
#define PIN_LED_DECEL 14
// Pin for hazard lights
#define PIN_LED_HAZARD 15
// Pin for backfire light
#define PIN_LED_BACKFIRE 8
// Pin for status led
#define PIN_LED_STATUS 21
// Pin for end point calibration button
#define PIN_EP_BTN 20
// Pin for clearing EEPROM settings
#define PIN_RST_BTN 19

/**
 * Lights Setup
 */
// Number of pixels in each decel light strip
#define DECEL_LED_PIXELS 8
// Reverse decel light strip direction
#define DECEL_LED_REVERSE true
// Number of pixels in brake light strip
#define BRAKE2_LED_PIXELS 16
// Use brake lights as tail lights when not braking (dimmed)
#define BRAKE_AS_TAIL_LIGHTS true
// Dim head lights after a period of inactivity (in ms) (-1 to disable)
#define HEADLIGHT_DIM_TIMEOUT 3000

/**
 * Log Verbosity
 *
 * Set 0 to enable virtual LED; -1 to disable Serial.
 */
#define VERBOSE -1

/**
 * Run dummy test (uncomment to enable)
 */
// #define RUN_TEST
