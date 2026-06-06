#pragma once

#include <Arduino.h>
#include <esp_sleep.h>

// Button-action handling for the xiao_epaper KEY1/KEY2 buttons.
//
// Centralizes everything KEY1/KEY2-related so bl.cpp stays clean of
// custom logic that would otherwise conflict with upstream changes.
// On boards without PIN_KEY1/PIN_KEY2 defined, all functions are
// no-ops and the playlist action is always "none" — so call sites
// don't need #ifdef guards.

// Returns base_mask with KEY1 and KEY2 bits OR-ed in when those buttons
// are supported by the board; otherwise returns base_mask unchanged.
// Use to build the bitmask for esp_sleep_enable_ext1_wakeup.
uint64_t button_handler_sleep_mask(uint64_t base_mask);

// Decode which button woke the device and, on KEY1, run short/long
// press detection (poll until release or the long-press threshold).
// Stores the resulting playlist action for later retrieval. Safe to
// call on any wake reason — only acts on ESP_SLEEP_WAKEUP_EXT1.
void button_handler_on_wake(esp_sleep_wakeup_cause_t wakeup_reason);

// The playlist action resolved from the last wake event: "next",
// "prev", or "none". Forwarded to Terminus in the X-Playlist-Action
// HTTP header on the next /api/display request.
const String &button_handler_get_playlist_action();
