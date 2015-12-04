/*
  Facebook likes detect machain
  Author: Lambor & Looove
 */

#include <Process.h>
#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <stdio.h>
#include <string.h>
#include "Adafruit_NeoPixel.h"

#define TFT_ENABLE     1

#define VIBRATOR_PIN   9

#define WS2812_PIN     8
#define NUMPIXELS      8

char *url = "http://69.163.34.132/getFacebookPage.php";  //Facebook likes link

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

//Clear buf
void clearStr(char *str)
{  
  for(int i=0; i < sizeof(str); i++)
  {
    str[i] = '\0';
  }
}
/*****************************************************/
// Function: void writeText(uint16_t size, uint16_t x, uint16_t y, char *buf
// param size: 1, 2, 3, 4  not work now
// param x: 0~12
// param y: 0~8
// Note: Wrie text to TFT screen, choose font size 3       
//       Screen revolution redefined to 12 * 8
/******************************************************/
void writeText(uint16_t size, uint16_t x, uint16_t y, char *buf)
{
  
  uint16_t X_now = 0;
  uint16_t Y_now = 0;
  uint16_t X_set = x;
  uint16_t Y_set = y;
  uint16_t Xmax = 240; 
  uint16_t Ymax = 320;
  
  int len = strlen(buf);
  for(int i = 0; i < len; i++)
  {    
    if(X_set >= 12)
    {
      Y_set += 1;
      X_set = 0;
    }
    X_now = 20 * X_set++;
    Y_now = 40 * Y_set;    
        
    Tft.drawChar(buf[i], X_now, Y_now, 3, GREEN); //draw char: buf, (0, 0), size: 3, color: GREEN  
  }
}

/*******************************************************************/
// Function: uint8_t bridge_curl(char *buf)
// param buf: buffer to get contant back
// return: index, contant length
// Note: Using yunbridge API Process, and string shell command "curl"
//       Finally get contant back 
/********************************************************************/
uint8_t bridge_curl(char *buf, char *url)
{
  uint8_t index = 0;
  
  Process p;    // Create a process and call it "p"
  p.begin("curl");  // Process that launch the "curl" command
  p.addParameter(url); // Add the URL parameter to "curl"
  p.run();    // Run the process and wait for its termination

  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available() > 0) {
    char c = p.read();
    buf[index++] = c;    
  }
  buf[index] = '\0';
  
  return index;
}

/*******************************************************************/
// Function: void ws2812_shine(int times)
// param times: led strip shines for times
// Note:  
/********************************************************************/
void ws2812_shine(int times)
{
  for(; times >= 0; times--)
  {
    for(int i=0;i<NUMPIXELS;i++){      
      //pixels.setPixelColor(i, pixels.Color(0,255 - 255/8*i, 255/8*i)); // Moderately bright green color.
      pixels.setPixelColor(i, pixels.Color(255,255 - 255/8*i, 255/8*i)); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(100); 
    }
  }
}

/*******************************************************************/
// Function: void vibrator(int on_off)
// param on_off: true ro false
// Note: Turn on or turn off vibrator. 
/********************************************************************/
void vibrator(bool on_off)
{
  on_off ? digitalWrite(VIBRATOR_PIN, HIGH) : digitalWrite(VIBRATOR_PIN, LOW);
}

/*******************************************************************/
// Function: void ws2812_vibrator(int times)
// param times: times 
// Note:  Action of ws2812 and vibrator
/********************************************************************/
void ws2812_vibrator(int times)
{
  for(; times >= 0; times--)
  {
    vibrator(HIGH);
    for(int i=0;i<NUMPIXELS;i++){      
      //pixels.setPixelColor(i, pixels.Color(0,255 - 255/8*i, 255/8*i)); // Moderately bright green color.
      pixels.setPixelColor(i, pixels.Color(255,255 - 255/8*i, 255/8*i)); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(100); 
    }
    vibrator(LOW);
  }
}

void setup() {
  // Initialize Bridge
  Bridge.begin();
  // Initialize Serial
  Serial.begin(9600);
  // Wait until a Serial Monitor is connected.
  //while (!Serial);
  delay(1000);
  Serial.println("Begin FaceBook Like!");
  
#if TFT_ENABLE  
  TFT_BL_ON;      // turn on the background light
  Tft.TFTinit();  // init TFT library  
#endif
  
  pinMode(VIBRATOR_PIN, OUTPUT); //vibrator
  
  pixels.begin();  
  //pixels.setBrightness(20);  
  ws2812_vibrator(1);
}

void loop() {
    
  uint8_t _index = 0;
  uint8_t len = 0;  
  
  char buf[32];   
  clearStr(buf);
  
  //uint16_t likes_int = 0;
  char likes[10];
  clearStr(likes);
  
  uint16_t last_likes = atoi(likes);  
  
  Serial.print("last_likes: ");
  Serial.println(last_likes);
  
  // Get curl contant - FacebookLikes into buf  
  len = bridge_curl(buf, url);
  
  // Divide string by ':', fetch the string number of likes
  _index = len;  
  while(_index--)
  {
    if(buf[_index] == ':') 
      break;
  }  
  
  
  uint8_t i = 0;  
  while( _index++ != len - 2 )
  {
    likes[i++] = buf[_index];
  }
  likes[i] = '\0';
  Serial.println(buf);
  Serial.print("[ ");
  Serial.print(millis());
  Serial.print(" S ]");
  Serial.println(likes);
  
  // Ensure the last bit of data is sent.
  Serial.flush();   
     
#if TFT_ENABLE
  Tft.fillScreen(0,239,0,319,BLUE);
  writeText(0, 0, 1, "FaceBookLike");  // revsolution: 12*8
  //writeText(0, 0, 3, buf);
  writeText(0, 4, 3, likes);
#endif
  
  // If likes add ws2812 shine
  if( last_likes < atoi(likes) )
  {
    ws2812_shine(2);
  }
  
  delay(2000);
}