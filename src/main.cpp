#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define boardLED 2

// --- CONFIGURAÇÕES ---
// ID único para esta "moto"
const char *MOTORCYCLE_ID = "MOTO-01";

// Wi-Fi (para o simulador Wokwi)
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// Broker MQTT Público (sem autenticação)
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;

// Tópico MQTT para publicação
#define TOPICO_PUBLISH "fma/patio/motos"

// --- VARIÁVEIS GLOBAIS ---
WiFiClient espClient;
PubSubClient MQTT(espClient);
char json_buffer[256];

// GPS (via Serial2: RX=16, TX=17 no Wokwi)
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

// Função para simular o status da moto
const char* getMotorcycleStatus() {
  const char* statuses[] = {"estacionada", "em_movimento", "manutencao"};
  int randomIndex = random(0, 3);
  return statuses[randomIndex];
}

void initWiFi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT...");
    // Tenta conectar com um ID de cliente único
    if (MQTT.connect(MOTORCYCLE_ID)) {
      Serial.println("Conectado ao Broker!");
    } else {
      Serial.print("Falha na conexão. Estado: ");
      Serial.print(MQTT.state());
      Serial.println(" Tentando novamente em 2 segundos");
      delay(2000);
    }
  }
}

void checkConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexão Wi-Fi perdida. Reconectando...");
    initWiFi();
  }
  if (!MQTT.connected()) {
    Serial.println("Conexão MQTT perdida. Reconectando...");
    initMQTT();
  }
  MQTT.loop(); // Mantém a conexão MQTT ativa
}

void publishTelemetry() {
  // Garante que o GPS processe os dados seriais
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  // Publica apenas se a localização do GPS for válida
  if (gps.location.isValid()) {
    StaticJsonDocument<200> doc;
    
    // Adiciona os dados ao JSON
    doc["motorcycle_id"] = MOTORCYCLE_ID;
    doc["lat"] = gps.location.lat();
    doc["lng"] = gps.location.lng();
    doc["status"] = getMotorcycleStatus();

    // Serializa o JSON para o buffer
    serializeJson(doc, json_buffer);

    // Publica a mensagem no tópico MQTT
    if (MQTT.publish(TOPICO_PUBLISH, json_buffer)) {
      Serial.println("Telemetria publicada:");
      Serial.println(json_buffer);
    } else {
      Serial.println("Falha ao publicar telemetria.");
    }

  } else {
    Serial.println("Aguardando dados válidos do GPS...");
  }
}

void blinkLed() {
  digitalWrite(boardLED, HIGH);
  delay(150);
  digitalWrite(boardLED, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(boardLED, OUTPUT);
  digitalWrite(boardLED, LOW);
  
  // Inicia a semente para números aleatórios
  randomSeed(analogRead(0));

  initWiFi();
  initMQTT();

  // Inicia a comunicação serial com o módulo GPS
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
  checkConnections();
  publishTelemetry();
  blinkLed();
  delay(10000); // Envia dados a cada 10 segundos
}
