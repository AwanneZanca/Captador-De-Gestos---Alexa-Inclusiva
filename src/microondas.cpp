// Inclui as definições e variáveis globais das simulações.
#include "simulacoes.h" 

// Lógica do beep do micro-ondas
// Controla os bipes intermitentes (a cada 1 seg) e desliga após 10 seg.
void loopMicroondas() {
  
  // Bloco de inicialização: Executa apenas uma vez quando a simulação começa.
  if (!microwaveMessageSent) {
    Serial.println("Iniciando alarme do micro-ondas...");
    microwaveMessageSent = true; // Marca que a mensagem inicial foi enviada.
    previousBeepTime = millis(); // Define o tempo inicial para o primeiro bipe.
  }

  // Verifica se já passou 1 segundo desde o último bipe.
  if (millis() - previousBeepTime > 1000) {
    previousBeepTime = millis(); // Reseta o cronômetro do bipe.
    tone(BUZZER_PIN, 1000, 400); // Bipa por 0.4 segundos.
  }

  // Lógica de parada: Verifica se a simulação já dura mais de 10 segundos.
  // (Assume que 'microwaveStartTime' foi definido quando a simulação começou).
  if (millis() - microwaveStartTime > 10000) {
    Serial.println("Alarme do micro-ondas finalizado.");
    
    // Reseta as variáveis de controle para finalizar a simulação.
    isBuzzerBeeping = false;
    soundDetectionActive = false;
    currentSimulationMode = ""; 
    noTone(BUZZER_PIN); // Garante que o buzzer pare.
    microwaveMessageSent = false; // Prepara a flag para a próxima vez.
  }
}