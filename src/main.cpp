// =================== BIBLIOTECAS ===================
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> // Para a interface web
#include <AsyncTCP.h>
#include <SinricPro.h> // Integração com Alexa
#include <SinricProDoorbell.h>
#include "LittleFS.h" // Hospedagem do 'index.html'
#include "simulacoes.h" // Arquivo de cabeçalho com as lógicas separadas

// =================== CREDENCIAIS DO SINRIC PRO E WIFI ===================
#define APP_KEY             "SEU_APP_KEY_AQUI"
#define APP_SECRET          "SEU_APP_SECRET_AQUI"

#define DOORBELL_ID         "ID_DO_DISPOSITIVO_CAMPANHA"
#define MICROWAVE_ID        "ID_DO_DISPOSITIVO_MICROONDAS"
#define LAMPADA_PARAR_ID     "ID_DO_DISPOSITIVO_LAMPADA_PARAR"
#define LAMPADA_LIGAR_ID     "ID_DO_DISPOSITIVO_LAMPADA_LIGAR"
#define LAMPADA_DESLIGAR_ID  "ID_DO_DISPOSITIVO_LAMPADA_DESLIGAR"

#define WIFI_SSID        "NOME_DA_REDE_WIFI"
#define WIFI_PASS        "SENHA_DA_REDE_WIFI"

// =================== CONFIGURAÇÕES DO HARDWARE ===================
const int BUZZER_PIN = 25;
const int SOUND_SENSOR_PIN = 34;
#define SOUND_THRESHOLD 700 // Limiar de sensibilidade do sensor de som

// =================== OBJETOS DE SERVIDOR ===================
AsyncWebServer server(80); // Servidor Web na porta 80
AsyncWebSocket ws("/ws");  // WebSocket no endpoint '/ws'

// =================== VARIÁVEIS DE CONTROLE GLOBAIS ===================
volatile bool isSinricConnected = false; 

// Flags de estado das simulações 
bool isBuzzerBeeping = false;     
bool isDoorbellRinging = false;   
bool soundDetectionActive = false; 
String currentSimulationMode = ""; 

// Declaração dos dispositivos Sinric Pro
// Nota: Todos os dispositivos (Ligar, Parar, Desligar) são tratados como 'Doorbell'
// para simplificar o envio de eventos "push" para a Alexa, acionando rotinas.
SinricProDoorbell &myDoorbell = SinricPro[DOORBELL_ID];
SinricProDoorbell &myMicrowave = SinricPro[MICROWAVE_ID];
SinricProDoorbell &myLampadaParar = SinricPro[LAMPADA_PARAR_ID]; 
SinricProDoorbell &myLampadaLigar = SinricPro[LAMPADA_LIGAR_ID]; 
SinricProDoorbell &myLampadaDesligar = SinricPro[LAMPADA_DESLIGAR_ID]; 

// =========================================================================
// Definição das variáveis globais das simulações
// =========================================================================
unsigned long previousBeepTime = 0;
unsigned long microwaveStartTime = 0;
unsigned long doorbellToneStartTime = 0;
int doorbellState = 0;
bool microwaveMessageSent = false;
// =========================================================================

// Flag para evitar envio duplicado de eventos para a Alexa
bool eventSentForThisSimulation = false;


