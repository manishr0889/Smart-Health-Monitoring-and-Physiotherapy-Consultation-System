/*<<<<

#define BLYNK_TEMPLATE_ID "*******"
#define BLYNK_TEMPLATE_NAME "*****"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <DHT.h>
#include <MAX30100_PulseOximeter.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN D4       
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

PulseOximeter pox;

// DS18B20 Body Temp Sensor Setup
#define ONE_WIRE_BUS D3  // DS18B20 data pin connected to GPIO0 (D3)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = "YS2HRS8vGbcFHcFdRn_6LgZ5z7buBBS7";  // Replace with Blynk auth token
char ssid[] = "mk202";         // Replace with your WiFi SSID
char pass[] = "00998877";     // Replace with your WiFi password
*/ //<<< remove this after update blynk id,name and wife ssid password

bool max30100Ready = false;

void onBeatDetected() {
    Serial.println("❤️ Heart Beat Detected!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    Blynk.begin(auth, ssid, pass);
    dht.begin();
    sensors.begin();  // Initialize DS18B20

    Serial.println("Initializing MAX30100...");
    if (!pox.begin()) {
        Serial.println("❌ ERROR: MAX30100 NOT FOUND!");
    } else {
        Serial.println("✅ MAX30100 Initialized!");
        max30100Ready = true;
        pox.setOnBeatDetectedCallback(onBeatDetected);
        pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    }
}

void sendSensorData() {
    if (max30100Ready) {
        pox.update();
        float spo2 = pox.getSpO2();
        float bpm = pox.getHeartRate();

        if (spo2 > 0 && spo2 <= 100) {
            Blynk.virtualWrite(V1, spo2);
        } else {
            Serial.println("⚠️ Invalid SpO2 reading!");
        }

        if (bpm > 30 && bpm < 200) {
            Blynk.virtualWrite(V2, bpm);
        } else {
            Serial.println("⚠️ Invalid BPM reading!");
        }

        Serial.print("SpO2: "); Serial.print(spo2); Serial.print("%, ");
        Serial.print("BPM: "); Serial.print(bpm); Serial.println();
    } else {
        Serial.println("❌ MAX30100 NOT Initialized! Skipping SpO2 & BPM...");
    }

    float roomTemp = dht.readTemperature();

    if (!isnan(roomTemp)) {
        Blynk.virtualWrite(V5, roomTemp);  // Room Temperature now on V5
        Serial.print("Room Temp: "); Serial.print(roomTemp); Serial.println("°C");
    } else {
        Serial.println("⚠️ Failed to read Room Temp from DHT11!");
    }

    sensors.requestTemperatures();
    float bodyTemp = sensors.getTempCByIndex(0);

    if (bodyTemp != DEVICE_DISCONNECTED_C) {
        Blynk.virtualWrite(V4, bodyTemp);  // Body Temperature on V4
        Serial.print("Body Temp: "); Serial.print(bodyTemp); Serial.println("°C");
    } else {
        Serial.println("⚠️ Failed to read Body Temp from DS18B20!");
    }
}

void loop() {
    Blynk.run();
    sendSensorData();
    delay(2000);
}
