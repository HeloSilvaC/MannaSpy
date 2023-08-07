#include <BluetoothSerial.h>

// Inicializa o objeto BluetoothSerial
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32Bluetooth"); // Defina o nome do dispositivo Bluetooth
}

void loop() {
  if (SerialBT.available()) { // Verifica se há dados disponíveis para leitura
    String receivedData = SerialBT.readString(); // Lê os dados recebidos em formato de string
    Serial.println("Texto recebido: " + receivedData); // Exibe o texto recebido no monitor serial
  }
  // Outras ações que você queira realizar no loop, se necessário
}
