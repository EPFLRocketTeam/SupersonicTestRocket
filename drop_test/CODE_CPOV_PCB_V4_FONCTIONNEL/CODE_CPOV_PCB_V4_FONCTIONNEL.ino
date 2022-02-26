#include <SoftwareSerial.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// 80 characters ===============================================================

#define SERIAL // if this line is not commented, serial output happens in loop


// SENSOR SETUP ================================================================
Adafruit_BMP280 bmp; // barometer
MPU6050 mpu6050(Wire); // IMU


// GLOBAL VARIABLES ============================================================

// Constants -------------------------------------------------------------------
// Defined used const expr to save dynamic memory and place values in the flash
// Similar to using #define but also allows type casting

// Recovery settings
constexpr uint16_t LAUNCHPAD_MAX_ALTITUDE = 600;
constexpr uint16_t LAUNCHPAD_MIN_ALTITUDE = 300;

constexpr float START_ACCELERATION = 0.4;
constexpr float STOP_ACCELERATION = 1.8;

constexpr uint8_t DROGUE_TIMER = 200; // Timer to deploy the drogue
constexpr uint16_t MAIN_CHUTE_ALTITUDE = 200; // Altitude to deploy the main
constexpr uint8_t MAIN_CHUTE_MARGIN_RESET = 30;
constexpr uint8_t LIFTOFF_MARGIN_RESET = 10;
constexpr uint8_t STOP_MARGIN_RESET = 10;

// Pins
constexpr uint8_t BUZZER_PIN = 2;
constexpr uint8_t MINI_CHUTE_PIN = 8;

// Varibles --------------------------------------------------------------------

File data_file;

// Readings
long liftoffTime;
float baseAltitude;
float peakAltitude;
long measTime;

// Recovery
int chuteMargin = 30;               //Marge du main
int stopMargin = 10;                //Marge pour arrêter le log
bool drogueStatus = 0;
bool mainStatus = 0;


// USER FUNCTION SIGNATURES ====================================================

void continuousBeep(uint8_t buzzer_pin, uint16_t milliseconds);
void limitedBeep(uint8_t buzzer_pin, uint16_t milliseconds, uint8_t bips);

// SETUP =======================================================================

void setup() {
  // Pin setup
  Serial.println(F("Pins"));
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  // Sensor setup
  SoftwareSerial MyBlue(0, 1); // Bluetooth, RX TX
  MyBlue.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();

  // Setup SD card
  Serial.println(F("SD"));
  if (!SD.begin(10)) {   //check de la SD
    Serial.println(F("Fail"));
    continuousBeep(2, 3);
  }
  data_file = SD.open("test.txt", FILE_WRITE);
  data_file.println(F("measTime,measAltitude, pressure, accX, accY, accZ,\
                      gyroX, gyroY, gyroZ,\
                      peakAltitude, miniChuteStatus, chuteStatus"));
  data_file.close();

  // Setup barometer
  Serial.println(F("Baro"));
  if (!bmp.begin(0x76)) { //check de l'altimètre
    Serial.print(F("Fail"));
    continuousBeep(2, 1);
  }

  // Setup IMU
  // mpu6050.setAccelSensitivity(3); // doesn't work on newer version of library
  mpu6050.calcGyroOffsets(true);
  mpu6050.update();

  if (0.8 > mpu6050.getAccZ() || mpu6050.getAccZ() > 1.2 ||
      LAUNCHPAD_MIN_ALTITUDE > bmp.readAltitude(1013.25) ||
      bmp.readAltitude(1013.25) > LAUNCHPAD_MAX_ALTITUDE) {
    //check de la cohérence de l'altitude du launchpad
    // et de l'accélération (doit ressentir -1g)
    Serial.println("");
    Serial.println("Incoherent launching conditions!");
    Serial.print("accel: ");
    Serial.print(mpu6050.getAccZ());
    continuousBeep(2, 0.5);
  }

  // success beeps
  limitedBeep(2, 0.05, 3);
  delay(500);
  limitedBeep(2, 0.05, 3);
  delay(500);
  limitedBeep(2, 1, 1);

  // loop waiting for liftoff
  uint8_t BT_flag = 5;
  uint8_t liftoffMargin = LIFTOFF_MARGIN_RESET;
  while (liftoffMargin) {
    mpu6050.update();
    float acceleration = sqrt(pow(mpu6050.getAccX(), 2)
                              + pow(mpu6050.getAccY(), 2)
                              + pow(mpu6050.getAccZ(), 2));
    if (!BT_flag) { // lower BT logging frequency or else app panics
      if (MyBlue.available()) {
        //module bluetooth pour check les données avant lancement, pas nécessaire
        Serial.print("  ");
        Serial.print(liftoffMargin);
        Serial.print("  ");
        Serial.print(acceleration);
        Serial.print("  ");
        Serial.println(bmp.readAltitude(1013.25));
        BT_flag = 5;
      }
      BT_flag--;
    }

    // Detect drop test/launch
    if (acceleration <= START_ACCELERATION) {
      liftoffMargin--;
    }
    else {
      liftoffMargin = LIFTOFF_MARGIN_RESET;
    }
  }
  Serial.println(F("LIFTOFF CONFIRMED"));
  limitedBeep(2, 0.1, 1);               // A SUPPRIMER
  liftoffTime = millis();
  baseAltitude = bmp.readAltitude(1013.25);
  peakAltitude = baseAltitude;
}

