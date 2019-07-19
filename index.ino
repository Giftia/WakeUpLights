#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SSD1306.h>
ESP8266WebServer server(80);
static const int Light = 4;
int flag = 0;
static const String uptime = "06:30";
String now;
static const char *ssid     = "";
static const char *password = "";
int counter = 0;
MDNSResponder mdns;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "120.25.108.11", 28800, 2400);
SSD1306 display(0x3c, 0, 2, OLED_RST, GEOMETRY_128_64);
IPAddress ip;
String temp;

void handleRoot() {
  sendHtml();
};

void sendHtml() {
  server.send(200, "text/html", "<!DOCTYPE html><html lang='zh-cn'><head><meta name='renderer' content='webkit'/><meta name='force-rendering' content='webkit'/><meta http-equiv='X-UA-Compatible' content='IE=Edge,chrome=1'/><link rel='icon' href='data:image/ico;base64,aWNv'><meta http-equiv='Content-Type' content='text/html; charset=utf-8'><title>起床灯</title></head><body><div style='border-radius: 25px; border: 5px solid #555; padding: 20px; width: auto; height: auto;'><h1 style='text-align:center;font-size:100px;'>起床灯控制</h1><h1><a href='/ON'><button style='width:100%;background-color:#4CAF50;border:none;color:white;padding:15px 32px;text-align:center;text-decoration:none;display:inline-block;font-size:100px;border-radius:20px;'>灯开</button></a></h1><h1><a href='/OFF'><button style='width:100%;background-color:RED;border:none;color:white;padding:15px 32px;text-align:center;text-decoration:none;display:inline-block;font-size:100px;border-radius:20px;'>灯关</button></a></h1></div></body></html>");
};

void setup() {
  WiFi.begin(ssid, password);
  pinMode(Light, OUTPUT);
  digitalWrite(Light, LOW);
  display.init();
  display.clear();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  delay(10);
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    counter++;
    display.clear();
    int progress = (counter / 4) % 100;
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 0, String(progress) + "%");
    display.drawProgressBar(0, 24, 127, 12, progress);
    display.drawString(64, 42, "Connecting WiFi...");
    display.display();
  };
  ip = WiFi.localIP();
  int buf[3];
  for (int i = 0; i < 4; i++)
  {
    buf[i] = ip[i];
  }
  temp = String(buf[0]);
  temp += '.';
  temp += String(buf[1]);
  temp += '.';
  temp += String(buf[2]);
  temp += '.';
  temp += String(buf[3]);
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, temp);
  display.display();

  timeClient.begin();

  server.on("/", handleRoot);

  server.on("/ON", []() {
    sendHtml();
    digitalWrite(Light, HIGH);
  });

  server.on("/OFF", []() {
    sendHtml();
    digitalWrite(Light, LOW);
  });

  server.begin();
}

void loop() {
  if (flag) {
    digitalWrite(Light, HIGH);
    display.clear();
    display.drawString(64, 0, uptime);
    display.display();
    ESP.deepSleep(1000000 * 60 * 60);
  } else {
    server.handleClient();
    timeClient.update();
    now = String(timeClient.getFormattedTime()).substring(0, 5);
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 0, temp);
    display.setFont(ArialMT_Plain_24);
    display.drawString(64, 38, timeClient.getFormattedTime());
    display.display();
    if (now == uptime) {
      flag = 1;
    };
    delay(1000);
  };
}
