#include <Wire.h>
#include <BME280_MOD-1022.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>

#include "Config.h"
#include "Version.h"
#include "EspUpdater.h"
#include "StatsdReporter.h"

static StatsdReporter statsdReporter(STATSD_HOST, STATSD_PORT);
static EspUpdater updater(UPDATE_URL, VERSION);

void setup() {
	Serial.begin(115200);
	pinMode(D5, OUTPUT);
	Wire.begin(SCL, SDA);
	BME280.readCompensationParams();
	Serial.println("Starting up!");
}

bool connectWiFi() {
	Serial.println("Connecting to WiFi " WIFI_SSID);
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	
	WiFi.waitForConnectResult();

	if (WiFi.status() == WL_CONNECTED) {
		return true;
	}

	return false;
}

bool disconnectWiFi() {
	Serial.println("Disconnecting WiFi");
	WiFi.disconnect(true);
}

void reportValues() {
	BME280.readCompensationParams();

	BME280.writeOversamplingPressure(os16x);
	BME280.writeOversamplingTemperature(os16x);
	BME280.writeOversamplingHumidity(os16x);


	BME280.writeMode(smForced);
	Serial.println("Reporting values...");
	while (BME280.isMeasuring()) {
		delay(100);
	}
	BME280.readMeasurements();

	statsdReporter.gauge(STATSD_METRIC ".temperature", BME280.getTemperature());
	statsdReporter.gauge(STATSD_METRIC ".humidity", BME280.getHumidity());
	statsdReporter.gauge(STATSD_METRIC ".pressure", BME280.getPressureMoreAccurate());

	Serial.println("Done!");
}

void loop() {
	if (connectWiFi()) {
		updater.update();
		reportValues();
		disconnectWiFi();
	}
	Serial.printf("Going to sleep for %d seconds\n", SLEEP_INTERVAL);
	ESP.deepSleep(SLEEP_INTERVAL * 1000000L);
	delay(100);
}
