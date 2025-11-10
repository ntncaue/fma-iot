# Projeto de Monitoramento de Frota para Motos (FMA-IoT)

Este projeto implementa uma solução de IoT de ponta a ponta para o monitoramento em tempo real de uma frota de motocicletas, utilizando uma arquitetura simplificada sem backend.

O sistema é composto por dois componentes principais:

1.  **Dispositivo IoT (Simulado):** Um firmware para ESP32 que simula uma motocicleta, coletando e enviando dados de telemetria via MQTT.
2.  **Frontend (Dashboard):** Uma interface web que se conecta diretamente ao broker MQTT para visualizar os dados e enviar comandos em tempo real.

---

## Arquitetura (Backend-less)

```
[Dispositivo IoT] <--(MQTT over TCP)--> [Broker MQTT] <--(MQTT over WebSocket)--> [Dashboard Frontend]
```

---

## Tecnologias Utilizadas

- **Dispositivo IoT (ESP32):**
  - Framework: Arduino
  - Bibliotecas: `PubSubClient`, `ArduinoJson`.
  - Simulação: Wokwi

- **Frontend:**
  - HTML5, CSS3, JavaScript
  - Comunicação MQTT: MQTT.js
  - Visualização de Pátio: CSS Grid

---

## Como Executar o Projeto

Siga os passos abaixo para rodar a solução completa.

### 1. Simulação do Dispositivo IoT (Wokwi)

1.  **Abra o Wokwi:** Vá para [Wokwi](https://wokwi.com/projects/new/esp32)
2.  **Copie o código:**
    - Copie o conteúdo de `src/main.cpp` e cole no editor de código do Wokwi.
3.  **Adicione as bibliotecas:**
    - No gerenciador de bibliotecas do Wokwi (botão "+"), procure e adicione `PubSubClient` e `ArduinoJson`.
4.  **Inicie a simulação:** Clique no botão verde "Start Simulation". Você deverá ver as mensagens de telemetria sendo publicadas no console serial.

### 2. Visualizar o Dashboard

1.  Navegue até a pasta `frontend`.
2.  Abra o arquivo `index.html` diretamente no seu navegador (ex: Chrome, Firefox).

O dashboard se conectará automaticamente ao broker MQTT e começará a exibir os dados das motos em tempo real no pátio e no painel lateral.

---
