
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(8, 9); // CE, CSN

const int light = A5;
const int forward = 5;
const int backward = 3;
const int delayAmount = 50;
const int cycles = 50;
int count = 0;
bool lightState = false;
const uint64_t pipe = 0xE6E6E6E6E6E6;
int ReceivedMessage[1] = {000}; // Used to store value received by the NRF24L01

void toggleLight(){
  if (count >= cycles){
    lightState = !lightState;
    if (lightState){
      analogWrite(light, 255);
      Serial.print("Turning Light On");
    }
    else{
      analogWrite(light, 0);
      Serial.print("Turning Light Off");
    }
    Serial.println();
    count = 0;
  }
  count++;
}

void goForward(){
  Serial.print("Forward");
  digitalWrite(backward, LOW);
  digitalWrite(forward, HIGH);
}

void goBackward(){
  Serial.print("Reverse");
  digitalWrite(forward, LOW);
  digitalWrite(backward, HIGH);
}

void stopGoing(){
  Serial.print("Stop");
  digitalWrite(forward, LOW);
  digitalWrite(backward, LOW);
}

void readFromRadio(){
  while(!radio.available());
  char text[32] = "";
  radio.read(ReceivedMessage, 1);
  if (ReceivedMessage[0] == 100){
    goForward();
  }
  else if (ReceivedMessage[0] == 010){
    goBackward();
  }
  else{
    stopGoing();
  }
}

void setup() {
  // put your setup code here, to run once:
  count = 0;
  pinMode(light, OUTPUT);
  pinMode(backward, OUTPUT);
  pinMode(forward, OUTPUT);
  analogWrite(light, 0);
  digitalWrite(backward, LOW);
  digitalWrite(forward, LOW);
  lightState = false;
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  // radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  toggleLight();

  while(radio.available())
  {
    readFromRadio();
    delay(delayAmount);
  }
  

  delay(delayAmount);
}
