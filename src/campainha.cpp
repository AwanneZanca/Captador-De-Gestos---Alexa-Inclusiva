// Inclui as definições e variáveis globais das simulações.
#include "simulacoes.h"

// Função de setup dos pinos
// Configura os pinos do buzzer e do sensor de som.
void setupSimulacoes() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  
  digitalWrite(BUZZER_PIN, LOW); // Garante que o pino comece desligado
}

// Lógica da campainha
// Controla o toque da campainha em etapas (máquina de estados) sem travar o código.
void loopCampainha() {
  
  // Estado 0: Toca o primeiro tom (agudo e curto).
  if (doorbellState == 0) {
    Serial.println("Tocando a campainha... (Tom 1)");
    tone(BUZZER_PIN, 1500, 200);
    doorbellToneStartTime = millis(); // Salva o tempo de início
    doorbellState = 1; // Avança para o próximo estado
  } 
  // Estado 1: Após uma pausa (250ms), toca o segundo tom (grave e mais longo).
  else if (doorbellState == 1 && millis() - doorbellToneStartTime > 250) {
    Serial.println("Tocando a campainha... (Tom 2)");
    tone(BUZZER_PIN, 1000, 300);
    doorbellToneStartTime = millis(); // Salva o tempo de início
    doorbellState = 2; // Avança para o próximo estado
  } 
  // Estado 2: Após o fim do segundo tom (300ms), desliga a simulação.
  else if (doorbellState == 2 && millis() - doorbellToneStartTime > 300) {
    Serial.println("Campainha finalizada.");
    noTone(BUZZER_PIN); 
    
    // Reseta as variáveis de controle para finalizar a simulação.
    isDoorbellRinging = false;
    soundDetectionActive = false;
    currentSimulationMode = "";
    doorbellState = 0; // Retorna ao estado inicial
  }
}