/*
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h"
#include "images.h"

#define BAND    915E6 //433E6, 868E6, 915E6 → Depende da frênquencia da placa LoRa
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 5, logo_width, logo_height, logo_bits);
  Heltec.display->display();
}

void LoRaData() {

  Serial.println(packet);
  
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, rssi);
  Heltec.display->drawString(0 , 15 , "Received " + packSize + " bytes");
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
  Heltec.display->display();
}

void cbk(int packetSize) {
  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read();
  }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

void setup() {
  Heltec.begin(true, true, true, true, BAND);

  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();

  Heltec.display->drawString(0, 0, "Heltec.LoRa iniciada com sucesso!");
  Heltec.display->drawString(0, 10, "Esperando por dados...");
  Heltec.display->display();
  delay(1000);
  LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    cbk(packetSize);

    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  delay(10);
}
