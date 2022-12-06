#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 5000;
int connectingForMs = 0;

void registerAP(const char* ssid, const char* password) {
	wifiMulti.addAP(ssid, password);
}

bool connectToWifi() {
	WiFi.mode(WIFI_STA);
	while (wifiMulti.run() != WL_CONNECTED) {
		Serial.print(".");
		delay(100);
		connectingForMs += 100;
		if (connectingForMs > connectTimeoutMs) {
			return false;
		}
	}
	return true;
}

bool isWifiConnected() {
	return wifiMulti.run(connectTimeoutMs) == WL_CONNECTED;
}
