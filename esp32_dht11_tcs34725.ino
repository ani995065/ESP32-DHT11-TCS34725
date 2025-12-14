#include <Wire.h>
#include "DHT.h"
#include "Adafruit_TCS34725.h"

// ==================== DHT11 SENSOR ====================
#define DHTPIN 4       // GPIO for DHT11 data pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==================== TCS34725 SENSOR ====================
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_600MS, 
  TCS34725_GAIN_1X
);

// ==================== BUILT-IN LED ====================
#define LEDPIN 2  // Most ESP32 Dev Boards built-in LED
bool ledState = false;
unsigned long previousMillis = 0;
const long interval = 1000; // 1-second blink

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000); // allow Serial Monitor to open
  Serial.println("Setup starting...");

  pinMode(LEDPIN, OUTPUT); // initialize LED

  Wire.begin();

  // ----------------- Initialize DHT11 -----------------
  dht.begin();
  delay(100);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Warning: DHT11 sensor not detected or wiring issue!");
  } else {
    Serial.println("DHT11 detected successfully!");
  }

  // ----------------- Initialize TCS34725 -----------------
  if (!tcs.begin()) {
    Serial.println("Warning: TCS34725 sensor not detected or wiring issue!");
  } else {
    Serial.println("TCS34725 detected successfully!");
  }

  Serial.println("Setup complete!\n");
}

// ==================== LOOP ====================
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LEDPIN, ledState); // toggle LED
  }

  // ----------- DHT11 Readings -----------
  float humidity = dht.readHumidity();
  float tempC    = dht.readTemperature();

  // ----------- TCS34725 Readings -----------
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  uint16_t colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  uint16_t lux       = tcs.calculateLux(r, g, b);

  // ----------- Output Results -----------
  Serial.println("======================================");
  Serial.print("DHT11 Temp (C): ");
  if (isnan(tempC)) Serial.print("Error!"); else Serial.print(tempC,1);
  Serial.print("\tHumidity (%): ");
  if (isnan(humidity)) Serial.println("Error!"); else Serial.println(humidity,1);

  Serial.print("TCS34725 R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.print(b);
  Serial.print(" C: "); Serial.println(c);

  Serial.print("Color Temp (K): "); Serial.println(colorTemp);
  Serial.print("Lux: "); Serial.println(lux);
  Serial.println("======================================\n");

  delay(2000); // 2-second update for sensors
}