/*
 * Hermes Relay
 *
 * Michael Ha - EPFL EL BA6
 *
 * 30.05.2022
 */

#include <cmath>
#include <time.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Xbee.h"
#include <Bounce.h>


#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUTTON1_PIN 33
#define BUTTON2_PIN 34


// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 'KSP_HERMES', 128x64px   HERMES II logo
const unsigned char KSP_HERMES [] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xe5, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xbd, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf9, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x86, 0x00, 0x07, 0xfb, 
  0x9f, 0xc7, 0xe0, 0x1c, 0x01, 0xfc, 0x3f, 0x87, 0xfe, 0x00, 0xf8, 0x03, 0x87, 0x00, 0x03, 0xf3, 
  0x9f, 0xc7, 0x80, 0x1c, 0x00, 0x78, 0x1f, 0x03, 0xf8, 0x00, 0xe0, 0x03, 0xcf, 0x00, 0x01, 0xf3, 
  0x9f, 0xc7, 0x00, 0x1c, 0x00, 0x38, 0x9f, 0x13, 0xf8, 0x00, 0xe0, 0x03, 0xcf, 0xff, 0xf1, 0xe7, 
  0x9f, 0xc7, 0x3f, 0xff, 0xfe, 0x39, 0x8f, 0x11, 0xf1, 0xff, 0xc7, 0xff, 0x87, 0xff, 0xf1, 0xcf, 
  0x9f, 0xc7, 0x3f, 0xff, 0xff, 0x39, 0x8f, 0x19, 0xf1, 0xff, 0xc7, 0xf8, 0x00, 0x7f, 0xf1, 0xdf, 
  0x9f, 0xc7, 0x1f, 0xff, 0xff, 0x39, 0xcf, 0x18, 0xf9, 0xff, 0xc7, 0xff, 0x87, 0xff, 0xe1, 0x9f, 
  0x80, 0x07, 0x80, 0x1f, 0x00, 0x39, 0xc7, 0x18, 0xf8, 0x00, 0xe0, 0x07, 0xcf, 0xfe, 0x03, 0x3f, 
  0x80, 0x07, 0x80, 0x1e, 0x00, 0x79, 0xe7, 0x1c, 0xfc, 0x00, 0xf0, 0x03, 0xcf, 0xe0, 0x06, 0x7f, 
  0x8f, 0xc7, 0x1f, 0xfc, 0x00, 0xf9, 0xe3, 0x1c, 0x78, 0xff, 0xff, 0xe3, 0xcf, 0x00, 0x1c, 0xff, 
  0x9f, 0xc7, 0x3f, 0xfc, 0x78, 0xf9, 0xe3, 0x1c, 0x71, 0xff, 0xff, 0xf1, 0xde, 0x01, 0xf9, 0xff, 
  0x9f, 0xc7, 0x3f, 0xfc, 0x7c, 0xf9, 0xf3, 0x1e, 0x71, 0xff, 0xff, 0xf1, 0xde, 0x1f, 0xf3, 0xff, 
  0x9f, 0xc7, 0x1f, 0xfc, 0x7c, 0x79, 0xf1, 0x3e, 0x30, 0xff, 0xff, 0xe1, 0xdc, 0x7f, 0xe3, 0xff, 
  0x9f, 0xc7, 0x00, 0x1c, 0x7e, 0x39, 0xf8, 0x3f, 0x38, 0x00, 0xe0, 0x03, 0xdc, 0x7f, 0xc7, 0xff, 
  0x9f, 0xc7, 0x80, 0x1c, 0x7e, 0x39, 0xf8, 0x3f, 0x1c, 0x00, 0xe0, 0x07, 0xdc, 0x7f, 0x8f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdc, 0x00, 0x11, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdc, 0x00, 0x61, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xde, 0x00, 0xc1, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xf9, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xf3, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xc7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0x9f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xde, 0x3f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbc, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa1, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// States of the display FSM
enum displayState {MAX7_, ADIS16470_, AIS_, ALTIMAX_, HONEYWELL_, MAX31855_, AD8556_};
displayState currentState(MAX7_);

// Declaration and initialization of all data structures
struct sensorStatus {
  bool TEST;
  bool ADIS;
  bool AIS;
  bool ALTIMAX;
  bool HONEYWELL;
  bool MAX7;
  bool MAX31855;
  bool AD8556;
} sensorStatus;

