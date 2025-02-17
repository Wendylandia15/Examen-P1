#include "ThingSpeak.h"
#include "WiFi.h"
#include "DHT.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>

#define pin2 32 // Pin del DHT22.
#define BMP_SDA 21 // Pin SDA del BMP280
#define BMP_SCL 22 // Pin SCL del BMP280
#define BUZZER 19 // Pin del buzzer (ajusta según tu conexión)

const char* ssid = "UAM-ROBOTICA"; //SSID de vuestro router.
const char* password = "m4nt32024uat"; //Contraseña de vuestro router.

unsigned long channelID = 2844405; //ID de vuestro canal.
const char* WriteAPIKey = "TWHFO1O77CH8QUCW"; //Write API Key de vuestro canal.

WiFiClient cliente;

DHT dht2(pin2, DHT22); //El blanco.
Adafruit_BMP280 bmp; // Objeto para el sensor BMP280

// Variables para almacenar las lecturas de los sensores
float t2, h2, presion;


void setup() {
Serial.begin(115200);
Serial.println("Test de sensores:");

WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("Wifi conectado!");

ThingSpeak.begin(cliente);

dht2.begin();

// Inicializar el sensor BMP280
Wire.begin(BMP_SDA, BMP_SCL); // Inicializar la comunicación I2C con pines específicos
if (!bmp.begin(0x76)) { // Dirección I2C del BMP280 (0x76 o 0x77)
Serial.println("No se pudo encontrar el sensor BMP280");
while (1);
}

pinMode(BUZZER, OUTPUT); // Configurar el pin del buzzer como salida
}

void loop() {
delay(2000);
leerdht2();

// Activar el buzzer si la temperatura es mayor o igual a 27°C o la humedad es mayor o igual a 75%
if (t2 >= 27.0 || h2 >= 75.0) {
digitalWrite(BUZZER, HIGH); // Encender el buzzer
Serial.print("¡Alerta! ");
if (t2 >= 27.0) Serial.print("Temperatura alta. ");
if (h2 >= 75.0) Serial.print("Humedad alta.");
Serial.println();
} else {
digitalWrite(BUZZER, LOW); // Apagar el buzzer
}

ThingSpeak.setField(3, t2);
ThingSpeak.setField(4, h2);
ThingSpeak.setField(5, presion);

ThingSpeak.writeFields(channelID, WriteAPIKey);

Serial.println("Datos enviados a ThingSpeak!");
delay(14000);
}

void leerdht2() {
t2 = dht2.readTemperature();
h2 = dht2.readHumidity();
presion = bmp.readPressure() / 100.0F; // Leer la presión y convertirla a hPa

while (isnan(t2) || isnan(h2) || isnan(presion)) {
Serial.println("Lectura fallida en algún sensor, repitiendo lectura...");
delay(2000);
t2 = dht2.readTemperature();
h2 = dht2.readHumidity();
presion = bmp.readPressure() / 100.0F;
}

Serial.print("Temperatura DHT22: ");
Serial.print(t2);
Serial.println(" ºC.");

Serial.print("Humedad DHT22: ");
Serial.print(h2);
Serial.println(" %.");

Serial.print("Presión BMP280: ");
Serial.print(presion);
Serial.println(" hPa.");

Serial.println("-----------------------");
}