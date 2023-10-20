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
bool aguardandoResposta = false;
int etapaConversa = 0;  // Etapa da conversa

// LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED
#define PIN_RED 21    // Pino GIOP21 (cor vermelha)
#define PIN_GREEN 22  // Pino GIOP22 (cor verde)
#define PIN_BLUE 23   // Pino GIOP23 (cor azul)

#define LEDC_CHANNEL_0 0

// Definindo os pinos SDA e SCL para o ESP32
#define SDA_PIN 32
#define SCL_PIN 33

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

String substituirCaracteresEspeciais(String texto) {
  // Substitua os caracteres especiais aqui, se necessário
  texto.replace("á", "a");
  texto.replace("à", "a");
  texto.replace("â", "a");
  texto.replace("ã", "a");
  texto.replace("é", "e");
  texto.replace("è", "e");
  texto.replace("ê", "e");
  texto.replace("í", "i");
  texto.replace("ì", "i");
  texto.replace("î", "i");
  texto.replace("ó", "o");
  texto.replace("ò", "o");
  texto.replace("ô", "o");
  texto.replace("õ", "o");
  texto.replace("ú", "u");
  texto.replace("ù", "u");
  texto.replace("û", "u");
  // Adicione mais substituições conforme necessário.
  return texto;
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
    receivedData = substituirCaracteresEspeciais(receivedData);  // Substitui caracteres acentuados e especiais
    Serial.println("Texto recebido: " + receivedData);
    aguardandoResposta = true;
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

  // Inicializar o Bluetooth
  SerialBT.begin("ESP32test");
  Serial.println("O dispositivo foi iniciado, agora você pode pareá-lo com o Bluetooth!");

  // Configurar o pino trigPin como saída
  pinMode(trigPin, OUTPUT);
  // Configurar o pino echoPin como entrada
  pinMode(echoPin, INPUT);


  // Inicializar o LCD
  Wire.begin(SDA_PIN, SCL_PIN);  // Inicia o barramento I2C
  lcd.init();
  lcd.backlight();  // Liga o backlight do LCD
  lcd.clear();      // Limpa o LCD
}

String nome = "";

void saudacaoInicial() {
  lcd.clear();
  lcd.print("Ola, eu sou o");
  lcd.setCursor(0, 1);
  lcd.print("MannaSpy!");
  delay(5000);
}

void pedirNome() {
  lcd.clear();
  lcd.print("Qual o seu nome?");
  while (receivedData.isEmpty()) {
    processarDadosRecebidos();
    delay(1000);
  }
  nome = receivedData;
  lcd.clear();
  lcd.print("Ola, " + nome + "!");
  delay(5000);
  receivedData = "";  // Limpa receivedData para a próxima resposta
}

void processarRespostaBemEstar() {
  lcd.clear();
  lcd.print("Esta tudo bem?");
  lcd.setCursor(0, 1);
  lcd.print("(Sim/Nao)");

  while (receivedData.isEmpty()) {
    processarDadosRecebidos();
    delay(1000);
  }

  String respostaMinuscula = receivedData;
  respostaMinuscula.toLowerCase();

  if (respostaMinuscula == "sim") {
    lcd.clear();
    lcd.print("Fico feliz");
    lcd.setCursor(0, 1);
    lcd.print("em saber!");
    mostrarCores();
  } else if (respostaMinuscula == "não") {
    lcd.clear();
    lcd.print("Sinto muito");
    lcd.setCursor(0, 1);
    lcd.print("ouvir isso.");
    mostrarCores();
    delay(5000);
  }
  receivedData = "";  // Limpa receivedData para a próxima resposta
}

void perguntas() {
  mostrarCores();
  lcd.clear();
  lcd.print("Vamos falar da");
  lcd.setCursor(0, 1);
  lcd.print("internet?");
  delay(5000);
  lcd.clear();
  lcd.print("(Sim/Nao)");
  while (receivedData.isEmpty()) {
    processarDadosRecebidos();
    delay(1000);
  }

  String respostaMinuscula = receivedData;
  respostaMinuscula.toLowerCase();

  if (respostaMinuscula == "sim") {
    receivedData = "";
    lcd.clear();
    lcd.print("Legal!");
    lcd.setCursor(0, 1);
    lcd.print("Vamos aprender.");
    delay(5000);

    mostrarCores();
    lcd.clear();
    lcd.print("O que mais gosta");
    lcd.setCursor(0, 1);
    lcd.print("de fazer online?");
    while (receivedData.isEmpty()) {
      processarDadosRecebidos();
      delay(1000);
    }

    receivedData = "";
    mostrarCores();
    lcd.clear();
    lcd.print("Muito divertido!");
    delay(5000);

    lcd.clear();
    lcd.print("Gosta de jogos?");
    lcd.setCursor(0, 1);
    lcd.print("(sim/nao)");
    while (receivedData.isEmpty()) {
      processarDadosRecebidos();
      delay(1000);
    }
    if (respostaMinuscula == "sim") {
      lcd.clear();
      lcd.print("Que legal!");
    } else if (respostaMinuscula == "não") {
      lcd.clear();
      lcd.print("Faz parte.");
    }

    receivedData = "";

    mostrarCores();
    lcd.clear();
    lcd.print("Sabe que");
    lcd.setCursor(0, 1);
    lcd.print("nao pode...");
    delay(5000);

    mostrarCores();
    lcd.clear();
    lcd.print("falar com");
    lcd.setCursor(0, 1);
    lcd.print("estranhos...");
    delay(5000);

    mostrarCores();
    lcd.clear();
    lcd.print("na internet, ne?");
    lcd.setCursor(0, 1);
    lcd.print("(sim/nao)");
    delay(5000);
    while (receivedData.isEmpty()) {
      processarDadosRecebidos();
      delay(1000);
    }

    if (respostaMinuscula == "sim") {
      receivedData = "";
      lcd.clear();
      lcd.print("boaa, " + nome);
    }

    else if (respostaMinuscula == "não") {
      receivedData = "";
      lcd.clear();
      lcd.print("agora voce sabe");
      lcd.setCursor(0, 1);
      lcd.print("que nao pode, " + nome);
    }

    mostrarCores();
  } else if (respostaMinuscula == "não") {
    lcd.clear();
    lcd.print("Sinto muito");
    lcd.setCursor(0, 1);
    lcd.print("ouvir isso.");
    mostrarCores();
    delay(5000);

    lcd.clear();
    lcd.print("Quem sabe");
    lcd.setCursor(0, 1);
    lcd.print("uma próxima, " + nome);
  }
}

void loop() {
  enviarPulso();
  distanceCm = medirDistancia();

  if (distanceCm <= 30) {
    mostrarCores();
    if (etapaConversa == 0) {
      saudacaoInicial();
      pedirNome();
      processarRespostaBemEstar();
      perguntas();
    }
  } else {
    setColor(0, 0, 0);
  }

  delay(1000);
}
