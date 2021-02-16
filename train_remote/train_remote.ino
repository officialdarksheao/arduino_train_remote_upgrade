#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(8, 7); // CE, CSN

const int blueButton = 4;
const int redButton = 5;

const int blueLight = A4;
const int redLight = A5;

const int countBetweenSends = 100;
int count = 0;

bool shouldGo = false;
bool previousShouldGo = false;
bool redPrevious = false;
bool bluePrevious = false;

const int valueForGo = 150;
int currentGoValue = 0;
int previousGoValue = 0;
bool shouldReverse = false;

const uint64_t pipe = 0xE6E6E6E6E6E6;
int SentMessage[1] = {000};

void send(int SentMessage[1]){
  analogWrite(redLight, 200);
  Serial.print("Sending: ");
  radio.write(SentMessage, 1);
  char str[10];
  int index = 0;
  for (int i=0; i<3;i++){
    index += sprintf(&str[index], "%d", SentMessage[i]);
  }
  Serial.print(str);
  Serial.print("Done");
  Serial.println();
  analogWrite(redLight, 0);
}

void trySend(int SentMessage[1]){
  if (count >= countBetweenSends){
    send(SentMessage);
    count = 0;
  }
}

void setup() {
  count = 0;
  
  radio.begin();
  radio.openWritingPipe(pipe);
  //radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();


  // blue
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(blueLight, OUTPUT);
 
  // red
  pinMode(redButton, INPUT_PULLUP);
  pinMode(redLight, OUTPUT);

  analogWrite(redLight, 0);
  analogWrite(blueLight, 0);

  Serial.begin(9600);
}
void loop() {
  count++;
  bool redPressed = digitalRead(redButton) == LOW;
  bool bluePressed = digitalRead(blueButton) == LOW;

  if (bluePrevious != bluePressed) {
    // state of the blue button has changed since last loop
    shouldGo = bluePressed;
    Serial.print("Blue Pressed");
    Serial.println();
    count = countBetweenSends;
    bluePrevious = bluePressed;
  }
  
  if (redPrevious != redPressed){
    // state of the button has changed since last loop
    shouldReverse = redPressed;
    if (shouldGo && redPressed){
      shouldGo = false;
      // stop trying to go forward
    }
    Serial.print("Red Pressed");
    Serial.println();
    count = countBetweenSends;
    
    redPrevious = redPressed;
  }


  if (previousShouldGo != shouldGo){
    if (shouldGo) {
      currentGoValue = 250;
    }
    else{
      currentGoValue = 0;
    }
    previousShouldGo = shouldGo;
  }

  if (currentGoValue > valueForGo){
    currentGoValue--;
  }

  if (currentGoValue != previousGoValue){
    analogWrite(blueLight, currentGoValue);
    previousGoValue = currentGoValue;
  }
  
  if (shouldGo){
    SentMessage[0] = 010;
  }
  else if (shouldReverse) {
    SentMessage[0] = 100;
  }
  else{
    SentMessage[0] = 000;
  }
  
  trySend(SentMessage);
  delay(20);
}
