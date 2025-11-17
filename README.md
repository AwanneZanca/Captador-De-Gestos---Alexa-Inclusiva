# TCC - Alexa Inclusiva

> Protótipo de um sistema de acessibilidade para pessoas com deficiência auditiva, integrando o microcontrolador ESP32, reconhecimento de gestos com MediaPipe e a assistente virtual Alexa (via Sinric Pro).

---

## 🚀 Sobre o Projeto

Este repositório contém o código-fonte (software embarcado e front-end) do projeto "Alexa Inclusiva", desenvolvido como um Trabalho de Conclusão de Curso (TCC).

O objetivo principal deste protótipo não é criar um sistema de classificação de som, mas sim **validar uma arquitetura de baixo custo** que utiliza tecnologia IoT para fornecer inclusão a pessoas com deficiência auditiva.

O sistema foca-se em duas funcionalidades principais:

1.  **Controlo por Gestos:** Permitir que o utilizador controle dispositivos (como lâmpadas inteligentes) através da Alexa, usando gestos manuais capturados pelo MediaPipe, sem necessidade de comandos de voz.
2.  **Notificação de Alertas Sonoros:** Simular sons domésticos (campainha, micro-ondas) e usar um sensor de som para detetar o próprio ruído da simulação, enviando um evento para a Alexa. A Alexa, por sua vez, aciona uma **rotina de sinalização visual** (como piscar uma lâmpada), notificando o utilizador de um evento que, de outra forma, passaria despercebido.

## ✨ Funcionalidades Principais

* **Servidor Web no ESP32:** O ESP32 hospeda uma página web (`index.html`) acessível pela rede local.
* **Comunicação em Tempo Real:** Um WebSocket bidirecional é usado para a comunicação instantânea entre a página web (cliente) e o ESP32 (servidor).
* **Reconhecimento de Gestos (Cliente):** O JavaScript na página web usa o **MediaPipe Hands** para detetar os gestos "Ligar", "Parar" e "Desligar" através da câmara do dispositivo.
* **Integração com a Nuvem (Embarcado):** O ESP32 usa a biblioteca **Sinric Pro** para se conectar à Alexa como múltiplos dispositivos virtuais (do tipo "Doorbell").
* **Simulação e Deteção Sonora (Embarcado):** O ESP32 simula sons (com um *buzzer*) e utiliza um sensor de som (KY-037) para detetar o seu próprio som, validando o evento e enviando-o para a Alexa.

## 🛠️ Tecnologias Utilizadas

* **Hardware:**
    * ESP32 (Placa de desenvolvimento)
    * Sensor de Som KY-037
    * Buzzer Ativo
* **Software Embarcado (C++):**
    * PlatformIO (VS Code)
    * Framework Arduino
    * SinricPro
    * ESPAsyncWebServer
    * LittleFS
* **Software Front-End (Cliente):**
    * HTML / CSS / JavaScript
    * Google MediaPipe (Hands)
* **Plataforma de Nuvem:**
    * Sinric Pro
    * Amazon Alexa (para a criação das Rotinas de sinalização visual)
