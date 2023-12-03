#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
// #include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ssid "Wifi Name"
#define password "password"

#define HOST "weather.visualcrossing.com"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient client;
WiFiClientSecure sclient;
HTTPClient http;
// HTTPClient https;

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

  sclient.setInsecure();

}

void loop() {
  String time;
  String rDate;

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Sending request");

    //HTTP Details
    //Srting usls = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/Lucknow,IN?key=LDW572622H8U67G76L54FACW8";
    String url = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";
    http.begin(client, url);

    //http.setAuthorization("Basic token");
    //http.setAuthorization("Bearer token");

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

    if (!sclient.connect(HOST, 443))
  {
    Serial.println(F("Connection failed"));
  }

  yield();
  rDate = time.substring(0,10);
  String urls = "/VisualCrossingWebServices/rest/services/timeline/Lucknow,IN/"+rDate+"?key=LDW572622H8U67G76L54FACW8";
  sclient.print(F("GET "));
  sclient.print(urls);
  sclient.println(F(" HTTP/1.1"));

  //Headers
  sclient.print(F("Host: "));
  sclient.println(HOST);

  sclient.println(F("Cache-Control: no-cache"));

  if (sclient.println() == 0)
  {
    Serial.println(F("Failed to send request"));
  }

  char status[32] = {0};
  sclient.readBytesUntil('\r', status, sizeof(status));
  Serial.println(status);

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!sclient.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
  }

  while (sclient.available())
  {
    String weatherData = sclient.readStringUntil('\n');
    // Serial.print(line);
  StaticJsonDocument<224> filter;
    filter["latitude"] = true;
    filter["longitude"] = true;
    filter["resolvedAddress"] = true;
    filter["description"] = true;

  JsonObject filter_days_0 = filter["days"].createNestedObject();
    filter_days_0["datetime"] = true;
    filter_days_0["tempmax"] = true;
    filter_days_0["tempmin"] = true;
    filter_days_0["temp"] = true;
    filter_days_0["dew"] = true;
    filter_days_0["humidity"] = true;
    filter_days_0["conditions"] = true;
    filter_days_0["description"] = true;

  DynamicJsonDocument doc(512);
  deserializeJson(doc, weatherData, DeserializationOption::Filter(filter));

  float latitude = doc["latitude"]; // 26.8547
  float longitude = doc["longitude"]; // 80.9213
  const char* resolvedAddress = doc["resolvedAddress"]; // "Lucknow, Uttar Pradesh, India"

  JsonObject days_0 = doc["days"][0];
  const char* datetime = days_0["datetime"]; // "2020-12-15"
  String tempmax = days_0["tempmax"]; // 72.2
  String tempmin = days_0["tempmin"]; // 54.2
  String temp = days_0["temp"]; // 61.2
  String dew = days_0["dew"]; // 51.7
  String humidity = days_0["humidity"]; // 73.2
  const char* conditions = days_0["conditions"]; // "Partially cloudy"
  const char* description = days_0["description"]; // "Partly cloudy throughout the day."


  Serial.print(temp);
  }

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
