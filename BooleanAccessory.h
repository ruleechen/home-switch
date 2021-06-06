#ifndef BooleanAccessory_h
#define BooleanAccessory_h

#include <Arduino.h>
#include "BaseAccessory.h"

namespace Victoria {
  namespace Components {
    class BooleanAccessory: public BaseAccessory {
      typedef std::function<void(const AccessoryState&)> TStateChangeHandler;
      public:
        BooleanAccessory(String id, uint8_t outputPin);
        virtual AccessoryState getState();
        virtual void setState(const AccessoryState& state);
        TStateChangeHandler onStateChange;
      private:
        static void _setter_ex(homekit_characteristic_t *ch, const homekit_value_t value);
    };
  }
}

#endif //BooleanAccessory_h
