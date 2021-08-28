#include "HomeKitService.h"

namespace Victoria::HomeKit {

  HomeKitService::HomeKitService(const String& id, const ServiceSetting& setting, homekit_characteristic_t* characteristic) {
    serviceId = id;
    serviceSetting = setting;
    serviceCharacteristic = characteristic;
  }

  HomeKitService::~HomeKitService() {
    onStateChange = NULL;
    if (serviceCharacteristic) {
      serviceCharacteristic->setter_ex = NULL;
      delete serviceCharacteristic;
      serviceCharacteristic = NULL;
    }
    console.log("service disposed " + serviceId);
  }

  void HomeKitService::setup() {}

  void HomeKitService::loop() {}

  ServiceState HomeKitService::getState() { return {}; }

  void HomeKitService::setState(const ServiceState& state) {}

  void HomeKitService::notifyState() {
    if (serviceCharacteristic) {
      homekit_characteristic_notify(serviceCharacteristic, serviceCharacteristic->value);
    }
  }

  void HomeKitService::_fireStateChange(const ServiceState& state) {
    if (onStateChange) {
      onStateChange(state);
    }
  }

} // namespace Victoria::HomeKit