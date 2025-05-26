#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define boardLED 2

// Wi-Fi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// MQTT Broker
const char *BROKER_MQTT = "172.190.149.52";
const int BROKER_PORT = 1883;
const char *mqttUser = "admin_fiap";
const char *mqttPassword = "admin_fiap123";

#define TOPICO_PUBLISH "patio/motos"

// MQTT / WiFi
WiFiClient espClient;
PubSubClient MQTT(espClient);
char buffer[256];

// GPS (via Serial2: RX=16, TX=17)
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

void initWiFi()
{
  WiFi.begin(SSID, PASSWORD);
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

void initMQTT()
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  while (!MQTT.connected())
  {
    Serial.println("Conectando ao Broker MQTT...");
    if (MQTT.connect("ESP32", mqttUser, mqttPassword))
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
  if (WiFi.status() != WL_CONNECTED)
    initWiFi();
  if (!MQTT.connected())
    initMQTT();
  MQTT.loop();
}

void publicaLocalizacao()
{
  while (SerialGPS.available() > 0)
    gps.encode(SerialGPS.read());

  if (gps.location.isValid())
  {
    StaticJsonDocument<128> doc;
    doc["lat"] = gps.location.lat();
    doc["lng"] = gps.location.lng();

    serializeJson(doc, buffer);
    MQTT.publish(TOPICO_PUBLISH, buffer);

    Serial.println("Localização publicada:");
    Serial.println(buffer);
  }
  else
  {
    Serial.println("Aguardando dados válidos do GPS...");
  }
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

  SerialGPS.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (ajuste conforme seu circuito real)
}

void loop()
{
  verificaConexoesWiFiEMQTT();
  publicaLocalizacao();
  piscaLed();
  delay(10000);
}
