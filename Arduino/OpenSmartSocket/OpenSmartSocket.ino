#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>     // for time(), localtime()

// NTP設定
#define TIMEZONE_JPN  (3600 * 9)  // JST is UTC+9hours
#define DAYLIGHTOFFSET_JPN  (0)   // No summer time
#define NTP_SERVER1   "ntp.nict.jpntp.nict.jp"  // NTP
#define NTP_SERVER2   "ntp.jst.mfeed.ad.jp"   // sub NTP

#ifndef STASSID
#define STASSID "M+S-Fibre-G"
#define STAPSK  "****"
#endif

#define MDNSNAME "power"
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int LED = 2;
const int RELAY=0;

void handleRoot() {
  digitalWrite(LED, 1);
  server.send(200, "text/plain", "hello from esp8266!\r\n");
  digitalWrite(LED, 0);
}

void handleNotFound() {
  digitalWrite(LED, 1);
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
  digitalWrite(LED, 0);
}

void log(char* msg)
{
  time_t timeNow = time(NULL);
  struct tm* tmNow = localtime(&timeNow);

  char szTime[32];
  sprintf( szTime, "%02d:%02d:%02d %s\n",
            tmNow->tm_hour,
            tmNow->tm_min,
            tmNow->tm_sec,
            msg
            );
  Serial.print(szTime);  
}

void setup(void) {
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);

  digitalWrite(LED, 0);
  digitalWrite(RELAY, HIGH); // suppose "on"

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  delay(400);
  digitalWrite(RELAY, LOW); // suppose "off"

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(400);
  digitalWrite(RELAY, HIGH); // suppose "on"

  if (MDNS.begin(MDNSNAME)) {
    Serial.println("MDNS responder started");
  }

  configTime( TIMEZONE_JPN, DAYLIGHTOFFSET_JPN, NTP_SERVER1, NTP_SERVER2 );
  Serial.println("NTP READY");

  server.on("/", handleRoot);

  server.on("/off", []() {
    server.send(200, "text/plain", "OFF");
    log("OFF");
    digitalWrite(RELAY, LOW);
    digitalWrite(LED, 0); // suppose "on"
  });

  server.on("/on", []() {
    server.send(200, "text/plain", "ON");
    log("ON");
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED, 1); // suppose "off"
  });

  server.onNotFound(handleNotFound);

  server.begin();
  log("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();

  time_t timeNow = time(NULL);
  struct tm* tmNow = localtime(&timeNow);

  if (((tmNow->tm_hour == 22)||(tmNow->tm_hour == 2))&&(tmNow->tm_min==0)&&(tmNow->tm_min==0)){
    log("Good night.");
    digitalWrite(RELAY, LOW);
    digitalWrite(LED, 0); // suppose "on"
    delay(1000);
  }
}
