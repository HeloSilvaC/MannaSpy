// Definições de pinos e constantes
const int trigPin = 5;
const int echoPin = 18;

// Velocidade do som em cm/uS
#define VELOCIDADE_DO_SOM 0.034
#define CM_PARA_POLEGADAS 0.393701

// Variáveis globais
long duration;
float distanceCm;

// Bluetooth
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth não está habilitado! Por favor, execute `make menuconfig` para ativá-lo
#endif

BluetoothSerial SerialBT;
String receivedData = "";

// LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definindo os pinos SDA e SCL para o ESP32
#define SDA_PIN 32
#define SCL_PIN 33

LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED
#define PIN_RED 21    // Pino GIOP21 (cor vermelha)
#define PIN_GREEN 22  // Pino GIOP22 (cor verde)
#define PIN_BLUE 23   // Pino GIOP23 (cor azul)

#define LEDC_CHANNEL_0 0

// Função para enviar um pulso ultrassônico
void enviarPulso() {
  digitalWrite(trigPin, LOW);  // Define o pino trigPin em estado LOW para garantir um pulso limpo
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Define o pino trigPin em estado HIGH para enviar um pulso ultrassônico
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  // Retorna o pino trigPin para o estado LOW após enviar o pulso
}

// Função para medir a distância com o sensor ultrassônico
float medirDistancia() {
  duration = pulseIn(echoPin, HIGH);                    // Lê o pino echoPin e retorna o tempo de viagem da onda sonora em microssegundos
  float distanceCm = duration * VELOCIDADE_DO_SOM / 2;  // Calcula a distância
  return distanceCm;
}

// Função para definir a sequência de cores do LED RGB
void mostrarCores() {
  // Vermelho
  setColor(255, 0, 0);
  delay(500);

  // Verde
  setColor(0, 255, 0);
  delay(500);

  // Azul
  setColor(0, 0, 255);
  delay(500);

  // Amarelo
  setColor(255, 255, 0);
  delay(500);

  // Branco
  setColor(255, 255, 255);
  delay(500);

  // Laranja
  setColor(255, 165, 0);
  delay(500);

  // Rosa
  setColor(255, 20, 147);
  delay(500);
}

// Função para configurar a cor do LED RGB
void setColor(int R, int G, int B) {
  ledcWrite(LEDC_CHANNEL_0, R);      // Define a intensidade do canal RED
  ledcWrite(LEDC_CHANNEL_0 + 1, G);  // Define a intensidade do canal GREEN
  ledcWrite(LEDC_CHANNEL_0 + 2, B);  // Define a intensidade do canal BLUE
}

void processarDadosRecebidos() {
  if (SerialBT.available()) {
    lcd.clear();
    receivedData = SerialBT.readString();
    Serial.println("Texto recebido: " + receivedData);

    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);

    if (receivedData.length() <= 16) {
      lcd.print(receivedData);
    } else {
      lcd.print(receivedData.substring(0, 16));
      lcd.setCursor(0, 1);
      lcd.print(receivedData.substring(16));
    }
  }
}


void setup() {
  // Configurar os pinos do LED RGB como saída
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  // Configurar o LEDC para controlar o LED RGB
  ledcSetup(LEDC_CHANNEL_0, 5000, 8);            // Configura o canal LEDC para frequência de 5000 Hz e resolução de 8 bits
  ledcAttachPin(PIN_RED, LEDC_CHANNEL_0);        // Associa o pino RED ao canal LEDC
  ledcAttachPin(PIN_GREEN, LEDC_CHANNEL_0 + 1);  // Associa o pino GREEN ao canal LEDC
  ledcAttachPin(PIN_BLUE, LEDC_CHANNEL_0 + 2);   // Associa o pino BLUE ao canal LEDC

  // Inicializar a comunicação serial
  Serial.begin(115200);

  // Configurar o pino trigPin como saída
  pinMode(trigPin, OUTPUT);
  // Configurar o pino echoPin como entrada
  pinMode(echoPin, INPUT);

  // Inicializar o Bluetooth
  SerialBT.begin("ESP32test");
  Serial.println("O dispositivo foi iniciado, agora você pode pareá-lo com o Bluetooth!");

  // Inicializar o LCD
  Wire.begin(SDA_PIN, SCL_PIN);  // Inicia o barramento I2C
  lcd.init();
  lcd.backlight();  // Liga o backlight do LCD
  lcd.clear();      // Limpa o LCD
}

void loop() {
  enviarPulso();                  // Envia pulso ultrassônico
  distanceCm = medirDistancia();  // Mede a distância

  // Verifica se a distância é menor ou igual a 30 cm
  if (distanceCm <= 30) {
    // Define a sequência de cores do LED RGB
    mostrarCores();
  } else {
    // Desliga o LED RGB
    setColor(0, 0, 0);
  }

  processarDadosRecebidos();  // Processa os dados recebidos via Bluetooth
}

