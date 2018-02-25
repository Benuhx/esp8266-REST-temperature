#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//## PINS
const byte PIN_TEMP = D2;
//## ENDE PINS

//## WiFi:
const char *wifiSsid = "WiFi SSID";
const char *wifiPassword = "Wifi Password";
const char *wifiHostname = "ESP 8266 Temp Sensor"
//## Ende WiFi

//# SERVER:
ESP8266WebServer server(80);
//# Ende Server

//##Temp
const byte maxTempMessungen = 3;
const int wartezeitMessreicheMs = 500;
const float wertUngueltigeTempMessung = -999;
OneWire oTemp(PIN_TEMP);
DallasTemperature sensors(&oTemp);

float aktuellerTempWert;
//#Ende Temp

void setup()
{
  Serial.begin(9600);
  sensors.begin();
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to WiFi...");
  connectToWiFi();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());

  server.on("/", handleGetTemp);
  server.begin();
}

void loop()
{
  server.handleClient();
  aktuellerTempWert = getTemp();
  server.handleClient();

  if (WiFi.status() != WL_CONNECTED)
  {
    yield();
    connectToWiFi();
    yield();
    delay(3000);
    yield();
    if (WiFi.status() == WL_CONNECTED)
    {
      server.begin();
    }
  }
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.hostname(wifiHostname);
  WiFi.begin(wifiSsid, wifiPassword);
}

float getTemp()
{
  byte counter = 0;
  do
  {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    if (temp != 85.00 && temp != -127.0)
    {
      return temp;
    }

    counter++;
    delay(wartezeitMessreicheMs);
  } while (counter <= maxTempMessungen);
  return wertUngueltigeTempMessung;
}

String getAktuellenJson()
{
  String daten = "{\"value\":" + String(aktuellerTempWert) + ", \"valueUnit\":" + "\"C\"" + "}";
  return daten;
}

void handleGetTemp()
{
  server.send(200, "application/json", getAktuellenJson());
}
