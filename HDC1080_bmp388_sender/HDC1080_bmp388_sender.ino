#include <Wire.h>

//HDC1080
#include "ClosedCube_HDC1080_wire1.h"
ClosedCube_HDC1080_wire1 hdc1080;

//BMP388
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
Adafruit_BMP3XX bmp; // I2C

//Lora
#include "heltec.h"
#include "images.h"
#define BAND    915E6 //433E6, 868E6, 915E6 → Depende da frênquencia da placa LoRa

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 5, logo_width, logo_height, logo_bits);
  Heltec.display->display();
}

void setup()
{
  //Inicialização placa LoRa ↓
  Heltec.begin(true, true, true, true, BAND);

  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
 
  logo();
  delay(1500);
  Heltec.display->clear();

  Heltec.display->drawString(0, 0, "Heltec.LoRa iniciada com sucesso!");
  Heltec.display->display();

  //Inicialização da segunda interface I2C ↓
  Wire1.begin(21, 22);

  //Inicialização do sensor HDC1080 ↓
  hdc1080.begin(0x40);
  
  //Inicialização do sensor BMP388 ↓
  if (!bmp.begin_I2C(0x77, &Wire1)) {
    Serial.println("Não foi possível encontrar um sensor BMP3. Verifique as ligações dos fios!");
    while (1);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  delay(1000);
}

double bmp_pressure_hpa = 0;
double bmp_temperature = 0;
double bmp_altitude = 0;
double hdc1080_temperature = 0;
double hdc1080_humidity = 0;
int package_numb = 1;
void loop()
{
  hdc1080_temperature = hdc1080.readTemperature();
  hdc1080_humidity = hdc1080.readHumidity();

  bmp_temperature = bmp.temperature;
  bmp_altitude = bmp.readAltitude(bmp_pressure_hpa);

  if (!bmp.performReading()) {
    Serial.println("Falha ao ler os dados do sensor BMP388");
    return;
  }

  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);

  Heltec.display->drawString(0, 0, "HDC Temp= " + String(hdc1080_temperature));
  Heltec.display->drawString(0, 10, "HDC RH= " + String(hdc1080_humidity));
  Heltec.display->drawString(0, 20, "BMP Press= " + String(bmp_pressure_hpa));
  Heltec.display->drawString(0, 30, "BMP Temp= " + String(bmp_temperature));
  Heltec.display->drawString(0, 40, "BMP Alt= " + String(bmp_altitude));
  Heltec.display->drawString(0, 50, "Med. Temp= " + String((hdc1080_temperature + bmp_temperature) / 2));
  Heltec.display->display();

  LoRa.beginPacket();
  LoRa.setTxPower(20, RF_PACONFIG_PASELECT_PABOOST);

  LoRa.print("Pack. Numb=");
  LoRa.print(String(package_numb));
  LoRa.print("&");
  LoRa.print("HDC Temp=");
  LoRa.print(String(hdc1080_temperature));
  LoRa.print("&");
  LoRa.print("HDC RH=");
  LoRa.print(String(hdc1080_humidity));
  LoRa.print("&");
  LoRa.print("BMP Press=");
  LoRa.print(String(bmp_pressure_hpa));
  LoRa.print("&");
  LoRa.print("BMP Temp=");
  LoRa.print(String(bmp_temperature));
  LoRa.print("&");
  LoRa.print("BMP Alt=");
  LoRa.print(String(bmp_altitude));
  
  LoRa.endPacket();

  package_numb += 1;
  bmp_pressure_hpa = bmp.pressure / 100.0;
  
  digitalWrite(LED, HIGH);
  delay(1500);
  digitalWrite(LED, LOW);
  delay(1500);
}
