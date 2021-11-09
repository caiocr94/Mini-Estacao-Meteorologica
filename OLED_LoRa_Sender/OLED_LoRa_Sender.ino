/*
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h"
#include "images.h"

#define BAND    915E6 //433E6, 868E6, 915E6 → Depende da frênquencia da placa LoRa
unsigned int counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 5, logo_width, logo_height, logo_bits);
  Heltec.display->display();
}

void setup() {
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(true, true, true, true, BAND);

  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();

  Heltec.display->drawString(0, 0, "Heltec.LoRa iniciada com sucesso!");
  Heltec.display->display();
  delay(1000);
}

void loop()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);

  Heltec.display->drawString(0, 0, "Enviando pacote: ");
  Heltec.display->drawString(90, 0, String(counter));
  Heltec.display->display();

  // send packet
  LoRa.beginPacket();
  LoRa.setTxPower(20, RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print("Olá ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}
