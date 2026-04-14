#include <WiFi.h>      // Note the # at the start
#include <WiFiUdp.h>

#include <SPI.h>
#include <LoRa.h>
#include<DHT.h>

// --- Network Credentials ---
const char* ssid     = "MAGESH";     // Put your WiFi name here
const char* password = "MaGeSh@2112"; // Put your WiFi password here

IPAddress targetIP(10, 128, 140, 200);
unsigned int targetPort = 4210;

WiFiUDP udp;

char incomingPacket[255];

DHT dht (4, DHT11);

// ---------- LoRa Pins ----------
#define SS   5
#define RST  14
#define DIO0 26

// ---------- HX711 Pins ----------


// ---------- Ultrasonic Pins ----------
#define rain 39
#define soil 36

String a;

void setup() {
  Serial.begin(115200);

  // sensor setup
  pinMode(rain, OUTPUT);
  pinMode(soil, INPUT);
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());


  // LoRa setup
  Serial.println("Initializing LoRa...");
  SPI.begin();
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
  Serial.println("LoRa started successfully!");
}

void loop() {

  int S = analogRead(soil);
  int R = analogRead(rain);

  float tt = dht.readTemperature();
  float h = dht.readHumidity();

  int v1 = map(S, 0, 4095, 50, 80);
  int v2 = map(R, 0, 4095, 50, 80);
  float t = map(tt, 28, 60, 35, 50);

  int packetSize = udp.parsePacket();
  if (packetSize) {

    int len = udp.read(incomingPacket, 254);
    if (len > 0) incomingPacket[len] = 0;

    Serial.print("Raw Data: ");
    Serial.println(incomingPacket);

    a = String(incomingPacket);
  }

  Serial.print("Soil: ");
  Serial.print(S);
  Serial.print(" | Rain: ");
  Serial.print(R);
  Serial.print("T: ");
  Serial.print(t);
  Serial.print("H: ");
  Serial.print(h);
  Serial.print("A: ");
  Serial.println(a);

  // ---------- LoRa Data ----------
  String data = "S:" + String(v1) + "R:" + String(v2) + "T:" + String(t) + "H:" + String(h) + "A:" + String(a);

  Serial.print("Sending: ");
  Serial.println(data);

  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  String data1 = "v1:" + String(v1) + ",v2:" + String(v2) +
                 ",v3:" + String(t) + ",v4:" + String(h);

  // Send the UDP packet
  udp.beginPacket(targetIP, targetPort);
  udp.print(data1);
  udp.endPacket();

  // Print to Serial Monitor so you can see it working
  Serial.println("Sent: " + data1);

  delay(2000);
}