#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

void definirCorLED(byte v, byte g, byte a);
int lerUltrassonic(byte echo, byte trigger);

WiFiClientSecure redeSegura;
PubSubClient mqtt_cliente(redeSegura);

const byte trigger_1 = 22;
const byte echo_1 = 23;

const byte trigger_2 = 12;
const byte echo_2 = 13;

const byte led_r = 14;
const byte led_g = 26;
const byte led_b = 25;

int dist_1 = 0;
int dist_2 = 0;
bool est_ant_1 = false;
bool est_ant_2 = false;

void setup() {
  Serial.begin(115200);

  ledcAttach(led_r, 5000, 8);
  ledcAttach(led_g, 5000, 8);
  ledcAttach(led_b, 5000, 8);
  definirCorLED(255, 0, 0);

  redeSegura.setInsecure();

  pinMode(trigger_1, OUTPUT);
  pinMode(echo_1, INPUT);
  pinMode(trigger_2, OUTPUT);
  pinMode(echo_2, INPUT);

  WiFi.begin(SSID, PASS);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    definirCorLED(0, 0, 255);
    delay(300);
  }
  Serial.println("\nWiFi OK!");
  definirCorLED(0, 255, 0);

  mqtt_cliente.setServer(BROKER_URL, BROKER_PORT);
  conectarMqtt();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconectarWiFi();
  }

  if (!mqtt_cliente.connected()) {
    conectarMqtt();
  }

  mqtt_cliente.loop();

  dist_1 = lerUltrassonic(echo_1, trigger_1);
  dist_2 = lerUltrassonic(echo_2, trigger_2);

  bool pres_1 = (dist_1 > 0 && dist_1 < 30);
  if (pres_1 != est_ant_1) {
    const char* mensagem = pres_1 ? "1" : "0";
    mqtt_cliente.publish(TOPIC5, mensagem);
    Serial.print("Presenca 1: "); Serial.println(mensagem);
    est_ant_1 = pres_1;
  }

  bool pres_2 = (dist_2 > 0 && dist_2 < 30);
  if (pres_2 != est_ant_2) {
    const char* mensagem = pres_2 ? "1" : "0";
    mqtt_cliente.publish(TOPIC6, mensagem);
    Serial.print("Presenca 2: "); Serial.println(mensagem);
    est_ant_2 = pres_2;
  }

  if (pres_1 || pres_2) {
    definirCorLED(255, 255, 0);
  } else {
    definirCorLED(0, 255, 0);
  }

  Serial.println("---");
  Serial.print("Dist 1: "); Serial.print(dist_1); Serial.println(" cm");
  Serial.print("Dist 2: "); Serial.print(dist_2); Serial.println(" cm");
  Serial.println("---");

  delay(1000);
}

void conectarMqtt() {
  Serial.print("Conectando MQTT...");
  String id_unico = "S2-" + String(random(0xffff), HEX);
  while (!mqtt_cliente.connected()) {
    if (mqtt_cliente.connect(id_unico.c_str(), BROKER_USER_NAME, BROKER_USER_PASS)) {
      Serial.println("\nMQTT OK!");
      definirCorLED(0, 255, 0);
    } else {
      Serial.print(".");
      delay(500);
    }
  }
}

void reconectarWiFi() {
  Serial.println("Reconectando WiFi...");
  WiFi.disconnect();
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi Reconectado!");
}

void definirCorLED(byte v, byte g, byte a) {
  ledcWrite(led_r, v);
  ledcWrite(led_g, g);
  ledcWrite(led_b, a);
}

int lerUltrassonic(byte echo, byte trigger) {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  unsigned long tempo = pulseIn(echo, HIGH, 30000);
  if (tempo == 0) return -1;
  return (tempo * 0.0343) / 2;
}
