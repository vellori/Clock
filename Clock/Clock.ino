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
#define DEBUG 1
#define debug(x) if (DEBUG) { Serial.println(x); }

#define TFT_CS 5
#define TFT_DC 6
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

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
#define NUMBUTTONS 4
Adafruit_GFX_Button buttons[4];
char plus[] = "+";
char minus[] = "-";

boolean buttonShown;

void setup() {

  Serial.begin(9600);
  debug("init");
  tft.begin();
  initScreen();
  tft.setRotation(3);
  if (timeStatus() == timeNotSet) {
    debug("clock");
    setTime(10, 10, 5, 1, 9, 2015);
  }

  buttons[0].initButton(&tft,
                        HSTARTX + LETTERWIDTH / 2,
                        HSTARTY - LETTERHEIGHT ,
                        LETTERWIDTH * 1.5,
                        LETTERHEIGHT * 1.5,
                        BUTTONLINE,
                        BUTTONBG,
                        BUTTONLINE,
                        plus, 2);
  buttons[1].initButton(&tft,
                        HSTARTX + LETTERWIDTH / 2,
                        LETTERHEIGHT * 2 + HSTARTY + 2,
                        LETTERWIDTH * 1.5,
                        LETTERHEIGHT * 1.5,
                        BUTTONLINE,
                        BUTTONBG,
                        BUTTONLINE,
                        minus, 2);
  buttons[2].initButton(&tft,
                        HSTARTX + LETTERWIDTH * 2 + DELTAHHMM + LETTERWIDTH / 2,
                        HSTARTY - LETTERHEIGHT ,
                        LETTERWIDTH * 1.5,
                        LETTERHEIGHT * 1.5,
                        BUTTONLINE,
                        BUTTONBG,
                        BUTTONLINE,
                        plus, 2);
  buttons[3].initButton(&tft,
                        HSTARTX + LETTERWIDTH * 2 + DELTAHHMM + LETTERWIDTH / 2,
                        LETTERHEIGHT * 2 + HSTARTY + 2,
                        LETTERWIDTH * 1.5,
                        LETTERHEIGHT * 1.5,
                        BUTTONLINE,
                        BUTTONBG,
                        BUTTONLINE,
                        minus, 2);
  drawButtons(buttonShown);
}

void drawButtons(boolean showThem) {
  if (showThem) {
    for (int i; i < NUMBUTTONS; i++) {
      buttons[i].drawButton(false);
    }
  } else {
    initScreen();
  }
}

void initScreen() {
  tft.fillScreen(BGCOLOR);
  debug("IS");
}
uint32_t lastTouch;
void loop() {
  Point p;
  boolean refreshScreen;
  boolean forceUpdate=false;
  p = ts.getPoint();
  if (ts.isTouching() & (p.z > 300)) {
    if (!buttonShown) {
      buttonShown = true;
      drawButtons(buttonShown);
    }
    lastTouch = millis();
  } else {
    p.x = p.y = p.z = -1;
    if ((millis() - lastTouch) > 5000) {
      if (buttonShown) {
        buttonShown = false;
        drawButtons(buttonShown);
        forceUpdate = true;
      }
    }
  }

  if (p.z != -1) {
    Serial.println("X: " + String(p.x) + " Y:" + String(p.y));
    uint16_t temp  = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.x = tft.height() - map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    p.y = temp;
    
  }
#if DEBUG==1
  uint32_t myTime = millis();
  boolean updated =
#endif
    updateTimeIfNeeded(forceUpdate);
  manageButtons(p);
  reactButtons();

#if DEBUG==1
  if (updated) {
    Serial.print(millis() - myTime, DEC);
    debug("ms");
  }
#endif
}



uint8_t lastHour;
uint8_t lastMinute;
boolean intermezz;
boolean updateTimeIfNeeded(boolean force)
{
  uint8_t currentHour = hour();
  uint8_t currentMinute = minute();
  boolean updated = false;
  if ((currentHour != lastHour) | force) {
    drawHours(currentHours());
    lastHour = currentHour;
    updated = true;
  }
  if ((currentMinute != lastMinute) | force) {
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

void manageButtons(Point p)
{
  if (buttonShown)  {
    for (uint8_t b = 0; b < NUMBUTTONS; b++) {
      if (buttons[b].contains(p.x, p.y)) {
        if (!buttons[b].isPressed()) buttons[b].drawButton(true);
        buttons[b].press(true);
      } else {
        if (buttons[b].isPressed() ) buttons[b].drawButton(false);
        buttons[b].press(false);
      }
    }
  }
}

void reactButtons() {
  if (!buttonShown) {
    return;
  }
  int adj[] = {60 * 60, -60 * 60, 60, -60};
  for (int i = 0; i < NUMBUTTONS; i++) {
    if (buttons[i].isPressed() & buttons[i].justPressed()) {
      adjustTime(adj[i]);
    }
  }
}

