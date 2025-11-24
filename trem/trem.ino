#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include "env.h"

WiFiClientSecure conexao_wifi;
PubSubClient mqtt_broker(conexao_wifi);

const int led_esquerda = 18;
const int led_direita = 19;

void setup() {
  Serial.begin(115200);
  conexao_wifi.setInsecure();  
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Wifi OK");

  mqtt_broker.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Iniciando Broker");
  String identificador = "Motor-";
  identificador += String(random(0xffff), HEX);

  while (!mqtt_broker.connected()) {
    mqtt_broker.connect(identificador.c_str(), BROKER_USER_NAME, BROKER_USER_PASS); 
    Serial.print(".");
    delay(200);
  }
  Serial.println("Broker OK!");
  mqtt_broker.subscribe(TOPIC11);
  mqtt_broker.setCallback(receber_callback);
  pinMode(led_esquerda, OUTPUT);
  pinMode(led_direita, OUTPUT);
  digitalWrite(led_esquerda, LOW);
  digitalWrite(led_direita, LOW);
}

void loop() {
  String entrada = "";
  if (Serial.available() > 0) {
    entrada = Serial.readStringUntil('\n');
    Serial.print("Digite: ");
    Serial.println(entrada);
    mqtt_broker.publish("bezinho", entrada.c_str());
  }
  mqtt_broker.loop();
}

void receber_callback(char* topic, byte* payload, unsigned long length){
  String valor_recebido = "";
  for(int i = 0; i < length; i++){
    valor_recebido += (char) payload[i];
  }
  valor_recebido.trim();
  Serial.println(valor_recebido);
  
  int velocidade_motor = valor_recebido.toInt();

  Serial.print("Velocidade: ");
  Serial.println(velocidade_motor);

  if (velocidade_motor == 0) {
    digitalWrite(led_esquerda, LOW);
    digitalWrite(led_direita, LOW);
    Serial.println("Parado");
  } else if (velocidade_motor > 0) {
    digitalWrite(led_esquerda, HIGH);
    digitalWrite(led_direita, LOW);
    Serial.println("Frente");
  } else {
    digitalWrite(led_esquerda, LOW);
    digitalWrite(led_direita, HIGH);
    Serial.println("Tras");
  }
}
