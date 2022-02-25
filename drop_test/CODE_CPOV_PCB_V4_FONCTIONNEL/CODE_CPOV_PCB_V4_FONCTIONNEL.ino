#include <SoftwareSerial.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//SoftwareSerial MyBlue(0, 1); //RX TX

Adafruit_BMP280 bmp;
MPU6050 mpu6050(Wire);

File data;

#define LAUNCHPAD_MAX 600       //Intervales de validité pour le Launchpad
#define LAUNCHPAD_MIN 300

float baseAltitude;    //Variables
long liftoffTime;
float accelerationX;
float accelerationY;
float accelerationZ;
float acceleration;
float peakAltitude = 0;
float measAltitude;
long measTime;
long delayMAIN;
long delayDROGUE;
long ittime;
long ittemp;
int BT;

const byte BUZZER_PIN = 2;      //PINS
const byte MINI_CHUTE_PIN = 8;

const float START_ACCELERATION = 0.4;
const float STOP_ACCELERATION = 1.8;

const long TIMER = 200;             //TIMER de déploiement du DROGUE
const float chuteAltitude = 200; //Altitude de déploiement du MAIN

int chuteSecReset = 30;          //Marge du main
int chuteSec = 30;               //Marge du main
int liftoffSec = 10;             //Marge du décollage
int stopSec = 10;                //Marge pour arrêter le log
const int LIFTOFF_SEC_RESET = 10  ;  //Marge du décollage
const int STOP_SEC_RESET = 10  ;  //Marge pour arrêter le log

int chuteStatus = 0;        //Status
int miniChuteStatus = 0;

void continuousBeep(int BUZZER_PIN, float seconds);
void limitedBeep(int BUZZER_PIN, float seconds, int bips);


void setup() {
  Serial.println("========================================");
  Serial.println("SETUP");
  Serial.println("Setting up pins.");
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600);
  //  MyBlue.begin(9600);
  Wire.begin();
  mpu6050.begin();
  Serial.println("Pins setup successfully.");


  Serial.println("Setting up SD card.");
  //data.println("### measAltitude, pressure, accX, accY, accZ, gyroX, gyroY, gyroZ, peakAltitude, miniChuteStatus, chuteStatus");
  if (!SD.begin(10)) {   //check de la SD
    Serial.print("sd");
    continuousBeep(2, 3);
  }
  data = SD.open("test.txt", FILE_WRITE);
  data.println("###");
//  data.close();
  Serial.println("SD card set up succesfully.");

  Serial.println("Setting up altimeter.");
  if (!bmp.begin(0x76)) { //check de l'altimètre
    Serial.print("bmp");
    continuousBeep(2, 1);
  } else {
    Serial.println("Altimeter set up succesfully.");
  }
  Serial.println("");
  Serial.println("========================================");


  //mpu6050.setAccelSensitivity(3);
  mpu6050.calcGyroOffsets(true);
  mpu6050.update();


  baseAltitude = bmp.readAltitude(1013.25);
  peakAltitude = baseAltitude;
  accelerationX = mpu6050.getAccX();
  accelerationY = mpu6050.getAccY();
  accelerationZ = mpu6050.getAccZ();
  acceleration = sqrt(pow(accelerationX, 2) + pow(accelerationY, 2) + pow(accelerationZ, 2));


  if (!(1.2 > accelerationZ and accelerationZ > 0.8) || !(LAUNCHPAD_MIN < baseAltitude < LAUNCHPAD_MAX)) { //check de la cohérence de l'altitude du launchpad et de l'accélération (doit ressentir -1g)
    Serial.println("");
    Serial.println("Incoherent launching conditions!");
    Serial.print("accel");
    Serial.print(acceleration);
    continuousBeep(2, 0.5);
  }
  limitedBeep(2, 0.05, 3);
  delay(500);
  limitedBeep(2, 0.05, 3);
  delay(500);
  limitedBeep(2, 1, 1);


  while (liftoffSec) {
    mpu6050.update();
    accelerationX = mpu6050.getAccX();
    accelerationY = mpu6050.getAccY();
    accelerationZ = mpu6050.getAccZ();
    acceleration = sqrt(pow(accelerationX, 2) + pow(accelerationY, 2) + pow(accelerationZ, 2));
    //    if (BT > 5) { //ce truc là sert juste à pas trop spammer le bluetooth sinon l'appli panique
    //      if (MyBlue.available())       //module bluetooth pour check les données avant lancement, pas nécessaire
    Serial.print("  ");
    Serial.print(liftoffSec);
    Serial.print("  ");
    Serial.print(acceleration);
    Serial.print("  ");
    Serial.println(bmp.readAltitude(1013.25));
    //      BT = 0;
    //    }
    //    BT += 1;


    if (acceleration <= START_ACCELERATION) { //détection du décollage
      liftoffSec--;
    }
    else {
      liftoffSec = LIFTOFF_SEC_RESET;
    }
  }
  liftoffTime = millis();
  Serial.println("");
  Serial.println("========================================");
  Serial.println("LIFTOFF CONFIRMED");
  limitedBeep(2, 0.1, 1);               // A SUPPRIMER
}


