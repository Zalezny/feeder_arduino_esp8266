#include "LiquidCrystal_I2C.h"
#include "wifi_env.h"
#include <Wire.h>
#include <Stepper.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

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

#define HTTP_REST_PORT 8080
ESP8266WebServer server(HTTP_REST_PORT);

void initTimer() {
  long currentTime = (tm.Hour * 3600L + tm.Minute * 60 + tm.Second);
  long expectedTime = ((h * 3600L) + m * 60 + s);
  countdown_time = expectedTime - currentTime;
}
void initFeeder() {

  if (!DS1307_RTC.begin()) {
    Serial.println("Couldn't find RTC");
    // while(1);
  }
  DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

  initTimer();
  lcd.begin();
  lcd.clear();
  lcd.backlight();
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(OWN_SSID, OWN_PASSWORD);
  Serial.println("");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(OWN_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if(MDNS.begin("feeder")){
    Serial.println("MDNS responder started");
  }
}
void getHelloWord() {
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

void restServer() {
  server.on("/", HTTP_GET, [](){
    server.send(200, F("text/html"), F("Welcome to the Feeder REST"));

  });
  server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
  server.on(F("/setTimer"), HTTP_POST, setTimer);

}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setTimer(){
  String postBody = server.arg("plain");
    Serial.println(postBody);
 
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
        // if the file didn't open, print an error:
        Serial.print(F("Error parsing JSON "));
        Serial.println(error.c_str());
 
        String msg = error.c_str();
 
        server.send(400, F("text/html"),
                "Error in parsing json body! <br>" + msg);
 
    } else {
        JsonObject postObj = doc.as<JsonObject>();
 
        Serial.print(F("HTTP Method: "));
        Serial.println(server.method());
 
        if (server.method() == HTTP_POST) {
            if (postObj.containsKey("hour") && postObj.containsKey("minutes")&& postObj.containsKey("seconds")) {
 
                Serial.println(F("done."));
 
                // Here store data or doing operation
                h = postObj["hour"]; 
                m = postObj["minutes"]; 
                s = postObj["seconds"]; 
                initTimer();
                // Create the response
                // To get the status of the result you can get the http status so
                // this part can be unusefully
                DynamicJsonDocument doc(512);
                doc["status"] = "OK";
 
                Serial.print(F("Stream..."));
                String buf;
                serializeJson(doc, buf);
 
                server.send(201, F("application/json"), buf);
                Serial.print(F("done."));
 
            }else {
                DynamicJsonDocument doc(512);
                doc["status"] = "KO";
                doc["message"] = F("No data found, or incorrect!");
 
                Serial.print(F("Stream..."));
                String buf;
                serializeJson(doc, buf);
 
                server.send(400, F("application/json"), buf);
                Serial.print(F("done."));
            }
        }
    }
}
void setup() {
  Serial.begin(9600);

  initFeeder();
  initWiFi();
  restServer();
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
#ifndef ESP8266
  while (!Serial);  // wait for serial port to connect. Needed for native USB
#endif
}

void loop() {
  server.handleClient();
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
