#ifdef WIFI_ENABLE
#include "template.h"

#include "eeprom.h"
#include "fan.h"
#include "led.h"
#include "version.h"
#include "wifi.h"

String template_process(const String &var) {
  if (var == "FW_VERSION") {
    return String(String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + "." +
                  String(VERSION_PATCH));
  } else if (var == "BUILD_DATE") {
    return String(__DATE__);
  } else if (var == "BUILD_TIME") {
    return String(__TIME__);

  } else if (var == "DEVICE_NAME") {
    return String(eeprom_get_name());
  } else if (var == "CURRENT_PWM") {
    return String(fan_get_pwm());
  } else if (var == "DEVICE_MAC") {
    return String(wifi_get_mac());
  } else if (var == "FAN_VOLTAGE") {
    if (fan_get_12V()) {
      return String("12");
    }
    return String("5");
  } else if (var == "ACT_LED") {
    if (led_get_blinking()) {
      return String("blinking");
    }
    return String("off");
  } else if (var == "CURRENT_RPM") {
    return String(fan_get_rpm());
  }

  return String("__MISSING_VAR__");
}
#endif /* WIFI_ENABLE */