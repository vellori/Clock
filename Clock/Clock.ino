#include <SPI.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

#include <Time.h>  

#define YP A2  
#define XM A1  
#define YM 14  
#define XP 17  
TouchScreen ts = TouchScreen(XP, YP, XM, YM);
#define TFT_CS 5
#define TFT_DC 6
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define DEBUG 1

void setup() {
  Serial.begin(9600);
  Serial.print("Init.");
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(3);
  setTime(10,10,5,1,9,2015);
}

void loop() {
#if DEBUG==1
  uint32_t myTime = millis();
  boolean updated =
#endif
  updateTimeIfNeeded();
#if DEBUG==1
  if (updated) {
    Serial.print(millis() - myTime, DEC);
    Serial.println(" ms");
  }
#endif
}



uint8_t lastHour;
uint8_t lastMinute;
boolean intermezz;
boolean updateTimeIfNeeded()
{
  uint8_t currentHour = hour();
  uint8_t currentMinute = minute();
  boolean updated=false;
  if (currentHour != lastHour) {
    drawHours(currentHours());
    lastHour = currentHour;
    updated = true;
  }
  if (currentMinute != lastMinute) {
    drawMinutes(currentMinutes());
    lastMinute = currentMinute;
    updated = true;
  }
  boolean newIntermezz;
  
  newIntermezz = (second() %2) == 1;

  if (newIntermezz != intermezz) {
    drawIntermezz(newIntermezz);
    intermezz = newIntermezz;
    updated = true;
  }
  return updated;
}
#define HSTARTX 10
#define HSTARTY 10
#define FONTSIZE 5
#define DELTAHHMM FONTSIZE*6/2 
#define FORECOLOR ILI9341_WHITE
#define BGCOLOR ILI9341_BLACK

void drawIntermezz(boolean there)
{
  tft.setCursor(HSTARTX + (FONTSIZE * 6  * 2) - (DELTAHHMM/2) -1,HSTARTY);
  tft.setTextSize(FONTSIZE);
  if (there)  {
    tft.setTextColor(FORECOLOR);
  } else {
    tft.setTextColor(BGCOLOR);
  }
  tft.println(":");
}

void drawHours(String hours) 
{
  tft.setCursor(HSTARTX,HSTARTY);
  tft.setTextSize(FONTSIZE);
  tft.setTextColor(FORECOLOR,BGCOLOR);
  tft.println(hours);
}

void drawMinutes(String minutes) 
{
  tft.setCursor(HSTARTX + (FONTSIZE * 6 * 2) + DELTAHHMM,HSTARTY);
  tft.setTextSize(FONTSIZE);
  tft.setTextColor(FORECOLOR,BGCOLOR);
  tft.println(minutes);
}

String currentHours() 
{
  String hourS = String(hour());
  return hourS.length()==1? " " + hourS:hourS;
}

String currentMinutes() 
{
  String minuteS = String(minute());
  return minuteS.length()==1? "0" + minuteS:minuteS;
}

