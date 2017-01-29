#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
#include "U8glib.h"
#include "dht.h"
#include "LowPower.h"

#define dht_apin A0
U8GLIB_SH1106_128X64 u8g(10, 9, 12, 11, 13);  // D0=SCK=10, D1=SDA=9, CS=12, DC=11, Reset=RES=13
Adafruit_BMP085 bmp;
SoftwareSerial portOne(2, 3); //bte
SoftwareSerial portTwo(4, 5); //dust

int incomingByte = 0;   // for incoming serial data
dht DHT;
unsigned int Pm25 = 0;
unsigned int Pm10 = 0;

void draw(void) {
  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;

  u8g.setFont(u8g_font_unifont);  // select font
  
  portTwo.listen();
  while (portTwo.available() > 0) {
    // read the incoming byte:
    mData = portTwo.read(); 
    Serial.print( "mPkt[0]=");
    Serial.print( mData, HEX);
    Serial.print( "\t");

    delay(2);
    if(mData == 0xAA)  {   //head1 ok
        mPkt[0] =  mData;
        mData = portTwo.read();
        Serial.print( "mPkt[1]=");
        Serial.print( mData, HEX);
        Serial.print( "\t");
        if(mData == 0xc0)  {  //head2 ok 
          mPkt[1] =  mData;
          mCheck = 0;
          for(i=0;i < 6;i++)//data recv and crc calc
          {
             mPkt[i+2] = portTwo.read();
             delay(2);
             mCheck += mPkt[i+2];
          }
          Serial.print( "mPkt[2]=");
          Serial.print( mPkt[2] , HEX);
          Serial.print( "\t");
          Serial.print( "mPkt[3]=");
          Serial.print( mPkt[3] , HEX);
          Serial.print( "\t");
          Serial.print( "mPkt[4]=");
          Serial.print( mPkt[4] , HEX);
          Serial.print( "\t");
          Serial.print( "mPkt[5]=");
          Serial.print( mPkt[5] , HEX);
          Serial.print( "\t");
          Serial.print( "mPkt[6]=");
          Serial.print( mPkt[6] , HEX);
          Serial.print( "\t");
          Serial.print( "mPkt[7]=");
          Serial.print( mPkt[7] , HEX);
          Serial.print( "\t");
          mPkt[8] = portTwo.read();
          Serial.print( "mPkt[8]=");
          Serial.print( mPkt[8] , HEX);
          Serial.print( "\t");
          delay(1);
          mPkt[9] = portTwo.read();
          Serial.print( "mPkt[9]=");
          Serial.print( mPkt[9] , HEX);
          Serial.print( "\t");
          if(mCheck == mPkt[8])//crc ok
          {
            portTwo.flush();
            //Serial.write(mPkt,10);

            Pm25 = (uint16_t)mPkt[2] | (uint16_t)(mPkt[3]<<8);
            Pm10 = (uint16_t)mPkt[4] | (uint16_t)(mPkt[5]<<8);
          Serial.print( "Pm25=");
          Serial.print( Pm25 , DEC);
          Serial.print( "\t");
          Serial.print( "Pm10=");
          Serial.print( Pm10 , DEC);
          Serial.print( "\n");
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
  u8g.drawStr( 0, 10, "P2.5");
  u8g.setPrintPos(40, 10);  // set position
  u8g.print(Pm25);  // dust
  u8g.drawStr( 70, 10, "P10");
  u8g.setPrintPos(100, 10);  // set position
  u8g.print(Pm10);  // dust

  u8g.drawStr( 0,23, "Temp: ");
  u8g.setPrintPos(54, 23);  // set position
  u8g.print(bmp.readTemperature(),1);
  u8g.setPrintPos(92, 23);  // set position
  u8g.print(DHT.temperature, 1);  // display temperature from DHT11
    
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
  u8g.print(bmp.readAltitude(),0);

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
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
   }
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
