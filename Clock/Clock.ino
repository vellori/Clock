#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

#include <Time.h>

#define YP A2
#define XM A1
#define YM 18
#define XP 21
TouchScreen ts = TouchScreen(XP, YP, XM, YM);

#define TFT_CS 5
#define TFT_DC 6
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define DEBUG 0

 #define HSTARTX 15
 #define HSTARTY  90
 #define FONTSIZE 7
 #define LETTERWIDTH FONTSIZE*8
 #define LETTERHEIGHT FONTSIZE*6
 #define DELTAHHMM FONTSIZE * 6 / 2
 #define FORECOLOR ILI9341_WHITE
 #define BGCOLOR ILI9341_BLACK
 #define BUTTONBG ILI9341_ORANGE
 #define BUTTONTEXT ILI9341_WHITE
 #define BUTTONLINE ILI9341_WHITE
 #define TS_MINX 116*2
 #define TS_MAXX 890*2
 #define TS_MINY 83*2
 #define TS_MAXY 912*2

Adafruit_GFX_Button buttons[4];
char plus[]="+";
char minus[]="-";
void setup() {

  Serial.begin(9600);
  Serial.println("Init");
  tft.begin();
  tft.fillScreen(BGCOLOR);
  tft.setRotation(3);
  if (timeStatus()==timeNotSet) {
    Serial.println("Clock");
    setTime(10, 10, 5, 1, 9, 2015);
  }

  buttons[0].initButton(&tft, 
                        HSTARTX+LETTERWIDTH/2,
                        HSTARTY - LETTERHEIGHT , 
                        LETTERWIDTH *1.5, 
                        LETTERHEIGHT*1.5, 
                        BUTTONLINE, 
                        BUTTONBG, 
                        BUTTONLINE, 
                        plus, 2);
  buttons[1].initButton(&tft, 
                        HSTARTX+LETTERWIDTH/2,
                        LETTERHEIGHT * 2+ HSTARTY+2, 
                        LETTERWIDTH*1.5, 
                        LETTERHEIGHT*1.5, 
                        BUTTONLINE, 
                        BUTTONBG, 
                        BUTTONLINE, 
                        minus, 2);
buttons[2].initButton(&tft, 
                        HSTARTX+LETTERWIDTH*2 + DELTAHHMM + LETTERWIDTH/2,
                        HSTARTY - LETTERHEIGHT , 
                        LETTERWIDTH *1.5, 
                        LETTERHEIGHT*1.5, 
                        BUTTONLINE, 
                        BUTTONBG, 
                        BUTTONLINE, 
                        plus, 2);
  buttons[3].initButton(&tft, 
                        HSTARTX+LETTERWIDTH*2 + DELTAHHMM + LETTERWIDTH/2,
                        LETTERHEIGHT * 2+ HSTARTY+2, 
                        LETTERWIDTH*1.5, 
                        LETTERHEIGHT*1.5, 
                        BUTTONLINE, 
                        BUTTONBG, 
                        BUTTONLINE, 
                        minus, 2);
  buttons[0].drawButton(false);
  buttons[1].drawButton(false);
  buttons[2].drawButton(false);
  buttons[3].drawButton(false);
}

void loop() {
  Point p;

  if (ts.isTouching()) {
    p = ts.getPoint();
  } else {
    p.x = p.y = p.z = -1;
  }
  if (p.z != -1) {
    uint16_t temp  = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.x =tft.height()- map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    p.y = temp;
  }
#if DEBUG==1
  uint32_t myTime = millis();
  boolean updated =
#endif
    updateTimeIfNeeded();
    manageButtons(p);
    reactButtons();

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
inline boolean updateTimeIfNeeded()
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

inline void drawIntermezz(boolean there)
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

inline void drawHours(String hours)
{
  tft.setCursor(HSTARTX, HSTARTY);
  tft.setTextSize(FONTSIZE);
  tft.setTextColor(FORECOLOR, BGCOLOR);
  tft.println(hours);
}

inline void drawMinutes(String minutes)
{
  tft.setCursor(HSTARTX + (FONTSIZE * 6 * 2) + DELTAHHMM, HSTARTY);
  tft.setTextSize(FONTSIZE);
  tft.setTextColor(FORECOLOR, BGCOLOR);
  tft.println(minutes);
}

inline String currentHours()
{
  String hourS = String(hour());
  return hourS.length() == 1 ? " " + hourS : hourS;
}

inline String currentMinutes()
{
  String minuteS = String(minute());
  return minuteS.length() == 1 ? "0" + minuteS : minuteS;
}

inline void manageButtons(Point p)
{
    for (uint8_t b = 0; b < 4; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      if (!buttons[b].isPressed()) buttons[b].drawButton(true); 
      buttons[b].press(true);  
    } else {
      if (buttons[b].isPressed() ) buttons[b].drawButton(false);
      buttons[b].press(false); 
    }
  }
}

inline void reactButtons() {
 int adj[] = {60*60,-60*60,60,-60};
  for (int i=0;i<4;i++) {
    if (buttons[i].isPressed() & buttons[i].justPressed()) {
      adjustTime(adj[i]);
    }
  }
}