struct packetReceived {
  bool ADIS;
  bool AIS;
  bool ALTIMAX;
  bool HONEYWELL;
  bool MAX7;
  bool MAX31855;
  bool AD8556;
} packetReceived;

struct ADIS16470 {
  uint32_t gyroX;
  uint32_t gyroY;
  uint32_t gyroZ;
  uint32_t accelX;
  uint32_t accelY;
  uint32_t accelZ;
  uint32_t temp;  
} ADIS16470;

struct MAX7 {
  uint32_t lat;
  uint32_t lon;
  uint32_t alt;
} MAX7;

struct AIS {
  uint32_t accelX;
  uint32_t accelY;
} AIS;

struct ALTIMAX {
  bool pin0;
  bool pin1;
  bool pin2;
} ALTIMAX;

struct HONEYWELL {
  uint32_t pressure;
  uint32_t temp;  
} HONEYWELL;

struct MAX31855 {
  uint32_t probeTemp;
  uint32_t sensorTemp;
} MAX31855;

struct AD8556 {
  uint32_t thrust;
} AD8556;

static unsigned int packetCount(0);
static unsigned int elapsed_time(0);
static bool dim(0);

IntervalTimer display_timer;     // create IntervalTimer object
XBee xbee = XBee();

Bounce pushButton1 = Bounce(BUTTON1_PIN, 5); // 5ms debounce
Bounce pushButton2 = Bounce(BUTTON2_PIN, 5); // 5ms debounce

void setup() {
  Serial.begin(115200);   // serial monitor
  Serial1.begin(115200);  // xbee serial
  xbee.setSerial(Serial1);

  Wire.setSDA(18);    // set SDA AND SCL pins
  Wire.setSCL(19);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(100);         // wait for initializing
  oled.clearDisplay(); // clear display
  // display bitmap
  oled.drawBitmap(0, 0, KSP_HERMES, 128, 64, WHITE);
  // invert display
  oled.invertDisplay(1);
  oled.display();
  delay(2000);
  
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color

  oled.clearDisplay();
  oled.invertDisplay(0);
  oled.drawRect(0,56,8*8,8,WHITE);
  oled.display();

  display_timer.priority(255);  // lowest priority interrupt
  display_timer.begin(drawDisplay, 100000); // call ISR every 100ms to refresh display  
}


void updateState(bool direction_) {
  if(direction_) {  // increment state
    if(currentState == MAX7_)
      currentState = ADIS16470_;
    else if(currentState == ADIS16470_)
      currentState = AIS_;
    else if(currentState == AIS_)
      currentState = ALTIMAX_;
    else if(currentState == ALTIMAX_)
      currentState = HONEYWELL_;
    else if(currentState == HONEYWELL_)
      currentState = MAX31855_;
    else if(currentState == MAX31855_)
      currentState = AD8556_;
    else
      currentState = MAX7_;
  } else {          // decrement state
    if(currentState == MAX7_)
      currentState = AD8556_;
    else if(currentState == AD8556_)
      currentState = MAX31855_;
    else if(currentState == MAX31855_)
      currentState = HONEYWELL_;
    else if(currentState == HONEYWELL_)
      currentState = ALTIMAX_;
    else if(currentState == ALTIMAX_)
      currentState = AIS_;
    else if(currentState == AIS_)
      currentState = ADIS16470_;
    else
      currentState = MAX7_;
  }
}

