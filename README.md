# Projeto de Monitoramento de Frota para Motos (FMA-IoT)

Este projeto implementa uma solução de IoT de ponta a ponta para o monitoramento em tempo real de uma frota de motocicletas, como parte da avaliação da FMA (Fast Motorcycle Agreement).

O sistema é composto por três componentes principais:
1.  **Dispositivo IoT (Simulado):** Um firmware para ESP32 que simula uma motocicleta, coletando e enviando dados de telemetria.
2.  **Backend:** Um serviço de API que recebe os dados, os persiste em um banco de dados e os expõe para consumo.
3.  **Frontend (Dashboard):** Uma interface web que visualiza os dados recebidos em tempo real.

---

## Arquitetura

```
[ESP32 com GPS] --(MQTT)--> [Broker MQTT Público] --(MQTT)--> [Backend Python] <--(HTTP)--> [Dashboard Web]
                                                                    |
                                                                    v
                                                              [Banco de Dados SQLite]
```

---

## Tecnologias Utilizadas

- **Dispositivo IoT (ESP32):**
  - Framework: Arduino
  - Bibliotecas: `PubSubClient` (para MQTT), `ArduinoJson`, `TinyGPS++`.
  - Simulação: Wokwi

- **Backend:**
  - Linguagem: Python 3
  - Framework: FastAPI
  - Comunicação: `paho-mqtt`
  - Banco de Dados: SQLite com SQLAlchemy

- **Frontend:**
  - HTML5, CSS3, JavaScript (Vanilla)

---

## Como Executar o Projeto

Siga os passos abaixo para rodar a solução completa.

### 1. Simulação do Dispositivo IoT (Wokwi)

1.  **Abra o Wokwi:** Vá para [Wokwi](https://wokwi.com/)
2.  **Crie um novo projeto:** Escolha a placa "ESP32".
3.  **Copie os arquivos:**
    - Copie o conteúdo de `src/main.cpp` para a aba `sketch.cpp` no Wokwi.
    - Copie o conteúdo de `platformio.ini` para a aba `platformio.ini`.
    - Na aba `diagram.json`, adicione um módulo GPS (`"type": "wokwi-gps-module"`) e conecte os pinos RX/TX aos pinos 16/17 do ESP32.
4.  **Inicie a simulação:** Clique no botão verde "Start Simulation". Você deverá ver as mensagens de telemetria sendo publicadas no console serial.

### 2. Executar o Backend

O backend escuta as mensagens MQTT e as salva. Para executá-lo, você precisa do Python 3 instalado.

```bash
# 1. Navegue até a pasta do backend
cd backend

# 2. (Opcional, mas recomendado) Crie um ambiente virtual
python -m venv venv
# Ative o ambiente (Windows)
source venv/Scripts/activate
# Ative o ambiente (macOS/Linux)
# source venv/bin/activate

# 3. Instale as dependências
pip install -r requirements.txt

# 4. Inicie o servidor
uvicorn main:app --reload
```
O backend estará rodando em `http://127.0.0.1:8000`.

### 3. Visualizar o Dashboard

1.  Navegue até a pasta `frontend`.
2.  Abra o arquivo `index.html` diretamente no seu navegador (ex: Chrome, Firefox).

O dashboard se conectará automaticamente ao backend e começará a exibir os dados das motos assim que eles forem recebidos.

---

## Cenários de Teste

Para atender aos requisitos da avaliação, você pode simular múltiplos dispositivos:

- **Múltiplas Motos:** Abra várias abas do navegador com a simulação do Wokwi. Em cada uma, edite o `MOTORCYCLE_ID` no código para um valor diferente (ex: `MOTO-02`, `MOTO-03`). Inicie todas as simulações.
- **Status Variados:** O código já simula status diferentes (`estacionada`, `em_movimento`, `manutencao`) de forma aleatória a cada envio.
- **Visualização:** O dashboard mostrará cada moto como um card separado, atualizando seu status e localização em tempo real.