#include "DHTesp.h"
#include "myInflux.h"
#include "myLcd.h"
#include "myCO2Sensor.h"
#include "myWifi.h"


DHTesp dht;

void setup()
{
	Serial.begin(9600);
	Serial.swap();
	lcdInit();
	co2Init();

	// DHT
	dht.setup(D4, DHTesp::DHT11); // GPIO14


	// Influx
	influxInit();
	
	// Wifi
	registerAP("Dno Pytle", "ReadyForAnAdventure21");
	registerAP("Dno Pytle 2", "ReadyForAnAdventure21");

	lcdPrint("Connecting...");
	if (connectToWifi()) {
		lcdPrint("Wifi ok", 0, 0, true);
	} else {
		lcdPrint("Wifi failed", 0, 0, true);
	}
	
	// Check server connection
	if (influxClient.validateConnection()) {
		lcdPrint("Influx ok", 0, 0, true);
	} else {
		lcdPrint("InfluxDB failed", 0, 0, true);
	}

}

void loop()
{
	// Get sensors data
	float humidity = dht.getHumidity();
	float temperature = dht.getTemperature();
	int co2ppm = co2GetReading();	

	lcdClear();
	lcdPrint("H " + String(humidity, 1) + "% T " + String(temperature, 1) + "C", 0, 0);

	if (co2ppm == -1) {
		lcdPrint("CO2 fail", 1, 0);
	} else {
		lcdPrint("CO2 " + String(co2ppm) + " ppm", 1, 0);
	}	

	if (!co2IsReady() || co2ppm==500 || co2ppm > 5000) {	
		lcdPrint("W", 1, 13);
		co2ppm = 0;	
	}

	// Maintain WiFi connection	
	lcdPrint("  ", 1, 14);
	if (isWifiConnected()) {
		lcdPrint("Y", 1, 14);
		if (influxClient.validateConnection()){
			lcdPrint("Y", 1, 15);
			uploadWifiStatus(WiFi.SSID(), WiFi.RSSI());
			uploadAirQuality(temperature, humidity, co2ppm);
		}else{
			lcdPrint("N", 1, 15);
		}	
	}else{
		lcdPrint("N", 1, 14);
	}	
}

