#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

const char *ssid = "wemostestserver";
const char *password = "beerforpassword";
WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incomingPacket[255];
String  replyPacket = "Undecided";
ESP8266WebServer server(80);

const int numReadings = 25;
int MHreadings[numReadings];
int MHreadIndex = 0;
int MHtotal = 0;
int MHaverage = 0;

int MKreadings[numReadings];
int MKreadIndex = 0;
int MKtotal = 0;
int MKaverage = 0;

void setup() {
  delay(1000);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    MHreadings[thisReading] = 10;
    MKreadings[thisReading] = 10;
  }
  MHtotal = (numReadings*10);
  MKtotal = (numReadings*10);

  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("HTTP server started");
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    switch (incomingPacket[0]- '0') {
      case 0:
        MHtotal = MHtotal - MHreadings[MHreadIndex];
        MHreadings[MHreadIndex] = (String(incomingPacket[1]) + String(incomingPacket[2])).toInt();
        MHtotal = MHtotal + MHreadings[MHreadIndex];
        MHreadIndex = MHreadIndex + 1;
        if (MHreadIndex >= numReadings) {
          MHreadIndex = 0;
        }
        MHaverage = MHtotal / numReadings;
        Serial.printf("Vote for HENDERSON - ");
        Serial.println(String(incomingPacket[1]) + String(incomingPacket[2]));
        Serial.printf("H - ");
        Serial.print(MHtotal);
        Serial.printf(" vs K - ");
        Serial.print(MKtotal);
        Serial.println();
      break;
      case 1:
        MKtotal = MKtotal - MKreadings[MKreadIndex];
        MKreadings[MKreadIndex] = (String(incomingPacket[1]) + String(incomingPacket[2])).toInt();
        MKtotal = MKtotal + MKreadings[MKreadIndex];
        MKreadIndex = MKreadIndex + 1;
        if (MKreadIndex >= numReadings) {
          MKreadIndex = 0;
        }
        MKaverage = MKtotal / numReadings;
        Serial.printf("Vote for KREINHEDER - ");
        Serial.println(String(incomingPacket[1]) + String(incomingPacket[2]));
        Serial.printf("H - ");
        Serial.print(MHtotal);
        Serial.printf(" vs K - ");
        Serial.print(MKtotal);
        Serial.println();
      break;
      default:
        Serial.println("No Vote");
      break;
    }
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    if(MKtotal > MHtotal){
      Serial.println("You are evil");
      Serial.println("-------------");
      replyPacket = "1";
    } else{
      Serial.println("I am evil");
      Serial.println("-------------");
      replyPacket = "0";
    }
    Udp.print(replyPacket);
    Udp.endPacket();
  }
  delay(1);
}