void loop() {
  
  mpu6050.update();
  accelerationX = mpu6050.getAccX();
  accelerationY = mpu6050.getAccY();
  accelerationZ = mpu6050.getAccZ();
  acceleration = sqrt(pow(accelerationX, 2) + pow(accelerationY, 2) + pow(accelerationZ, 2));
  measAltitude = bmp.readAltitude(1013.25) - baseAltitude;

  measTime = millis() - liftoffTime;
  ittime = measTime - ittemp;
  ittemp = measTime;

  Serial.print(ittime);
  Serial.print(", ");
  Serial.print(measTime);
  Serial.print(", ");
  Serial.print(accelerationX);
  Serial.print(", ");
  Serial.print(accelerationY);
  Serial.print(", ");
  Serial.print(accelerationZ);
  Serial.print(", ");
  Serial.print(acceleration);
  Serial.print(", ");
  Serial.print(measAltitude);
  Serial.print(", ");

  if (data) {
    data.print(measTime);
    data.print(", ");
    data.print(measAltitude);
    data.print(", ");
    data.print(bmp.readPressure());
    data.print(", ");
    data.print(accelerationX);
    data.print(", ");
    data.print(accelerationY);
    data.print(", ");
    data.print(accelerationZ);
    data.print(", ");
    data.print(mpu6050.getAngleX());
    data.print(", ");
    data.print(mpu6050.getAngleY());
    data.print(", ");
    data.print(mpu6050.getAngleZ());
    data.print(", ");
    data.print(peakAltitude);
    data.print(", ");
    data.print(miniChuteStatus);
    data.print(", ");
    data.println(chuteStatus);

    data.flush();
  }

//  if (measTime >= TIMER and miniChuteStatus == 0) { //charge redondante du drogue
//    digitalWrite(MINI_CHUTE_PIN, HIGH);
//    Serial.print("DROGUE");
//    delayDROGUE += ittime;
//    if (delayDROGUE > 50) {
//      digitalWrite(MINI_CHUTE_PIN, LOW);
//      miniChuteStatus = 1;
//      limitedBeep(2,0.5,1); //RRRRaRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//    }
//  }

  if (measAltitude > peakAltitude) {       //Apogée, sert à rien mais pourquoi pas
    peakAltitude = measAltitude;
  }
//
//  if (chuteStatus == 0 and measAltitude <= chuteAltitude and measTime > TIMER and measAltitude > 10) { //Détection de l'altitude de sortie du main. Le TIMER est là pour empêcher de le sortir à la montée
//
//    if (chuteSec) {
//      chuteSec--;
//    }
//
//    else {
//      digitalWrite(7, HIGH);
//      Serial.println("MAINNNNNNNNNNN");
//      delayMAIN += ittime;
//      if (delayMAIN > 300) {
//        chuteStatus = 1;
//        digitalWrite(7, LOW);
//        //limitedBeep(2,1,1);
//      }
//    }
//
//  }
//  else {
//    chuteSec = chuteSecReset;
//  }

  if (acceleration >= STOP_ACCELERATION) { //détection du décollage
    if (!stopSec) {
      stopSec--;
      Serial.println(stopSec);
    }
    else {
      limitedBeep(2, 0.3, 3);               // A SUPPRIMER
      Serial.println("EXITING LOG");
      data.close();
      exit(0);
    }
  }
  else {
    stopSec = STOP_SEC_RESET;
    Serial.println(stopSec);
  }
}

void continuousBeep(int BUZZER_PIN, float seconds) {                            //beep
  int del = seconds * 1000;
  while (1) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(del);
    digitalWrite(BUZZER_PIN, LOW);
    delay(del);
  }
}

void limitedBeep(int BUZZER_PIN, float seconds, int bips) {           //beep
  int del = seconds * 1000;
  while (bips) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(del);
    digitalWrite(BUZZER_PIN, LOW);
    delay(del);
    bips--;
  }
}
