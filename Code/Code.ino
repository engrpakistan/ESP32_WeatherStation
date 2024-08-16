/*
 ----------------------------------------------------------------------------------
 Project Name: ESP32 Weather Station
 It is intended for educational purposes to help users learn about ESP32 ,API,OLED,LOPAKA.

 Features:
  -> No exrenal sensors
  -> Access weather of any city
  -> DIY at home
  ->Easy to complete

 Attribution:
 If you use or modify this code, please give appropriate credit to the original author.
 Link to the original repository: https://github.com/engrpakistan

 Disclaimer:
 This code is provided "as-is" without any warranties. Use it at your own risk.

 ----------------------------------------------------------------------------------


---------------------------------------------- WEBSITE LINKS ----------------------------------------------------------------------

OPEN WEATHER 👉https://openweathermap.org/current

LOPAKA                👉https://lopaka.app/

-----------------------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------  CODE AND WIRING DIAGRAM ---------------------------------------------

GITHUB LINK 👉 https://github.com/engrpakistan

-----------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------- SOCIAL MEDIA ---------------------------------------------------------------------- 

Linktree   👉 https://linktr.ee/Engineer_from_Pakistan 

Instagram  👉https://www.instagram.com/engr_from_pakistan

Youtube    👉https://www.youtube.com/@EngineerFromPakistan/videos

LinkedIn   👉https://www.linkedin.com/in/malik-maaz-27165723b

TikTok     👉 https://www.tiktok.com/@engineerfrompakistan?_t=8nQvHGNBg6U&_r=1
-----------------------------------------------------------------------------------------------------------------------------------------------
If you have any queries EMAIL at engineerfrompakistan003@gmail.com.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const char* ssid = "your_ssid"; //Enter your WiFi name 
const char* password = "your_password"; //Enter your WiFi password

String ApiKey = "your_API_KEY";//Get your API key from open weather

// METHOD 1 LONGITUDE AND LATITUDE
String URL1 = "https://api.openweathermap.org/data/2.5/weather?lat="; //URL for Longitude and Latitude 
String lon = "your_longitude"; //Enter your longitude 
String lat = "your_latitude"; //Enter your latitiude

// METHOD 2 YOUR CITY
String URL = "https://api.openweathermap.org/data/2.5/weather?q="; //URL for your City
String your_city = "your_city"; //Enter your city name

int LED = 5;

void setup() {
  pinMode(LED,OUTPUT);
  Serial.print(115200);
  u8g2.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    no_wifi(); // Display Error message
  }
  local_ip(); //displays ip address on OLED Display

  digitalWrite(LED,HIGH);
  delay(1000);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    //  Use either one of the URL one URL is for method 1 and second is for method 2 
    // Dont use both at the same time. Must comment one URL before uploading

    // http.begin(URL1 + lat + "&lon=" + lon + "&appid=" + ApiKey + "&units=metric"); // URL for Longitude and Latitude

    http.begin(URL+ your_city + "&appid=" + ApiKey + "&units=metric"); // URL for your city 


    int httpJSON = http.GET();

    if (httpJSON > 0) {
      String JSON_Data = http.getString();
      Serial.println(JSON_Data);
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      //Storing information that it received from the JSON file in the variables
      JsonObject obj = doc.as<JsonObject>();
      const char* description = obj["weather"][0]["main"].as<const char*>();
      const float temperature = obj["main"]["temp"].as<float>();
      const float feel = obj["main"]["feels_like"].as<float>();
      const float wind = obj["wind"]["speed"].as<float>();
      const char* city = obj["name"].as<const char*>();
      const char* country = obj["sys"]["country"].as<const char*>();
      const float humidity = obj["main"]["humidity"].as<float>();
      //Converting the information into String Data type
      String descriptionStr = String(description);
      String temperatureStr = String(temperature, 1);  // 1 decimal place
      String feelStr = String(feel, 1);         // 1 decimal place
      String windStr = String(wind, 1);         // 1 decimal place
      String cityStr = String(city);
      String countryStr = String(country);
      String humidityStr = String(humidity, 0);

      //Passing the variable to the display function 
      displayWeather(cityStr, countryStr, temperatureStr, feelStr, humidityStr, windStr, descriptionStr);

      blink();
      delay(30000);  // Delay for 1 minute
    } else {
      error();
    }
    http.end();
  }
}

void displayWeather(String City, String Country, String Temp, String feel, String Humd, String Wind, String Desp) {
  char buffer[32];
  String city = City;
  String country = Country;
  String temp = Temp;
  String flt = feel;
  String humd = Humd;
  String wind = Wind;
  String desp = Desp;
  //Lopaka design to display all the information.
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  String location = city + " , " + country;

  u8g2.setFont(u8g2_font_4x6_tr);
  location.toCharArray(buffer, location.length() + 1);
  u8g2.drawStr(5, 53, buffer);
  u8g2.drawStr(3, 62, "ENGRPAK");
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);

  u8g2.drawStr(78, 19, ("Temp:" + temp).c_str());
  u8g2.setFont(u8g2_font_4x6_tr);
  u8g2.drawStr(78, 28, ("FEEL:" + flt).c_str());
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  u8g2.drawStr(78, 45, ("Humd:" + humd).c_str());
  u8g2.setFont(u8g2_font_profont22_tr);
  desp.toCharArray(buffer, desp.length() + 1);
  u8g2.drawStr(5, 37, buffer);
  u8g2.drawFrame(0, 7, 127, 49);
  u8g2.setFont(u8g2_font_4x6_tr);
  u8g2.drawStr(35, 6, "Weather Station");
  u8g2.drawLine(34, 57, 34, 61);
  u8g2.drawLine(0, 0, 0, 0);
  u8g2.drawLine(35, 58, 35, 60);
  u8g2.drawLine(39, 57, 39, 61);
  u8g2.drawLine(36, 59, 36, 59);
  u8g2.drawLine(40, 58, 40, 60);
  u8g2.drawLine(41, 59, 41, 59);
  u8g2.drawLine(44, 57, 44, 61);
  u8g2.drawLine(45, 58, 45, 60);
  u8g2.drawLine(46, 59, 46, 59);
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(6, 17, ("Wind:" + wind + "m/s").c_str());
  u8g2.sendBuffer();
}
void no_wifi() {
  static const unsigned char image_wifi_not_connected_bits[] U8X8_PROGMEM = { 0x84, 0x0f, 0x00, 0x68, 0x30, 0x00, 0x10, 0xc0, 0x00, 0xa4, 0x0f, 0x01, 0x42, 0x30, 0x02, 0x91, 0x40, 0x04, 0x08, 0x85, 0x00, 0xc4, 0x1a, 0x01, 0x20, 0x24, 0x00, 0x10, 0x4a, 0x00, 0x80, 0x15, 0x00, 0x40, 0x20, 0x00, 0x00, 0x42, 0x00, 0x00, 0x85, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00 };
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(54, 13, 19, 16, image_wifi_not_connected_bits);
  u8g2.setFont(u8g2_font_profont11_tr);
  u8g2.drawStr(16, 39, "NOT CONNECTED !!!");
  u8g2.sendBuffer();
}
void local_ip() {
  char buffer[32];
  IPAddress localIP = WiFi.localIP();

  String ipString = "IP:" + localIP.toString();

  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(38, 27, "IP ADDRESS");
  u8g2.setFont(u8g2_font_profont17_tr);
  ipString.toCharArray(buffer,ipString.length()+1);
  u8g2.drawStr(5, 42, buffer);
  u8g2.sendBuffer();
}
void error() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont29_tr);
  u8g2.drawStr(14, 38, "ERROR !!!");
  u8g2.sendBuffer();
}
void blink(){
   digitalWrite(LED,LOW);
  delay(500);
  digitalWrite(LED,HIGH);
  delay(2000);
  digitalWrite(LED,LOW);
  delay(500);
}