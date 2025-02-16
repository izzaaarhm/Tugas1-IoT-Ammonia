#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>

const char* ssid = "wonuu";
const char* password = "wonwoo!96";
const char* scriptUrl = "https://script.google.com/macros/s/AKfycbwmIpCyY--ocMlr-KQ0XadTbvGLC3GRsUvr6wlgY4kdnqvSRszbYzb2zrxMuXFHP9GS/exec";

const int mq2Pin = 34;
const float RL = 10.0;  // Load resistor value (kΩ)
const float R0 = 9.83;  // R0 (calibrated in clean air)
const float m = -0.58;  // Slope (MQ-2 datasheet)
const float b = 1.58;   // Intercept (MQ-2 datasheet)

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32...");
  delay(3000);

  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi...");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(3000);
  }
  Serial.println("\nConnected to the WiFi!");
}

void logDataToSheets(float gasLevel) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    String url = String(scriptUrl) + "?gasLevel=" + String(gasLevel);
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("Data sent to Google Sheets: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Error sending data to Google Sheets");   
    }
    
    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}

void loop() {
  int gasValue = analogRead(mq2Pin);

  // Convert ADC value to voltage
  float sensorVoltage = (gasValue / 4095.0) * 3.3;  
    
  // Calculate sensor resistance (RS)
  float RS = ((3.3 - sensorVoltage) / sensorVoltage) * RL;

  // Compute RS/R0 ratio
  float ratio = RS / R0;

  // Calculate NH₃ PPM using datasheet equation
  float ammoniaPPM = pow(10, ((log10(ratio) - b) / m));

  Serial.print(" NH3 Gas Level on ESP-2: ");
  Serial.println(ammoniaPPM, 2);
  Serial.println(gasValue);

  logDataToSheets(ammoniaPPM);

  delay(10000);
}