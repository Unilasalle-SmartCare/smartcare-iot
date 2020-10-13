
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <Ultrasonic.h>
#include <MQTTClient.h>
#include<ctype.h>

void setup_wifi();
void reconnect();
void publicar(const char* topic, String mensagem, boolean retained);
void on_message(char* topic, byte* payload, unsigned int length);
void verificaIdosoDeitado();
void  verificaUltrasonico();
void  verificaPresenca();

const char* ssid = "NET-WIFI-907";
const char* password = "tekbond793";
WiFiClient espClient;

// long lastMsg = 0;
// int value = 0;
char msg[50];
const char* mqtt_server = "ec2-18-209-178-192.compute-1.amazonaws.com";
PubSubClient client(espClient);

String clientId = "D1 mini";
String clientUser = "smartcare";
String clientPass = "unilasalle";
String strClienteId(clientId);

#define pinoPIRQuarto D7 // D7 PIR do quarto
#define pinoButtonQuarto   D8 // D8 BOTAO
#define pinoLedVermelho  D2 // D2 LED VERMELHO - INDICA O BOTÃO
#define pinoLedAmarelo  D4 // D4 LED AMARELO - INDICA O SENSOR ULTRASÔNICO
#define pinoLedQuarto  D1 // D1 LED BRANCO - INDICA O SENSOR DE PRESENÇA
#define pinoTriggerQuarto D5 // D5
#define pinoEchoQuarto D6 // D6

Ultrasonic ultrasonic(pinoTriggerQuarto, pinoEchoQuarto, 30000); // (Trig PIN,Echo PIN)
int statusPIRQuarto; // Guarda o valor do sensor de presença

void setup() {  
  pinMode(pinoPIRQuarto, INPUT);  
  pinMode(pinoButtonQuarto, INPUT);
  pinMode(pinoLedQuarto, OUTPUT);
  pinMode(pinoLedVermelho, OUTPUT);
  pinMode(pinoLedAmarelo, OUTPUT);  
  
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(on_message);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  verificaIdosoDeitado();
  verificaUltrasonico();  
  verificaPresenca();

  client.loop();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

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
      client.subscribe("home/quarto/atuadores/#");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publicar(const char* topic, String mensagem, boolean retained)
{ 
  // Now we can start to publish the message
  client.beginPublish(topic, mensagem.length(), false);
  client.print(mensagem);
  // Now we're done!
  client.endPublish();
}

void on_message(char* topic, byte* payload, unsigned int length) 
{
  char msg[length];
  Serial.print("Message arrived on topic: ");  
  Serial.print("[ ");
  Serial.print(topic);
  Serial.print(" ] = ");
  for (int i = 0; i < (int)length; i++) {
    msg[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  } 
  Serial.println(' ');

  String msgStr(msg);
  // String topicStr(topic);

  if ((strncmp(topic, "home/quarto/sensor/BTN-01", strlen(topic)) == 0) && msg[0] == '1')
  {
    Serial.println("Dormindo.");    
    digitalWrite(pinoLedVermelho, HIGH);
  }
  else if ((strncmp(topic, "home/quarto/sensor/BTN-01", strlen(topic)) == 0) && msg[0] == '0')
  {
    Serial.println("Acordou.");  
    digitalWrite(pinoLedVermelho, LOW);
  }
  else if ((strncmp(topic, "home/quarto/sensor/UIR-11", strlen(topic)) == 0))
  {    
    Serial.print("Idoso passou à ");  Serial.println(msgStr);
    digitalWrite(pinoLedAmarelo, HIGH);
    digitalWrite(pinoLedAmarelo, LOW);
        
  }  
  else if ((strncmp(topic, "home/quarto/sensor/PIR-01", strlen(topic)) == 0) && msg[0] == '1')
  {
    Serial.println("Tem gente.");
    digitalWrite(pinoLedQuarto, HIGH);
  }
  else if ((strncmp(topic, "home/quarto/sensor/PIR-01", strlen(topic)) == 0) && msg[0] == '0')
  {
    Serial.println("Ta vazio.");
    digitalWrite(pinoLedQuarto, LOW);
  }
  else
  {
    Serial.println("Tópico não identificado.");
  }
}

void verificaPresenca(){
  int sensorVal = digitalRead(pinoPIRQuarto);
  if (sensorVal == HIGH) {      
    client.publish_P("home/quarto/sensor/PIR-01", "1", true);    
  } else {
    client.publish_P("home/quarto/sensor/PIR-01", "0", true);
  }
}

void verificaUltrasonico() {  
  float distancia = ultrasonic.Ranging(CM); 
  char _dist[5]  = "";
  sprintf(_dist, "%.f", distancia); 
  client.publish_P("home/quarto/sensor/UIR-11", _dist, true);
}

void verificaIdosoDeitado(){
  int sensorVal = digitalRead(pinoButtonQuarto);  
  if (sensorVal == HIGH) {    
    client.publish_P("home/quarto/sensor/BTN-01", "0", true);
  } else if(sensorVal == LOW){
    client.publish_P("home/quarto/sensor/BTN-01", "1", true);
  }
}