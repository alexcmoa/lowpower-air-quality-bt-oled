#include "U8glib.h"
#include "dht.h"

#define dht_apin A0

dht DHT;

U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9, 8);  // D0=13, D1=11, CS=10, DC=9, Reset=8

void draw(void) {
//  u8g.drawBitmapP( 76, 5, 6, 50, brainy_bitmap);  // put bitmap
  u8g.setFont(u8g_font_unifont);  // select font
//  u8g.drawStr( 0, 18, "Astrid vous aime");
 u8g.drawStr( 0, 17, "Astrid");
 u8g.drawStr( 30, 28, "vous aime !!");
// u8g.drawStr( 90, 34, "aime");
//  u8g.drawStr( 0, 18, "Alexandre");

  u8g.drawStr(10, 48, "Temp: ");  // put string of display at position X, Y
  u8g.drawStr(10, 60, "Hum: ");
  u8g.setPrintPos(54, 48);  // set position
  u8g.print(DHT.temperature, 0);  // display temperature from DHT11
  u8g.drawStr(70, 48, "c ");
  u8g.setPrintPos(54, 60);
  u8g.print(DHT.humidity, 0);  // display humidity from DHT11
  u8g.drawStr(70, 60, "% ");
}

void setup(void) {
}

void loop(void) {
  DHT.read11(dht_apin);  // Read apin on DHT11

  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  delay(5000);  // Delay of 5sec before accessing DHT11 (min - 2sec)
}
