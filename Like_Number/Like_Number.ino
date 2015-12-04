/*  draw number's APP
    drawNumber(long long_num,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
    drawFloat(float floatNumber,INT8U decimal,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
    drawFloat(float floatNumber,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
*/

#include <Process.h>
#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <stdio.h>
#include <string.h>
#include "Adafruit_NeoPixel.h"

#define TFT_ENABLE     1
#define SERIAL_ENABLE  1
#define VIBRATOR_PIN   9
#define WS2812_PIN     8
#define NUMPIXELS      8


char *url = "http://69.163.34.132/getFacebookPage.php";  //Facebook likes link

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

/* Function   : softwareReset()
 * Description: board reset by software, SP point to zero address
 * Parameter  : 
 * Return     :
 */
void softwareReset() 
{
  asm volatile ("jmp 0");
}

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
  char err[6]={'\0', '\0', '\0', '\0', '\0', '\0',};
  
  //unsigned long timestart = millis();
  
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
  for(; times > 0; times--)
  {
    vibrator(HIGH);
    pixels.clear();
    pixels.show();
    for(int i=0;i<NUMPIXELS;i++){            
      //pixels.setPixelColor(i, pixels.Color(255,255 - 255/8*i, 255/8*i)); // yellow to red
      pixels.setPixelColor(i, pixels.Color(255/8*i, 0, 255 - 255/8*i)); // blue to red.
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(100); 
    }
    vibrator(LOW);
  }
}

//Char* to unsigned long
int getNum(char *str)
{
    int num_len = 0;
    int num[10];

    for(int i=0; str[i]!='\0' && str[i] != ','; i++)
    {
        if(str[i]>='0' && str[i]<='9')
        {
            num[num_len++] = str[i]-'0';
        }
    }

    int x10[] = {1, 10, 100, 1000, 10000, 100000, 1000000};

    int views = 0;
    for(int i=0; i<num_len; i++)
    {
        views += num[i]*x10[num_len-i-1];
    }

    return views;
}

// initialize
void drawHello()
{
    Tft.fillScreen_random(0, 79, 0, 319);
    Tft.fillScreen_random(160, 239, 0, 319);
}

// draw number here
void drawNum(unsigned long num)
{
    static unsigned long num_buf = 0;
    static unsigned int x_buf = 0;

    if(num_buf == num)return;
    
    int x = 0;
    if(num < 10) x = (320-42)/2;
    else if(num < 100) x = (320-42*2)/2;
    else if(num < 1000) x = (320-42*3)/2;
    else if(num < 10000) x = (320-42*4)/2;
    else if(num < 100000) x = (320-42*5)/2;
    else if(num < 1000000) x = (320-42*6)/2;
    
    if(x_buf != x)
        Tft.drawNumber(num_buf, x_buf, 95, 7, BLACK);
    x_buf = x;
    num_buf = num; 
    Tft.drawNumber(num, x, 95, 7, WHITE);       // draw a float number: 1.2345 Location: (0, 250), size: 4, decimal: 4, color: RED   
}

uint16_t getFacebookLikes(void)
{
  //FaceBook likes
  static char likes[10];
  uint8_t _index = 0;
  uint8_t len = 0;
  
  clearStr(likes);

  char buf[32];   
  clearStr(buf);                        
  
  // Get curl contant - FacebookLikes into buf  
  len = bridge_curl(buf, url);
  
  if(len < 17){  // "curl: (7) Error"
#if SERIAL_ENABLE  
    Serial.println("Network error!");    
#endif    
    return -1;
  }
  
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
  
#if SERIAL_ENABLE
  Serial.println(buf);
  Serial.print("[ ");
  Serial.print(millis());
  Serial.print(" S ]");
  Serial.println(getNum(likes));
  
  // Ensure the last bit of data is sent.
  Serial.flush();
#endif
  
  return getNum(likes);
}

//FaceBook likes
uint16_t facebook_likes = 0;
uint16_t last_likes = 0;
  
void setup()
{
    // Initialize Bridge
    Bridge.begin();
    // Initialize Serial
    Serial.begin(9600);
    // Wait until a Serial Monitor is connected.
    //while (!Serial);
    delay(1000);
    Serial.println("Begin FaceBook Like!");
    
    // Get first facebook like number
    facebook_likes = getFacebookLikes();
    last_likes = facebook_likes;
#if TFT_ENABLE  
    TFT_BL_ON;      // turn on the background light
    Tft.TFTinit();  // init TFT library 
    drawHello();    
    drawNum(0);    
#endif
    
    pinMode(VIBRATOR_PIN, OUTPUT); //vibrator  
    pixels.begin();  
    //pixels.setBrightness(20);  
    ws2812_vibrator(1);      
}
    
void loop()
{     
start:   
    facebook_likes = getFacebookLikes(); 
    if(facebook_likes < 0)
    {
#if TFT_ENABLE
    Tft.drawString("Error",(320-42*5)/2, 97, 7, RED);     
#endif
     goto start;
    }     

    drawNum(facebook_likes);
    // If likes add ws2812 shine
    if( last_likes < facebook_likes )
    {
      ws2812_vibrator(2);
    }

    last_likes = facebook_likes;       
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/