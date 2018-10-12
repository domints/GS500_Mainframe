#include <Wire.h>

bool initEeprom = false;
#define uRTCLIB_STM32_INIT_FIX() { if (!initEeprom) { initEeprom = true; eeprom_read(0); delay(10); } }

#define uRTCLIB_decToBcd(val) ((uint8_t) ((val / 10 * 16) + (val % 10)))
#define uRTCLIB_bcdToDec(val) ((uint8_t) ((val / 16 * 10) + (val % 16)))
#define uRTCLIB_WIRE_DELAY 5

int _rtc_address = 0x68;
int _ee_address = 0x57;

      

TwoWire wire(PB10, PB11, SOFT_STANDARD);

void initRTC()
{
  wire.begin();
}

void refreshRTC() {
  wire.beginTransmission(_rtc_address);
  wire.write(0); // set DS3231 register pointer to 00h
  wire.endTransmission();
  wire.requestFrom(_rtc_address, 7);
  // request seven uint8_ts of data starting from register 00h
  _second = wire.read();
  _second = uRTCLIB_bcdToDec(_second);
  _minute = wire.read();
  _minute = uRTCLIB_bcdToDec(_minute);
  _hour = wire.read() & 0b111111;
  _hour = uRTCLIB_bcdToDec(_hour);
  _dayOfWeek = wire.read();
  _dayOfWeek = uRTCLIB_bcdToDec(_dayOfWeek);
  _day = wire.read();
  _day = uRTCLIB_bcdToDec(_day);
  _month = wire.read();
  _month = uRTCLIB_bcdToDec(_month);
  _year = wire.read();
  _year = uRTCLIB_bcdToDec(_year);

   byte tMSB, tLSB;
     // temp registers (11h-12h) get updated automatically every 64s
     wire.beginTransmission(_rtc_address);
     wire.write(0x11);
     wire.endTransmission();
     wire.requestFrom(_rtc_address, 2);

     // Should I do more "if available" checks here?
    if(wire.available()) {
    tMSB = wire.read(); //2's complement int portion
    tLSB = wire.read(); //fraction portion
    _temp = ((((short)tMSB << 8) | (short)tLSB) >> 6) / 4.0;
    }
    else {
    _temp = 9999; // Some obvious error value
    }
}

void setRTC(const uint8_t second, const uint8_t minute, const uint8_t hour, const uint8_t dayOfWeek, const uint8_t dayOfMonth, const uint8_t month, const uint8_t year) {
  wire.beginTransmission(_rtc_address);
  wire.write(0); // set next input to start at the seconds register
  wire.write(uRTCLIB_decToBcd(second)); // set seconds
  wire.write(uRTCLIB_decToBcd(minute)); // set minutes
  wire.write(uRTCLIB_decToBcd(hour)); // set hours
  wire.write(uRTCLIB_decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  wire.write(uRTCLIB_decToBcd(dayOfMonth)); // set date (1 to 31)
  wire.write(uRTCLIB_decToBcd(month)); // set month
  wire.write(uRTCLIB_decToBcd(year)); // set year (0 to 99)
  wire.endTransmission();
}

byte eeprom_read(const unsigned int address) {
  uRTCLIB_STM32_INIT_FIX()
  byte rdata = 0xFF;
  wire.beginTransmission(_ee_address);
  wire.write((int)(address >> 8)); // MSB
  wire.write((int)(address & 0xFF)); // LSB
    delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
  if (wire.endTransmission() == 0) {
    wire.requestFrom(_ee_address, 1);
        delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
    if(wire.available()) {
      rdata = (byte) wire.read();
            delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
    }
  }
  return rdata;
}

bool eeprom_read(const unsigned int address, byte *data, const uint8_t n) {
    bool ret = false;
  uRTCLIB_STM32_INIT_FIX()
  wire.beginTransmission(_ee_address);
  wire.write((int)(address >> 8)); // MSB
  wire.write((int)(address & 0xFF)); // LSB
    delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
  if (wire.endTransmission() == 0) {
    wire.requestFrom(_ee_address, (int) n);
        delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
    if(wire.available()) {
      byte i = 0, j;
            for (; i < n && wire.available(); i++) {
                *(data + i) = (byte) wire.read();
            delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
        // Added to wait if needed but cut after a failure (timeout)
              for (j = 0; j < 255 && !wire.available(); j++) {
              delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
        }
            }
            ret = (i == n);
    }
  }
  return ret;
}

bool eeprom_write(const unsigned int address, const byte data) {
  wire.beginTransmission(_ee_address);
  wire.write((int)(address >> 8)); // MSB
  wire.write((int)(address & 0xFF)); // LSB
  wire.write(data);
  delay(uRTCLIB_WIRE_DELAY); // Little delay to assure EEPROM is able to process data; if missing and inside for look meses some values
  return wire.endTransmission() == 0;
}

bool eeprom_write(const unsigned int address, void *data, const uint8_t n) {
  bool r = true;
  uint8_t i;
  for (i = 0; i < n; i++) {
    r &= eeprom_write(address + i, (byte) *(((byte *) data) + i));
  }
Serial.println();
  return r;
}
