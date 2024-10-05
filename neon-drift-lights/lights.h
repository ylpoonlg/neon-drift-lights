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

/**
 * Thresholds
 */
// Values lower than this are considered braking (for brake lights)
#define BRAKE_THRESH -30
// Values within +/-DECEL_NULL_THRESH are considered idle
#define DECEL_NULL_THRESH 10
// Values lower than this are considered decel (for decel lights)
#define DECEL_BRAKE_THRESH -60
// Backfire is enabled above this value (when power reduces)
#define BACKFIRE_THRESH_L 10
// Backfire is enable above this value (always on)
#define BACKFIRE_THRESH_H 80

/**
 * Smoothing
 *
 * Size of average filter applied to the input/output signal. (0 to disable)
 */
// Head lights gradient effect
#define HEAD_SMOOTHING 4
// Brake lights gradient effect
#define BRAKE_SMOOTHING 2
// Smoothing for decel lights when increasing power
#define DECEL_SMOOTHING_UP 2
// Smoothing for decel lights when braking
#define DECEL_SMOOTHING_DN 1
// Gives a "delay" effect to the backfires
#define BACKFIRE_SMOOTHING 8

/**
 * Timings (Subject to REFRESH_INTERVAL)
 */
// How often the decel lights updates (in ms)
#define DECEL_UPDATE_RATE 50
// Maximum interval between two backfires (in ms)
#define BACKFIRE_MAX_INTERVAL 200
// Maximum duration of a backfire (in ms)
#define BACKFIRE_MAX_DURATION 50

/**
 * Colors and Intensities
 */
#define DECEL_COLOR_RED 0xaa0000
#define DECEL_COLOR_YELLOW 0xaa8800
#define DECEL_COLOR_GREEN 0x00aa00
#define DECEL_COLOR_BLUE 0x0000aa
#define BRAKE2_COLOR_RED 0xaa0000
#define HEAD_LIGHT_MAX 128
#define HEAD_LIGHT_MID 64
#define BRAKE_LIGHT_MAX 255
#define BRAKE_LIGHT_MID 64

void handle_lights(const Channel channels[]);

void setup_lights();
