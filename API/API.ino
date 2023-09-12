#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "MERCUSYS_0DAC";
const char* password = "tatinha123";
const char* apiKey = "AIzaSyCdG6T1UiCiO88p9Wq_eV_HzYNUnTh3kFI";
const int isd1820PlayPin = 25; // Pino de controle para reprodução no ISD1820 (D25)

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(isd1820PlayPin, OUTPUT);
  digitalWrite(isd1820PlayPin, LOW); // Certifique-se de que o pino está em estado baixo inicialmente

  // Conecte-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.println("Conectado ao Wi-Fi!");

  // Texto para conversão em áudio
  String textToConvert = "Olá Helo";

  // Configura a URL da API do Google Text-to-Speech
  String apiUrl = "https://texttospeech.googleapis.com/v1/text:synthesize?key=";
  apiUrl += apiKey;

  // Configura o corpo da solicitação HTTP
  String requestBody = "{ \"input\": { \"text\": \"" + textToConvert + "\" }, \"voice\": { \"languageCode\": \"pt-BR\", \"name\": \"pt-BR-Standard-C\" }, \"audioConfig\": { \"audioEncoding\": \"LINEAR16\" } }";

  // Realiza a solicitação HTTP POST
  HTTPClient http;
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(requestBody);

  // Verifica se a solicitação foi bem-sucedida
  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.println("Resposta da API:");
    Serial.println(response);

    // Decodifica a resposta Base64 manualmente
    String base64Audio = response; // Sua resposta Base64 aqui
    base64Audio.replace("\n", ""); // Remova quebras de linha, se houver

    int len = base64Audio.length();
    uint8_t *decodedBytes = (uint8_t*)malloc(len);
    int decodedLen = base64_decode(decodedBytes, base64Audio.c_str(), len);

    // Acione o pino para iniciar a reprodução no módulo ISD1820
    digitalWrite(isd1820PlayPin, HIGH);
    delay(100); // Aguarde um curto período de tempo
    digitalWrite(isd1820PlayPin, LOW);

    free(decodedBytes);

  } else {
    Serial.print("Erro na chamada da API: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {
  // Nada a fazer no loop
}
