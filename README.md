# ProjetoIOT-Minitrilhos

Sistema IoT para monitoramento e controle de mini trilhos utilizando ESP32 com sensores e comunicação MQTT.

## Estrutura do Projeto

- **S1**: Sensor de temperatura, umidade e luminosidade com detecção de presença
- **S2**: Duplo sensor ultrassônico para detecção de presença em múltiplas regiões
- **S3**: Receptor de sinais MQTT para controle de dispositivos
- **trem**: Controle de motores e direcionamento via MQTT

## Características

- Conectividade WiFi segura
- Comunicação MQTT com HiveMQ Cloud
- Sensores de temperatura, umidade e luminosidade
- Sensores ultrassônicos para detecção de presença
- LED RGB para indicação de status
- Controle remoto via MQTT

## Hardware Necessário

- ESP32
- Sensor DHT11
- Sensor LDR
- Sensores Ultrassônicos HC-SR04
- LED RGB
- Jumpers e componentes adicionais

## Instalação

1. Configure as credenciais WiFi e MQTT no arquivo `env.h`
2. Carregue os sketches Arduino em cada ESP32
3. Verifique as conexões dos sensores

## Uso

Os sensores e atuadores se comunicam automaticamente através do broker MQTT após a inicialização.

## Licença

GNU General Public License v2
