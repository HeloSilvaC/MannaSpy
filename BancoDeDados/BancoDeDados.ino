#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>

const char* ssid = "MERCUSYS_0DAC";
const char* password = "contrera";
const char* server = "api.thingspeak.com";
const String apiKey = "KRRK9BT06KYJMAPK";

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Nome do dispositivo Bluetooth

  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Conectando à rede WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado à rede WiFi. Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (SerialBT.available()) {
    String receivedText = SerialBT.readString(); // Lê o texto recebido pelo Bluetooth
    Serial.println("Texto recebido: " + receivedText);

    WiFiClient client;
    HTTPClient http;

    // Constrói a URL com os parâmetros a serem enviados para o ThingSpeak
    String url = "http://";
    url += server;
    url += "/update?api_key=";
    url += apiKey;
    url += "&field1=";
    url += receivedText;

    http.begin(client, url);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Dados enviados para o ThingSpeak com sucesso!");
    } else {
      Serial.print("Erro ao enviar dados para o ThingSpeak. Código de erro HTTP: ");
      Serial.println(httpCode);
    }

    http.end();

    delay(5000); // Aguarda 5 segundos antes de enviar o próximo conjunto de dados
  }
}
