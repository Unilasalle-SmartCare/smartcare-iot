
void verificaPresenca(){
  int sensorVal = digitalRead(pinoPIRQuarto);
  if (sensorVal == HIGH) {      
    client.publish_P("home/quarto/sensor/PIR-1", "1", true);    
  } else {
    client.publish_P("home/quarto/sensor/PIR-1", "0", true);
  }
}

void verificaUltrasonico() {  
  float distancia = ultrasonic.Ranging(CM); 
  
  char _dist[20]  = "";  sprintf(_dist, "%f", ultrasonic.Ranging(CM));   
  
  Serial.print("distancia: ");  Serial.println(distancia);

  Serial.print("_dist: ");  Serial.println(_dist);

  if (distancia < 30)
  {
    client.publish_P("home/quarto/sensor/UIR-1", _dist, true);
  }
}

void verificaIdosoDeitado(){
  estadoButton = digitalRead(pinoButtonQuarto);  // lê o estado do botão - HIGH OU LOW
  
  if (estadoButton == HIGH) {    //Se botão estiver pressionado (HIGH)
    client.publish_P("home/quarto/sensor/BTN-1", "ON", true);    
  } else {
    client.