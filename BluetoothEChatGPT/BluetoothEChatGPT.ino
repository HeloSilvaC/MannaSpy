#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>

const char* ssid = "MERCUSYS_0DAC";
const char* password = "contrera";

BluetoothSerial SerialBT;
char chatGptApiKey[] = "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";
char chatGptUrl[] = "https://api.openai.com/v1/engines/davinci-codex/completions";

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando à rede WiFi...");
  }
  Serial.println("Conectado à rede WiFi!");
}

void loop() {
  if (SerialBT.available()) {
    char texto[256];
    int length = SerialBT.readBytesUntil('\n', texto, sizeof(texto) - 1);
    texto[length] = '\0';

    Serial.print("Texto recebido: ");
    Serial.println(texto);

    String resposta = enviarTextoParaChatGpt(texto);

    Serial.print("Resposta do modelo GPT: ");
    Serial.println(resposta);
  }
}

String enviarTextoParaChatGpt(const char* texto) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, chatGptUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer ");
  http.addHeader("Authorization", chatGptApiKey);

  char payload[512];
  snprintf(payload, sizeof(payload), "{\"prompt\": \"%s\", \"max_tokens\": 50}", texto);

  int httpCode = http.POST(payload);
  String resposta;

  if (httpCode == HTTP_CODE_OK) {
    String responseBody = http.getString();
    resposta = extrairRespostaChatGpt(responseBody);
  } else {
    Serial.print("Erro ao enviar texto para o modelo GPT. Código de erro HTTP: ");
    Serial.println(httpCode);
  }

  http.end();
  return resposta;
}

String extrairRespostaChatGpt(const String& responseBody) {
  int startIndex = responseBody.indexOf("\"choices\":[{\"text\":\"") + 20;
  int endIndex = responseBody.indexOf("\"}],", startIndex);
  String resposta = responseBody.substring(startIndex, endIndex);
  resposta.trim();
  return resposta;
}
