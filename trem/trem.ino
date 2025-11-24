#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include "env.h" // Credenciais separadas por segurança

// Criação do cliente WiFi seguro
WiFiClientSecure conexao_wifi;

// Cliente MQTT usando a conexão segura
PubSubClient mqtt_broker(conexao_wifi);

// Definição dos LEDs (simulando o motor)
const int led_esquerda = 18;  
const int led_direita = 19;

void setup() {
  Serial.begin(115200);

  // A conexão será insegura para evitar problemas com certificados do HiveMQ Cloud
  conexao_wifi.setInsecure();  

  // Conexão com a rede Wi-Fi
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nWiFi conectado!");

  // Conexão com o broker MQTT
  mqtt_broker.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Conectando ao Broker...");

  // Gerando um ID aleatório para evitar conflitos
  String identificador = "Motor-";
  identificador += String(random(0xffff), HEX);

  // Loop até conectar
  while (!mqtt_broker.connected()) {
    mqtt_broker.connect(identificador.c_str(), BROKER_USER_NAME, BROKER_USER_PASS); 
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nBroker MQTT conectado!");

  // Inscrevendo no tópico que controla o motor
  mqtt_broker.subscribe(TOPIC11);

  // Define a função que será chamada quando chegar uma mensagem
  mqtt_broker.setCallback(receber_callback);

  // Configuração dos LEDs (motor simulado)
  pinMode(led_esquerda, OUTPUT);
  pinMode(led_direita, OUTPUT);
  digitalWrite(led_esquerda, LOW);
  digitalWrite(led_direita, LOW);
}

void loop() {
  // Envia algo para o MQTT se você digitar no Serial Monitor
  String entrada = "";
  if (Serial.available() > 0) {
    entrada = Serial.readStringUntil('\n');
    Serial.print("Enviado: ");
    Serial.println(entrada);

    // Publica no tópico bezinho
    mqtt_broker.publish("bezinho", entrada.c_str());
  }

  // Mantém a conexão MQTT ativa
  mqtt_broker.loop();
}

// Função chamada sempre que chega uma mensagem no tópico inscrito
void receber_callback(char* topic, byte* payload, unsigned long length){
  String valor_recebido = "";

  // Converte o payload recebido em texto
  for(int i = 0; i < length; i++){
    valor_recebido += (char) payload[i];
  }
  valor_recebido.trim();

  Serial.println("Valor recebido: " + valor_recebido);

  // Converte o texto em número (velocidade)
  int velocidade_motor = valor_recebido.toInt();

  Serial.print("Velocidade interpretada: ");
  Serial.println(velocidade_motor);

  // LÓGICA DO MOTOR (simulado)
  if (velocidade_motor == 0) {
    // Parado
    digitalWrite(led_esquerda, LOW);
    digitalWrite(led_direita, LOW);
    Serial.println("Motor parado");

  } else if (velocidade_motor > 0) {
    // Frente
    digitalWrite(led_esquerda, HIGH);
    digitalWrite(led_direita, LOW);
    Serial.println("Motor para frente");

  } else {
    // Trás
    digitalWrite(led_esquerda, LOW);
    digitalWrite(led_direita, HIGH);
    Serial.println("Motor para trás");
  }
}
