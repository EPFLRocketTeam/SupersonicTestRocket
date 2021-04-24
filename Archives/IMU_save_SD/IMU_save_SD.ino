#include<Wire.h>
#include <SPI.h>
#include <SD.h>

const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
String fileName;
unsigned long loopstart;

File counterFile, myFile;

void setup(){  
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  char counterString[5] = "0000"; // initialize counter
  counterFile = SD.open("/logs/COUNTER.txt", FILE_WRITE); // open the counter file
  counterFile.seek(counterFile.size()-6); // last line of file. 4 chars + cr + lf
  int i;
  for (i = 0; i < 4; ++i) {
    counterString[i] = counterFile.read();
  }
  counterFile.seek(counterFile.size());

  // write the next file number
  int counterInt;
  sscanf(counterString, "%d", &counterInt); // cast counter into an int
  snprintf(counterString, sizeof(counterString), "%04d",counterInt+1);
  counterFile.println(counterString);
  counterFile.close();

  // open the logging file
  char fileName[16];
  snprintf(fileName, sizeof(fileName), "/logs/%04d.txt",counterInt+1);
  myFile = SD.open(fileName, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) { 
    Serial.print("Writing.");
    myFile.println("mil lis, Ax, Ay, Az, Gx, Gy, Gz");
    myFile.flush();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  loopstart = millis();
}

void loop(){
  // flush to the SD card if it's been more than 10 seconds
  if (millis()-loopstart > 10000) {
    Serial.println("Flushing to SD card");
    myFile.flush();
    loopstart = millis();
  }
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true); 
  AcX=Wire.read()<<8|Wire.read();    
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read();  
  GyZ=Wire.read()<<8|Wire.read();  

  // Write the info on the SD card
  myFile.print(millis()); myFile.print(",");
  myFile.print(AcX); myFile.print(",");
  myFile.print(AcY); myFile.print(",");
  myFile.print(AcZ); myFile.print(",");
  myFile.print(GyX); myFile.print(",");
  myFile.print(GyY); myFile.print(",");
  myFile.println(GyZ);

  // Print data out on the serial monitor
  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = "); Serial.println(AcZ); 
  
  Serial.print("Gyroscope: ");
  Serial.print("X = "); Serial.print(GyX);
  Serial.print(" | Y = "); Serial.print(GyY);
  Serial.print(" | Z = "); Serial.println(GyZ);
  Serial.println(" ");
}
