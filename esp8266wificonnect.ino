#include <ESP8266HTTPClient.h>
#include<ESP8266WiFi.h>
#include<ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ssid "TanmayNord"
#define password "wifi1234"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.clearDisplay();

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    display.print(".");
    display.display();
  }

  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Cnnected to ");
  display.print(ssid);
  display.display();
  Serial.print("IP");
  Serial.println(WiFi.localIP());

  delay(1000);

  display.clearDisplay();
}

void loop() {
  String time;

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Sending request");

    //HTTP Details
    String url = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";
    http.begin(client, url);
    // http.setAuthorization("Basic token");
    //http/setAuthorization("Bearer token");

    int responseCode = http.GET();
    Serial.print("response code");
    Serial.println(responseCode);

    if (responseCode == HTTP_CODE_OK) 
    {
          auto payload = http.getString();
          // Serial.println(payload);

          StaticJsonDocument<32> filter;
          filter["datetime"] = true;
          filter["timezone"] = true;

          DynamicJsonDocument doc(1024);
          deserializeJson(doc, payload,DeserializationOption::Filter(filter));

          const char* datetime = doc["datetime"]; // "2023-11-21T14:33:17.967466+05:30"
          const char* timezone = doc["timezone"];      
          time = datetime;
          // Serial.println(time);
    } 
    else 
    {
        Serial.printf("Error Code: ", responseCode);
    }
    http.end();
  //2023-11-21T15:21:16.510294+05:30
    String year = time.substring(0,4);
    String month = time.substring(5,7);
    String day =  time.substring(8,10);
    String otime = time.substring(11,19);
    String date = day+"/"+month+"/"+year;

    display.clearDisplay();

    display.setTextSize(1);
    int textWidth = date.length() * 6; 
    int x1 = (SCREEN_WIDTH - textWidth) / 2;
    int y1 = SCREEN_HEIGHT / 2;
    display.setCursor(x1,10);
    display.println(date);

    display.setTextSize(2);
    int textWidth2 = otime.length() * 12; 
    int x2 = (SCREEN_WIDTH - textWidth) / 2;
    int y2 = SCREEN_HEIGHT / 2;
    display.setCursor(x2-20,30);
    display.println(otime);
    display.display();
    
    delay(900);
}}
