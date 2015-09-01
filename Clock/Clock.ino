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

static uint8_t HSTARTX, HSTARTY, FONTSIZE, DELTAHHMM;
static uint16_t FORECOLOR, BGCOLOR;
static unsigned int TS_MINX, TS_MAXX, TS_MINY, TS_MAXY;

Adafruit_GFX_Button buttons[4];
void setup() {
  HSTARTX = 10;
  HSTARTY = 10;
  FONTSIZE = 5;
  DELTAHHMM = FONTSIZE * 6 / 2;
  FORECOLOR = ILI9341_WHITE;
  BGCOLOR = ILI9341_BLACK;
  
  TS_MINX = 116*2;
  TS_MAXX = 890*2;
  TS_MINY = 83*2;
  TS_MAXY = 912*2;
  Serial.begin(9600);
  Serial.print("Init.");
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(3);
  setTime(10, 10, 5, 1, 9, 2015);

  buttons[0].initButton(&tft, FONTSIZE * 8 + HSTARTX,
                        FONTSIZE * 6 * 2+ HSTARTY, 32, 32, ILI9341_WHITE, ILI9341_ORANGE, ILI9341_WHITE, "+", 2);
  buttons[1].initButton(&tft, FONTSIZE * 8 + HSTARTX,
                        FONTSIZE * 6 * 3+ HSTARTY, 
                        32, 32, ILI9341_WHITE, ILI9341_ORANGE, ILI9341_WHITE, "-", 2);
  buttons[0].drawButton(false);
  buttons[1].drawButton(false);
}

void loop() {
  Point p;

  if (ts.isTouching()) {
    p = ts.getPoint();
    
  } else {
    p.x = p.y = p.z = -1;
  }
  if (p.z != -1) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    Serial.println(String(p.y) + "  " + String(p.x));
  }
#if DEBUG==1
  uint32_t myTime = millis();
  boolean updated =
#endif
    updateTimeIfNeeded();

  for (uint8_t b = 0; b < 2; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      Serial.print("Pressing: "); Serial.println(b);
      if (!buttons[b].isPressed()) buttons[b].drawButton(true); 
      buttons[b].press(true);  
    } else {
      if (buttons[b].isPressed() ) buttons[b].drawButton(false);
      buttons[b].press(false); 
      
    }
    
  }
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
  boolean updated = false;
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

  newIntermezz = (second() % 2) == 1;

  if (newIntermezz != intermezz) {
    drawIntermezz(newIntermezz);
    intermezz = newIntermezz;
    updated = true;
  }
  return updated;
}

void drawIntermezz(boolean there)
{
  tft.setCursor(HSTARTX + (FONTSIZE * 6  * 2) - (DELTAHHMM / 2) - 1, HSTARTY);
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
  tft.setCursor(HSTARTX, HSTARTY);
  tft.setTextSize(FONTSIZE);
  tft.setTextColor(FORECOLOR, BGCOLOR);
  tft.println(hours);
}

void drawMinutes(String minutes)
{
  tft.setCursor(HSTARTX + (FONTSIZE * 6 * 2) + DELTAHHMM, HSTARTY);
  tft.setTextSize(FONTSIZE);
  tft.setTextColor(FORECOLOR, BGCOLOR);
  tft.println(minutes);
}

String currentHours()
{
  String hourS = String(hour());
  return hourS.length() == 1 ? " " + hourS : hourS;
}

String currentMinutes()
{
  String minuteS = String(minute());
  return minuteS.length() == 1 ? "0" + minuteS : minuteS;
}

