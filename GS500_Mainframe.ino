
#include "Addresses.h"
#include "CmdsLCD.h"

volatile byte fuel = 0;
volatile byte gear = 0;

// ------ SETTINGS ------
#define LCD_REFRESH_RATE 1000
// ------ END OF SETTINGS ------

// ------ CALCULATED MACROS ------
#define LCD_REFRESH_REATE_US (LCD_REFRESH_RATE * 1000)
// ------ END OF CALCULATED MACROS ------

HardwareTimer lcdTimer(2);

// RTC rad data
     volatile uint8_t _second = 0;
     volatile uint8_t _minute = 0;
     volatile uint8_t _hour = 0;
     volatile uint8_t _day = 0;
     volatile uint8_t _month = 0;
     volatile uint8_t _year = 0;
     volatile uint8_t _dayOfWeek = 0;
     volatile float _temp = 9999;

void setupTimers() {
  lcdTimer.pause();
  lcdTimer.setPeriod(LCD_REFRESH_REATE_US);
  lcdTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  lcdTimer.setCompare(TIMER_CH1, 1);
  lcdTimer.attachCompare1Interrupt(refreshLCD);
  //lcdTimer.refresh();
  //lcdTimer.resume();
}

void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);
  randomSeed(analogRead(0));
  delay(3000);
  initRTC();
  //setRTC(0, 0, 0, 6, 13, 10, 18);
  
  setupTimers();
  
}

void loop() {
}

void readGear() {
  
}

void refreshLCD(void) {
  readGear();
  refreshRTC();
  sWrite(ADDR_LCD);
  sWrite(15);
  sWrite(CMD_LCD_FUEL);
  sWrite(fuel);
  sWrite(CMD_LCD_GEAR);
  sWrite(gear);
  sWrite(CMD_LCD_TIME);
  sWrite(_day);
  sWrite(_month);
  sWrite(_year);
  sWrite(_hour);
  sWrite(_minute);
  sWrite(_second);
  int volt = analogRead(PA1);
  byte volt1 = volt / 100;
  byte volt2 = volt - volt1;
  sWrite(CMD_LCD_VOLT);
  sWrite(volt1);
  sWrite(volt2);
  sWrite(CMD_LCD_REFRESH);
  sRead(false);
  byte rSize = sRead(false);
  if(rSize)
    LCDResponse(rSize);
}

void LCDResponse(byte rSize) {
  byte cmd = sRead(false);
  if(cmd == CMD_LCD_SETTIME)
  {
      _day = sRead(false);
      _month = sRead(false);
      _year = sRead(false);
      _hour = sRead(false);
      _minute = sRead(false);
      _second = sRead(false);
      setRTC(_second, _minute, _hour, 1, _day, _month, _year);
  }
}

byte sRead(bool hw)
{
  if(hw) return Serial.read();
  else return Serial1.read();
}

bool sWrite(byte data)
{
    Serial1.write(data);
    return Serial1.read() == data;
}

