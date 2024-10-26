/* Baseado no teste do sensor DHT11 
 * Por: Daniel Quadros - 31/07/18 
 * https://dqsoft.blogspot.com/2018/08/sensor-de-umidade-e-temperatura-dht11.html
*/
static const int DHTPIN = PA5;  
static const int timeout = 200; 
static const int LED = PB6;

// Resposta do sensor  
typedef struct  
{  
  byte umidInt;  
  byte umidDec;  
  byte tempInt;  
  byte tempDec;  
  byte checksum;  
} RESPOSTA;  
  
// Iniciação  
void setup() {  
  pinMode (DHTPIN, INPUT);
  pinMode (LED, OUTPUT);

  // Conecta a UART aos pinos ligados à USB
  Serial.setRx(PA_10_R);
  Serial.setTx(PA_9_R);  
  Serial.begin (115200); 

}  
  
// Laço Principal  
void loop() {  
  RESPOSTA resp;  
  delay (5000);   // lê a cada 5 segundos  
  if (leDHT11(&resp)) { 
    Serial.print ("Temperatura ");  
    Serial.print (resp.tempInt);  
    Serial.print (",");  
    Serial.print (resp.tempDec);  
    Serial.print ("C Umidade ");  
    Serial.print (resp.umidInt);  
    Serial.print (",");  
    Serial.print (resp.umidDec);  
    Serial.println ("%");

    if (resp.tempInt>= 30 || resp.umidInt<= 60){
      digitalWrite(LED, HIGH);
    }
    else{
      digitalWrite(LED, LOW);
    }
  }  
  else {  
    Serial.println ("Falha na leitura");  
  }  
}  
  
// Efetua a leitura  
bool leDHT11 (RESPOSTA *pResp) {  
  byte *pDados = (byte *) pResp;  
  byte iByte, iBit;  
  unsigned long to;  
  
  // Solicita leitura  
  pinMode (DHTPIN, OUTPUT);  
  digitalWrite (DHTPIN, LOW);  
  delay(20);    
  digitalWrite (DHTPIN, LOW);  
  
  // Aguarda confirmar  
  pinMode (DHTPIN, INPUT);  
  to = micros() + timeout;  
  while (digitalRead(DHTPIN) == HIGH) {  
    if (micros() > to) {  
      return false;  
    }  
  }  
  while (digitalRead(DHTPIN) == LOW) {  
    if (micros() > to) {  
      return false;  
    }  
  }  
  while (digitalRead(DHTPIN) == HIGH) {  
    if (micros() > to) {  
      return false;  
    }  
  }  
  
  // Le os dados  
  iByte = iBit = 0;  
  while (iByte < 5) {  
    // pulso inicial  
    to = micros() + timeout;  
    while (digitalRead(DHTPIN) == LOW) {  
      if (micros() > to) {  
        return false;  
      }  
    }  
    // valor do bit  
    to = micros() + timeout;  
    while (digitalRead(DHTPIN) == HIGH) {  
      if (micros() > to) {  
        return false;  
      }  
    }  
    pDados[iByte] = pDados[iByte] << 1;  
    if (((micros() + timeout) - to) > 40) {  
      pDados[iByte] |= 1;  
    }  
    // passa para o próximo bit  
    if (++iBit > 7) {  
      iBit = 0;  
      iByte++;  
    }  
  }  
    
  // Confere o checksum  
  return (pDados[0]+pDados[1]+pDados[2]+pDados[3]) == pDados[4];  
}  
