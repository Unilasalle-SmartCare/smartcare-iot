# smartcare-iot
### Este repositório contém o desenvolvimento do projeto SmartCare Environment - Unilasalle-RJ
   ![Planta baixa com a marcação dos sensores](/img/planta_baixa_sensores.png)

### Componentes do projeto

- Raspberry Pi 3 B+ como Servidor Mqtt-Broker
- Maquina Virtual como C-Mqtt-client
- D1-mini-lite(Esp8266) como Python-Mqtt-client
    - Sensores: 
        - PIR
        - Ultrassônico
        - Peso
    - Atuadores:
        - Leds

### Servidor Mqtt-Broker
- Responsável por receber publicação dados dos sensores em tópicos e enviar esses dados para quem se inscreveu nesses tópicos.

### C-Mqtt-client
- Responsável pela leitura e envio dos dados dos sensores conectados a ele para o Mqtt-Broker, e por escutar os tópicos aos quais se inscreveu e acionar os atuadores caso seja solicitado.

### Maquina Virtual Python-Mqtt-client
- Responsável por escutar todos os tópicos do Mqtt-Broker e salvar os dados no banco de dados
