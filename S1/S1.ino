#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "env.h"

void definirCorLED(byte vermelho, byte verde, byte azul);
int obterDistanciaUltrassonica(byte pino_echo, byte pino_trigger);

#define SENSOR_LDR 34
#define SENSOR_DHT 4
#define PINO_TRIGGER 22
#define PINO_ECHO 23

const byte pino_vermelho = 14;
const byte pino_verde = 26;
const byte pino_azul = 25;
#define TIPO_DHT DHT11

DHT sensorDHT(SENSOR_DHT, TIPO_DHT);
int dist = 0;
bool presenca_anterior = false;

WiFiClientSecure cliente_wifi;
PubSubClient broker(cliente_wifi);

void conectar_wifi() {
  Serial.print("Conectando-se: ");
  Serial.println(SSID);
  definirCorLED(0, 0, 255);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  definirCorLED(0, 255, 0);
}

void reconectar_broker() {
  while (!broker.connected()) {
    Serial.print("Conectando broker... ");
    if (broker.connect("ESP32_S1", BROKER_USER_NAME, BROKER_USER_PASS)) {
      Serial.println("OK!");
      definirCorLED(0, 255, 0);
    } else {
      Serial.print("Erro(");
      Serial.print(broker.state());
      Serial.println("). Tentando em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_LDR, INPUT);
  sensorDHT.begin();
  pinMode(PINO_TRIGGER, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  ledcAttach(pino_vermelho, 5000, 8);
  ledcAttach(pino_verde, 5000, 8);
  ledcAttach(pino_azul, 5000, 8);
  definirCorLED(255, 0, 0);

  conectar_wifi();

  cliente_wifi.setInsecure();

  broker.setServer(BROKER_URL, BROKER_PORT);
}

void loop() {
  if (!broker.connected()) {
    reconectar_broker();
  }
  broker.loop();

  int luz = analogRead(SENSOR_LDR);
  float temp = sensorDHT.readTemperature();
  float umid = sensorDHT.readHumidity();

  char msg_luminosidade[10];
  sprintf(msg_luminosidade, "%d", luz);
  broker.publish(TOPIC3, msg_luminosidade);

  char msg_temperatura[10];
  dtostrf(temp, 4, 1, msg_temperatura);
  broker.publish(TOPIC1, msg_temperatura);

  char msg_umidade[10];
  dtostrf(umid, 4, 1, msg_umidade);
  broker.publish(TOPIC2, msg_umidade);

  Serial.println("Enviando dados:");
  Serial.print("Luz: "); Serial.println(luz);
  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Umid: "); Serial.println(umid);
  Serial.println("---");

  dist = obterDistanciaUltrassonica(PINO_ECHO, PINO_TRIGGER);
  if (dist > 0) {
    Serial.print("Distancia: ");
    Serial.print(dist);
    Serial.println(" cm");
  }

  bool presenca_atual = (dist > 0 && dist < 30);

  if (presenca_atual != presenca_anterior) {
    if (presenca_atual) {
      broker.publish(TOPIC4, "1");
      Serial.println("Presenca detectada!");
      definirCorLED(255, 255, 0);
    } else {
      broker.publish(TOPIC4, "0");
      Serial.println("Presenca finalizada!");
      definirCorLED(0, 255, 0);
    }
    presenca_anterior = presenca_atual;
  }

  delay(5000);
}

int obterDistanciaUltrassonica(byte echo, byte trigger) {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  unsigned long duracao = pulseIn(echo, HIGH, 30000);
  if (duracao == 0) return -1;
  return (duracao * 0.0343) / 2;
}

void definirCorLED(byte vermelho, byte verde, byte azul) {
  ledcWrite(pino_vermelho, vermelho);
  ledcWrite(pino_verde, verde);
  ledcWrite(pino_azul, azul);
}
