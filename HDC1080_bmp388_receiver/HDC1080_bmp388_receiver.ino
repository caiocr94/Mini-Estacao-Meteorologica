#include <WiFi.h>
#include <HTTPClient.h>
WiFiClient client;
// Configuração SSID e Senha da Wifi ↓
//const char* ssid = "NOME DA REDE";
//const char* password =  "SENHA DA REDE";
#include "wifiConf.h"

#include "heltec.h"
#include "images.h"

#define BAND    915E6 //433E6, 868E6, 915E6 → Depende da frênquencia da placa LoRa
String rssi = "RSSI --";
String packSize = "--";
String packet;
int packet_qtd_params;
char packet_char_helper;
int packet_line_to_write = 14;
String packet_getValue_helper;
String packetValue_getKey_helper;
String packetValue_getValue_helper;

// Faz o split da string passando um separador específico
//   → data: A string
//   → separator: O separador
//   → index: O index que deseja retornar
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 5, logo_width, logo_height, logo_bits);
  Heltec.display->display();
}

void LoRaData() {
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, rssi + " (Rec. " + packSize + "bytes)");
  
  String httpRequestData = String("version=1");
  
  //Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
  for (int i = 0; i < packet_qtd_params; i++) {
    if ((packet_line_to_write + 10) <= 64) {
      packet_getValue_helper = getValue(packet, '&', i);
      packetValue_getKey_helper = getValue(packet_getValue_helper, '=', 0);
      packetValue_getValue_helper = getValue(packet_getValue_helper, '=', 1);

      if(packetValue_getKey_helper == "HDC Temp"){
        httpRequestData += "&hdc1080_temperature=" + packetValue_getValue_helper;
      }else if(packetValue_getKey_helper == "HDC RH"){
        httpRequestData += "&hdc1080_humidity=" + packetValue_getValue_helper;
      }else if(packetValue_getKey_helper == "BMP Press"){
        httpRequestData += "&bmp_pressure=" + packetValue_getValue_helper;
      }else if(packetValue_getKey_helper == "BMP Temp"){
        httpRequestData += "&bmp_temperature=" + packetValue_getValue_helper;
      }else if(packetValue_getKey_helper == "BMP Alt"){
        httpRequestData += "&bmp_altitude=" + packetValue_getValue_helper;
      }
      
      Heltec.display->drawStringMaxWidth(0 , packet_line_to_write , 128, packet_getValue_helper);
      packet_line_to_write += 10;
    }
  }
  Heltec.display->display();

  
  //Enviando informações para o banco de dados
  HTTPClient http;
  http.begin(client, "ENDPOINT_REDE"); //Link para o local onde está o arquivo "index.php", dentro da pasta "Site"
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  packet_qtd_params = 0;
  packet_line_to_write = 14;
}

void cbk(int packetSize) {
  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i < packetSize; i++) {
    packet_char_helper = (char) LoRa.read();
    packet += packet_char_helper;

    if (packet_char_helper == '&') {
      packet_qtd_params++;
    }

  }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

void setup() {
  //Inicialização placa LoRa ↓
  Heltec.begin(true, true, true, true, BAND);

  //Inicialização Wifi ↓
  WiFi.begin(ssid, password);
  int qtd_connect_wifi = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(String("Conectando a WiFi "+ String(ssid) +" (") + qtd_connect_wifi + ") ...");
    qtd_connect_wifi++;

    if (qtd_connect_wifi >= 10) {
      Serial.println(String("Reinicinado ESP ..."));
      ESP.restart();
    }
  }
  Serial.println("WiFi Conectada");

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
    packet_qtd_params++;
    cbk(packetSize);

    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  delay(10);
}
