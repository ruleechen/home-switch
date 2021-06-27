#ifndef BaseAccessory_h
#define BaseAccessory_h

#include <map>
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "Commons.h"

namespace Victoria::Components {
  class BaseAccessory {
    typedef std::function<void(const AccessoryState&)> TStateChangeHandler;
    public:
      BaseAccessory(String id, uint8_t outputPin, homekit_server_config_t* serverConfig, homekit_characteristic_t* mainCharacteristic);
      ~BaseAccessory();
      String accessoryId;
      TStateChangeHandler onStateChange;
      virtual AccessoryState getState();
      virtual void setState(const AccessoryState& state);
      static BaseAccessory* findAccessoryById(const String& accessoryId);
      static void heartbeatAll();
      static void loopAll();
      static void resetAll();
    protected:
      uint8_t _outputPin;
      homekit_server_config_t* _serverConfig;
      homekit_characteristic_t* _mainCharacteristic;
      void _init();
      void _notify();
      static BaseAccessory* _findAccessory(homekit_characteristic_t* mainCharacteristic);
  };
} // namespace Victoria::Components

#endif // BaseAccessory_h
