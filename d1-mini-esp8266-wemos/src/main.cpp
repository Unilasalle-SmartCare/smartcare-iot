
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>
#include <string.h>

#include <MQTTClient.h>
// Update these with values suitable for your network.

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.0.74";

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "D1 mini";
String clientUser = "smartcare";
String clientPass = "unilasalle";
long lastMsg = 0;
char msg[50];
int value = 0;


void publicar(const char* topic, String mensagem, boolean retained);
void on_message(char* topic, byte* payload, unsigned int length);
void verificaIdosoDeitado();
void  verificaUltrasonico();
void  verificaPresenca();

#define pinoPIRQuarto 13 // D7 PIR do quarto
int statusPIRQuarto; //Variavel para guardar valor do sensor do quarto

#define pinoTriggerQuarto 14 // D5
#define pinoEchoQuarto 12 // D6
Ultrasonic ultrasonic(pinoTriggerQuarto, pinoEchoQuarto);
int distance;

#define pinoButtonQuarto   15 // D8 BOTAO
#define pinoLedVermelho  4 // D2 LED VERMELHO
#define pinoLedAmarelo  2 // D4 LED AMARELO
#define pinoLedQuarto  5 // D1 LED BRANCO

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
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("quarto/#");
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
  pinMode(pinoButtonQuarto, INPUT);
  pinMode(pinoLedQuarto, OUTPUT); //DEFINE O PINO do led COMO SAÍDA
  pinMode(pinoLedVermelho, OUTPUT); //DEFINE O PINO do led COMO SAÍDA
  pinMode(pinoLedAmarelo, OUTPUT); //DEFINE O PINO do led COMO SAÍDA  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }

  Serial.println();
  String msgStr(msg);
  // Find out how many bottles we should generate lyrics for
  String topicStr(topic);

  if (topicStr.indexOf('/') >= 0) {
    // The topic includes a '/', we'll try to read the number of bottles from just after that
    topicStr.remove(0, topicStr.indexOf('/')+1);
    // Now see if there's a number of bottles after the '/'
  }  
  
  int topicValue;
  if (topicStr.compareTo("quarto/cama"))
  {
    topicValue = 1;
  }
  else
  {
    if (topicStr.compareTo("quarto/porta"))
    {
      topicValue = 2;
    }
    else
    {
      if (topicStr.compareTo("quarto/teto"))
      {
        topicValue = 3;
      }
      else
      {
        Serial.println("Tópico não identificado.");
      }
    }
  }
  
  switch (topicValue) {
  case 1:  //cama

    break;
  case 2: //porta
    
    break;

  case 3: //teto

    break;
    
  default:      
    break;
  };
}

void verificaUltrasonico() {  
  float distancia = ultrasonic.read(); 
  Serial.println(distancia);

  if (distancia < 20)
  {
    digitalWrite(pinoLedAmarelo, HIGH);
    client.publish("home/quarto/sensor/UIR-1", "1");
  } else {
    digitalWrite(pinoLedAmarelo, LOW);
  }  
}

void verificaIdosoDeitado(){
  int sensorVal = digitalRead(pinoButtonQuarto);  
  if (sensorVal == HIGH) {
    digitalWrite(pinoLedVermelho, LOW);
    Serial.println("O idoso esta deitado");
  } else {
    digitalWrite(pinoLedVermelho, HIGH);
    client.publish("home/quarto/sensor/BTN-1", "1");
    Serial.println("O idoso nao esta deitado");
  }
}

void verificaPresenca(){
  int sensorVal = digitalRead(pinoPIRQuarto);  
  if (sensorVal == HIGH) {
    digitalWrite(pinoLedQuarto, HIGH);    
    client.publish("home/quarto/sensor/PIR-1", "1");
    //Serial.println("Tem alguem no quarto");
  } else {
    digitalWrite(pinoLedQuarto, LOW);
    //Serial.println("O quarto esta vazio");
  }
}