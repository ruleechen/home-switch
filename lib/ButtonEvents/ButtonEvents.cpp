#include "ButtonEvents.h"

namespace Victoria::Events {

  ButtonEvents::ButtonEvents(String accessoryId, uint8_t inputPin) {
    _accessoryId = accessoryId;
    _inputPin = inputPin;
  }

  void ButtonEvents::loop() {
    auto state = _loadState();
    if (state != _lastState) {
      _lastState = state;
      if (onClick) {
        onClick(_accessoryId, state);
      }
    }
  }

  int ButtonEvents::_loadState() {
    int clicks = 0;
    if (_buttonState == AWAIT_PRESS) {
      clicks = 0;
      if (digitalRead(_inputPin) == LOW) {
        _buttonState = DEBOUNCE_PRESS;
        _eventTime = millis();
      }
    }

    else if (_buttonState == DEBOUNCE_PRESS) {
      if ((millis() - _eventTime) > _debouncePressTime) {
        _buttonState = AWAIT_RELEASE;
        _eventTime = millis();
      }
    }

    else if (_buttonState == AWAIT_RELEASE) {
      if (digitalRead(_inputPin) == HIGH) {
        if ((millis() - _eventTime) > _holdTime) {
          clicks = -1;
        }
        _buttonState = DEBOUNCE_RELEASE;
        _eventTime = millis();
      }
    }

    else if (_buttonState == DEBOUNCE_RELEASE) {
      if ((millis() - _eventTime) > _debounceReleaseTime) {
        if (clicks < 0) {
          _buttonState = AWAIT_PRESS;
          return -1;
        }
        clicks += 1;
        _buttonState = AWAIT_MULTI_PRESS;
        _eventTime = millis();
      }
    }

    else { // (_buttonState == AWAIT_MULTI_PRESS)
      if (digitalRead(_inputPin) == LOW) {
        _buttonState = DEBOUNCE_PRESS;
        _eventTime = millis();
      } else if ((millis() - _eventTime) > _multiClickTime) {
        _buttonState = AWAIT_PRESS;
        return clicks;
      }
    }

    return 0;
  }

} // namespace Victoria::Events