#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure conexao_segura;
PubSubClient mqtt(conexao_segura);

void setup() {
  Serial.begin(115200);
  conexao_segura.setInsecure();
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Wifi Conectado!");

  mqtt.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Iniciando Broker");
  String id = "S3-";
  id += String(random(0xffff), HEX);

  while (!mqtt.connected()){
    mqtt.connect(id.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
    Serial.print(".");
    delay(200);
  }
  Serial.println("Broker OK!");
  mqtt.subscribe(TOPIC4);
  mqtt.subscribe(TOPIC3);
  mqtt.subscribe(TOPIC5);
  mqtt.subscribe(TOPIC6);
  mqtt.setCallback(processar_callback);
  pinMode(2, OUTPUT);
}

void loop() {
  String dado = "";
  if (Serial.available() > 0) {
    dado = Serial.readStringUntil('\n');
    Serial.print("Digite: ");
    Serial.println(dado);
    mqtt.publish("Bezao", dado.c_str());
  }
  mqtt.loop();
}

void processar_callback(char* topico, byte* payload, unsigned long tamanho){
  String msg_final = "";
  for(int i = 0; i < tamanho; i++){
    msg_final += (char) payload[i];
  }
  Serial.println(msg_final);
  if(msg_final == "1") {
    digitalWrite(2, HIGH);
    Serial.println("Ligado!");
  }
  if(msg_final == "0") {
    digitalWrite(2, LOW);
    Serial.println("Desligado!");
  }
}
