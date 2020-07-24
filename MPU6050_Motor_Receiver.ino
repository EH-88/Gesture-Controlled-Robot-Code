#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);

const byte address[6] = "00001";

struct package
{
  float roll;
  float pitch;
};

int enableM1 = 10;
int input1M1 = 6;
int input2M1 = 5;
int input1M2 = 4;
int input2M2 = 3;
int enableM2 = 9;

typedef struct package Package;
Package data;

void setup() {
  Serial.begin(115200);
  
  pinMode(enableM1, OUTPUT);
  pinMode(input1M1, OUTPUT);
  pinMode(input2M1, OUTPUT);
  pinMode(input1M2, OUTPUT);
  pinMode(input2M2, OUTPUT);
  pinMode(enableM2, OUTPUT);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  Serial.print("Roll: ");
  Serial.print(data.roll);
  Serial.print(" Pitch: ");
  Serial.print(data.pitch);
  Serial.println();

  delay(2);
  
  if(radio.available()){
    while(radio.available()){
      radio.read(&data, sizeof(data));
    }

    if(data.pitch > 30){
      digitalWrite(input1M1, HIGH);
      digitalWrite(input2M1, LOW);
      digitalWrite(input1M2, HIGH);
      digitalWrite(input2M2, LOW);

      if(data.roll > 30){
        analogWrite(enableM1, map(abs(data.pitch), 0, 180, 0,255));
        analogWrite(enableM2, 0);
      }
      else if(data.roll < -30){
        analogWrite(enableM1, 0);
        analogWrite(enableM2, map(abs(data.pitch), 0, 180, 0, 255));
      }
      else{
        analogWrite(enableM1, map(abs(data.pitch), 0, 180, 0, 255));
        analogWrite(enableM2, map(abs(data.pitch), 0, 180, 0, 255));
      }
    }
    else if (data.pitch < -30){
      digitalWrite(input1M1, LOW);
      digitalWrite(input2M1, HIGH);
      digitalWrite(input1M2, LOW);
      digitalWrite(input2M2, HIGH);
      
      if(data.roll > 30){
        analogWrite(enableM1, map(abs(data.pitch), 0, 180, 0,255));
        analogWrite(enableM2, 0);
      }
      else if(data.roll < -30){
        analogWrite(enableM1, 0);
        analogWrite(enableM2, map(abs(data.pitch), 0, 180, 0, 255));
      }
      else{
        analogWrite(enableM1, map(abs(data.pitch), 0, 180, 0, 255));
        analogWrite(enableM2, map(abs(data.pitch), 0, 180, 0, 255));
      }
    }

    
  }

  if(data.pitch < 20 && data.pitch > -20){
    analogWrite(enableM1, 0);
    analogWrite(enableM2, 0);
  }
}

/*
void goForward(){
  digitalWrite(input1M1, HIGH);
  digitalWrite(input2M1, LOW);
  digitalWrite(input1M2, HIGH);
  digitalWrite(input2M2, LOW);

  analogWrite(enableM1, map(abs(data.pitch), 0, 180, 0, 255));
  analogWrite(enableM2, map(abs(data.pitch), 0, 180, 0, 255));
}

void goBackward(){
  digitalWrite(input1M1, LOW);
  digitalWrite(input2M1, HIGH);
  digitalWrite(input1M2, LOW);
  digitalWrite(input2M2, HIGH);

  analogWrite(enableM1, map(abs(data.pitch), 0, 180, 0, 255));
  analogWrite(enableM2, map(abs(data.pitch), 0, 180, 0, 255));
}

void goRight(){
  digitalWrite(input1M1, HIGH);
  digitalWrite(input2M1, LOW);
  digitalWrite(input1M2, HIGH);
  digitalWrite(input2M2, LOW);
  
  analogWrite(enableM1, map(abs(data.roll), 0, 180, 0, 255));
  analogWrite(enableM2, 0);
}

void goLeft(){
  digitalWrite(input1M1, HIGH);
  digitalWrite(input2M1, LOW);
  digitalWrite(input1M2, HIGH);
  digitalWrite(input2M2, LOW);

  analogWrite(enableM1, 0);
  analogWrite(enableM2, map(abs(data.roll), 0, 180, 0, 255));
}
*/