uint32_t ReverseBytes(uint32_t value) {   // function to reverse the bytes of a 32bit integer, used to convert from big to little endian
                                          // see https://stackoverflow.com/questions/18145667/how-can-i-reverse-the-byte-order-of-an-int                                 
  return (value & 0x000000FFU) << 3*8 | (value & 0x0000FF00U) << 8 |
         (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 3*8;
}

void xbeeReceive() {
    if(Serial1.available()) {
      elapsed_time = millis();
      if(dim == 1) {
        oled.ssd1306_command(SSD1306_SETCONTRAST);
        oled.ssd1306_command(143);
      }
      // if serial data is available, read packet length
      uint8_t start[3];
      Serial1.readBytes(start, 3);
      if(start[0] == 0x7E) {  // check that start delimiter is present
        packetCount++;
        Serial.print("Total received packets : ");
        Serial.println(packetCount);
        
        uint16_t length((((uint16_t)start[1] << 8)| start[2]) + 1); // extract length and add 1 to include checksum
        uint8_t packet[length]; 
        Serial1.readBytes(packet, length);    // read packet content
        
        uint8_t datagramID(packet[15-3]);     // payload start at offset 15 (starting with EPFL header)

        uint8_t sensorData[length-1-25];      // sensor data starts at offset 15+10 = 25
        for(int i=25; i<length-1; ++i) {
          sensorData[i-25] = packet[i];
        }

        uint32_t* dataptr = (uint32_t*)(sensorData);    // pointer to 4 bytes blocks of sensorData

        if(datagramID == 27) {                                          // TEST packet
          Serial.println("HERMES TEST packet received");
          
          if(sensorStatus.TEST == 0)
            sensorStatus.TEST = 1;            

        } else if(datagramID == 28) {                                   // ADIS16470 packet
          Serial.println("ADIS16470 packet received");

          if(sensorStatus.ADIS == 0)
            sensorStatus.ADIS = 1;
          if(packetReceived.ADIS == 0)
            packetReceived.ADIS = 1;
          else
            packetReceived.ADIS = 0; 
                      
          ADIS16470.gyroX = ReverseBytes(*(dataptr+0));   // reverse bytes in each 32bit words because packets are in BIG endian
          ADIS16470.gyroY = ReverseBytes(*(dataptr+1));
          ADIS16470.gyroZ = ReverseBytes(*(dataptr+2));
          ADIS16470.accelX = ReverseBytes(*(dataptr+3));
          ADIS16470.accelY = ReverseBytes(*(dataptr+4));
          ADIS16470.accelZ = ReverseBytes(*(dataptr+5));
          ADIS16470.temp = ReverseBytes(*(dataptr+6));
              
        } else if(datagramID == 29) {                                   // AISx120SX packet
          Serial.println("AISx120SX packet received");

          if(sensorStatus.AIS == 0)
            sensorStatus.AIS = 1;
          if(packetReceived.AIS == 0)
            packetReceived.AIS = 1;
          else
            packetReceived.AIS = 0;         
            
          AIS.accelX = ReverseBytes(*(dataptr+0));
          AIS.accelY = ReverseBytes(*(dataptr+1));
          
        } else if(datagramID == 30) {                                   // ALTIMAX packet
          Serial.println("Altimax packet received");

          if(sensorStatus.ALTIMAX == 0)
            sensorStatus.ALTIMAX = 1;
          if(packetReceived.ALTIMAX == 0)
            packetReceived.ALTIMAX = 1;
          else
            packetReceived.ALTIMAX = 0; 
            
          uint32_t Altimax_data = *(dataptr+0);
          uint8_t Altimax_pinStates[4];
          memcpy(Altimax_pinStates, &Altimax_data, sizeof(Altimax_data));
          ALTIMAX.pin0 = (bool)Altimax_pinStates[0];
          ALTIMAX.pin1 = (bool)Altimax_pinStates[1];
          ALTIMAX.pin2 = (bool)Altimax_pinStates[2];
          
        } else if(datagramID == 31) {                                   // HONEYWELLRsc pressure packet
          Serial.println("HoneywellRsc_Pressure packet received"); 
          
          if(sensorStatus.HONEYWELL == 0)
            sensorStatus.HONEYWELL = 1;
          if(packetReceived.HONEYWELL == 0)
            packetReceived.HONEYWELL = 1;
          else
            packetReceived.HONEYWELL = 0; 
               
          HONEYWELL.pressure = ReverseBytes(*(dataptr+0));   
          
        } else if(datagramID == 32) {                                   // HONEYWELLRsc temperature packet
          Serial.println("HoneywellRsc_Temp packet received");

          if(sensorStatus.HONEYWELL == 0)
            sensorStatus.HONEYWELL = 1;
          if(packetReceived.HONEYWELL == 0)
            packetReceived.HONEYWELL = 1;
          else
            packetReceived.HONEYWELL = 0; 
                      
          HONEYWELL.temp = ReverseBytes(*(dataptr+0));
             
        } else if(datagramID == 33) {                                   // MAX7 GPS packet
          Serial.println("MAX7 GPS packet received");

          if(sensorStatus.MAX7 == 0)
            sensorStatus.MAX7 = 1;
          if(packetReceived.MAX7 == 0)
            packetReceived.MAX7 = 1;
          else
            packetReceived.MAX7 = 0; 
            
          MAX7.lat = ReverseBytes(*(dataptr+0));
          MAX7.lon = ReverseBytes(*(dataptr+1));
          MAX7.alt = ReverseBytes(*(dataptr+2));

        } else if(datagramID == 34) {                                   // MAX31855 packet
          Serial.println("MAX31855 packet received");

          if(sensorStatus.MAX31855 == 0)
            sensorStatus.MAX31855 = 1;
          if(packetReceived.MAX31855 == 0)
            packetReceived.MAX31855 = 1;
          else
            packetReceived.MAX31855 = 0; 
            
          MAX31855.probeTemp = ReverseBytes(*(dataptr+0));
          MAX31855.sensorTemp = ReverseBytes(*(dataptr+1));
          
        } else if(datagramID == 35) {                                   // AD8556 packet
          Serial.println("AD8556 packet received");

          if(sensorStatus.AD8556 == 0)
            sensorStatus.AD8556 = 1;
          if(packetReceived.AD8556 == 0)
            packetReceived.AD8556 = 1;
          else
            packetReceived.AD8556 = 0; 
            
          AD8556.thrust = ReverseBytes(*(dataptr+0));
          
        } else {
          Serial.println("wrong datagramID");
        }
      }
    }
    return;
}

void drawDisplay() {
  /*
  Serial.println("Entering drawDisplay ISR");
  Serial.print("Time since last display interrupt : ");
  Serial.println(millis()-time_spent);
  time_spent = millis();
  */
  oled.clearDisplay();
  oled.invertDisplay(0);

  oled.setCursor(6,56);
  oled.print("STATUS");
  oled.drawRect(58,56,8*8,8,WHITE);

  // Check which packets have between received and update the on-screen indicator
  if(sensorStatus.TEST)
    oled.fillRect(58,56,8,8,WHITE);
  if(sensorStatus.ADIS)
    oled.fillRect(58+8,56,8,8,WHITE);
  if(packetReceived.ADIS)
    oled.drawRect(58+8+3,56+3,2,2,BLACK);
  oled.drawRect(58+8+2,56+2,4,4,BLACK);
  if(sensorStatus.AIS)
    oled.fillRect(58+2*8,56,8,8,WHITE);
  if(packetReceived.AIS)
    oled.drawRect(58+2*8+3,56+3,2,2,BLACK);
  oled.drawRect(58+2*8+2,56+2,4,4,BLACK);       
  if(sensorStatus.ALTIMAX)
    oled.fillRect(58+3*8,56,8,8,WHITE);
  if(packetReceived.ALTIMAX)
    oled.drawRect(58+3*8+3,56+3,2,2,BLACK); 
  oled.drawRect(58+3*8+2,56+2,4,4,BLACK);    
  if(sensorStatus.HONEYWELL)
    oled.fillRect(58+4*8,56,8,8,WHITE);
  if(packetReceived.HONEYWELL)
    oled.drawRect(58+4*8+3,56+3,2,2,BLACK); 
  oled.drawRect(58+4*8+2,56+2,4,4,BLACK);   
  if(sensorStatus.MAX7)
    oled.fillRect(58+5*8,56,8,8,WHITE);
  if(packetReceived.MAX7)
    oled.drawRect(58+5*8+3,56+3,2,2,BLACK);
  oled.drawRect(58+5*8+2,56+2,4,4,BLACK);      
  if(sensorStatus.MAX31855)
    oled.fillRect(58+6*8,56,8,8,WHITE);
  if(packetReceived.MAX31855)
    oled.drawRect(58+6*8+3,56+3,2,2,BLACK);
  oled.drawRect(58+6*8+2,56+2,4,4,BLACK);    
  if(sensorStatus.AD8556)
    oled.fillRect(58+7*8,56,8,8,WHITE);
  if(packetReceived.AD8556)
    oled.drawRect(58+7*8+3,56+3,2,2,BLACK);  
  oled.drawRect(58+7*8+2,56+2,4,4,BLACK);    


  // For each display state, display the correct sensor data
  switch(currentState) {
    case MAX7_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("MAX7 GPS DATA"); // text to display
        oled.setCursor(0, 14);
        oled.print("lat : ");
        oled.print(MAX7.lat*pow(10,-7), 7);
        oled.println((char)247);    // degree symbol
            
        oled.print("lon : ");
        oled.print(MAX7.lon*pow(10,-7), 7);
        oled.println((char)247); 
            
        oled.print("alt : ");
        oled.print(MAX7.alt/1000);
        oled.println(" m");
        break;
    case ADIS16470_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("ADIS16470 SENSOR DATA");
  
        oled.setCursor(12, 14);
        oled.print("w[");
        oled.print((char)247);    // degree symbol
        oled.print("/s]");
        oled.setCursor(0, 24);
        oled.print("x ");
        oled.println((float&)ADIS16470.gyroX, 1);
        oled.setCursor(0, 34);
        oled.print("y ");
        oled.println((float&)ADIS16470.gyroY, 1);
        oled.setCursor(0, 44);
        oled.print("z ");
        oled.println((float&)ADIS16470.gyroZ, 1);
  
        oled.setCursor(55, 14);
        oled.print("acc[g]");
        oled.setCursor(55, 24);
        oled.println((float&)ADIS16470.accelX, 1);
        oled.setCursor(55, 34);
        oled.println((float&)ADIS16470.accelY, 1);
        oled.setCursor(55, 44);
        oled.println((float&)ADIS16470.accelZ, 1);
  
        oled.setCursor(98, 14);
        oled.print("T[");
        oled.print((char)247);    // degree symbol
        oled.print("C]");
        oled.setCursor(98, 24);
        oled.println((float&)ADIS16470.temp, 1);
        break;
    case AIS_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("AISx120SX SENSOR DATA");

        oled.setCursor(0, 14);
        oled.print("xaccel : ");
        oled.print((float&)AIS.accelX, 2);
        oled.println("    [g]");
        oled.print("yaccel : ");
        oled.print((float&)AIS.accelY, 2);
        oled.println("    [g]");      
        break;
    case ALTIMAX_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("ALTIMAX SENSOR DATA");

        oled.setCursor(0, 14);
        oled.print("pin0 : ");
        oled.println(ALTIMAX.pin0);
        oled.print("pin1 : ");
        oled.println(ALTIMAX.pin1);
        oled.print("pin2 : ");
        oled.println(ALTIMAX.pin2);
        break;
    case HONEYWELL_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("HONEYWELL SENSOR DATA");

        oled.setCursor(0, 14);
        oled.print("pressure : ");
        oled.print((float&)HONEYWELL.pressure, 2);
        oled.println(" [bar]");       
        oled.print("temp     : ");
        oled.print((float&)HONEYWELL.temp, 2);
        oled.print(" [");
        oled.print((char)247);    // degree symbol
        oled.println("C]");
        break;
    case MAX31855_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("MAX31855 SENSOR DATA");

        oled.setCursor(0, 14);
        oled.print("T_probe");
        oled.setCursor(64, 14);
        oled.print((float&)MAX31855.probeTemp, 2);
        oled.print(" [");
        oled.print((char)247);    // degree symbol
        oled.print("C]");
        oled.setCursor(0, 24);
        oled.print("T_sensor");
        oled.setCursor(64, 24);
        oled.print((float&)MAX31855.sensorTemp, 2);
        oled.print(" [");
        oled.print((char)247);    // degree symbol
        oled.print("C]");
        break;
    case AD8556_:
        oled.drawLine(0,10,128,10,WHITE);
        oled.setCursor(0,0);
        oled.print("AD8556 SENSOR DATA");

        oled.setCursor(0, 14);
        oled.print("thrust : ");
        oled.print((float&)AD8556.thrust, 2);
        oled.println(" N");      
        break;
  }        
  oled.display();
}


void loop() {
  
  xbeeReceive();
 
  if(pushButton1.update()) {
    if(pushButton1.fallingEdge()) {
      elapsed_time = millis();
      if(dim == 1) {
        oled.ssd1306_command(SSD1306_SETCONTRAST);
        oled.ssd1306_command(143);
      }
      Serial.println("button 1 pressed");
      updateState(0);
    }
  }
  if(pushButton2.update()) {
    if(pushButton2.fallingEdge()) {
      elapsed_time = millis();
      if(dim == 1) {
        oled.ssd1306_command(SSD1306_SETCONTRAST);
        oled.ssd1306_command(143);
      }
      Serial.println("button 2 pressed");
      updateState(1);
    }
  }

  // dim the screen to save power if nothing is happening
  if(millis() - elapsed_time > 5000) {
    dim = 1;
    oled.ssd1306_command(SSD1306_SETCONTRAST);
    oled.ssd1306_command(1);
    delay(100);
  }
}
