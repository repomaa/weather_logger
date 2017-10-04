BOARD := nodemcu
INCLUDE_VARIANT := nodemcu
BUILD_DIR := ./build
VERSION := $(shell sed 's/.*"\(.*\)".*/\1/' Version.h)

LIBS = $(ESP_LIBS)/ESP8266WiFi \
	   $(ESP_LIBS)/ESP8266httpUpdate \
	   $(ESP_LIBS)/ESP8266HTTPClient \
	   $(ESP_LIBS)/ESP8266HTTPClient \
	   $(ESP_LIBS)/Wire

include $(HOME)/makeEspArduino/makeEspArduino.mk

release: $(MAIN_EXE)
	ssh esp_updater@jokke.space mkdir -p /var/lib/esp_updater/weather
	rsync $(MAIN_EXE) esp_updater@jokke.space:/var/lib/esp_updater/weather/$(VERSION).bin

.PHONY: all release dirs