//float measAltitude;
//long measTime;
//long ittime;
//long ittemp;
void loop() {
  // update measurements
  mpu6050.update();
  float measAltitude = bmp.readAltitude(1013.25) - baseAltitude;
  float acceleration = sqrt(pow(mpu6050.getAccX(), 2)
                            + pow(mpu6050.getAccY(), 2)
                            + pow(mpu6050.getAccZ(), 2));
  long iterationTime = millis() - measTime;
  measTime = millis() - liftoffTime;

  // Print data
#ifdef SERIAL
  Serial.print(iterationTime); Serial.print(", ");
  Serial.print(measTime); Serial.print(", ");
  Serial.print(measAltitude); Serial.print(", ");
  Serial.print(bmp.readAltitude(1013.25)); Serial.print(", ");
  Serial.print(mpu6050.getAccX()); Serial.print(", ");
  Serial.print(mpu6050.getAccY()); Serial.print(", ");
  Serial.print(mpu6050.getAccZ()); Serial.print(", ");
  Serial.print(mpu6050.getAngleX()); Serial.print(", ");
  Serial.print(mpu6050.getAngleY()); Serial.print(", ");
  Serial.print(mpu6050.getAngleZ()); Serial.print(", ");
  Serial.print(peakAltitude); Serial.print(", ");
  Serial.print(drogueStatus); Serial.print(", ");
  Serial.println(mainStatus);
#endif

  // Save data to file
  if (data_file) {
    data_file.print(iterationTime); data_file.print(", ");
    data_file.print(measTime); data_file.print(", ");
    data_file.print(measAltitude); data_file.print(", ");
    data_file.print(bmp.readAltitude(1013.25)); data_file.print(", ");
    data_file.print(mpu6050.getAccX()); data_file.print(", ");
    data_file.print(mpu6050.getAccY()); data_file.print(", ");
    data_file.print(mpu6050.getAccZ()); data_file.print(", ");
    data_file.print(mpu6050.getAngleX()); data_file.print(", ");
    data_file.print(mpu6050.getAngleY()); data_file.print(", ");
    data_file.print(mpu6050.getAngleZ()); data_file.print(", ");
    data_file.print(peakAltitude); data_file.print(", ");
    data_file.print(drogueStatus); data_file.print(", ");
    data_file.println(mainStatus);
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

  //  if (measAltitude > peakAltitude) {       //Apogée, sert à rien mais pourquoi pas
  //    peakAltitude = measAltitude;
  //  }
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
    if (!stopMargin) {
      stopMargin--;
      Serial.println(stopMargin);
    }
    else {
      limitedBeep(2, 0.3, 3);               // A SUPPRIMER
      Serial.println("EXITING LOG");
      data_file.close();
      exit(0);
    }
  }
  else {
    stopMargin = STOP_MARGIN_RESET;
    Serial.println(stopMargin);
  }
}


// USER FUNCTIONS ==============================================================

void continuousBeep(uint8_t buzzer_pin, uint16_t milliseconds) {
  while (1) {
    digitalWrite(buzzer_pin, HIGH);
    delay(milliseconds);
    digitalWrite(buzzer_pin, LOW);
    delay(milliseconds);
  }
}

void limitedBeep(uint8_t buzzer_pin, uint16_t milliseconds, uint8_t bips) {
  while (bips) {
    digitalWrite(buzzer_pin, HIGH);
    delay(milliseconds);
    digitalWrite(buzzer_pin, LOW);
    delay(milliseconds);
    bips--;
  }
}
