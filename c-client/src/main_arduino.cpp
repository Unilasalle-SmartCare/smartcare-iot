
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <Ultrasonic.h>
#include <MQTTClient.h>

#include <SPI.h>
#include <Ethernet.h>

#pragma region ethernet
// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10,0,1,20);
EthernetClient ethClient;
#pragma endregion ethernet

// #pragma region wifi

// const char* ssid = "NET-WIFI-907";
// const char* password = "tekbond793";
// WiFiClient espClient;

// #pragma endregion wifi

const char* mqtt_server = "192.168.0.74";
PubSubClient client(ethClient);

String clientId = "D1 mini";
String clientUser = "smartcare";
String clientPass = "unilasalle";
long lastMsg = 0;
char msg[50];
int value = 0;
String strClienteId(clientId);

void publicar(const char* topic, String mensagem, boolean retained);
void on_message(char* topic, byte* payload, unsigned int length);
//void verificaIdosoDeitado();
void  verificaUltrasonico();
void  verificaPresenca();


#define pinoLedAmarelo  2 // LED AMARELO
#define pinoLedVermelho  3 // LED VERMELHO
#define pinoLedQuarto  4 // LED VERDE
#define pinoPIRQuarto 5 // PIR - Sensor de presença
#define pinoButtonQuarto   6 // BOTAO
#define pinoTriggerQuarto 7 // Ultrasonic->Trigger - Sensor de distância
#define pinoEchoQuarto 8 // Ultrasonic->Echo - Sensor de distância

Ultrasonic ultrasonic(pinoTriggerQuarto, pinoEchoQuarto, 30000); // (Trig PIN,Echo PIN)
int statusPIRQuarto; //Variavel para guardar valor do sensor do quarto

void setup_ethernet() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(Ethernet.gatewayIP());

  Ethernet.begin(mac);

//   while (Ethernet.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), clientUser.c_str(), clientPass.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("home/quarto/dispositivo", clientId.c_str());
      // ... and resubscribe
      client.subscribe("home/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {  

  pinMode(pinoPIRQuarto, INPUT); //Define pino do sensor de presença como entrada  
  //pinMode(pinoButtonQuarto, INPUT);
  pinMode(pinoLedQuarto, OUTPUT); //DEFINE O PINO do led COMO SAÍDA
  //pinMode(pinoLedVermelho, OUTPUT); //DEFINE O PINO do led COMO SAÍDA
  pinMode(pinoLedAmarelo, OUTPUT); //DEFINE O PINO do led COMO SAÍDA  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(9600);
  setup_ethernet();
  client.setServer(mqtt_server, 1883);
  client.setCallback(on_message);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  //verificaIdosoDeitado();
  verificaUltrasonico();  
  verificaPresenca();

  client.loop();
}

void publicar(const char* topic, String mensagem, boolean retained)
{ 
    // Now we can start to publish the message
    client.beginPublish(topic, mensagem.length(), false);
    client.print(mensagem);
    // Now we're done!
    client.endPublish();
}

void on_message(char* topic, byte* payload, unsigned int length) {

  
  char msg[length];
  Serial.print("Message arrived on topic: ");  
  Serial.print("[ ");
  Serial.print(topic);
  Serial.print(" ] = ");
 
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }  
  Serial.println(' ');  delay(200);

  String msgStr(msg);
  String topicStr(topic);

  if ((strncmp(topic, "home/quarto/sensor/BTN-1", strlen(topic)) == 0) && msg[0] == '1')
  {
    Serial.println("Dormindo.");    
    //digitalWrite(pinoLedVermelho, HIGH);
  }
  else if ((strncmp(topic, "home/quarto/sensor/BTN-1", strlen(topic)) == 0) && msg[0] == '0')
  {
    Serial.println("Acordou.");  
    //digitalWrite(pinoLedVermelho, LOW);
  }
  else if (strncmp(topic, "home/quarto/sensor/UIR-1", strlen(topic)) == 0)
  {    
    Serial.print("porta: ");
    Serial.println(msg);
    digitalWrite(pinoLedAmarelo, HIGH);
    delay(200);    
    digitalWrite(pinoLedAmarelo, LOW);
  }
  else if ((strncmp(topic, "home/quarto/sensor/PIR-1", strlen(topic)) == 0) && msg[0] == '1')
  {
    Serial.println("Tem gente.");
    digitalWrite(pinoLedQuarto, HIGH);
  }
  else if ((strncmp(topic, "home/quarto/sensor/PIR-1", strlen(topic)) == 0) && msg[0] == '0')
  {
    Serial.println("Ta vazio.");
    digitalWrite(pinoLedQuarto, LOW);
  }
  else
  {
    Serial.println("Tópico não identificado.");
    delay(1000);
  }
}

void verificaPresenca(){
  int sensorVal = digitalRead(pinoPIRQuarto);
  // 
  // char _val[4]  = "";
  // sprintf(_val, "%d", sensorVal);  
  // client.publish_P("home/quarto/sensor/PIR-1", _val, true);
  // delay(200);
  if (sensorVal == HIGH) {      
    client.publish_P("home/quarto/sensor/PIR-1", "1", true);
    delay(200);
  } else {
    client.publish_P("home/quarto/sensor/PIR-1", "0", true);
    delay(200);
  }
}

void verificaUltrasonico() {  
  float distancia = ultrasonic.Ranging(CM); 
  char _dist[20]  = "";
  sprintf(_dist, "%.4f", distancia);  
  if (distancia < 30)
  {
    //digitalWrite(pinoLedAmarelo, HIGH);
    client.publish_P("home/quarto/sensor/UIR-1", _dist, true);
  } else {
    //digitalWrite(pinoLedAmarelo, LOW);
  }  
}

// void verificaIdosoDeitado(){
//   int sensorVal = digitalRead(pinoButtonQuarto);  
//   if (sensorVal == HIGH) {
//     digitalWrite(pinoLedVermelho, LOW);
//     Serial.println("O idoso esta deitado");
//   } else {
//     digitalWrite(pinoLedVermelho, HIGH);
//     client.publish("home/quarto/sensor/BTN-1", "1");
//     Serial.println("O idoso nao esta deitado");
//   }
// }