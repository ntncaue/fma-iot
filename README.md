## 📌 Descrição

Este projeto tem como objetivo o **rastreamento de localização de motocicletas** utilizando um microcontrolador **ESP32**. Inicialmente, os dados de geolocalização (latitude e longitude) são **simulados** no ambiente do Wokwi, mas o código já está preparado para uso com módulos GPS reais como o **NEO-6M**, bastando conectar o módulo e ajustar a leitura da localização.

Os dados são enviados periodicamente através do protocolo **MQTT** para um **broker MQTT local ou remoto**, permitindo o monitoramento da localização da moto em tempo real.

---

## ❗ Problema

Em pátios de motocicletas, principalmente em locais de grande volume como o da Mottu, é comum a dificuldade de localizar rapidamente uma moto específica entre dezenas ou centenas de unidades.

Essa falta de rastreabilidade pode causar:

- ⏱️ Perda de tempo na busca manual.
- ❌ Riscos de erros ou trocas de motos.
- 🚨 Falta de visibilidade em tempo real sobre a localização das motos.

## 💡 Solução

Este projeto propõe uma solução para monitorar em tempo real a localização de cada moto no pátio, utilizando os seguintes recursos:

- 📍 Um microcontrolador **ESP32** com um **módulo GPS** (como o **NEO-6M**) acoplado à moto.
- 🌐 Conexão via **Wi-Fi** e envio das coordenadas por protocolo **MQTT**.
- 📊 Os dados são enviados a um **App** com dashboard feito em **React Native**, onde a localização de todas as motos pode ser visualizada.

---

## ⚙️ Como será feito

1. Cada moto terá um **ESP32** com GPS conectado.
2. O ESP32 obtém as coordenadas geográficas em tempo real.
3. Os dados são enviados via **MQTT** para um **broker** (local ou na nuvem).
4. Um sistema front-end personalizado consome esses dados e exibe a localização das motos.

---

✅ Com isso, reduzimos drasticamente o tempo de localização, aumentamos a eficiência operacional e trazemos rastreabilidade e controle ao processo — tudo isso utilizando tecnologias acessíveis de IoT.

## 🛠️ Tecnologias Utilizadas

- **ESP32**: Microcontrolador com Wi-Fi embutido.
- **MQTT**: Protocolo leve para envio dos dados.
- **Wi-Fi**: Para conexão com a internet ou rede local.
- **Wokwi**: Simulador online para testes sem hardware real.
- **PlatformIO + VSCode**: Ambiente de desenvolvimento e build.
- **ArduinoJson**: Biblioteca para formatação dos dados em JSON.
- **PubSubClient**: Cliente MQTT para microcontroladores.
- **TinyGPSPlus** (para uso futuro com GPS real): Biblioteca para decodificar sinais GPS.

---

## ⚙️ Instalação

1. **Clone este repositório**:

   ```bash
   git clone https://github.com/seu-usuario/seu-repo.git

   Abra o projeto no VSCode com PlatformIO instalado:

   ```

2. **Abra o projeto no VSCode com PlatformIO instalado**:
   Vá em File > Open Folder e selecione a pasta do projeto.

3.**Compile o projeto**:

No menu lateral do PlatformIO, clique em ✅ “Build” para compilar o código.

4.**Executar a simulação ou gravar no ESP32**:

Se estiver simulando no Wokwi, execute o projeto por lá.

Se estiver usando ESP32 real, conecte-o via USB e clique em “Upload”.
