#ifdef WIFI_ENABLE
#include "wifi.h"

#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "eeprom.h"

#define AP_FALLBACK "OpenFAN-AP"
#define RETRIES 4
#define PERIOD 30000

static char mac[] = "000000000000";
static WiFiManager wifiManager{};

const char *wifi_get_mac() { return mac; }

void wifi_loop(void) {
  const uint32_t now = millis();
  static uint32_t status_time = now;

  if ((now - status_time) >= PERIOD) {
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      WiFi.reconnect();
    }

    status_time = now;
  }
}

void wifi_reset() { wifiManager.resetSettings(); }

void wifi_setup(void) {
  const char *name = eeprom_get_name();

  sprintf(mac, "%12llX", ESP.getEfuseMac());

  WiFi.setHostname(name);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  delay(1000);

  wifiManager.setConnectRetries(RETRIES);
  wifiManager.setHostname(name);

  if (!wifiManager.autoConnect(AP_FALLBACK)) {
    ESP.restart();
  }

  if (MDNS.begin(name)) {
    MDNS.addService("http", "tcp", 80);
  }
}
#endif /* WIFI_ENABLE */