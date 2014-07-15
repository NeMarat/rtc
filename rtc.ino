// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 
#include <Wire.h>
#include "RTClib.h"
#include "rtc.h"
#include "rtcSave.h"
 
RTC_DS1307 RTC;
/* заполнение значений по-умолчанию: если внесено прошедшее время, при первом включении будет срабатывание */
unsigned long nextTime = 10; 
byte interval = 4;       /* интервал в днях */
int active_time = 1200; /* продолжительность включенного состояния */
int cur_time;
bool cur_state = false;
DateTime nowTime = RTC.now();
 
void setup () {
    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
    pinMode(POMP_PIN, OUTPUT);
    digitalWrite(POMP_PIN, HIGH);
    
    if (RTC.readnvram(NEXT_CRC_ADDR) != NEXT_CRC_VAL) {
      writeLong(nowTime.unixtime(), NEXT_TIME_ADDR, &RTC);
      RTC.writenvram(INTERVAL_ADDR, interval);  
      writeInt(active_time, LENGTH_ACT_ADDR, &RTC); 
      RTC.writenvram(NEXT_CRC_ADDR, NEXT_CRC_VAL); /* для перенастройки должно отличаться от предыдущего значения */
    } else {
      nextTime = readLong(NEXT_TIME_ADDR, &RTC);
      interval = RTC.readnvram(INTERVAL_ADDR);
      active_time = readInt(LENGTH_ACT_ADDR, &RTC);
    }
 
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 
}
 
void loop () {
    nowTime = RTC.now();
    
    if (cur_state == true) {
      cur_time++;  /* предполагается задержка между проходами цикла в одну секунду */
      if (cur_time > active_time) {
        digitalWrite(POMP_PIN, HIGH);
        cur_state = false;
        nextTime = nowTime.unixtime() + interval * 86400L;
        writeLong(nextTime, NEXT_TIME_ADDR, &RTC);
      }
    }
    
    if (nowTime.unixtime() >= nextTime
      && cur_state == false) {
        digitalWrite(POMP_PIN, LOW);
        cur_state = true;
        cur_time = 0;
    }
/*
    Serial.print("next day ");
    Serial.print(nextDay, DEC);
    Serial.print("; ");
    Serial.print("cur_time ");
    Serial.print(cur_time, DEC);
    Serial.print("; ");
    Serial.print("active_time ");
    Serial.print(active_time, DEC);
    Serial.println();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
*/  
    delay(1000);

}
