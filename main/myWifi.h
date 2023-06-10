#include <ESP8266WiFiMulti.h>
#include "myLcd.h"

ESP8266WiFiMulti wifiMulti;
const uint32_t connectMaxAttempts = 10;
int attempt = 0;

void registerAP(const char* ssid, const char* password) {
	wifiMulti.addAP(ssid, password);
}

bool connectToWifi() {
	attempt = 0;
	lcdPrint("Wifi.. ");
	WiFi.mode(WIFI_STA);
	while (wifiMulti.run() != WL_CONNECTED) {
		if (attempt > connectMaxAttempts) {
			return false;
		}
		lcdPrint(String(attempt) + "/" + String(connectMaxAttempts), 0, 9);
		delay(100);
		attempt++;
	}
	return true;
}

bool isWifiConnected() {
	return wifiMulti.run(attempt) == WL_CONNECTED;
}
