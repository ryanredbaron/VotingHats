#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char *ssid = "wemostestserver";
const char *password = "beerforpassword";
const char * host = "192.168.4.1";
const int httpPort = 80;
unsigned int localUdpPort = 4210;
char incomingPacket[255];
char replyPacket[] = "1";

int SwitchTest = 1;

WiFiClient client;
WiFiUDP Udp;

void setup() {
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connecting...");
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  Udp.begin(localUdpPort);
  Serial.println("Connected...");
}

void loop() {
  int i = 0;
  char * str = "";
  char buf[12];
  char replyPacket[] = "1";
  strcat(replyPacket, itoa(random(10, 99), buf, 10));
  Udp.beginPacket(host, 4210);
  Udp.write(replyPacket);
  Udp.endPacket();
  Serial.print("Sending vote - ");
  Serial.println(replyPacket);
  Serial.println("Waiting for response...");
  int packetSize = Udp.parsePacket();
  delay(1000);
  if (packetSize) {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }
    delay(1000);
    Serial.printf("I received - ");
    Serial.print(incomingPacket[0]);
    Serial.println();
    switch (incomingPacket[0] - '0') {
      case 0:
        Serial.println("Server is Evil");
        break;
      case 1:
        Serial.println("Oh shit I'm Evil!");
        break;
      default:
        Serial.println("IDK what happened");
        break;
    }
  } else {
    Serial.println("*****No response. Dang.*****");
  }
  Serial.println("----------");
  delay(5000);
}