// =============== CALLBACK DO WEBSOCKET (Controlador da Interface Web) ===============
/**
 * @brief Função de callback acionada em eventos do WebSocket.
 * Gerencia a comunicação em tempo real com a interface web (index.html).
 * Processa comandos recebidos (gestos e simulações).
 */
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Evento de conexão de um novo cliente
  if (type == WS_EVT_CONNECT) { Serial.printf("[WebSocket] Cliente #%u conectado.\n", client->id()); }
  // Evento de desconexão de um cliente
  else if (type == WS_EVT_DISCONNECT) { Serial.printf("[WebSocket] Cliente #%u desconectado.\n", client->id()); }
  // Evento de recebimento de dados (mensagem)
  else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0; // Adiciona terminador nulo para tratar como string

      // Validação: Só processa comandos se o Sinric Pro (Alexa) estiver online.
      if (!isSinricConnected) {
        Serial.println("WS: Comando recebido, mas Sinric Pro nao esta conectado. Ignorando.");
        client->text("Erro: Nao conectado a Alexa. Aguarde e tente novamente.");
        return;
      }

      // Lógica para os comandos de Gesto (Ligar/Parar/Desligar)
      if (strcmp((char*)data, "ligar") == 0) {
        Serial.println("WS: Recebido 'ligar'. Enviando evento (Doorbell Event) para o Sinric.");
        // Envia um evento "Doorbell" para o dispositivo Lâmpada - Ligar.
        myLampadaLigar.sendDoorbellEvent();
      }
      
      else if (strcmp((char*)data, "parar") == 0) {
        Serial.println("WS: Recebido 'parar'. Enviando evento (Doorbell Event) para o Sinric.");
        myLampadaParar.sendDoorbellEvent();
      }

      else if (strcmp((char*)data, "desligar") == 0) {
        Serial.println("WS: Recebido 'desligar'. Enviando evento (Doorbell Event) para o Sinric.");
        myLampadaDesligar.sendDoorbellEvent();
      }
      
      // Lógica para iniciar as simulações de áudio (Micro-ondas, Campainha)
      else if (strcmp((char*)data, "microondas_on") == 0) {
        Serial.println("WS: Recebido 'microondas_on'");
        // Ativa as flags de estado que são processadas no loop principal.
        isBuzzerBeeping = true; soundDetectionActive = true; currentSimulationMode = "microondas";
        microwaveStartTime = millis(); // Define o tempo de início da simulação
        eventSentForThisSimulation = false;
        ws.textAll("microondas_beeping"); // Informa a UI que o som começou
      }
      else if (strcmp((char*)data, "campainha_on") == 0) {
        Serial.println("WS: Recebido 'campainha_on'");
        isDoorbellRinging = true; soundDetectionActive = true; currentSimulationMode = "campainha";
        doorbellState = 0; // Reseta a máquina de estados da campainha
        eventSentForThisSimulation = false;
        ws.textAll("campainha_ringing"); // Informa a UI que o som começou
      }
    }
  }
}

void onSinricProConnected() {
  Serial.println("Conectado ao Sinric Pro!");
  isSinricConnected = true; 
}

void onSinricProDisconnected() {
  Serial.println("Desconectado do Sinric Pro!");
  isSinricConnected = false; 
}

// =============== LÓGICA DO SENSOR DE SOM  ===============
void loopSensorSom() {
  if (!soundDetectionActive || eventSentForThisSimulation) return;
  
  if (analogRead(SOUND_SENSOR_PIN) > SOUND_THRESHOLD) {
    Serial.println("Enviando evento para a Alexa!");

    if (!isSinricConnected) {
      Serial.println("Falha ao enviar: Sinric desconectou AGORA.");
      eventSentForThisSimulation = true; 
      return;
    }

    bool success = false;
    
    if (currentSimulationMode == "campainha") {
      delay(500); 
      Serial.println("Evento campainha enviado");
      success = myDoorbell.sendDoorbellEvent(); 
    }
    else if (currentSimulationMode == "microondas") {
      delay(500);
      Serial.println("Evento micro-ondas enviado");
      success = myMicrowave.sendDoorbellEvent(); 
    }
    
    eventSentForThisSimulation = true;
    if(success) Serial.println("Evento enviado com sucesso.");
    else Serial.println("Falha ao enviar evento.");
  }
}

// =================== SETUP ===================
void setup() {
  Serial.begin(115200);
  
  setupSimulacoes(); 
  
  if(!LittleFS.begin()){ Serial.println("Ocorreu um erro ao montar o LittleFS"); return; }
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP para a Interface: http://"); Serial.println(WiFi.localIP());
  
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(LittleFS, "/index.html", "text/html"); 
  });
  
  server.begin();
  
  SinricPro.onConnected(onSinricProConnected);
  SinricPro.onDisconnected(onSinricProDisconnected);
  
  SinricPro.begin(APP_KEY, APP_SECRET);
  
  Serial.println("Sistema da Alexa Inclusiva pronto.");
}

// =================== LOOP PRINCIPAL ===================
void loop() {
  SinricPro.handle();
  ws.cleanupClients();

  if (isBuzzerBeeping) {
    loopMicroondas(); 
  }

  if (isDoorbellRinging) {
    loopCampainha(); 
  }

  if (soundDetectionActive) {
    loopSensorSom(); 
  }
}