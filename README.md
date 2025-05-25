w## 📌 Descrição

Este projeto utiliza um ESP32 para conectar-se a uma rede Wi-Fi e enviar dados simulados de localização (latitude e longitude) para um broker MQTT.

---

## ✅ Funcionalidades

- 📶 Conexão à rede Wi-Fi simulada.
- 📤 Envio periódico de dados de geolocalização via MQTT.
- 🧭 Ideal para simular o rastreamento de motos em um pátio.

---

## 🧰 Pré-requisitos

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- Conta no [Wokwi](https://wokwi.com/)
- Broker MQTT local (ex: [Mosquitto](https://mosquitto.org/)) ou online (ex: [HiveMQ](https://www.hivemq.com/))

---

## ⚙️ Instalação

1. Clone este repositório:

   ```bash
   git clone https://github.com/ntncaue/fma-iot.git

   ```

2. Abra o projeto no VSCode:

   Abra o Visual Studio Code e navegue até a pasta do projeto clonado.

3. Instale as dependências:

   O PlatformIO irá instalar automaticamente as bibliotecas necessárias durante a primeira compilação.

4. Utilizar

   Agora basta clicar em Build para compilar o código e após iniciar a simulação basta usar um cliente MQTT para receber e interagir com os dados.
