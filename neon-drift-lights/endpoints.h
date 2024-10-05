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

/** Time to hold down the end-point button for calibration (in ms) */
#define EP_BTN_HOLD 1500

/**
 * Poll and handle endpoint button events.
 */
void poll_ep_btn();

/**
 * Set up endpoint calibration button.
 */
void setup_ep_btn();
