#ifndef SIMULACOES_H
#define SIMULACOES_H

#include <Arduino.h>

// --- Declaração das Funções ---
// "Avisa" ao compilador que estas funções existem em algum lugar
void setupSimulacoes();
void loopMicroondas();
void loopCampainha();

// --- Variáveis Globais ---
// "Avisa" que estas variáveis são definidas no main.cpp,
// mas que outros arquivos (microondas.cpp, campainha.cpp) podem usá-las.

extern const int BUZZER_PIN;
extern const int SOUND_SENSOR_PIN;

extern bool isBuzzerBeeping;
extern bool isDoorbellRinging;
extern bool soundDetectionActive;
extern String currentSimulationMode;
extern bool eventSentForThisSimulation;

// Variáveis de estado específicas
extern unsigned long previousBeepTime;
extern unsigned long microwaveStartTime;
extern unsigned long doorbellToneStartTime;
extern int doorbellState;
extern bool microwaveMessageSent;

#endif


