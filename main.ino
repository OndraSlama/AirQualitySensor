#include "DHTesp.h"
#include "myInflux.h"
#include "myLcd.h"
#include "myCO2Sensor.h"
#include "myWifi.h"
#include "myDateTime.h"	

#include <PubSubClient.h>


#define mqtt_server "192.168.68.120"
#define mqtt_user "mqtt-user"
#define mqtt_password "6^GAw7E7NCLuoWI"

#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"
#define co2_topic "sensor/co2"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHTesp dht;

bool wifiConnected = false;

#define TEMPARATURE_OFFSET -1.05


void setup()
{
	Serial.begin(9600);
	Serial.swap();
	lcdInit();
	co2Init();

	// DHT
	dht.setup(D4, DHTesp::DHT11); // GPIO14

	mqttClient.setServer(mqtt_server, 1883);

	// Influx
	influxInit();
	
	// Wifi
	registerAP("Dno Pytle", "ReadyForAnAdventure21");
	registerAP("Dno Pytle 2", "ReadyForAnAdventure21");
	registerAP("Frazden", "doma1doma");

	wifiConnected = reconnectWifi();
	
	// Check server connection
	if (influxClient.validateConnection()) {
		lcdPrint("Influx ok", 0, 0, true);
	} else {
		lcdPrint("InfluxDB failed", 0, 0, true);
	}

	setupDatetime();
}

void reconnectMqtt() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    lcdPrint("Connecting to MQTT...",  0, 0);
    if (mqttClient.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      lcdPrint("MQTT connected",  0, 0, true);
	  delay(1000);
    } else {
      lcdPrint("MQTT failed",  0, 0, true);
      lcdPrint("Reconnect 5s",  1, 0);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool reconnectWifi(){
	if (connectToWifi()) {
		lcdPrint("Wifi ok", 0, 0, true);
		return true;
	} else {
		lcdPrint("Wifi connection", 0, 0, true);
		lcdPrint("failed", 1);
		return false;
	}
	
}

void loop()
{
	//check wifi connection
	wifiConnected = isWifiConnected();
	if (!wifiConnected) {
		wifiConnected = reconnectWifi();
		if (!wifiConnected) {
			return;
		}
	}
	// Check MQTT connection
	if (!mqttClient.connected()) {
		lcdPrint("Reconnecting MQTT", 1, 0);
		reconnectMqtt();
	}
	mqttClient.loop();

	// Get sensors data
	float humidity = dht.getHumidity();
	float temperature = dht.getTemperature() + TEMPARATURE_OFFSET;
	int co2ppm = co2GetReading();	
	String sensorReadings = String(humidity, 0) + "% " + String(temperature, 1) + "C " + String(co2ppm) + "ppm";
	
	lcdClear();
	lcdPrint(getFormattedDate(), 0, 0);
	lcdPrint(getFormattedTime(true), 0, 11);
	lcdPrint(sensorReadings, 1, 0);



	if (co2ppm == -1) {
		lcdPrint("CO2 fail", 1, 0);
	}

	if (!co2IsReady() || co2ppm==500 || co2ppm > 5000) {	
		// lcdPrint("W", 1, 13);
		co2ppm = 0;	
	}

	// lcdPrint("  ", 1, 14);

	// Check WiFi connection	
	if (!isWifiConnected()) {
		// lcdPrint("N", 1, 14);
		return;
	}
	// lcdPrint("Y", 1, 14);

	// Publish to MQTT
	mqttClient.publish(temperature_topic, String(temperature).c_str(), true);
	mqttClient.publish(humidity_topic, String(humidity).c_str(), true);
	if (co2ppm > 0){
		mqttClient.publish(co2_topic, String(co2ppm).c_str(), true);
	}

	// Upload to influx db
	if (influxClient.validateConnection()){
		// lcdPrint("Y", 1, 15);
		uploadWifiStatus(WiFi.SSID(), WiFi.RSSI());
		uploadAirQuality(temperature, humidity, co2ppm);
	}else{
		// lcdPrint("N", 1, 15);
	}	
}

