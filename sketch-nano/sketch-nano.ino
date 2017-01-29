#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>
#include "U8glib.h"
#include "dht.h"
#include "LowPower.h"

#define dht_apin A0
U8GLIB_SH1106_128X64 u8g(10, 9, 12, 11, 13);  // D0=SCK=10, D1=SDA=9, CS=12, DC=11, Reset=RES=13
Adafruit_BMP085 bmp; // A4 & A5 for pressure, temperature, altitude
SoftwareSerial portOne(2, 3); //RX,TX bte 
SoftwareSerial portTwo(4, 5); //RX,TX dust SDS011
dht DHT; // humidity & temperature

unsigned int Pm25 = 0;  
unsigned int Pm10 = 0;


void getPm(void) {
  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;

  portTwo.listen();
  while (portTwo.available() > 0) {
    // read the incoming byte:
    mData = portTwo.read(); 

  //  delay(1);
    if(mData == 0xAA)  {   //head1 ok
        mPkt[0] =  mData;
        mData = portTwo.read();
        if(mData == 0xc0)  {  //head2 ok 
          mPkt[1] =  mData;
          mCheck = 0;
          for(i=0;i < 6;i++)//data recv and crc calc
          {
             mPkt[i+2] = portTwo.read();
 //            delay(1);
             mCheck += mPkt[i+2];
          }
          mPkt[8] = portTwo.read();
  //        delay(1);
          mPkt[9] = portTwo.read();
          if(mCheck == mPkt[8])//crc ok
          {
            portTwo.flush();
            //Serial.write(mPkt,10);

            Pm25 = (uint16_t)mPkt[2] | (uint16_t)(mPkt[3]<<8);
            Pm10 = (uint16_t)mPkt[4] | (uint16_t)(mPkt[5]<<8);
//          Serial.print( "Pm25=");
//          Serial.print( Pm25 , DEC);
//          Serial.print( "\t");
//          Serial.print( "Pm10=");
//          Serial.print( Pm10 , DEC);
//          Serial.print( "\n");
            if(Pm25 > 9999)
              Pm25 = 9999;
            if(Pm10 > 9999)
              Pm10 = 9999;            
              //get one good packet
             return;
          }
        }      
     }
 
  }
}

void draw(void) {

  u8g.setFont(u8g_font_unifont);  // select font
  getPm();
  
  u8g.drawStr( 0, 10, "PM2.5:");
  u8g.setPrintPos(54, 10);  // set position
  u8g.print(Pm25);  // dust
  u8g.drawStr( 0, 23, "PM10:");
  u8g.setPrintPos(54, 23);  // set position
  u8g.print(Pm10);  // dust

  u8g.drawStr( 0,36, "Temp: ");
  u8g.setPrintPos(54, 36);  // set position
  u8g.print(bmp.readTemperature(),1);
  u8g.setPrintPos(90, 40);  // set position
  u8g.print( (char) 176);
  u8g.drawStr( 96,36, "C");

  //u8g.drawStr( 84,23, ",");
  //u8g.setPrintPos(92, 23);  // set position
  //u8g.print(DHT.temperature, 1);  // display temperature from DHT11
    
  u8g.drawStr(0, 49, "Hum: ");
  u8g.setPrintPos(54, 49);
  u8g.print(DHT.humidity, 0);  // display humidity from DHT11
  u8g.drawStr(70, 49, "% ");

  u8g.drawStr( 0,62,"Press: ");
  u8g.setPrintPos(54, 62);  // set position
  u8g.print(bmp.readPressure());
  u8g.drawStr( 98,62, "Pa");
    
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
 // u8g.drawStr( 0,62,"Alt: ");
 // u8g.setPrintPos(54, 62);  // set position
 // u8g.print(bmp.readAltitude(),0);

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  //  Serial.print("Real altitude = ");
  //  Serial.print(bmp.readAltitude(101500));
  // Serial.println(" meters");
}

void setup(void) {
//   Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
//   while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//   }
   portOne.begin(9600);
   portTwo.begin(9600);

   bmp.begin(); 
}

void loop(void) {
  DHT.read11(dht_apin);  // Read apin on DHT11

  u8g.firstPage();  
  do {
    draw();  
  } while( u8g.nextPage() );
  
 // delay(5000);  // Delay of 5sec before accessing DHT11 (min - 2sec)
 
  portOne.listen();
  portOne.print(Pm25); 
  portOne.print(Pm10); 
  portOne.print(bmp.readTemperature()); 
  portOne.print(DHT.humidity); 
  portOne.print(bmp.readPressure()); 
  portOne.print(bmp.readAltitude()); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
