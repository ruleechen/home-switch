#include "RadioPortal.h"

namespace Victoria::Components {

  RadioPortal::RadioPortal() {}

  RadioPortal::~RadioPortal() {
    if (_rf) {
      delete _rf;
      _rf = NULL;
    }
  }

  void RadioPortal::setup() {
    auto model = radioStorage.load();
    if (model.inputPin > -1) {
      pinMode(model.inputPin, INPUT);
      _rf = new RCSwitch();
      _rf->enableReceive(model.inputPin);
    }
  }

  void RadioPortal::loop() {
    auto now = millis();
    if (
      now - _lastAvailable > AVAILABLE_THROTTLE_TIMESPAN &&
      _rf && _rf->available()
    ) {
      // payload
      unsigned long value = _rf->getReceivedValue();
      unsigned int bits = _rf->getReceivedBitlength();
      unsigned int protocol = _rf->getReceivedProtocol();
      // logs
      auto received = String(protocol) + "/" + String(value) + "/" + String(bits) + "bits";
      console.log("[RadioPortal] received " + received);
      // message
      RadioMessage message = {
        .value = value,
        .bits = bits,
        .protocol = protocol,
        .timestamp = now,
      };
      // throttle
      auto lastMessage = radioStorage.getLastReceived();
      auto isThrottled = (
        lastMessage.value == message.value &&
        lastMessage.protocol == message.protocol &&
        now - lastMessage.timestamp < MESSAGE_THROTTLE_TIMESPAN
      );
      if (!isThrottled) {
        _handleMessage(message, PressTypeClick);
        radioStorage.broadcast(message);
      }
      // reset state
      _rf->resetAvailable();
      _lastAvailable = now;
    }
  }

  void RadioPortal::_handleMessage(const RadioMessage& message, PressType press) {
    auto model = radioStorage.load();
    for (const auto& rule : model.rules) {
      if (
        rule.value == message.value &&
        rule.protocol == message.protocol &&
        rule.press == press
      ) {
        _proceedAction(rule);
      }
    }
  }

  void RadioPortal::_proceedAction(const RadioRule& rule) {
    switch (rule.action) {
      case RadioActionWiFiSta:
        WiFi.mode(WIFI_STA);
        console.log("[RadioPortal] WiFi STA");
        break;
      case RadioActionWiFiStaAp:
        WiFi.mode(WIFI_AP_STA);
        console.log("[RadioPortal] WiFi STA+AP");
        break;
      case RadioActionWiFiReset:
        WiFi.disconnect(true);
        console.log("[RadioPortal] WiFi Reset");
        break;
      case RadioActionEspRestart:
        ESP.restart();
        console.log("[RadioPortal] ESP Restart");
        break;
      default:
        break;
    }
    if (onAction) {
      onAction(rule);
    }
  }

} // namespace Victoria::Components
