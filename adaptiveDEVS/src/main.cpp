#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "credentials.h"
// set hosts and ports
const char* server = "http://" + HOST_IP + ":" + 5000 + "/nodemcu";

// credentials
String wifi_ssid = WIFI_SSID;
String wifi_password = WIFI_PASSWORD;

const int moisture_pin = A0;

unsigned long lastTime = 0;
unsigned long delayTime = 5000;  // 5s

WiFiClient client;
HTTPClient http;

// function declarations
float getMoistureLevel();

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");

    // pin Modes
    pinMode(moisture_pin, INPUT);

    // connect to wifi
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\nConnected to\t");
    Serial.println(WiFi.localIP());

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
}

void loop() {
    // get moisture level
    float moisture = getMoistureLevel();
    // Serial.println(moisture);

    // rice moisture data
    int rice_min = 70;
    int rice_max = 90;

    // wheat moisture data
    int wheat_min = 50;
    int wheat_max = 70;

    // tomato moisture data
    int tomato_min = 30;
    int tomato_max = 40;

    // an example with rice

    if (moisture <= rice_min) {
        Serial.println("Put water in the field.");
    } else if (moisture >= rice_max) {
        Serial.println("Water is too much.");
    } else {
        Serial.println("Water is ok.");
    }

    // send data to server
    if ((millis() - lastTime) > delayTime) {
        if (WiFi.status() == WL_CONNECTED) {
            WiFiClient client;
            HTTPClient http;

            http.begin(client, server);

            http.addHeader("Content-Type", "application/json");

            String payload = "{\"moisture\":" + String(moisture) + "}";
            Serial.println(payload);

            int resposeCode = http.POST(payload);

            Serial.print("Response code:\t");
            Serial.println(resposeCode);

            http.end();
        } else {
            Serial.println("WiFi not connected");

            ESP.reset();
        }

        lastTime = millis();
    }

    // Todo: remove the delay
    // delay(2500);
}

float getMoistureLevel() {
    float moisture_level = analogRead(moisture_pin);
    moisture_level = 1023 - moisture_level;

    moisture_level = map(moisture_level, 0, 1023, 0, 100);
    if (moisture_level > 100) {
        moisture_level = 100;
    } else if (moisture_level < 0) {
        moisture_level = 0;
    }

    return moisture_level;
}