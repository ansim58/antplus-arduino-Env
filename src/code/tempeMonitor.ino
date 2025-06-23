#include <Arduino.h>
#include <ANTPLUS.h>
#include "Profiles/Environment/Profiles/ANTPLUS_ProfileEnvironmentMonitor.h"
#include <WiFi.h>

#define NETWORK_KEY {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}
#define DEVICE_NUMBER 12345
#define TRANSMISSION_TYPE 0

// Pin-Konfiguration
#define DS18B20_PIN 12

// ANT+ Objekte
AntPlusRouter router;
ProfileEnvironmentMonitor environmentMonitor(DEVICE_NUMBER, TRANSMISSION_TYPE);

void setup() {
    Serial.begin(115200);
    
    // WLAN und Bluetooth deaktivieren um Strom zu sparen
    #ifdef ESP32
        WiFi.mode(WIFI_OFF);
        btStop();
    #endif

    // GPIO-Pins konfigurieren um Strom zu sparen
    configureUnusedPins();

    // ANT+ Initialisierung
    uint8_t networkKey[] = NETWORK_KEY;
    router.setNetworkKey(networkKey);
    environmentMonitor.begin();
    
    // Handler registrieren
    environmentMonitor.createEnvironmentMonitorDefaultMsg(defaultMsgHandler);
    environmentMonitor.createEnvironmentMonitorManufacturerInformationMsg(manufacturerInfoHandler);
}

void loop() {
    router.loop();
    environmentMonitor.process();
    delay(1000); // Zykluszeit anpassen
}

// Handler-Funktionen
void defaultMsgHandler(EnvironmentMonitorDefaultMsg &msg, uintptr_t data) {
    // Dummy-Daten für Data Page 1
    static float temp = 20.0f;
    
    msg.setCurrentTemperature(temp);
    msg.setMaxTemperature(28.0f);
    msg.setMinTemperature(18.0f);
    
    temp += 0.5f;
    if(temp > 28.0f) temp = 20.0f;
}

void manufacturerInfoHandler(EnvironmentMonitorManufacturerInformationMsg &msg, uintptr_t data) {
    msg.setManufacturerIdLsb(0x01);
    msg.setSerialNumber(0x1234);
}

void configureUnusedPins() {
    const uint8_t outputSafePins[] = {4,5,13,14,16,17,18,19,21,22,23,25,26,27,32,33};
    const uint8_t inputOnlyPins[] = {34,35,36,39};
    const uint8_t bootPins[] = {0,2,12,15};

    // OUTPUT LOW für sichere GPIOs
    for(auto pin : outputSafePins) {
        if(pin == DS18B20_PIN) continue;
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // INPUT_PULLUP für Boot-Pins
    for(auto pin : bootPins) {
        if(pin == DS18B20_PIN) continue;
        pinMode(pin, INPUT_PULLUP);
    }

    // Input-only Pins
    for(auto pin : inputOnlyPins) {
        pinMode(pin, INPUT);
    }

    // DS18B20-Pin konfigurieren
    pinMode(DS18B20_PIN, INPUT);
}
