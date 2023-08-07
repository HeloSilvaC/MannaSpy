#include <SoftwareSerial.h>

#define VOICE_MODULE_RX_PIN 2
#define VOICE_MODULE_TX_PIN 3

SoftwareSerial voiceModule(VOICE_MODULE_RX_PIN, VOICE_MODULE_TX_PIN);

void setup() {
  Serial.begin(9600);
  voiceModule.begin(9600);
  
  Serial.println("Voice Module Recording Example");
  Serial.println("Enter 'r' to start recording a command.");
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    
    if (command == 'r') {
      startRecording();
    }
  }
  
  if (voiceModule.available()) {
    Serial.write(voiceModule.read());
  }
}

void startRecording() {
  voiceModule.print("AT+REC\r\n");
  
  if (voiceModule.find("OK")) {
    Serial.println("Recording started. Speak now...");
    
    delay(3000); // Adjust the delay time based on your recording length
    
    voiceModule.print("AT+STOP\r\n");
    
    if (voiceModule.find("OK")) {
      Serial.println("Recording complete!");
    }
  }
}
