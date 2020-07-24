#include <Wire.h>

#include <SPI.h>
#include <RF24.h>

RF24 radio(7,8);

const byte address[6] = "00001";

const int MPU_address = 0x68;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float accAngleX, accAngleY, totalVector;
float gyroAngleX, gyroAngleY;
float accXCal, accYCal, accZCal;
float gyroXCal, gyroYCal, gyroZCal;
float elapsedTime, currentTime, previousTime;

struct package
{
  float roll;
  float pitch;  
};

typedef struct package Package;
Package data;

void setup() {
  Serial.begin(115200);
  setup_MPU();
  setup_transmitter();

  calibrateAcc();
  calibrateGyro();
}

void loop() {
  radio.write(&data, sizeof(data));

  readAcc();

  accX /= 16384;
  accY /= 16384;
  accZ /= 16384;

  totalVector = sqrt(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2));
  accAngleX = asin(accX/totalVector) * -57.296;
  accAngleY = asin(accY/totalVector) * 57.296;

  accAngleX -= accXCal;
  accAngleY -= accYCal;
  
  readGyro();

  gyroX -= gyroXCal;
  gyroY -= gyroYCal;
  gyroZ -= gyroZCal;

  gyroX /= 131;
  gyroY /= 131;
  gyroZ /= 131;

  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime)/1000;

  gyroAngleX += gyroX * elapsedTime;
  gyroAngleY += gyroY * elapsedTime;

  data.roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  data.pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;

  Serial.print("Roll: ");
  Serial.print(data.roll);
  Serial.print(" Pitch: ");
  Serial.print(data.pitch);
  Serial.println();
}

void setup_MPU(){
  Wire.begin();
  Wire.beginTransmission(MPU_address);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
}

void setup_transmitter(){
  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}

void calibrateAcc(){
  for(int cal_acc = 0; cal_acc < 200; cal_acc++){
    readAcc();
    
    accX /= 16384;
    accY /= 16384;
    accZ /= 16384;
    
    accXCal += accX;
    accYCal += accY;
    accZCal += accZ;
    delay(2);
  }

  accXCal /= 200;
  accYCal /= 200;
  accZCal /= 200;
}

void calibrateGyro(){
  for(int cal = 0; cal < 200; cal++){
    readGyro();

    gyroXCal += gyroX;
    gyroYCal += gyroY;
    gyroZCal += gyroZ;

    delay(2);
  }

  gyroXCal /= 200;
  gyroYCal /= 200;
  gyroZCal /= 200;
}

void readAcc(){
  Wire.beginTransmission(MPU_address);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_address, 6, true);
  
  accX = (Wire.read() << 8 | Wire.read());
  accY = (Wire.read() << 8 | Wire.read());
  accZ = (Wire.read() << 8 | Wire.read());
}

void readGyro(){
  Wire.beginTransmission(MPU_address);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_address, 6, true);
  
  gyroX = (Wire.read() << 8 | Wire.read());
  gyroY = (Wire.read() << 8 | Wire.read());
  gyroZ = (Wire.read() << 8 | Wire.read());
}
