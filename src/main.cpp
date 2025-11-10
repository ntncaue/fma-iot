#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define boardLED 2

// --- CONFIGURAÇÕES ---
const char *MOTORCYCLE_ID = "MOTO-01";
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;

#define TOPICO_PUBLISH "fma/patio/motos"
// O tópico de subscrição agora é dinâmico

// Coordenadas pré-definidas para simular vagas no pátio
const float lats[] = {-23.5501, -23.5503, -23.5505, -23.5507, -23.5509};
const float lngs[] = {-46.6331, -46.6333, -46.6335, -46.6337, -46.6339};
const int num_coords = 5;

// --- VARIÁVEIS GLOBAIS ---
WiFiClient espClient;
PubSubClient MQTT(espClient);
char json_buffer[256];
char topic_subscribe_buffer[100];
const char* motorcycle_status = "estacionada";
float current_lat;
float current_lng;

void triggerFindMoto() {
  Serial.println("Comando 'find_moto' recebido! Piscando LED...");
  for (int i = 0; i < 10; i++) {
    digitalWrite(boardLED, HIGH);
    delay(100);
    digitalWrite(boardLED, LOW);
    delay(100);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  StaticJsonDocument<100> doc;
  deserializeJson(doc, payload, length);
  const char* command = doc["command"];

  if (strcmp(command, "set_status") == 0) {
    const char* new_status = doc["status"];
    if (new_status) {
      motorcycle_status = new_status;
      Serial.print("Status da moto alterado para: ");
      Serial.println(motorcycle_status);
    }
  } else if (strcmp(command, "find_moto") == 0) {
    triggerFindMoto();
  }
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
  MQTT.setCallback(callback);

  // Cria o tópico de subscrição dinamicamente
  snprintf(topic_subscribe_buffer, sizeof(topic_subscribe_buffer), "fma/patio/motos/%s/cmd", MOTORCYCLE_ID);

  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT...");
    if (MQTT.connect(MOTORCYCLE_ID)) {
      Serial.println("Conectado ao Broker!");
      MQTT.subscribe(topic_subscribe_buffer);
      Serial.print("Inscrito no tópico: ");
      Serial.println(topic_subscribe_buffer);
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
  MQTT.loop();
}

void publishTelemetry() {
  // Se a moto está "em movimento", sorteia uma nova posição
  if (strcmp(motorcycle_status, "em_movimento") == 0) {
    current_lat = lats[random(0, num_coords)];
    current_lng = lngs[random(0, num_coords)];
  }
  // Se estiver parada ou em manutenção, ela permanece no mesmo lugar.

  StaticJsonDocument<200> doc;
  doc["motorcycle_id"] = MOTORCYCLE_ID;
  doc["lat"] = current_lat;
  doc["lng"] = current_lng;
  doc["status"] = motorcycle_status;

  serializeJson(doc, json_buffer);

  if (MQTT.publish(TOPICO_PUBLISH, json_buffer)) {
    Serial.println("Telemetria publicada:");
    Serial.println(json_buffer);
  } else {
    Serial.println("Falha ao publicar telemetria.");
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
  
  randomSeed(analogRead(0));

  // Define uma posição inicial aleatória
  current_lat = lats[random(0, num_coords)];
  current_lng = lngs[random(0, num_coords)];

  initWiFi();
  initMQTT();
}

void loop() {
  checkConnections();
  publishTelemetry();
  blinkLed();
  delay(10000);
}
