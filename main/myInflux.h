#define DEVICE "ESP8266"
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_TOKEN "o2QbBB78bFxpLIVjGFVRQ-crR8aPexOUA_NDiEATgDPssswUy1HyMiGOw_1njtgrEoJER8lCF0hhAzq5YMJM3A=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "ondra.slama1@gmail.com"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "AirQuality"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"


// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

void influxInit(){	
	timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
}

void uploadWifiStatus(String ssid, int rssi){
	Point pointDevice("device_status");
	pointDevice.addTag("device", "ESP8266");
	pointDevice.addTag("SSID", ssid);
	pointDevice.addField("rssi", rssi);
	pointDevice.addField("uptime", millis());		
	influxClient.writePoint(pointDevice);
}

void uploadAirQuality(float temperature, float humidity, int co2ppm){
	Point pointAirQuality("air_quality");
	pointAirQuality.addField("temperature", temperature);
	pointAirQuality.addField("humidity", humidity);
	if (co2ppm > 0){
		pointAirQuality.addField("co2", co2ppm);
	}
	influxClient.writePoint(pointAirQuality);
}
