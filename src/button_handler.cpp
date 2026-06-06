#include <button_handler.h>
#include <config.h>
#include <trmnl_log.h>

#if defined(PIN_KEY1) && defined(PIN_KEY2)

static String s_playlist_action = "none";

uint64_t button_handler_sleep_mask(uint64_t base_mask)
{
  return base_mask | (1ULL << PIN_KEY1) | (1ULL << PIN_KEY2);
}

void button_handler_on_wake(esp_sleep_wakeup_cause_t wakeup_reason)
{
  s_playlist_action = "none";
  if (wakeup_reason != ESP_SLEEP_WAKEUP_EXT1) {
    return;
  }

  uint64_t mask = esp_sleep_get_ext1_wakeup_status();
  if (mask & (1ULL << PIN_KEY1)) {
    // KEY1: short press = prev, long press = next. Pin is pulled HIGH at
    // idle and pressed = LOW. At wake time it's already LOW (the press is
    // what woke us). Poll until release or the long-press threshold.
    const uint32_t LONG_PRESS_MS = 600;
    uint32_t start = millis();
    while ((millis() - start) < LONG_PRESS_MS && digitalRead(PIN_KEY1) == LOW) {
      delay(10);
    }
    bool long_press = (digitalRead(PIN_KEY1) == LOW);
    s_playlist_action = long_press ? "next" : "prev";
    Log_info("KEY1 %s press: action=%s",
             long_press ? "long" : "short",
             s_playlist_action.c_str());
  }
  // KEY2 still wakes the device but takes no playlist action — reserved
  // for future custom config.
}

const String &button_handler_get_playlist_action()
{
  return s_playlist_action;
}

#else  // PIN_KEY1/PIN_KEY2 not defined — no-op stubs for other boards

uint64_t button_handler_sleep_mask(uint64_t base_mask)
{
  return base_mask;
}

void button_handler_on_wake(esp_sleep_wakeup_cause_t /*wakeup_reason*/)
{
  // No-op: this board has no KEY1/KEY2 buttons to manage.
}

const String &button_handler_get_playlist_action()
{
  static const String none = "none";
  return none;
}

#endif
