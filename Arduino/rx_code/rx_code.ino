#define BLYNK_TEMPLATE_ID "TMPL3g_CjuVId"
#define BLYNK_TEMPLATE_NAME "Cloud burst"
#define BLYNK_AUTH_TOKEN "GiMbiNmkNqHQz5-XLmOKEvGxZyO8KhU1"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include<LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 27, 25, 33, 32);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "MAGESH";
char pass[] = "MaGeSh@2112";

#include <SPI.h>
#include <LoRa.h>

// ================= LORA PINS =================
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 26

// ================= MOTOR PINS =================

#define BUZZER 15   // safer pin

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  lcd.begin(16, 2);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Failed");
    while (1);
  }

  Serial.println("LoRa Initialized Successfully");
}

void loop() {
  Blynk.run();

  // -------- LORA RECEIVE --------
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.println("Received: " + received);
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());

    // Expected format: D=25,W=3.45
    int S = extractValue(received, "S:").toInt();
    int R = extractValue(received, "R:").toInt();
    float t = extractValue(received, "T:").toFloat();
    float h = extractValue(received, "H:").toFloat();
    String a = extractValue(received, "A:");

    Blynk.virtualWrite(V0, S);
    Blynk.virtualWrite(V1, R);
    Blynk.virtualWrite(V2, t);
    Blynk.virtualWrite(V3, h);

    Serial.print("S: ");
    Serial.print(S);
    Serial.print(" | R: ");
    Serial.print(R);
    Serial.print(" | T: ");
    Serial.print(t);
    Serial.print(" | H: ");
    Serial.println(h);

    lcd.setCursor(0, 0);
    lcd.print("Soil:");
    lcd.setCursor(5, 0);
    lcd.print(S);
    lcd.setCursor(9, 0);
    lcd.print("T:");
    lcd.setCursor(11, 0);
    lcd.print(t);
    lcd.setCursor(0, 1);
    lcd.print("Rain:");
    lcd.setCursor(5, 1);
    lcd.print(R);
    lcd.setCursor(9, 1);
    lcd.print("H:");
    lcd.setCursor(11, 1);
    lcd.print(h);

    if (a == "D") {
      lcd.clear();
      digitalWrite(BUZZER, 1);
      delay(1000);
      digitalWrite(BUZZER, LOW);
      lcd.setCursor(0, 0);
      lcd.print("detect Danger");
      Blynk.virtualWrite(V4, "Danger...");
      Serial.println("AT");
      delay(100);
      Serial.println("AT+CMGF=1");
      delay(100);
      Serial.println("AT+CMGS=\"+911234567890\"\r");
      delay(100);
      Serial.println("Danger...");
      delay(100);
      Serial.println((char)26);
      delay(1000);
      Serial.println("AT");
      delay(100);
      Serial.println("AT+CMGF=1");
      delay(100);
      Serial.println("AT+CMGS=\"+910987654321\"\r");
      delay(100);
      Serial.println("Danger...");
      delay(100);
      Serial.println((char)26);
      delay(2000);
    }
    else if (a == "S") {
      Blynk.virtualWrite(V4, "Safe....");
      delay(1000);
    }
    else {
      Blynk.virtualWrite(V4, "Processing..");
    }

    delay(2000);
    lcd.clear();
  }
}

// ================= HELPERS =================
String extractValue(String data, String key) {
  int start = data.indexOf(key);
  if (start == -1) return "";

  start += key.length();
  int end = data.indexOf(",", start);
  if (end == -1) end = data.length();

  return data.substring(start, end);
}