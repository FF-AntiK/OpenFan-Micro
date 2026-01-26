#ifdef WIFI_ENABLE
#include "web.h"

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "eeprom.h"
#include "fan.h"
#include "led.h"
#include "web_data.h"
#include "wifi.h"

void web_setup(void) {
  static AsyncWebServer server(80);

  web_data_init(server);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/index.html");
  });

  server.on("/api/v0/device/reboot", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("confirm")) {
                ESP.restart();
              } else {
                request->send(400, "application/json",
                              "{\"status\":\"fail\",\"message\":\"Missing "
                              "argument `confirm`!\"}");
              }
            });

  server.on("/api/v0/fan/0/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      char buf[200] = {0};
      const uint8_t pwm_value = request->getParam("value")->value().toInt();

      fan_set_pwm(pwm_value);

      snprintf(buf, 200,
               "{\"status\":\"ok\",\"message\":\"Setting PWM to %d\"}",
               pwm_value);

      request->send(200, "application/json", buf);
    } else {
      request->send(400, "application/json",
                    "{\"status\":\"fail\",\"message\":\"Missing argument!\"}");
    }
  });

  server.on("/api/v0/fan/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    char buf[200] = {0};

    snprintf(buf, 200,
             "{\"status\":\"ok\",\"data\":{\"rpm\":%d,\"pwm_percent\":%d}}",
             fan_get_rpm(), fan_get_pwm());

    request->send(200, "application/json", buf);
  });

  server.on("/api/v0/fan/voltage/high", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("confirm")) {
                fan_set_12V(true);

                request->send(200, "application/json",
                              "{\"status\":\"ok\",\"message\":\"Switching fan "
                              "output to 12V\"}");
              } else {
                request->send(400, "application/json",
                              "{\"status\":\"fail\",\"message\":\"Missing "
                              "argument `confirm`!\"}");
              }
            });

  server.on("/api/v0/fan/voltage/low", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("confirm")) {
                fan_set_12V(false);

                request->send(200, "application/json",
                              "{\"status\":\"ok\",\"message\":\"Switching fan "
                              "output to 5V\"}");
              } else {
                request->send(400, "application/json",
                              "{\"status\":\"fail\",\"message\":\"Missing "
                              "argument `confirm`!\"}");
              }
            });

  server.on("/api/v0/led/disable", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              led_set_blinking(false);

              request->send(
                  200, "application/json",
                  "{\"status\":\"ok\",\"message\":\"Activity LED disabled\"}");
            });

  server.on("/api/v0/led/enable", HTTP_GET, [](AsyncWebServerRequest *request) {
    led_set_blinking(true);

    request->send(200, "application/json",
                  "{\"status\":\"ok\",\"message\":\"Activity LED enabled\"}");
  });

  server.on(
      "/api/v0/openfan/name/set", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("name")) {
          String devName = request->getParam("name")->value();

          if (!eeprom_set_name(devName.c_str())) {
            request->send(
                400, "application/json",
                "{\"status\":\"fail\",\"message\":\"Invalid Devicename!\"}");

            return;
          }

          eeprom_save();

          request->send(200, "application/json",
                        "{\"status\":\"ok\",\"message\":\"Device renamed. "
                        "Restarting to apply...\"}");

          delay(1000);
          ESP.restart();
        } else {
          request->send(
              400, "application/json",
              "{\"status\":\"fail\",\"message\":\"Missing argument!\"}");
        }
      });

  server.on("/api/v0/openfan/status", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              char buf[200] = {0};

              snprintf(buf, 200,
                       "{\"status\":\"ok\",\"data\":{\"act_led_enabled\":\"%s\""
                       ",\"fan_is_12v\":\"%s\"}}",
                       (led_get_blinking() ? "true" : "false"),
                       (fan_get_12V() ? "true" : "false"));

              request->send(200, "application/json", buf);
            });

  server.on("/api/v0/wifi/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    char buf[200] = {0};

    snprintf(buf, 200,
             "{\"status\":\"ok\",\"data\":{\"SSID\":\"%s\",\"RSSI\":\"%4d\","
             "\"CH\":\"%2d\"}}",
             WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.channel());

    request->send(200, "application/json", buf);
  });

  server.on("/api/v0/wifi/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("confirm")) {
      request->send(200, "application/json",
                    "{\"status\":\"ok\",\"message\":\"WiFi configuration has "
                    "been reset. Please connect to OpenFAN Micro WiFi and "
                    "re-configure the device.\"}");

      wifi_reset();
    } else {
      request->send(400, "application/json",
                    "{\"status\":\"fail\",\"message\":\"Missing argument "
                    "`confirm`!\"}");
    }
  });

  server.begin();
}
#endif /*WIFI_ENABLE */