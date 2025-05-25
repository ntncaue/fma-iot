#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define boardLED 2

// Identificadores
const char *ID = "moto123";
const char *moduleID = "ESP32_LOC";

// Wi-Fi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// MQTT Broker
const char *BROKER_MQTT = "172.190.149.52";
const int BROKER_PORT = 1883;
const char *mqttUser = "admin_fiap";
const char *mqttPassword = "admin_fiap123";

#define TOPICO_PUBLISH "patio/motos"

WiFiClient espClient;
PubSubClient MQTT(espClient);
char buffer[256];

void initWiFi()
{
  WiFi.begin((char *)SSID, PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconectaWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Reconectando Wi-Fi...");
    initWiFi();
  }
}

void initMQTT()
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  while (!MQTT.connected())
  {
    Serial.println("Conectando ao Broker MQTT...");
    if (MQTT.connect(moduleID, mqttUser, mqttPassword))
    {
      Serial.println("Conectado ao Broker!");
    }
    else
    {
      Serial.print("Falha na conexão. Estado: ");
      Serial.println(MQTT.state());
      delay(2000);
    }
  }
}

void verificaConexoesWiFiEMQTT()
{
  reconectaWiFi();
  if (!MQTT.connected())
  {
    initMQTT();
  }
  MQTT.loop();
}

void publicaLocalizacao()
{
  StaticJsonDocument<256> doc;

  doc["id"] = ID;
  doc["Modulo"] = moduleID;

  float latitude = -23.5505 + random(-100, 100) / 10000.0; // variação de até ±0.01
  float longitude = -46.6333 + random(-100, 100) / 10000.0;

  doc["lat"] = latitude;
  doc["lng"] = longitude;

  serializeJson(doc, buffer);
  MQTT.publish(TOPICO_PUBLISH, buffer);

  Serial.println("Localização publicada:");
  Serial.println(buffer);
}

void piscaLed()
{
  digitalWrite(boardLED, HIGH);
  delay(300);
  digitalWrite(boardLED, LOW);
}

void setup()
{
  Serial.begin(115200);
  pinMode(boardLED, OUTPUT);
  digitalWrite(boardLED, LOW);

  initWiFi();
  initMQTT();
}

void loop()
{
  verificaConexoesWiFiEMQTT();

  publicaLocalizacao();
  piscaLed();

  delay(10000);
}
