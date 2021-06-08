#include <LittleFS.h>
#include "ConfigStore.h"

namespace Victoria {
  namespace Components {

    ConfigStore::ConfigStore() {
      if (!LittleFS.begin()) {
        console.error("Failed to mount file system");
      }
    }

    SettingModel ConfigStore::load() {
      // [default result]
      SettingModel model;

      // [open file]
      File file = LittleFS.open(CONFIG_FILE_PATH, "r");
      if (!file) {
        console.error("Failed to open config file");
        return model;
      }
      size_t size = file.size();
      if (size > DEFAULT_FILE_SIZE) {
        console.error("Config file size is too large");
        return model;
      }

      // [read file]
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      // We don't use String here because ArduinoJson library requires the input
      // buffer to be mutable. If you don't use ArduinoJson, you may as well
      // use file.readString instead.
      file.readBytes(buf.get(), size);

      // [deserialize]
      // DynamicJsonDocument doc(DEFAULT_FILE_SIZE); // Store data in the heap - Dynamic Memory Allocation
      StaticJsonDocument<DEFAULT_FILE_SIZE> doc; // Store data in the stack - Fixed Memory Allocation
      auto error = deserializeJson(doc, buf.get());
      if (error) {
        console.error("Failed to parse config file");
        return model;
      }

      // [convert]
      _deserializeFrom(model, doc);
      return model;
    }

    bool ConfigStore::save(SettingModel model) {
      // [convert]
      // DynamicJsonDocument doc(DEFAULT_FILE_SIZE); // Store data in the heap - Dynamic Memory Allocation
      StaticJsonDocument<DEFAULT_FILE_SIZE> doc; // Store data in the stack - Fixed Memory Allocation
      _serializeTo(model, doc);

      // [open file]
      File file = LittleFS.open(CONFIG_FILE_PATH, "w");
      if (!file) {
        console.error("Failed to open config file for writing");
        return false;
      }

      // [write]
      serializeJson(doc, file);

      // [log]
      if (VEnv == VTest) {
        char buffer[DEFAULT_FILE_SIZE];
        serializeJsonPretty(doc, buffer);
        console.debug(buffer);
      }

      return true;
    }

    void ConfigStore::_serializeTo(const SettingModel& model, StaticJsonDocument<DEFAULT_FILE_SIZE>& doc) {
      int i = 0;
      for (const auto& pair : model.settings) {
        String id = pair.first;
        AccessorySetting setting = pair.second;
        int type = setting.type; // convert enum to int
        doc["s"][i][0] = id;
        doc["s"][i][1] = setting.name;
        doc["s"][i][2] = type;
        doc["s"][i][3] = setting.outputIO;
        doc["s"][i][4] = setting.inputIO;
        doc["s"][i][5] = setting.outputLevel;
        doc["s"][i][6] = setting.inputLevel;
        i++;
      }
    }

    void ConfigStore::_deserializeFrom(SettingModel& model, const StaticJsonDocument<DEFAULT_FILE_SIZE>& doc) {
      auto settingsDoc = doc["s"];
      if (settingsDoc) {
        int index = -1;
        while (true) {
          auto item = settingsDoc[++index];
          if (!item) {
            break;
          }
          String id = item[0];
          if (!id) {
            break;
          }
          int type = item[2];
          AccessorySetting setting = {
            .name = item[1],
            .type = AccessoryType(type), // convert int to enum
            .outputIO = item[3],
            .inputIO = item[4],
            .outputLevel = item[5],
            .inputLevel = item[6],
          };
          model.settings[id] = setting;
        }
      }
    }

  }
}
