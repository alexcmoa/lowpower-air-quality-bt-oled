#include <Wire.h>
#include <Adafruit_BMP085.h>

//#include <Sodaq_BMP085.h>

#include "U8glib.h"
#include "dht.h"
#include "LowPower.h"

int incomingByte = 0;   // for incoming serial data
#define dht_apin A0

dht DHT;
//Sds011 dust;
unsigned int Pm25 = 0;
unsigned int Pm10 = 0;

//U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9, 8);  // D0=13, D1=11, CS=10, DC=9, Reset=8
U8GLIB_SH1106_128X64 u8g(10, 9, 12, 11, 13);  // D0=SCK=10, D1=SDA=9, CS=12, DC=11, Reset=RES=13

//Sodaq_BMP085 bmp;
Adafruit_BMP085 bmp;

void draw(void) {
 uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;

  
  u8g.setFont(u8g_font_unifont);  // select font

  u8g.drawStr( 0, 10, "Dust:");
  while (Serial.available() > 0) {
    // read the incoming byte:
    mData = Serial.read(); delay(2);
    if(mData == 0xAA)//head1 ok
     {
        mPkt[0] =  mData;
        mData = Serial.read();
        if(mData == 0xc0)//head2 ok
        {
          mPkt[1] =  mData;
          mCheck = 0;
          for(i=0;i < 6;i++)//data recv and crc calc
          {
             mPkt[i+2] = Serial.read();
             delay(2);
             mCheck += mPkt[i+2];
          }
          mPkt[8] = Serial.read();
          delay(1);
          mPkt[9] = Serial.read();
          if(mCheck == mPkt[8])//crc ok
          {
            Serial.flush();
            //Serial.write(mPkt,10);

            Pm25 = (uint16_t)mPkt[2] | (uint16_t)(mPkt[3]<<8);
            Pm10 = (uint16_t)mPkt[4] | (uint16_t)(mPkt[5]<<8);
            if(Pm25 > 9999)
             Pm25 = 9999;
            if(Pm10 > 9999)
             Pm10 = 9999;            
            //get one good packet
             return;
          }
        }      
     }

    
    
    u8g.setPrintPos(23, 10);  // set position
    u8g.print(Pm25, 0);  // dust
    u8g.setPrintPos(58, 10);  // set position
    u8g.print(Pm10, 0);  // dust
  }

  u8g.drawStr( 0,23, "Temp: ");
  u8g.setPrintPos(54, 23);  // set position
  u8g.print(bmp.readTemperature());
  u8g.setPrintPos(100, 23);  // set position
  u8g.print(DHT.temperature, 0);  // display temperature from DHT11
  u8g.drawStr(115, 23, "c ");
    

    u8g.drawStr(0, 36, "Hum: ");
  u8g.setPrintPos(54, 36);
  u8g.print(DHT.humidity, 0);  // display humidity from DHT11
  u8g.drawStr(70, 36, "% ");

  
  u8g.drawStr( 0,49,"Press: ");
  u8g.setPrintPos(54, 49);  // set position
  u8g.print(bmp.readPressure());
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
  u8g.drawStr( 0,62,"Alt: ");
  u8g.setPrintPos(54, 62);  // set position
  u8g.print(bmp.readAltitude());

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  //  Serial.print("Real altitude = ");
  //  Serial.print(bmp.readAltitude(101500));
  // Serial.println(" meters");

  

}

void setup(void) {
   Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
   bmp.begin(); 
 }

void loop(void) {
  DHT.read11(dht_apin);  // Read apin on DHT11

  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
 // delay(5000);  // Delay of 5sec before accessing DHT11 (min - 2sec)
 
  Serial.print("10"); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
