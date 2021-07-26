#ifndef RadioPortal_h
#define RadioPortal_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <RH_ASK.h>
#include "Commons.h"
#include "RadioStorage.h"

// timespan in millianseconds
#define DOUBLE_CLICK_TIMESPAN_FROM 300
#define DOUBLE_CLICK_TIMESPAN_TO 500
#define LONG_PRESS_TIMESPAN 2000
#define RESET_PRESS_TIMESPAN 2500

namespace Victoria::Components {
  class RadioPortal {
    typedef std::function<void(const RadioRule&)> TRadioAction;

   public:
    RadioPortal();
    ~RadioPortal();
    void setup();
    void loop();
    TRadioAction onAction;

   private:
    RH_ASK* _rf = NULL;
    RadioMessage _lastMessage = {};
    RadioPressState _lastPressState = PressStateAwait;
    void _handleMessage(const RadioMessage& message, RadioPressState press);
    void _proceedAction(const RadioRule& rule);
  };
} // namespace Victoria::Components

#endif // RadioPortal_h
