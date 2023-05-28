# Envio e recebimento de dados via MQTT com uso do TagoIO

## Descrição

- O projeto vai abordar o envio de dados via protocolo MQTT para uma aplicação na nuvem e execução de ações a partir dos valores medidos;
- Os itens utilizados serão modulo ESP-32 e sensores de temperatura, umidade e luminosidade, além de LEDs;
- Os módulos estarão conectados a rede via Wi-Fi, pelo fato de ser o protocolo mais acessível no momento. Em casos práticos, talvez fosse melhor empregar uma outra tecnologia com menor consumo.

## Objetivos

- Desenvolver o firmware do primeiro ESP32 para interpretar as medidas dos sensores e enviar os dados para a plataforma TagoIO;
- Elaborar o firmware do segundo ESP32 para receber as ações da plataforma TagoIO, e ligar os LEDs, que representarão: ar-condicionado, aquecedor, luzes e umidificador de ar;
- Criar um dashboard no TagoIO para mostrar os valores medidos pelos sensores e possuir um registro das medidas ao longo do tempo;

## Estrutura do sistema

![Estrutura do Sistema](./images/system_structure.png 'Estrutura do Sistema')
