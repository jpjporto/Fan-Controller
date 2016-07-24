/**
   esp_main.ino

    Created on: 04.27.2016

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

const char* host = "192.168.*.*"; // Add IP address of main desktop
const char* SSID = "My SSID"; // Add your SSID
const char* wifi_password = "My Password"; // Add your wifi password
const char* msi_auth_key = "17cc95b4017d496f82"; // Add the MSI afterburner remote server security key

HTTPClient http;
ESP8266WiFiMulti WiFiMulti;

unsigned long timenow, lasttime;

long cpu_temp = 0;
long gpu_temp = 0;
int Ts = 5000; //time for reading samples from MSI, 5seconds.
uint8_t flag = 0;

void getTemps(String);

int incomingByte = 0;

void setup() {

  Serial.begin(115200);

  for (uint8_t t = 4; t > 0; t--) {
    //Serial.printf("[SETUP] WAIT %d...\n", t);
    //Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP("Minas Anor", "SecretPassword");
  http.setReuse(true);
  lasttime = 0;
}

void loop() {
  // wait for WiFi connection
  timenow = millis();
  if ((timenow - lasttime) > Ts) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      lasttime = timenow;
      //Serial.print("[HTTP] begin...\n");
      // configure traged server and url
      http.begin(host, 82, "/mahm");
      //http.setAuthorization("TVNJQWZ0ZXJidXJuZXI6MTdjYzk1YjQwMTdkNDk2Zjgy");
	  http.setAuthorization("MSIAfterburner", msi_auth_key);

      //Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          getTemps(payload);
          flag = 1;
        } else {
          flag = 0;
        }
      } else {
        flag = 0;
      }

      http.end();
    } else {
      flag = 0;
    }
  }

  if (flag == 1) { //send data to teensy
    Serial.printf("C:%d;G:%d\n", cpu_temp, gpu_temp);
    flag = 0;
  }
}

void getTemps(String msi_data)
{
  int index = 0;
  int index_data = 0;
  //Serial.println(msi_data.substring(0, msi_data.indexOf("encoding")));

  //Might change later so it averages cpu data;
  index = msi_data.indexOf("CPU temperature");
  if (index > 0)
  {
    //Serial.println("Hello");
    index_data = msi_data.indexOf("<data>", index);
    if (index_data > 0)
    {
      String cpu_data = msi_data.substring(index_data + 6, index_data + 10); //size of "<data>" = 6; number should be less than 4 digits
      cpu_temp = cpu_data.toInt();
    }
  }

  index = msi_data.indexOf("GPU temperature");
  if (index > 0)
  {
    index_data = msi_data.indexOf("<data>", index);
    if (index_data > 0)
    {
      String gpu_data = msi_data.substring(index_data + 6, index_data + 10); //size of "<data>" = 6; number should be less than 4 digits
      gpu_temp = gpu_data.toInt();
    }
  }
}


