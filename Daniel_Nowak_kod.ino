#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <Stepper.h>
#include <TimeLib.h>
#include <RTClib.h>

unsigned long previous = 0;

RTC_DS1307 DS1307_RTC;

#define IN1 14
#define IN2 12
#define IN3 13
#define IN4 15

LiquidCrystal_I2C lcd(0x27, 20, 4);
Stepper mojSilnik(32, IN1, IN3, IN2, IN4);
tmElements_t tm;

int h = 1;
long m = 0;
long s = 10;
long int countdown_time;

void initTimer() {
  long currentTime = (tm.Hour * 3600L + tm.Minute * 60 + tm.Second);
  long expectedTime = ((h * 3600L) + m * 60 + s);
  countdown_time = expectedTime - currentTime;
}
void initFeeder() {
  if (!DS1307_RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  initTimer();
  lcd.begin();
  lcd.clear();
  lcd.backlight();
}

void setup() {
  Serial.begin(9600);
#ifndef ESP8266
  while (!Serial);  // wait for serial port to connect. Needed for native USB
#endif
  initFeeder();
  while (!Serial);
}

void loop() {
  long countdowntime_seconds = countdown_time - ((millis() / 1000) - previous);
  if (countdowntime_seconds >= 0) {
    long countdown_hour = countdowntime_seconds / 3600;
    long countdown_minute = ((countdowntime_seconds / 60) % 60);
    long countdown_sec = countdowntime_seconds % 60;
    lcd.setCursor(0, 0);
    lcd.print("Czas do wydania: ");
    lcd.setCursor(0, 1);
    if (countdown_hour < 10) {
      lcd.print("0");
    }
    lcd.print(countdown_hour);
    lcd.print(":");
    if (countdown_minute < 10) {
      lcd.print("0");
    }
    lcd.print(countdown_minute);
    lcd.print(":");
    if (countdown_sec < 10) {
      lcd.print("0");
    }
    lcd.print(countdown_sec);
    if (countdowntime_seconds == 0) {
      lcd.setCursor(0, 1);
      lcd.print("");
      lcd.print("Wydawanie...");
      mojSilnik.setSpeed(1000);
      mojSilnik.step(-10240);
      lcd.clear();
      previous = (millis() / 1000);
      initTimer();
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
  }
}
